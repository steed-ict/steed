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
 * @file   BoolVector.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    definitions and functions for BoolVector
 */

#pragma once

#include "BitVector.h"

namespace steed {

class BoolVector : public BitVector {
protected:
    /** 
     * get first set bit index from table 
     * Usage:
     *   s_first_set_table [ Byte ] = first set bit index 
     *   the first set bit index begin from 0 in the byte 
     */
    static const uint32_t s_first_set_table[];

    /** 
     * get population count from table  
     * Usage:
     *   s_pop_count_table [ Byte ] = set bit number in this Byte
     *   the number of set bits in the byte 
     */
    static const uint32_t s_pop_count_table[];

    /** 
     * smear the bit to right 
     *   e.g: s_smear_table[5] = (1<<5)-1; <0000 1111>
     *
     * Usage:
     *   s_smear_table [ Index ] = smear value set by index  
     */
    static uint64_t s_smear_table[sizeof(uint64_t) << 3]; 

    /** 
     * init func: init the static var 
     * @return 0 success; <0 failed
     */
    static int init(void);
    static int s_init;  //! init flag


public:
    BoolVector (void) : BitVector(uint64_t(1)) {}
    ~BoolVector(void) = default; 

public:
    /** 
     * init func: all elements are used and then modified 
     * @param elnum    element number 
     * @param len      binary content length
     * @param bgn      binary contetn begin 
     * @return 0 success; <0 failed
     */
    int init (uint64_t elnum, uint64_t len, const void *bgn)
    { return BitVector::init2read(elnum, len, bgn); }

public:
    uint64_t get(uint64_t ei) { return getByBit(ei); }
    int set(uint64_t ei)      { return setByBit(ei, 1); }
    int set(uint64_t bi, bool val)  { return setByBit(bi, uint64_t(val)); }

    void setAll  (void)  { memset(m_cont, 0xff, getContCap()); }
    void clearAll(void)  { memset(m_cont, 0x00, getContCap()); }
    void flipAll (void);

    /**
     * calc bit and operate 
     * @param bi    bit index to calc and 
     * @param v     bool val to calc and result 
     */
    int calcBitAnd(uint64_t bi, bool val)
    { return val ? get(bi) : 0; }
  
    /**
     * calc bit or operate 
     * @param bi    bit index to calc and 
     * @param val   bool val to calc and result 
     */
    int calcBitOr (uint64_t bi, bool val)
    { return val ? 1 : get(bi); }

    /**
     * set bit by range [bgn, end)
     * @param bgn   begin bit index to set 
     * @param end   end   bit index 
     * @return 0 success; < 0 failed 
     */
    int setBitByRange  (uint64_t bgn, uint64_t end);

    /**
     * clear bit by range [bgn, end)
     * @param bgn   begin bit index to clear 
     * @param end   end   bit index 
     * @return 0 success; < 0 failed 
     */
    int clearBitByRange(uint64_t bgn, uint64_t end);

    /**
     * calc bits and in range [bgn, end)
     * @param bgn   bit index to calc and 
     * @param end   bit index to calc and 
     * @param val   bool val to calc and result 
     */
    bool calcAndByRange(uint64_t bgn, uint64_t end)
    { return ((end - bgn) == getPopCount(bgn, end)); }

    /**
     * calc bits or in range [bgn, end)
     * @param bgn   bit index to calc or 
     * @param end   bit index to calc or 
     * @param val   bool val to calc and result 
     */
    bool calcOrByRange(uint64_t bgn, uint64_t end)
    { return (getPopCount(bgn, end) > 0); }

    /**
     * get next set bit index after specific index (including the begin bit): 
     * @param bi    bit index as begin search 
     * @return next set index after the bit index 
     */
    uint64_t getNextSetBit(uint64_t bi = 0);

    /**
     * get next set bit index after specific index [bgn, end):
     * @param bgn    bit begin index for pop count
     * @param end    bit end   index for pop count
     * @return set bit number
     */
    uint64_t getPopCount (uint64_t bgn = 0, uint64_t end = UINT64_MAX);

public:
    /**
     * merge the vectors using or operator
     * @param v     source vector
     * @param val   bool val to calc and result 
     */
    int mergeOr (BoolVector *v);

    /**
     * merge the vectors using and operator
     * @param v     source vector
     * @param val   bool val to calc and result 
     */
    int mergeAnd(BoolVector *v);

    /**
     * check vectors have the same mask and number of used bits
     * @param v    source vector
     * @return 0 success; <0 failed 
     */
    int check   (BoolVector *v);
}; // BoolVector

} // namespace steed

#include "BoolVector_inline.h"