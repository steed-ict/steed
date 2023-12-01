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
 * @file RowStructOperator.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   Binary row struct Operator definition 
 */

#pragma once

#include <assert.h>
#include <array>

#include "Row.h"
#include "RowStruct.h"

namespace steed {
namespace Row {

using std::array;

class RowStructOperator {
protected:
    /** RowStruct for different size of offset */
    array<RowStructBase*, Info::s_flag_num>  m_structs{};
    RowStructBase  *m_cur {nullptr};         /**< current row struct  */
    uint32_t        m_flag{Info::s_invalid}; /**< current offset flag */

    /** binary content begin ptr to write */
    char *m_wt_bgn{nullptr};       

public:
    RowStructOperator (void);
    ~RowStructOperator(void);

public: // read 
    /** is init flag */
    bool isInited (void)
    { return ((m_cur != nullptr) || (m_wt_bgn != nullptr)); }

    /**
     * init 2 read  
     * @param bgn  binary value content begin 
     */
    void init2read(char *bgn); 

    /** uninit members */
    void uninit   (void);

    // get content info
    uint32_t    getTotalSize(void) { return m_cur->getTotalSize(); }
    uint32_t    getHeadSize (void) { return sizeof(Size); }
    uint32_t    getInfoSize (void) { return sizeof(Info); }
    uint32_t    getElemNum  (void) { return m_cur->getElemNum(); }
    uint32_t    getOffSize  (void) { return m_cur->getOffSize(); }
    uint32_t    getValSize  (void) { return m_cur->getValSize(); }

    // get element info
    uint32_t    getOffset (uint32_t i) { return m_cur->getOffset (i); }
    uint32_t    getBinSize(uint32_t i) { return m_cur->getBinSize(i); }
    const char *getBinVal (uint32_t i) { return m_cur->getBinVal (i); }
    const char *getOffsetBegin  (void) { return m_cur->getOffsetBegin(); }


public: // write
    /**
     * init 2 write  
     * @param bgn  binary value content begin to write
     */
    void init2write (char *bgn) { m_wt_bgn =  bgn; }

    /**
     * get write begin pointer  
     */
    char *getWriteBegin (void) { return m_wt_bgn; }

    /**
     * package binary row content 
     * @param flag    offset flag
     * @param vlen    binary value length
     * @param mnum    member number in struct
     * @return binary content total used  
     */
    uint32_t package(uint32_t flag, uint32_t vlen, vector<uint32_t> &ov);

public: // output2debug 
    void output2debug(void);
}; // RowStructOperator





inline
RowStructOperator::RowStructOperator (void)
{
    m_structs[Info::s_invalid] = nullptr;
    m_structs[Info::s_flag_8 ] = new RowStruct<uint8_t >();
    m_structs[Info::s_flag_16] = new RowStruct<uint16_t>();
    m_structs[Info::s_flag_32] = new RowStruct<uint32_t>();
} // ctor 


inline
RowStructOperator::~RowStructOperator(void)
{
    this->uninit();
    delete m_structs[Info::s_invalid]; m_structs[Info::s_invalid] = nullptr;
    delete m_structs[Info::s_flag_8 ]; m_structs[Info::s_flag_8 ] = nullptr;
    delete m_structs[Info::s_flag_16]; m_structs[Info::s_flag_16] = nullptr;
    delete m_structs[Info::s_flag_32]; m_structs[Info::s_flag_32] = nullptr;
} // dtor


inline
void RowStructOperator::init2read(char *bgn)
{
    uint32_t  blen = *(Size*)bgn; // length
    uint32_t *info = (uint32_t*)(bgn + blen - sizeof(Info));
    m_flag = Info::getFlag(info); // read 
    m_cur  = m_structs[m_flag]; 
    m_cur->init2read(bgn); 
} // init2read 


inline
void RowStructOperator::uninit(void)
{
    m_wt_bgn = nullptr;
    m_flag = Info::s_invalid;
    if (m_cur != nullptr)
    {
        m_cur->uninit();
        m_cur = nullptr;
    } // if 
} // uninit


inline
uint32_t RowStructOperator::package(uint32_t flag, uint32_t vlen, vector<uint32_t> &ov)
{
    assert(m_wt_bgn != nullptr);

    // Row::Size + Binary Values
    m_flag = flag;
    m_cur  = m_structs[m_flag];
    m_cur->init2write(m_wt_bgn);
    uint32_t used = sizeof(Size) + vlen; 

    // Offset array + Row::Info
    char* off = m_wt_bgn + sizeof(Size) + vlen; 
    used += m_cur->package(m_flag, ov, off); 

    return (*(Size*)m_wt_bgn = used);
} // package 


inline
void RowStructOperator::output2debug(void)
{
    puts  ("RowStructOperator:");
    printf("\tCurrent:flag[%u]@[%p]\n", m_flag, m_cur);
    m_cur->output2debug();

    puts  ("RowStructOperator Done");
} // output2debug

} // namespace Row
} // namespace steed