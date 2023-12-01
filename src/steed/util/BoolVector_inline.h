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
 * @file BoolVector_inline.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   BoolVector inline functions 
 */


#pragma once

namespace steed {


inline
void BoolVector::flipAll(void)
{
    char    *bytes= (char*)m_cont;
    uint64_t bidx = 0;
    uint64_t bnum = getContCap();
    while   (bidx < bnum)
    { bytes[bidx++] ^= 0xff; }
} // flipAll


inline
int BoolVector::setBitByRange(uint64_t bgn, uint64_t end)
{
    if ((bgn >= end) || (end > m_bits_used))
    {
        printf("BoolVector::setBitByRange [%lu][%lu] failed!\n", bgn, end);
        return -1;
    } // if 

    /**
     * set range [1,5) 
     *            XXXX XXXX
     * pre_mask   0000 0001
     * bgn_mask   1111 1110
     * end_mask   0001 1111
     * post_mask  1110 0000
     */
    uint8_t *bytes   = (uint8_t*)m_cont;

    // include the begin bit
    uint8_t  bgn_bit  = bgn % 8; 
    uint64_t bgn_byte = bgn / 8;
    uint8_t  pre_mask = (uint8_t(1) << bgn_bit) - 1;
    uint8_t  bgn_mask = ~pre_mask; 
    uint8_t  bgn_cont = bytes[bgn_byte];

    // exclude the end  bit
    uint8_t  end_bit  = end % 8; 
    uint64_t end_byte = end / 8;
    uint8_t  end_mask = (uint8_t(1) << end_bit) - 1; 
    uint8_t  post_mask= ~end_mask; 
    uint8_t  end_cont = bytes[end_byte];

    // single byte
    if (bgn_byte == end_byte)
    {
        uint8_t sgl_mask  = (bgn_mask & end_mask); 
        bytes [bgn_byte] |=  sgl_mask;
    }
    else
    {
        uint8_t pre_bgn = (bgn_cont & pre_mask);
        bytes[bgn_byte] = (pre_bgn  | bgn_mask); // set from begin bit

        while (bgn_byte < end_byte) 
        { bytes[++bgn_byte] = uint8_t(-1); } 
    
        uint8_t  post_end= (end_cont & post_mask);
        bytes [end_byte] = (end_mask | post_end);
    } // if 

    return 0;
} // setBitByRange



inline
int BoolVector::clearBitByRange(uint64_t bgn, uint64_t end)
{
    if ((bgn >= end) || (end > m_bits_used))
    {
        printf("BoolVector::clearBitByRange [%lu][%lu] failed!\n", bgn, end);
        return -1;
    } // if 

    /**
     * set range [1,5) 
     *            XXXX XXXX
     * pre_mask   0000 0001
     * bgn_mask   1111 1110
     * end_mask   0001 1111
     * post_mask  1110 0000
     */
    uint8_t *bytes   = (uint8_t*)m_cont;

    // include the begin bit
    uint8_t  bgn_bit  = bgn % 8; 
    uint64_t bgn_byte = bgn / 8;
    uint8_t  pre_mask = (uint8_t(1) << bgn_bit) - 1;
    uint8_t  bgn_mask = ~pre_mask; 
    uint8_t  bgn_cont = bytes[bgn_byte];

    // exclude the end  bit
    uint8_t  end_bit  = end % 8; 
    uint64_t end_byte = end / 8;
    uint8_t  end_mask = (uint8_t(1) << end_bit) - 1; 
    uint8_t  post_mask= ~end_mask; 
    uint8_t  end_cont = bytes[end_byte];

    // single byte
    if (bgn_byte == end_byte)
    {
        uint8_t rng_mask  = (bgn_mask & end_mask); 
        bytes [bgn_byte] &= (~rng_mask); // clear range 
    }
    else
    {
        uint8_t pre_bgn = (bgn_cont & pre_mask);
        bytes[bgn_byte] = pre_bgn; // keep previous bits 

        while (bgn_byte < end_byte) 
        { bytes[++bgn_byte] = uint8_t(0); } 
    
        uint8_t post_end = (end_cont & post_mask);
        bytes [bgn_byte] = post_end; // keep post bits 
    } // if 

    return 0;
} // clearBitByRange 



inline
int BoolVector::mergeOr(BoolVector *v)
{
    if (check(v) < 0)  { return -1; }
  
    char *vc = (char*)(v->m_cont);
    char *mc = (char*)(m_cont); // my content in bytes
    uint64_t bused = Utility::calcBytesUsed(m_bits_used);
    for (uint64_t bidx = 0; bidx < bused; ++bidx)
    {   mc[bidx] |= vc[bidx];   }
    return 0;
} // mergeOr



inline
int BoolVector::mergeAnd(BoolVector *v)
{
    if (check(v) < 0)  { return -1; }
  
    char *vc = (char*)(v->m_cont);
    char *mc = (char*)(m_cont); // my content in bytes
    uint64_t bused = Utility::calcBytesUsed(m_bits_used);
    for (uint64_t bidx = 0; bidx < bused; ++bidx)
    {   mc[bidx] &= vc[bidx];   }
    return 0;
} // mergeAnd



inline
int BoolVector::check(BoolVector *v)
{
    bool same_mask = (m_mask_size == v->m_mask_size);
    bool same_used = (m_bits_used == v->m_bits_used);
    return ((same_mask && same_used) ? 0 : -1);
//    return ((same_mask && same_used) ? 0 : (abort(), -1));
} // check



} // namespace steed
