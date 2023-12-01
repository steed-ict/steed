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
 * @file   BitVector.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    definitions and functions for BitVector:
 *        fixed size bits for the values in vector
 * @see BitVector::Reference class
 */

#pragma once

#include <assert.h>
#include <stdint.h>
#include "Utility.h"

namespace steed {

class BitVector {
public: 
    class Reference {
    private: 
        BitVector&  m_bv;    /**< bonding instance */
        uint64_t    m_bi{0}; /**< begin bit index  */
    
    public:
        ~Reference(void) = default; 
        Reference (BitVector& bv, uint64_t ei) :
            m_bv(bv), m_bi(ei * bv.m_mask_size)
        {}
    
    public: 
        Reference& operator= (uint64_t val)
        { m_bv.setByBit(m_bi, val); return *this; }
    
        Reference& operator= (const Reference& r)
        { m_bv.setByBit(m_bi, r.m_bv.getByBit(r.m_bi)); return *this; }
    
        operator uint64_t () const 
        { return m_bv.getByBit(m_bi); }
    }; // Reference


protected: 
    // meta 
    uint64_t     m_mask      {0};     /**< value bit mask      */
    uint64_t     m_mask_size {0};     /**< mask bits used num  */

    // content  
    uint64_t    *m_cont{nullptr};     /**< content bgn pointer */ 
    uint64_t     m_bits_cap  {0};     /**< cap of bit in cont  */ 
    uint64_t     m_bits_used {0};     /**< used bits number    */
    uint64_t     m_elem_used {0};     /**< used elem number    */
    uint64_t     m_next_64bit{0};     /**< next 64 bits as uint*/

public: 
    /**
     * ctor to set the mem var 
     * @param msk_size   mask size foreach value
     */
    BitVector(uint64_t msk_size);
    virtual ~BitVector(void) = default;

public: 
    bool        operator==    (const BitVector& bv);
    Reference   operator[]    (uint64_t ei) { return Reference(*this, ei); }
    const void *getContent    (void)        { return m_cont         ; }
    uint64_t    getContCap    (void)        { return m_bits_cap >> 3; } // in bytes 

    uint64_t    getMaxValue   (void)        { return m_mask         ; }
    uint64_t    getMask       (void)        { return getMaxValue()  ; }
    uint64_t    getMaskSize   (void)        { return m_mask_size    ; }
    uint64_t    getElementUsed(void)        { return m_elem_used    ; }
    uint64_t    getElementCap (void)
    { return m_mask_size == 0 ? 0 : m_bits_cap / m_mask_size; }

    uint64_t    getUsedSize  (void)
    { return Utility::calcBytesUsed(m_bits_used); }
  
    int  resizeBitUsed  (uint64_t  bit_used);
    int  resizeElemUsed (uint64_t elem_used);
    void resizeCap(void* bgn, uint64_t bnum)
    { m_cont = (uint64_t*)bgn; m_bits_cap = (bnum * 8); }

public: 
    /*
     * init binary content to write
     * @param len    content length in bytes 
     * @param bgn    bit value array content begin
     * @return <0 failed; 0 success
     */
    int init2write(uint64_t len, void *bgn);

    /*
     * init binary content to read 
     * @param elnum    element number
     * @param len    content length in bytes 
     * @param bgn    bit value array content begin
     * @return <0 failed; 0 success
     */
    int init2read (uint64_t elnum, uint64_t len, const void *bgn);

    /**
     * copy binary content: after init2write called 
     * @param src    source bitvector to get content, src should init2read 
     * @return >=0 copy number; <0 failed  
     */
    int64_t copyContent(BitVector *src);

public:
    /**
     * set value by element index
     * @param ei     element index
     * @param val    set value 
     * @return 0 success; <0 failed
     */
    int set(uint64_t ei, uint64_t val)
    { return setByBit(ei * m_mask_size, val); }

    /**
     * get value by element index
     * @param ei  begin bit index 
     * @return value 
     */
    uint64_t get(uint64_t ei)
    { return getByBit(ei * m_mask_size); }

    /**
     * set value by bit index
     * @param bi     value's begin bit index
     * @param val    set value 
     * @return 0 success; <0 failed
     */
    int      setByBit(uint64_t bi, uint64_t val);
  
    /**
     * get value by bit index
     * @param bi  begin bit index 
     * @return value 
     */
    uint64_t getByBit(uint64_t bi);
   
protected:
    /**
     * get the uint64_t unit which containing the bit bi
     * @param bi     bit index to get
     * @param ubgn   uint bgn position
     * @param uidx   bit index in unit
     */
    void getBitUnit(uint64_t bi, uint64_t* &ubgn, uint64_t &uidx);

    /**
     * check whether the bit index is overflow
     * @param bi  begin bit index 
     * @return true or false 
     */
    bool bitIndexIsOverFlow(uint64_t bi) { return bi > m_bits_cap; }

public:
    /**
     * append an integer into the BitVector
     * @param val    integer value 
     * @return 0 success; <0 failed 
     */
    int   append       (uint64_t val);

protected:
    /**
     * append a zero value into the BitVector
     * @param val    integer value 
     * @return 0 success; <0 failed 
     */
    int   appendZero(void);

    /**
     * append non-value integer into the BitVector
     * @param val    integer value 
     * @return 0 success; <0 failed 
     */
    int   appendNotZero(uint64_t val);

public:
    void output2debug(void);
}; // BitVector

} // namespace steed



#include "BitVector_inline.h"
