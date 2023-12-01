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
 * @file ColumnAlignBlockInfo.h
 * @author  Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    definitions and functions for ColumnAlignBlockInfo
 */

#pragma once 

#include <string>
#include <stdint.h>

#include "Config.h"
#include "Block.h"
#include "Buffer.h"
#include "FileIO.h"
#include "CABItemInfo.h"
#include "ColumnValueInfo.h"

namespace  steed {

using std::string;

extern Config g_config;

//----------- ColumnAlignBlock Content Info -----------
class CABInfo {
public:
    // size
    uint32_t    m_strg_size{0};  /**< aligned storage size in col file */
    uint32_t    m_dsk_size {0};  /**< encode size on disk: R+D+cmp_B+F */
    uint32_t    m_mem_size {0};  /**< decode size in mem : R+D+org_B   */

    // type
    uint16_t    m_rep_type {0};  /**< rep type id by CABWriter   */
    uint16_t    m_cmp_type {0};  /**< compress Compressor::Type  */

    // CAB file offset 
    uint64_t    m_file_off   {0};  /**< CAB content begin offset in file*/

    /**
     * Column items Info in current CAB:
     * items counter for crucial, allnull and trivial cab type 
     */
    CABItemInfo      m_item_info {};  

    /**
     * Column value Info in current CAB:
     * value info for min and max value in current cab 
     */
    ColumnValueInfo  m_value_info{};  

public:
    bool       noStorageCont (void) { return m_strg_size == 0; }
    uint64_t   getBeginRecdID(void) { return m_item_info.m_bgn_recd; }
    uint32_t   getRecordNum  (void) { return m_item_info.m_recd_num; }
    uint64_t   getItemNumber (void) { return m_item_info.m_item_num; }
    uint64_t   getNullNumber (void) { return m_item_info.m_null_num; }
    CABItemInfo::Type getType(void) { return m_item_info.getType (); }

    void       setMinFlag    (void) { m_value_info.m_has_min = true; }
    void       setMaxFlag    (void) { m_value_info.m_has_max = true; }
    bool       hasMinimum    (void) { return m_value_info.m_has_min; }
    bool       hasMaximum    (void) { return m_value_info.m_has_max; }
    void      *getMinBin     (void) { return & (m_value_info.m_min); }
    void      *getMaxBin     (void) { return & (m_value_info.m_max); }

public:
    void       output2debug  (void);
}; // CABInfo





//----------- ColumnAlignBlock Info Buffer -----------
class CABInfoBuffer {
protected: 
    // footer info for Column
    typedef struct Footer {
        ColumnValueInfo   m_value_info {}; /**< col min and max info  */
        uint64_t          m_valid_recd{0}; /**< first valid record id */
        uint64_t          m_total_recd{0}; /**< total record number   */
        uint64_t          m_info_used {0}; /**< used CABInfo number   */
    } Footer;


protected: 
    /**
     * binary file content on disk, layout is as below:
     *   [ Blooming Filter Content ] | [ CABInfo Array ] | Footer
     *
     * m_buf buffer binary content in mem, layout is:
     *   [ CABInfo Array ] | Footer
     *
     * NOTE:
     * do not store the blooming content in m_buf, loadBloomContent when needed 
     *   [ Blooming Filter Content ]
     */
    Buffer          *m_buf  {nullptr}; 

    CABInfo         *m_infos{nullptr}; /**< CAB info array     */
    Footer           m_foot      {};   /**< buffer footer info */
    uint64_t         m_file_size{0};   /**< file written size  */

    uint64_t         m_next_idx {0};   /**< next info index    */
    uint8_t          m_io_tp{inmem};   /**< buffer init mode   */
    bool             m_got_tail{false};/**< got the tail for append */

public: // m_io_tp
    typedef enum Type {
        invalid = 0, 
        write   = 1, // write only
        read    = 2, // read  only
        modify  = 3, // TODO: read then write to modify
        inmem   = 4, // TODO: in memory without IO
        max     = 5,
    } Type; // enum Type

private: 
    static const int s_foot_size = sizeof(Footer ); 
    static const int s_info_size = sizeof(CABInfo); 
    static const int s_init_size = (4096 * 128); // 512KB


public:
    CABInfoBuffer (void) = default; 
    ~CABInfoBuffer(void);

public: 
    void  updateMemberPtr(void)
    { m_infos = (CABInfo*)m_buf->getPosition(0); } 

