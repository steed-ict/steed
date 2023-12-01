/*
 * Copyright 2023 Zhiyi Wang
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



/**
 * @file CAB.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *     definitions and functions for CAB
 *
 *     CAB (Column Align Block) layout on disk:
 *         trivial CAB: (empty)
 *         allnull CAB: rep bits + def bits 
 *         crucial CAB: rep bits + def bits + bin values 
 *     Tips:    
 *         rep + def bits are always un-compressed 
 *         bin values are compressed 
 */

#pragma once 

#include <stdio.h>
#include <vector>

#include "Buffer.h"
#include "Config.h"
#include "DataType.h"

#include "CABMeta.h"
#include "CABInfo.h"
#include "CABItemInfo.h"
#include "ColumnItemArray.h"


namespace steed {

extern Config g_config;
using std::vector; 


/**
 * CAB is short for Column Align Block,
 *   which is the basic unit to store column data.
 * The CAB is aligned by record, no matter how many items in it.
 */

class CAB {
public:
    /**
     * make ColumnItemArray + Buffer as an unit 
     * > ColumnItemArray:  explain and operate the binary content 
     * > Buffer         :  buffer  the binary content 
     * > own buffer flag:  whether the unit owns the Buffer ins  
     */ 
    class CABItemUnit; 
    friend class CABMemAligner ; /** align CollectionWriter by CAB in memory */
    friend class CABDiskAligner; /** align CollectionWriter by CAB in disk   */

protected:
    CABMeta    *m_meta{nullptr};  /**< cab meta info    */
    CABInfo    *m_info{nullptr};  /**< cab storage info */

    /**
     * the majority ColumnItems are saved in m_major_unit 
     * ColumnItemArray::s_non        -> m_minor_units.empty() == true (all  items)
     * ColumnItemArray::s_sgl | s_mtl-> m_minor_units.size () >  0    (some items)
     */
    CABItemUnit*         m_major_unit {nullptr}; 
    vector<CABItemUnit*> m_minor_units{ };       /**< minor   item uint */

    CABItemUnit*         m_cur_unit   {nullptr}; /**< current item uint */
    CABItemInfo          m_item_info  { };       /**< current item info */
    uint32_t             m_bva_bgn_off{0};       /**< bin begin offset  */ 

public: 
    const uint32_t m_align_size{0};
    const uint32_t m_buf_size  {0};
    const uint32_t m_recd_cap  {0};


public:
    /**
     * use Buffer in CABMeta to init CAB to read or write
     * @param mt    CABMeta instance
     * @param info  CABInfo instance
     */
    CAB (CABMeta *mt, CABInfo *info) : CAB(mt, info, mt->m_buf) {}

    /**
     * use Specified Buffer to init CAB to read during appending
     * @param mt    CABMeta instance
     * @param info  CABInfo instance
     * @param buf   Buffer instance with CAB binary content
     */
    CAB (CABMeta *mt, CABInfo *info, Buffer *buf);

    ~CAB(void);

public: 
    void      invalid(void)  { m_info = nullptr; }
    bool      valid  (void)  { return m_info != nullptr; }

    DataType *getDataType (void)  { return m_meta->m_dt; }
    uint64_t  getRecdCap  (void)  { return m_meta->m_recd_cap; }
    uint64_t  getItemNum  (void)  { return m_item_info.m_item_num; }
    uint64_t  getRecdNum  (void)  { return m_item_info.m_recd_num; }

    /**
     * get merged CAB binary content used 
     * @param tail    is tail CAB flag 
     * @return merged contents used size 
     */
    uint64_t getMergedUsed(bool tail); 

protected:
    /**
     * get CAB rep def array used 
     * @param tail    is tail CAB flag 
     * @return rep and def array used 
     */
    uint64_t getRepDefUsed(bool tail);

    /**
     * get binary value array used  
     * @param tail    is tail CAB flag 
     * @return binary value array used
     */
    uint64_t getValueUsed (bool tail);


public:
    /**
     * init this CAB to write 
     * @param bgn_rid    begin record id in CAB
     * @return <0 failed; 0 success 
     */
    int init2write(uint64_t bgn_rid);

    /**
     * write null item
     * @param rep    max repetition value
     * @param def    max definition value 
     * @return <0 failed; 0 CAB is full; 1 (>0) success
     */
    int writeNull(uint32_t rep, uint32_t def);

    /**
     * write next text item
     * @param rep    max repetition value
     * @param def    max definition value 
     * @param txt    text value content 
     * @param bin    binary value content
     * @return <0 failed; 0 CAB is full; 1 (>0) success
     */
    int writeText(uint32_t rep, uint32_t def, const char *txt, const void* &bin);

    /**
     * write next val item
     * @param rep    max repetition value
     * @param def    max definition value 
     * @param bin    binary value content 
     * @param len    binary value length  
     * @return <0 failed; 0 CAB is full; 1 (>0) success
     */
    int writeBinVal (uint32_t rep, uint32_t def, const void *bin, uint32_t len = 0);

//    /**
//     * get the tail bin value and length just be written 
//     * @return 1 success; 0 got nothing 
//     */
//    int getWrittenBin(const void* &bin, uint32_t &len);

    /**
     * merge contents of ColumnItemArray to one
     * @param mgr_buf    CAB binary content merged buffer 
     * @param is_tail    is tail CAB flag: tail CAB has to output bin content  
     * @return >=0 merge bytes number; <0 failed   
     */
    int64_t merge2Buffer(Buffer *mgr_buf, bool is_tail = false);

    /**
     * calculate this CABInfo after write items
     */
    void  updateInfo(void)
    { m_info->m_item_info = m_item_info; }

protected:
    /**
     * check this CAB is written full 
     * @param rep  next rep to write 
     * @return true is full; false not full 
     */
    bool checkFull(uint32_t rep)
    { return (rep == 0) && (m_item_info.m_recd_num + 1 > m_meta->m_recd_cap); }

    /**
     * create new CIA instance 
     * @param buf    Buffer containing binary content 
     * @param bva    BinaryValueArray instance 
     * @param cap    item capacity in CIA
     */
    ColumnItemArray *createCIA(Buffer *buf, BinaryValueArray *bva, uint64_t cap)
    { return new ColumnItemArray (cap, m_meta->m_dt, buf, bva); }

    /**
     * create minor CIA instance to write
     */
    CABItemUnit *createMinorUnit(void); 

    /**
     * merge content of this CAB to mgr_buf
     * @param mgr_buf    merged Buffer 
     * @param app_func   ColumnItemArray member func pointer  
     */
    int64_t mergeSegment(Buffer *mgr_buf, ColumnItemArray::AppendFunc func);


public:
    /**
     * init this CAB to read 
     * @param type    content type to read  
     * @return <0 failed; 0 success 
     */
    int init2read(CABItemInfo::Type type);

    /**
     * read ColumnItem whose index is idx 
     * @param idx    ColumnItem index to read 
     * @param ci     got ColumnItem 
     * @return >0 success; 0 is EOF; <0 failed  
     */
    int read(uint64_t idx, ColumnItem &ci);

    /**
     * get binary value array to compare in predicates 
     * @return BinaryValueArray ins: nullptr when all items are null 
     */
    BitVector *getRepBitsVec(void);

    /**
     * get binary value array to compare in predicates 
     * @return BinaryValueArray ins: nullptr when all items are null 
     */
    BinaryValueArray* getBinValueArray(void);


public:
    int64_t copyContent(CAB *cab);

public:
    void output2debug(void);
}; // CAB

} // namespace steed



#include "CAB_inline.h"
