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
 * @file CABMeta.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *     definitions and functions for CABMeta
 */

#pragma once 

#include <stdio.h>
#include <vector>

#include "Buffer.h"
#include "DataType.h"
#include "BinaryValueArray.h"


namespace steed {

/**
 * CAB Meta is the meta data of a CAB record.
 * These attributes are used to describe the CAB record 
 *   and DOES NOT change for different CABs.
 */
class CABMeta {
public:
    DataType          *m_dt {nullptr}; /**< type instance   */
    Buffer            *m_buf{nullptr}; /**< m_major_unit buffer*/
    BinaryValueArray  *m_bva{nullptr}; /**< bin value array */
    uint64_t           m_recd_cap {0}; /**< CAB record cap  */
    uint32_t           m_max_rep  {0}; /**< max rep value   */
    uint32_t           m_max_def  {0}; /**< max def value   */

public:
    void reset(void)
    { m_buf->clear(); m_bva->uninit(); }

public:
    void output2debug(void);
}; // CABMeta



inline
void CABMeta::output2debug(void)
{
    printf("Meta::m_recd_cap[%lu] m_rep[%u] m_def[%u]\n", m_recd_cap, m_max_rep, m_max_def);
    printf("Meta::m_buf @[%p]\n", m_buf); m_buf->output2debug();
    printf("Meta::m_date [%s]\n", DataType::s_type_desc[m_dt->getTypeID()].name);
    printf("Meta::m_bva @[%p]\n", m_bva); m_bva->output2debug();
    puts  ("\n");
} // output2debug
    
}; // namespace steed
