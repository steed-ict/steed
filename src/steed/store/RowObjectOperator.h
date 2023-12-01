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
 * @file RowObjectOperator.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   Binary row struct Operator definition 
 */

#pragma once

#include <stdlib.h> // bsearch
#include <string.h> // memcpy 
#include <vector> 

#include "RowStructOperator.h"

namespace steed {
namespace Row {

using std::vector; 

class RowObjectOperator {
protected:
    Row::ID        *m_ids{nullptr}; /**< Row::ID array in  bin row content */ 
    Row::RowStructOperator  m_op{}; /**< other sections of bin row content */ 

public:
    RowObjectOperator (void) = default;
    ~RowObjectOperator(void) = default; 

public:
    /** is init flag */
    bool isInited (void) 
    { return m_op.isInited(); }

    /**
     * init 2 read  
     * @param bgn  binary value content begin 
     */
    void init2read(char *bgn); 

    /** uninit members */
    void uninit   (void)
    { m_ids = nullptr; m_op.uninit(); }


public: // read 
    uint32_t    getTotalSize(void) { return m_op.getTotalSize(); }
    uint32_t    getHeadSize (void) { return m_op.getHeadSize (); }
    uint32_t    getInfoSize (void) { return m_op.getInfoSize (); }
    uint32_t    getElemNum  (void) { return m_op.getElemNum  (); }
    uint32_t    getOffSize  (void) { return m_op.getOffSize  (); }

    uint32_t    getIDSize   (void) { return sizeof (Row::ID) * getElemNum(); }
    uint32_t    getValSize  (void) { return m_op.getValSize() - getIDSize(); }

    uint32_t    lookupID  (Row::ID id);
    Row::ID     getRowID  (uint32_t i) { return m_ids[i]; }
    uint32_t    getOffset (uint32_t i) { return m_op.getOffset  (i); }
    uint32_t    getBinSize(uint32_t i);
    const char *getBinVal (uint32_t i) { return m_op.getBinVal  (i); }


public: // write
    /**
     * init 2 append: append more fields to binary row content
     *     the binary values of the appended fields will overwrite the id offset array
     * @param bgn        binary record begin 
     * @param ids        fields' row ids 
     * @param offs       fields' offsets 
     * @param vlen       all fields' value length 
     * @param ret_len    buffer return length to overwrite 
     */
    void init2append(char *bgn, vector<Row::ID> &ids, vector<uint32_t> &offs,
            uint32_t &vlen, uint32_t &ret_len);

    /**
     * init 2 write  
     * @param bgn  binary value content begin to write
     */
    void init2write (char *bgn)
    { m_op.init2write(bgn); }

    /**
     * package binary row content to bgn
     * @param flag    offset flag
     * @param vlen    binary value length
     * @param iv      ID     array
     * @param of      Offset array 
     * @return binary content total used  
     */
    uint32_t package(uint32_t flag, uint32_t vlen, vector<Row::ID> &iv, vector<uint32_t> &ov);

public: // output2debug 
    void output2debug(void);

public:
    /**
     * compare Row::ID   
     * @param l    left  row id  
     * @param r    right row id 
     * @return l - r 
     */
    static int compareID(const void *l, const void *r)
    { return int(int64_t(*(Row::ID*)l) - int64_t(*(Row::ID*)r)); }
}; // RowObjectOperator





inline
void RowObjectOperator::init2read(char *bgn)
{
    m_op.init2read(bgn);
    const char *off = m_op.getOffsetBegin();
    uint32_t    len = getIDSize();
    m_ids = (Row::ID*)(off - len); // id array is before offset array
} // init2read



inline
uint32_t RowObjectOperator::lookupID(Row::ID id)
{
    uint32_t mnum = this->getElemNum(); 
    Row::ID *got =
        (Row::ID*)bsearch(&id, m_ids, mnum, sizeof(Row::ID), compareID);
    return (got != nullptr) ? got - m_ids : uint32_t(-1);
} // lookupID



inline
uint32_t RowObjectOperator::getBinSize(uint32_t i)
{
    uint32_t got = m_op.getBinSize  (i); 
    bool tail = (i + 1 == getElemNum()); 
    if  (tail) 
    {
        // exclude the Row::ID array from tail bin size 
        got -= getIDSize();
    } // if
    return got;
} // getBinSize



inline
void RowObjectOperator::
    init2append(char *bgn, vector<Row::ID> &ids, vector<uint32_t> &offs,
        uint32_t &vlen, uint32_t &ret_len)
{
    assert(ids.empty());
    assert(ids.size() == offs.size());

    // recover id and offset array 
    this->init2read(bgn);
    uint32_t mnum = getElemNum();
    for (uint32_t mi = 0; mi < mnum; ++mi)
    {
        ids .emplace_back( getRowID (mi) ); 
        offs.emplace_back( getOffset(mi) ); 
    } // for mi 
    vlen = this->getValSize();
    
    // deallocate originally id and offset array from Buffer
    uint32_t id_len  = getIDSize  ();
    uint32_t off_len = getOffSize ();
    uint32_t info_len= getInfoSize();
    ret_len= id_len + off_len + info_len; // overwrite length  

    this->uninit();
    this->init2write(bgn);
} // init2append



inline
uint32_t RowObjectOperator::
    package(uint32_t flag, uint32_t vlen, vector<Row::ID> &iv, vector<uint32_t> &ov)
{
    // firstly write Row::ID array binary value content 
    char* bgn = m_op.getWriteBegin();
    uint32_t used = sizeof(Row::Size) + vlen; // Row::Size + Binary Values
    m_ids = (Row::ID*)(bgn + used); 

    uint32_t mnum = iv.size();
    uint32_t ilen = mnum * sizeof(Row::ID); // id length
    memcpy(m_ids, iv.data(), ilen);
    vlen += ilen;  

    // then write offset array and Info content
    return m_op.package(flag, vlen, ov);
} // package 



inline
void RowObjectOperator::output2debug(void)
{
    // NOTE: tail elem's length: exclude the size of ID array

    uint32_t mnum = getElemNum();
    puts  ("RowObjectOperator:");
    printf("\tm_ids@[%p] size[%lu]\n", m_ids, mnum * sizeof(Row::ID));

    for (uint32_t i = 0; i < mnum; ++i)
    { printf("[%u] ", m_ids[i]); }
    puts("");

    m_op.output2debug();
} // output2debug 

} // namespace row
} // namespace steed