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
 * @file BitVector_inline.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   BitVector inline functions 
 */

#include <stdio.h> 
#include <string.h> 

#pragma once

namespace steed {

inline
BitVector::BitVector(uint64_t msk_size) : m_mask_size(msk_size)
{
    m_mask = (uint64_t(1) << m_mask_size) - 1;
} // ctor


inline
bool BitVector::operator== (const BitVector& bv)
{
    bool mask = (m_mask == bv.m_mask);
    bool used = (m_bits_used  == bv.m_bits_used );
    bool elem = (m_elem_used  == bv.m_elem_used );
    bool next = (m_next_64bit == bv.m_next_64bit);

    bool cbgn = (m_mask_size == 0) || (m_cont != bv.m_cont);
    bool cont = (memcmp(m_cont, bv.m_cont, getUsedSize()) == 0);
    return mask && used && elem && next && cbgn && cont;
} // operator == 


inline
int BitVector::resizeBitUsed(uint64_t bit_used)
{
    if (m_mask_size == 0) { return 0; }

    if (bit_used > m_bits_cap) 
    { puts("BitVector: resize used bits failed!\n"); return -1; }

    m_bits_used = bit_used;
    m_elem_used = bit_used / m_mask_size;
    return 0;  
} // resizeBitUsed


inline
int BitVector::resizeElemUsed(uint64_t elem_used)
{
    if (m_mask_size == 0)  { return 0; }
    
    if (elem_used > getElementCap()) 
    { puts("BitVector: resize used elem failed!\n"); return -1; }

    uint64_t bused = elem_used * m_mask_size;
    m_bits_used = bused;
    m_elem_used = elem_used;
    m_next_64bit = Utility::calcAlignBegin(64, bused) + 64; // next 64 bit

    return 0;  
} // resizeElemUsed


inline
int BitVector::init2write(uint64_t len, void *bgn)
{
    assert (bgn != nullptr);

    if (m_mask_size > 32)
    { puts("BitVector:: too long to init2write!"); return -1; }
    else if (m_mask_size == 0)
    { return 0; }

    m_cont = (uint64_t*) bgn;
    memset(m_cont, 0x00, len);
    m_bits_cap  = len << 3;  // byte # * 2^3 = bit #
    m_bits_used = m_elem_used = m_next_64bit = 0;
    return 0;
} // init2write


inline
int BitVector::init2read(uint64_t elnum, uint64_t len, const void *bgn)
{
    assert (bgn != nullptr);

    if (m_mask_size > 32)
    { puts("BitVector:: too long to init2read!"); return -1; }
    else if (m_mask_size == 0)
    { return 0; }

    m_cont = (uint64_t*)bgn;
    m_bits_cap  = len << 3;  // byte # * 2^3 = bit #

    uint64_t bused = m_mask_size * elnum;  // bit used 
    if (bused > m_bits_cap)
    { puts("BitVector: init2read overflow!"); return -1; }

    // update state
    m_bits_used  = bused;
    m_elem_used  = elnum;  
    m_next_64bit = Utility::calcAlignBegin(64, bused) + 64; // next 64 bit

    return 0;
} // init2read


inline
int64_t BitVector::copyContent(BitVector *src)
{
    assert (m_mask == src->m_mask);

    if (this == src)      { return 0; }
    if (m_mask_size == 0) { return 0; }
    
    if (m_bits_cap < src->m_bits_cap)
    { puts("BitVector: copy overflow!\n"); return -1; }

    m_bits_used  = src->m_bits_used ;
    m_elem_used  = src->m_elem_used ;
    m_next_64bit = src->m_next_64bit;
    
    const void *cp_bgn = src->getContent ();
    uint64_t    cp_len = src->getUsedSize();
    memcpy(m_cont, cp_bgn, cp_len);

    return int64_t(cp_len);
} // copyContent


inline
int BitVector::setByBit(uint64_t bi, uint64_t val) 
{
    if (m_mask_size == 0) { return 0; }

    uint64_t *ubgn = nullptr;
    uint64_t  uidx = 0;
    getBitUnit(bi, ubgn, uidx);
 
    val   &=   m_mask;          // getByBit value in bits 
    *ubgn &= ~(m_mask << uidx); // 1. clear val bits 2. keep others
    *ubgn |=  (val    << uidx); // setByBit val bits 
  
    return 0;
} // setByBit


inline
uint64_t BitVector::getByBit(uint64_t bi) 
{
    if (m_mask_size == 0) { return 0; }

    uint64_t *ubgn = nullptr;
    uint64_t  uidx = 0;
    getBitUnit(bi, ubgn, uidx);

    uint64_t val = *ubgn & (m_mask << uidx);
    return  (val >> uidx);
} // getByBit


inline
void BitVector::getBitUnit(uint64_t bi, uint64_t* &ubgn, uint64_t &uidx)
{
    ubgn = m_cont + (bi / 64);
    uidx = bi % 64;
    if (uidx + m_mask_size > 64) 
    {
        // avoid unit overflow  
        ubgn = (uint64_t*)((char*)ubgn + 4);
        uidx -= 32;  
    } // if
} // getBitUnit


inline
int BitVector::append (uint64_t val)
{
    if (m_mask_size == 0)  { return 0; }

    // check unit overflow
    if (m_bits_used + m_mask_size > m_next_64bit)  
    {
        if (m_next_64bit + 64 <= m_bits_cap)
        {
            m_cont[m_next_64bit / 64] = 0;
            m_next_64bit += 64;
        }
        else
        {
            // next 64 is overflow than m_bits_cap 
            uint64_t my_last    = (m_bits_cap - m_next_64bit);
            uint64_t my_mask    = (uint64_t(1) << my_last) - 1;
            uint64_t other_mask = ~my_mask;
            m_cont[m_next_64bit / 64] &= other_mask; // keeps other's bits
            m_next_64bit += 64;
        } // if 
    } // if 

    return (val == 0) ? appendZero() : appendNotZero(val);
} // append 


inline 
int BitVector::appendZero (void) 
{
    // content is cleared by m_next_64bit
    ++m_elem_used;
    m_bits_used += m_mask_size;
    return 0;
} // appendZero


inline
int BitVector::appendNotZero (uint64_t val) 
{
    uint64_t  *ubgn = nullptr;
    uint64_t   uidx = 0;
    getBitUnit(m_bits_used, ubgn, uidx);

    val   &= m_mask;
    *ubgn |= (val << uidx);

    ++m_elem_used;
    m_bits_used += m_mask_size;
  
    return 0;    
} // appendNotZero



inline
void BitVector::output2debug(void)
{
    printf("BitVector@[%p]\n", m_cont);
    printf("m_mask[%lu] m_mask_size[%lu] m_bits_cap[%lu]\n", 
        m_mask, m_mask_size, m_bits_cap);
    printf("m_bits_used[%lu] m_elem_used[%lu] m_next_64bit@[%lu]\n",
        m_bits_used, m_elem_used, m_next_64bit);
    for (uint64_t ei = 0; ei < m_elem_used; ++ei)
    {
        uint64_t v = (*this)[ei];
        printf("<%lu> ", v);
        if (ei % 8 == 7) { puts(""); } 
    }
    puts("");
} // output2debug 

} // namespace steed