    void     setNextIdx  (uint64_t i) { m_next_idx = i; }
    void     resetNextIdx(void)       { m_next_idx = 0; }
    uint64_t getNextIndex(void)       { return m_next_idx; }
    CABInfo* getNextInfo (void)       { return getCABInfo(m_next_idx++); }
    CABInfo* getCABInfo  (uint64_t i)
    { return (i < m_foot.m_info_used) ? (m_infos + i) : nullptr; }

    int emplaceTailBack(void);

public:
    uint64_t getFileSize      (void) { return m_file_size; }
    uint64_t getValidRecdIdx  (void) { return m_foot.m_valid_recd; }
    uint64_t getUsedNumber    (void) { return m_foot.m_info_used ; }
    CABInfo* getNextInfo2Read (void) { return this->getNextInfo(); }
    CABInfo* getNextInfo2Write(void)
    { return ((emplaceTailBack() < 0) ? nullptr : getNextInfo ()); }

    /** get tail CABInfo used to append */
    CABInfo* getTailInfo(void)
    { uint64_t i = getUsedNumber() - 1; return getCABInfo(i); }

    ColumnValueInfo *getValueInfo(void) { return &(m_foot.m_value_info); }
    void            *getMinBin   (void) { return &(m_foot.m_value_info.m_min); }
    void            *getMaxBin   (void) { return &(m_foot.m_value_info.m_max); }


public: // write
    /**
     * write CAB infos to files  
     * @param n      CAB Info array file name string 
     * @param rbgn   valid record begin id (index) to write 
     * @return >0 success; <0 failed 
     */
    int init2write(const string &n, uint64_t rbgn);

    /**
     * append footer to file content
     * @return 0 success; <0 failed  
     */
    int appendFooter(void);

public: // read 
    /**
     * read CAB infos from file 
     * @param n    CAB Info array file name string 
     * @return >0 success; ==0 EOF; <0 failed  
     */
    int init2read(const string &n);

public: // append
    /**
     * append more CABinfos to files  
     * @param n    CAB Info array file name string 
     * @return >0 success; <0 failed 
     */
    int init2append(const string &n);

    /**
     * get tail CABInfo used to append
     * @return tail CABInfo pointer
     */
    CABInfo* getTailInfo2Append(void);

private:
    /**
     * read CAB infos from file 
     * @param n    CAB Info array file name string 
     * @return >0 success; ==0 EOF; <0 failed  
     */
    void readFile(void);

public:
    void output2debug (void);
}; // CABInfoBuffer

} // namespace steed


#include "CABInfo_inline.h"



// CABInfo 
#if DEFINED_BLM
    // blooming content: do not compress
    uint64_t    m_blm_bgn_off{0};  /**< bgn offset, aligned by s_mem_alignment */ 
    uint32_t    m_blm_mem_len{0};  /**< memory len, aligned by bit (uint64_t)  */ 
    uint32_t    m_blm_dsk_len{0};  /**< disk   len, aligned by s_mem_alignment */ 
#endif

#if DEFINED_BLM
    uint64_t   getBlmBeginOff(void) { return m_blm_bgn_off; }
    uint32_t   getBlmMemSize (void) { return m_blm_mem_len; }
    uint32_t   getBlmDskSize (void) { return m_blm_dsk_len; }
#endif 

// CABInfoBuffer
#if DEFINED_BLM
    /**
     * flush blooming content to file, 
     *     and update blooming info in CABInfo 
     * @param info      related CABInfo, saving blooming info 
     * @param blmbin    blooming binary content 
     * @param memlen    blooming content length (memory used)
     * @param dsklen    blooming content disk used 
     * @return >0 write bytes as success; < 0 failed
     */
    int64_t flushBloomContent(CABInfo *info, char* blmbin, uint64_t memlen, uint64_t dsklen);
#endif

#if DEFINED_BLM
    /**
     * load blooming content from file, 
     *     and update blooming info in CABInfo 
     * @param info      related CABInfo, saving blooming info 
     * @param blmbin    blooming binary content of BloomFilter::mBitSet
     * @return >0 success; 0 EOF; < 0 failed
     */
    int64_t loadBloomContent(CABInfo *info, char* blmbin);
#endif 

