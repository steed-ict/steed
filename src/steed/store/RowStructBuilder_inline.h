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
 * @file RowStruct_inline.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   Binary row struct layout definition 
 */

#pragma once

namespace steed {
namespace Row {


inline
RowObjectBuilder::~RowObjectBuilder(void)
{
    this->clear(); m_buf = nullptr;
    delete  m_obj; m_obj = nullptr;
} // dtor


inline
void RowObjectBuilder::clear(void)
{
    m_obj->uninit(); 
    m_ids .clear (); m_offs.clear();
    m_vlen = 0, m_init = false;
} // clear 


inline
int  RowObjectBuilder::init(void)
{
    m_init = true;
    char   *bgn = (char*)m_buf->allocate(sizeof(Row::Size), false);
    return (bgn == nullptr) ? -1 : (m_obj->init2write(bgn), 0);
} // init


inline
int  RowObjectBuilder::init(uint32_t bgn)
{
    uint32_t len = 0; // overwrite length 
    char    *bin = (char*)m_buf->getPosition(bgn);
    m_obj->init2append(bin, m_ids, m_offs, m_vlen, len);
    m_buf->deallocate (len);
    m_init = true; 
    return 0;
} // init


inline
uint32_t RowObjectBuilder::package(void)
{
    uint32_t mnum     = m_offs.size();
    uint32_t id_use   = mnum * sizeof(Row::ID);
    uint32_t flag     = Row::Info::calcFlag(m_vlen); 
    uint32_t off_use  = Row::Info::getOffsetArrayUsed(flag, mnum);
    uint32_t info_use = Row::Info::getInfoUsed();
    uint32_t app_size = id_use + off_use + info_use;
    return  (m_buf->allocate(app_size, false) == nullptr) ?
        uint32_t(-1) : m_obj->package(flag, m_vlen, m_ids, m_offs);
} // package





inline
RowArrayBuilder::~RowArrayBuilder(void)
{
    this->clear(); m_buf = nullptr;
    delete  m_arr; m_arr = nullptr;
} // dtor


inline
void RowArrayBuilder::clear(void)
{
    m_arr->uninit();
    m_offs.clear ();
    m_vlen = 0, m_init = false;
} // clear


inline
int RowArrayBuilder::init(void)
{
    m_init = true;
    char   *bgn = (char*)m_buf->allocate(sizeof(Row::Size), false);
    return (bgn == nullptr) ? -1 : (m_arr->init2write(bgn), 0);
} // init 


inline
uint32_t RowArrayBuilder::package(void)
{
    uint32_t mnum     = m_offs.size();
    uint32_t flag     = Row::Info::calcFlag(m_vlen); 
    uint32_t off_use  = Row::Info::getOffsetArrayUsed(flag, mnum);
    uint32_t info_use = Row::Info::getInfoUsed();
    uint32_t app_size = off_use + info_use;
    return (m_buf->allocate(app_size, false) == nullptr) ?
            uint32_t(-1) : m_arr->package(flag, m_vlen, m_offs);
} // package


} // namespace Row
} // namespace steed