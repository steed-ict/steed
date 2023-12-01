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
 * @file Row.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   Binary row member definition 
 */

#pragma once

#include <stdint.h>
#include <stdio.h>

#include "Row.h"

namespace steed {
namespace Row {

/**
 * Member info: member number in this content
 */
class Info {
/**
 * Member info, uint32_t:
 *     XXBB BBBB BBBB BBBB BBBB BBBB BBBB BBBB
 * 
 * X : offset used bytes flag 
 *     00 invalid     01 offset_8
 *     10 offset_16   11 offset_32
 *
 * B : member number in this content
 */
protected:
    uint32_t    m_info{0};  /**< Info content */

public:
    Info (const Info &i) = default; 
    Info (void) = default;
    ~Info(void) = default;
    Info &operator=(Info& mi)  { m_info = mi.m_info; return *this; }
 
public:
    /** number of bits for elem's number */
    const static uint32_t s_bit4num  = 30; 
    const static uint32_t s_off_mask = uint32_t(3) << s_bit4num; 
    const static uint32_t s_num_mask =(uint32_t(1) << s_bit4num) - 1; 

    /** offset length flag */ 
    const static uint32_t s_invalid  = 0;
    const static uint32_t s_flag_8   = 1; 
    const static uint32_t s_flag_16  = 2; 
    const static uint32_t s_flag_32  = 3; 
    const static uint32_t s_flag_num = 4;

    /** offset length encoding in binary row content */ 
    const static uint32_t s_off_8  = (s_flag_8  << s_bit4num); 
    const static uint32_t s_off_16 = (s_flag_16 << s_bit4num); 
    const static uint32_t s_off_32 = (s_flag_32 << s_bit4num); 

public: // read 
    bool     isOffset8 (void)  { return getType() == s_off_8 ; }
    bool     isOffset16(void)  { return getType() == s_off_16; }
    bool     isOffset32(void)  { return getType() == s_off_32; }

    void     clear     (void)  { m_info = 0; }
    uint32_t getNumber (void)  { return m_info & s_num_mask   ; }
    uint32_t getFlag   (void)  { return getType() >> s_bit4num; }
    uint32_t getOffsetArrayUsed(void);

private: // only for internal use
    uint32_t getType   (void)  { return m_info & s_off_mask; }

public: // write 
    void     set      (uint32_t flag , uint32_t num);
    void     setFlag  (uint32_t flag);
    void     setNumber(uint32_t num );

public:
    /**
     * use value used and elem number to decided offset flag 
     * NOTE:
     *   this calculation ONLY need to consider the sum of all values' length 
     * 
     * @param vlen    total used of binary values (ONLY THE VALUES)
     * @return offset length flag
     */ 
    static uint32_t calcFlag(uint32_t vlen);

    /**
     * get offset bytes used via flag
     * @param flag    offset flag 
     * @return offset used  
     */ 
    static uint32_t getOffsetUsed(uint32_t flag);

    /**
     * get offset array used 
     * @param flag    offset flag got by calcFlag 
     * @param mnum    used element number 
     * @return offset array used
     */ 
    static uint32_t getOffsetArrayUsed (uint32_t flag, uint32_t mnum);

    /**
     * get Info size 
     * @return info content used
     */ 
    static uint32_t getInfoUsed(void);

    /**
     * get flag from binary content 
     * @param  bin    binary row struct content
     * @return offset flag 
     */ 
    static uint32_t getFlag(uint32_t *bin)
    { return (*bin & s_off_mask) >> s_bit4num; }

public:
    void output2debug(void)
    { printf("Row::Info: flag:[%u] num:[%u]\n", getFlag(), getNumber()); } 
}; // Info





inline
uint32_t Info::getOffsetArrayUsed(void)
{
    uint32_t num = getNumber();
    uint32_t flg = getFlag  ();
    uint32_t len = Info::getOffsetUsed(flg);
    return  (len * num);
} // getOffsetArrayUsed


inline
void Info::set(uint32_t flag, uint32_t num)
{
    uint32_t prefix  = (flag << s_bit4num);
    uint32_t postfix = (num  & s_num_mask);
    m_info = postfix | prefix;
} // set


inline
void Info::setFlag(uint32_t flag)
{
    uint32_t prefix  = (flag << s_bit4num);
    uint32_t postfix = getNumber();
    m_info = prefix | postfix;
} // setFlag


inline
void Info::setNumber(uint32_t num)
{
    uint32_t prefix  = getType();
    uint32_t postfix = (num & s_num_mask);
    m_info = prefix | postfix;
} // setNumber


inline
uint32_t Info::calcFlag(uint32_t vlen)
{
    // Offset encodes the begin position for each element:
    // ONLY NEED to consider the sum of all values' length 

    uint32_t got = s_invalid;
    if      (vlen <= UINT8_MAX ) { got = s_flag_8 ; }
    else if (vlen <= UINT16_MAX) { got = s_flag_16; }
    else if (vlen <  UINT32_MAX) { got = s_flag_32; } 
    return   got;
} // calcFlag


inline
uint32_t Info::getOffsetUsed(uint32_t flag)
{   return (1 << (flag - 1));   }


inline
uint32_t Info::getOffsetArrayUsed(uint32_t flag, uint32_t mnum)
{
    uint32_t size = Info::getOffsetUsed(flag); // offset size
    return   size * mnum; 
} // getOffsetArrayUsed 


inline
uint32_t Info::getInfoUsed(void)
{   return sizeof(Info);   }


} // namespace Row
} // namespace steed
