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
 * @file CABWriter_inline.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   CABWriter inline functions 
 */

#pragma once 

namespace steed {



inline
CABWriter::~CABWriter(void)
{
    flush(true);

    m_cur_info = nullptr;
    m_file_io  = nullptr;

    delete m_cont_buf; m_cont_buf = nullptr;
    delete m_layouter; m_layouter = nullptr;
    delete m_info_buf; m_info_buf = nullptr;
    delete m_cur_cab ; m_cur_cab  = nullptr;
} // dtor



inline
int CABWriter::prepareCAB2write(void)
{
    assert (m_cur_cab == nullptr);
    assert (m_recd_num % g_config.m_cab_recd_num == 0);

    // prepare CAB meta 
    m_cab_meta.m_buf->clear ();
    m_cab_meta.m_bva->uninit();

    // prepare CAB info  
    int retval = getInfo2Write();
    if (retval < 0)
    {   return retval;   }

    // prepare CAB ins 
    m_cur_cab = new CAB(&m_cab_meta, m_cur_info);
    retval    = m_cur_cab->init2write(m_recd_num);
    if (retval < 0)
    {
        printf("CABWriter: create CAB failed!\n");
        return retval; 
    } // if 

#if DEFINE_BLM
    // BloomFilter
    if ((m_use_bloom) && (m_bloom == nullptr))
    {
        // create m_bloom @ the first CAB 
        m_bloom = new BloomFilter(Config::s_cab_recd_num);
    } // if
#endif 

    return retval; 
} // prepareCAB2write



inline
int CABWriter::getInfo2Write(void) 
{ 
    m_cur_info = m_info_buf->getNextInfo2Write(); 
    if (m_cur_info == nullptr)
    {
        printf("CABWriter: get CAB header failed!\n");
        return -1;
    } // if 

    m_cur_info->m_rep_type = m_rept->type();
    m_cur_info->m_cmp_type = m_cmp_type;
    m_cur_info->m_file_off = m_file_off;

    m_cur_info->m_item_info.m_bgn_recd = m_recd_num; 

    initValueInfo( &(m_cur_info->m_value_info) );

    return 1;
} // getInfo2Write





inline
int CABWriter::initValueInfo  (ColumnValueInfo *info)
{
    info->m_has_min = false;
    info->m_has_max = false;

    // fixed size DataType only 
    DataType* dt = this->getDataType();
    if (dt->getDefSize() != 0)
    {
        dt->fillNull( &(info->m_min), 1);
        dt->fillNull( &(info->m_max), 1);
    } // if 

    return 0;
} // initValueInfo 



inline
int CABWriter::updateValueInfo(const void *bin, ColumnValueInfo *info)
{
    // fixed size DataType only 
    DataType* dt = m_cur_cab->getDataType();
    if (dt->getDefSize() == 0) { return 0; }

    // null OR less tham min 
    void *min = &(info->m_min);
    if (!(info->m_has_min) || (dt->compareLess(bin, min) > 0))
    {
        info->m_has_min = true;
        dt->copy(bin, min);
    } // if 

    // null OR less tham max 
    void *max = &(info->m_max);
    if (!(info->m_has_max) || (dt->compareGreater(bin, max) > 0))
    {
        info->m_has_max = true;
        dt->copy(bin, max); 
    } // if 

    return 0;
} // updateValueInfo 



inline
int CABWriter::mergeValueInfo(ColumnValueInfo *cab_info, ColumnValueInfo *file_info)
{
    // fixed size DataType only 
    DataType* dt = m_cur_cab->getDataType();
    if (dt->getDefSize() == 0) { return 0; }
 

    if (cab_info->m_has_min)
    {
        void *cab_min  = &(cab_info ->m_min);
        void *file_min = &(file_info->m_min);

        // null OR less tham file_min 
        if (!(file_info->m_has_min) || (dt->compareLess(cab_min, file_min) > 0))
        {
            file_info->m_has_min = true;
            dt->copy(cab_min, file_min);
        } // if 
    } // if  


    if (cab_info->m_has_max)
    {
        void *cab_max  = &(cab_info ->m_max);
        void *file_max = &(file_info->m_max);

        // null OR less tham file_max 
        if (!(file_info->m_has_max) || (dt->compareGreater(cab_max, file_max) > 0))
        {
            file_info->m_has_max = true;
            dt->copy(cab_max, file_max); 
        } // if 
    } // if 


    return 0;
} // mergeValueInfo



} // namespace steed
