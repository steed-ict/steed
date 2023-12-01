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
 * @file BoolVector.cpp
 * @author  Zhiyi Wang  <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for BoolVector.
 */
#include "BoolVector.h"

namespace steed {

const uint32_t BoolVector::s_first_set_table[] = {
// bit    7  6  5  4  3  2  1  0

// vertically read the values below:
// val  | 1                       |
//      | 2  6  3  1              |
//      V 8  4  2  6  8  4  2  1  V

/* 0 */   uint32_t(-1),
/* 1 */   0, 
/* 2 */   1, 0, 
/* 4 */   2, 0, 1, 0,
/* 8 */   3, 0, 1, 0, 2, 0, 1, 0,
/*16 */   4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,

/*32 */   5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
          4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,

/*64*/    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
          4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
          5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
          4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,

/*128*/   7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
          4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
          5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
          4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
          6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
          4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
          5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
          4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
}; // s_first_set_table


const uint32_t BoolVector::s_pop_count_table[] = {
// max bit 
/*1*/  0, 1,
/*2*/  1, 2,
/*3*/  1, 2, 2, 3,
/*4*/  1, 2, 2, 3, 2, 3, 3, 4,
/*5*/  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,

/*6*/  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
       2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
       
/*7*/  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
       2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
       2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
       3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
       
/*8*/  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
       2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
       2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
       3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
       2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
       3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
       3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
       4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
}; // s_pop_count_table


uint64_t BoolVector::s_smear_table[sizeof(uint64_t) << 3];

int BoolVector::s_init = BoolVector::init();

int BoolVector::init(void)
{
    uint64_t bnum = sizeof(uint64_t) << 3;
    uint64_t bmax = bnum - 1;
    for (uint64_t bi = 0; bi < bmax; ++bi)
    {
        s_smear_table[bi] = (uint64_t(1) << bi) - 1;
    } // for
    s_smear_table[bmax] = uint64_t(-1);
    return 1;
} // init


uint64_t BoolVector::getNextSetBit(uint64_t bi)
{
    uint64_t bit_bgn  = bi % 8; 
    uint64_t byte_idx = bi / 8;
    uint64_t got_idx  = byte_idx * 8; // aligned by 8 
    uint8_t  skp_mask = (uint16_t(1) << bit_bgn) - 1;  // skip bit mask 
    uint8_t  bgn_mask = ~skp_mask; 
    
    uint8_t *bytes = (uint8_t*)m_cont;
    uint8_t  bcont = bytes [byte_idx];
    bcont &= bgn_mask;

    uint64_t got_bit = 0;
    uint64_t bit_used  = m_bits_used % 8;
    uint64_t byte_used = m_bits_used / 8;
    do 
    {
        got_bit = s_first_set_table[bcont]; 
        if  (got_bit != uint32_t(-1))
        {
            got_idx  += got_bit;
            break;
        } 
        else
        {
            got_idx += 8; // sizof(uint8_t) == 8 bits
            bcont = bytes[++byte_idx];
        } // if 
    } while (byte_idx <= byte_used);

    return ((byte_idx <  byte_used) ||
        ((byte_idx == byte_used) && (got_bit < bit_used))) ?  got_idx : uint64_t(-1);
} // getNextSetBit 


uint64_t BoolVector::getPopCount (uint64_t bgn, uint64_t end)
{
    uint64_t pop_cnt = 0;
    uint64_t bgn_bit  = bgn % 8; 
    uint64_t bgn_byte = bgn / 8;
    uint8_t  bgn_mask = ~((uint8_t(1) << bgn_bit) - 1); // include begin
    
    end =   (end != UINT64_MAX) ? end : m_bits_used;     // exclude end  
    uint64_t end_bit  = end % 8; 
    uint64_t end_byte = end / 8;
    uint8_t  end_mask = ((uint16_t(1) << end_bit) - 1); 

    uint8_t *bytes = (uint8_t*)m_cont;
    uint8_t  cont  = bytes [bgn_byte];
    cont &= bgn_mask;
    while (bgn_byte < end_byte) 
    {
        pop_cnt += s_pop_count_table[cont];
        cont     = bytes[++bgn_byte];
    } // while 
    
    cont    &= end_mask;
    pop_cnt += s_pop_count_table[cont];
 
    return pop_cnt;
} // getPopCount


} // namespace steed 