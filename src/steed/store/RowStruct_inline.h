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
 * @file RowStruct_inline.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   Binary row struct layout definition 
 */

#include <string.h>

#pragma once

namespace steed {
namespace Row {


inline
void RowStructBase::output2debug(void)
{
    puts  ("RowStructBase pointers:");
    printf("\tBgn:[%p] Size:[+%ld] Value:[+%ld] Info:[+%ld]\n",
            m_rbgn, (char*)m_size-m_rbgn, m_vals-m_rbgn, (char*)m_info-m_rbgn);
    puts("");

    puts  ("RowStructBase members:");
    uint32_t mnum = getElemNum();
    for (uint32_t i = 0; i < mnum; ++i)
    {
        uint32_t off = getOffset (i);
        uint64_t bgn = getBinVal (i) - m_vals;
        uint32_t len = getBinSize(i);
        printf("\t%u>> <off[%3u]:val[+%ld]:len[%u]>\n", i, off, bgn , len);
    } // for

    if (mnum > 0) { puts(""); }

    puts  ("RowStructBase varibles:");
//    printf("\tSize:[%u] thrd:[%u][%u][%u]\n\t", *m_size, UINT8_MAX, UINT16_MAX, UINT32_MAX);
    printf("\tSize:[%u]\n\t", *m_size);
    m_info->output2debug();

    puts  ("RowStructBase Done!");
} // output2debug





template <class OT> inline
void RowStruct<OT>::uninit(void)
{
    m_rbgn = nullptr;
    m_size = nullptr, m_vals = nullptr, m_offs = nullptr, m_info = nullptr;  
} // uninit


template <class OT> inline
void RowStruct<OT>::init2read(char *bgn)
{
    m_rbgn = bgn;
    m_size = (Size*)bgn;
    m_vals = bgn + sizeof(Size);

    Size s = *m_size; // size of row struct
    m_info = (Info*)(bgn + s - sizeof(Info));

    char *tmp = (char*)m_info;
    uint32_t used = m_info->getOffsetArrayUsed(); // read offset array used 
    m_offs = (OT*)(tmp - used);
} // init2read


template <class OT> inline
uint32_t RowStruct<OT>::getBinSize(uint32_t i)
{
    bool tail = (i + 1 == getElemNum()); // elem i is the tail
    return (!tail) ? (m_offs[i+1] - m_offs[i]) : (getValSize() - m_offs[i]);
} // getBinSize


template <class OT> inline
uint32_t RowStruct<OT>::
    package(uint32_t flag, vector<uint32_t> &ov, char* bin)
{
    // append offset array content 
    uint32_t num  = ov.size(); 
    uint32_t used = num * Info::getOffsetUsed(flag);
    m_offs = (OT*)bin;

    bool alinged = ((uint64_t(bin) % sizeof(OT)) == 0); 
    if  (alinged)
    {
        for (uint32_t i = 0; i < num; ++i)
        {   m_offs[i] = ov[i];   }
    }
    else
    {
        // BUG: directly set NT content got segmentation fault @ gcc O3 
        // Unaligned Memory Accesses: 
        //   m_offs may be not evenly divisible by sizeof(OT)
        // @ see https://www.kernel.org/doc/html/latest/process/unaligned-memory-access.html
    
        // cast every uint32_t to type OT, then copy content 
        vector<OT> trans(num); 
        for (uint32_t i = 0; i < num; ++i)
        {   trans[i] = ov[i];   }

        uint32_t cplen = num * sizeof(OT);
        memcpy(m_offs, trans.data(), cplen);

        assert (used == cplen);
    } // if 
    
    // append Row::Info
    m_info = (Info*)(bin + used); 
    m_info = new (m_info) Info();      // placement new 
    m_info-> set (flag, num);
    operator delete (m_info, nullptr); // placement delete: does nothing

    return (used += sizeof(Info));
} // package


} // namespace Row
} // namespace steed