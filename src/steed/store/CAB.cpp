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
 * @file CAB.cpp
 * @author  Zhiyi Wang  <zhiyiwang@ict.ac.cn>
 * @version 0.4
 * @section DESCRIPTION
 *   definitions and functions for CAB
 */

#include "CAB.h"

namespace steed {


int64_t CAB::merge2Buffer(Buffer *mgr_buf, bool tail)
{
    int64_t used = 0, total = 0;
    m_info->m_item_info = m_item_info; // flush CAB Info  
    CABItemInfo::Type type = m_item_info.getType(); 

    // trivial CAB output nothing 
    if ((type == CABItemInfo::trivial) && (!tail))
    {   return total;   }

    // all null CAB output rep + def
    {
        used = mergeSegment(mgr_buf, &ColumnItemArray::appendRep);
        if (used < 0)
        {
            printf("CAB: merge2Buffer appendRep failed!\n");
            return used;
        }
        total += used;
        
        used = mergeSegment(mgr_buf, &ColumnItemArray::appendDef);
        if (used < 0)
        {
            printf("CAB: merge2Buffer appendDef failed!\n");
            return used;
        }
        total += used;
    }
    if ((type == CABItemInfo::allnull) && (!tail))
    {   return total;   }
    
    // crucial CAB output full content:
    // rep + def + [offset array] + bin value array  
    {
        used = mergeSegment(mgr_buf, &ColumnItemArray::appendOffsets);
        if (used < 0)
        {
            printf("CAB: merge2Buffer appendOffsets failed!\n");
            return used;
        }
        total += used;
        
        used = mergeSegment(mgr_buf, &ColumnItemArray::appendValues);
        if (used < 0)
        {
            printf("CAB: merge2Buffer appendValues failed!\n");
            return used;
        }
        total += used;
    }
    return total;
} // merge2Buffer





int64_t CAB::mergeSegment(Buffer *mgr_buf, ColumnItemArray::AppendFunc func)
{
    int64_t used = 0, total = 0;
    ColumnItemArray *mj_cia = m_major_unit->m_cia;
    used = (mj_cia->*func)(mgr_buf);
    if (used < 0)
    {
        printf("CAB: merge major failed !\n");
        return -1;
    } 
    total += used;

    for (auto & unit : m_minor_units)
    {
        ColumnItemArray *mn_cia = unit->m_cia;
        used = (mn_cia->*func)(mgr_buf);
        if (used < 0)
        {
            printf("CAB: merge major failed !\n");
            return -1;
        } 
        total += used;
    } // for 

    return total;
} // mergeSegment





void CAB::output2debug(void)
{
    puts("\n\n\nCAB:");
    
    m_item_info.output2debug();
    
    printf("CABMeta:[%p]\n", m_meta);
    m_meta->output2debug();
    puts("----------------------------------------");
    
    printf("Info:[%p]\n", m_info);
    m_info->output2debug();
    puts("----------------------------------------");


    printf("major uint:[%p]\n", m_major_unit);
    m_major_unit->output2debug();
    puts("----------------------------------------");
    
    uint32_t mn_cnt = 0;
    printf("minor uint:[%lu]\n", m_minor_units.size());
    for (auto & mu : m_minor_units)
    {
        printf("<<%u>>\n", mn_cnt);
        mu->output2debug();
        puts("----------------------------------------");
    }
    
    puts("========================================\n\n\n");
} // output2debug



} // namespace 
