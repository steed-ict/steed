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
 * @file BitMap.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    definitions and functions for BitMap 
 */

#pragma once 

#include <string>
#include <stdint.h>

#include "Buffer.h"
#include "BoolVector.h"
#include "Utility.h"

namespace steed {

class BitMap {
protected:
    Buffer        *m_buf  {nullptr}; /**< item flag buffer   */
    BoolVector    *m_flag {nullptr}; /**< item compare flag  */
    uint64_t       m_set_bit_num{0}; /**< m_flag set bit num */

public:
    BitMap (uint32_t size);
    ~BitMap(void);

public:
    BoolVector *getBoolVector(void) { return m_flag; }
    uint64_t   &getSetBitNum (void) { return m_set_bit_num; }

    uint64_t getNextSetBit(uint64_t bi = 0)
    { return m_flag->getNextSetBit (bi); }

    int setByBit(uint64_t bi, uint64_t val)
    { m_set_bit_num += (val != 0); return m_flag->setByBit(bi, val); }

    /**
     * init func 
     * @param itm_num    column item number to mark
     */
    void init    (uint64_t itm_num);
    void uninit  (void) { this->clearAll(); m_buf->clear(); }

    void clearAll(void) { m_flag->clearAll(); m_set_bit_num = 0; }
    void setAll  (void)
    { m_flag->setAll(); m_set_bit_num = m_flag->getElementUsed(); }

    /** calc the set bit number by m_flag content */
    void calcSetBitByContent(void)
    {   m_set_bit_num = m_flag->getPopCount(); }
}; // BitMap



inline
BitMap::BitMap (uint32_t size)
{
    m_buf  = new Buffer(size);
    m_flag = new BoolVector();
} // ctor  

inline
BitMap::~BitMap(void)
{
    delete m_flag; m_flag = nullptr;
    delete m_buf ; m_buf  = nullptr;
} // dtor 

inline
void BitMap::init(uint64_t itm_num)
{
    bool realloc = true; 
    uint64_t byte_cap = Utility::calcBytesUsed(itm_num);
    const void * cont = m_buf->allocate(byte_cap, realloc);
    m_flag->init (itm_num, byte_cap, cont);
} // init

} // namespace