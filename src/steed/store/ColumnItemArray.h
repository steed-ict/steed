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
 * @file ColumnItemArray.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for ColumnItemArray in memory
 */

#pragma once 

#include "Buffer.h"
#include "Utility.h"
#include "DataType.h"
#include "BitVector.h"
#include "BinaryValueArray.h"
#include "ColumnItem.h"
#include "CABItemInfo.h"

namespace steed {

/**
 * ColumnItemArray is an array of ColumnItem without considering the record alignment.
 * The number and capacity of ColumnItem in ColumnItemArray is
 *   m_item_num and m_item_cap. 
 */

class ColumnItemArray {
protected:
    /**
     * binary cont buffer:
     *   fixed    length value: rep bits + def bits + binary values  
     *   variable length value: rep bits + def bits + offsets array 
     */
    Buffer           *m_buffer{nullptr};
    DataType         *m_dt    {nullptr}; /**< data type instance  */

    BitVector        *m_reps  {nullptr}; /**< rep value vector    */
    BitVector        *m_defs  {nullptr}; /**< def value vector    */
    BinaryValueArray *m_values{nullptr}; /**< bin value vector    */

    uint64_t          m_item_cap    {0}; /**< item vector capacity*/
    uint64_t          m_item_num    {0}; /**< item number used    */

    /** ColumnItemArray bin content type */
    CABItemInfo::Type m_type{CABItemInfo::crucial}; 


public: 
    ~ColumnItemArray(void); 
    ColumnItemArray (uint32_t cap, DataType *dt, Buffer *buf, BinaryValueArray *bva);

public:
    CABItemInfo::Type getType      (void) { return m_type; } 
    BitVector        *getRepBitsVec(void) { return m_reps; }
    BitVector        *getDefBitsVec(void) { return m_defs; }
    BinaryValueArray *getValueArray(void) { return m_values; }
    uint64_t          getItemNumber(void) { return m_item_num; }

    /**
     * get rep and def array bytes used 
     * @return rep and def array size
     */
    uint64_t getRepDefUsed(void);

    /**
     * get binary value bytes used 
     * @return value size
     */
    uint64_t getValueUsed (void);

    /**
     * get binary content bytes used
     * @return total size
     */
    uint64_t getContentUsed(void);

public: // init 
    /** 
     * init ColumnItemArray to write 
     * @param max_rep    max repetition value  
     * @param max_def    max definition value  
     * @return 0 success; <0 failed  
     */
    int init2write (uint32_t max_rep, uint32_t max_def);

    /**
     * init ColumnItemArray to read  
     * @param type       CIA Content type, @see ColumnAlignBlockItemInfo.h
     * @param max_rep    max repetition value  
     * @param max_def    max definition value  
     * @param item_num   item number used in this vector 
     * @return 0 success; <0 failed  
     */
    int init2read(CABItemInfo::Type type, uint32_t max_rep, uint32_t max_def,
            uint64_t item_num);

    /**
     * prepare ColumnItemArray to append
     * @param cia    read ColumnItemArray content  
     * @return >0 copy length; <0 failed  
     */
    int64_t copyContent(ColumnItemArray *cia);


public: // read 
    /**
     * read ColumnItem whose index is idx 
     * @param idx    ColumnItem index to read 
     * @param ci     got ColumnItem 
     * @return >0 success; 0 is EOF; <0 failed  
     */
    int read(uint64_t idx, ColumnItem &ci);


public: // write 
    /**
     * write null as next value 
     * @param rep    next rep  value 
     * @param def    next def  value 
     * @return >0 success; 0 array is full; <0 failed  
     */
    int writeNull(uint32_t rep, uint32_t def);

    /**
     * write next ColumnItem: value is text 
     * @param rep    next rep  value 
     * @param def    next def  value 
     * @param txt    next text value 
     * @param bin    binary value 
     * @return >0 success; 0 array is full; <0 failed  
     */
    int writeText(uint32_t rep, uint32_t def, const char *txt, const void* &bin);

    /**tB!CuVr4xYP#aCfv
     * write next ColumnItem: value is binary 
     * @param rep    next rep value 
     * @param def    next def value 
     * @param bin    next bin value 
     * @param len    next bin value length
     * @return >0 success; 0 array is full; <0 failed  
     */
    int writeBinVal(uint32_t rep, uint32_t def, const void *bin, uint32_t len);

//    /**
//     * get the tail bin value and length just wrotten
//     * @return 1 success; 0 got nothing 
//     */
//    int getWrittenBin(const void* &bin, uint32_t &len)
//    {   return m_values->getWrittenBin(bin, len);   } 

private:
    /**
     * write the rep and def value of the column item
     * @param rep    next rep value
     * @param def    next def value
     * @return 0 success; <0 failed
     */
    int writeRepDef(uint32_t rep, uint32_t def);


public: // flush 
    /**
     * member function pointer to append content 2 buffer 
     * @param buf    Buffer instance 
     * @return >=0 append number as success; <0 failed  
     */
    typedef int64_t (ColumnItemArray::*AppendFunc)(Buffer *buf);

    /**
     * append rep value array to Buffer
     * @param buf    Buffer instance 
     * @return >=0 append number as success; <0 failed  
     */
    int64_t appendRep(Buffer *buf)
    { return appendBitsContent(m_reps, buf); }

    /**
     * append def value array to Buffer
     * @param buf    Buffer instance 
     * @return >=0 append number as success; <0 failed  
     */
    int64_t appendDef(Buffer *buf)
    { return appendBitsContent(m_defs, buf); }

    /**
     * append offsets array to Buffer
     * @param buf    Buffer instance 
     * @return >=0 append number as success; <0 failed  
     */
    int64_t appendOffsets(Buffer *buf)
    { return m_values->appendOffsets(buf); } 

    /**
     * append binary value array to Buffer
     * @param buf    Buffer instance 
     * @return >=0 append number as success; <0 failed  
     */
    int64_t appendValues(Buffer *buf)
    { return m_values->appendValues(buf); } 

protected:
    /**
     * append BitVector binary content to Buffer
     * @param bits    BitVector operator 
     * @param buf     Buffer instance 
     * @return >=0 append number as success; <0 failed  
     */
    int64_t appendBitsContent(BitVector *bv, Buffer *buf);


public:
    void output2debug(void); 
}; // ColumnItemArray

} // namespace steed  



#include "ColumnItemArray_inline.h"
