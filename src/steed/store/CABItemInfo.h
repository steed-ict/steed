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
 * @file CABItemInfo.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *     definitions and functions for CABItemInfo
 */

#pragma once 

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

namespace steed {

class CABItemInfo {
public:
    uint64_t  m_bgn_recd{0}; /**< begin record id */  
    uint32_t  m_recd_num{0}; /**< CAB record num  */
    uint32_t  m_item_num{0}; /**< column item num */    
    uint32_t  m_null_num{0}; /**< null    Item num: <r:d:null> */
    uint32_t  m_triv_num{0}; /**< trivial Item num: <0:0:null>, trivial is also null */

public: 
    typedef uint8_t Type; 
    enum CABItemInfoType {
        crucial = 0, /**< some items got bin value: R+D+V  */
        allnull = 1, /**< all  items are bin null : R+D    */
        trivial = 2, /**< all  items are trivial  : (empty)*/
    }; //

//    static  const uint8_t s_crucial = 0; /**< some items got bin value: R+D+V  */
//    static  const uint8_t s_allnull = 1; /**< all  items are bin null : R+D    */
//    static  const uint8_t s_trivial = 2; /**< all  items are trivial  : (empty)*/


public:
    ~CABItemInfo(void) = default;
    CABItemInfo (void) = default;

public:
    /** get CAB items type by info */
    Type getType(void);

    /**
     * update item info 
     * @param rep      ColumnItem rep value 
     * @param def      ColumnItem def value 
     * @param max_def  column max def value
     */ 
    void update (uint32_t rep, uint32_t def, uint32_t max_def);

public:
    void output2debug(void);
}; // CABItemInfo





inline
CABItemInfo::Type CABItemInfo::getType(void)
{
    assert  (m_null_num <= m_item_num);
    assert  (m_triv_num <= m_null_num);

    uint8_t isnull = (m_null_num == m_item_num);
    uint8_t istriv = (m_triv_num == m_item_num);
    return  isnull + istriv;
} // getType



inline
void CABItemInfo::update(uint32_t rep, uint32_t def, uint32_t max_def)
{
    assert(rep <= def);

    ++m_item_num; 
    m_recd_num += (rep == 0); 
    m_null_num += (def <  max_def);
    m_triv_num +=((def == 0) && (rep == 0));
} // update



inline
void CABItemInfo::output2debug(void)
{
    printf("CABItemInfo@[%p]\n", this);
    printf("Record: bgn[%lu] num[%u]\n", m_bgn_recd, m_recd_num);
    printf("Item: total[%u] null[%u] triv[%u]\n",
                m_item_num, m_null_num, m_triv_num);
} // output2debug



} // namespace 