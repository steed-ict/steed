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
 * @file CABAppender.cpp
 * @brief CABAppender class
 * @version 1.0
 * @section LICENSE
 * TBD
 * @section DESCRIPTION
 * CABAppender class
 */

#include "CABAppender.h"

namespace steed {

int CABAppender::init2append(const string &base, SchemaTree* tree, SchemaPath &path,
    uint64_t cap)
{
    if (CABOperator::init(tree, path, cap) < 0)
    {
        printf("CABAppender: init base 2 append failed!\n");
        return -1;
    } // init


    // CAB content file
    string cab_bin(base);
    cab_bin.append(".cab");
    m_cont_buf = new Buffer(0); // buffer to read and modify 
    if (m_cont_buf->init2modify(cab_bin) < 0) // init as InMemory
    {
        printf("CABAppender: init Content Buffer 2 modify for append failed!\n");
        return -1;
    } // if 
    m_file_io = m_cont_buf->getFileIO();
    m_layouter = new CABLayouter(m_cont_buf, m_cmp_type);


    // CAB info file
    string cab_info(cab_bin);
    cab_info.append(".info");
    m_info_buf = new CABInfoBuffer();
    if  (m_info_buf->init2append(cab_info) < 0)
    {
        printf("CABAAppender: init Info Buffer 2 append for append failed!\n");
        return -1; 
    } // if 


    // check tail CAB is full or not
    m_cur_info = m_info_buf->getTailInfo2Append();
    m_recd_num = m_cur_info->getBeginRecdID() + m_cur_info->getRecordNum();
    m_file_off = m_cur_info->m_file_off;

    uint32_t tail_cab_recd_num = m_cur_info->getRecordNum();
    bool cab_is_full = ((tail_cab_recd_num % m_cab_meta.m_recd_cap) == 0);
    if  (cab_is_full) 
    {
        // tail CAB is full:
        // seek to the end of CAB content file
        m_file_off += m_cur_info->m_strg_size;
        m_file_io->seekContent(m_file_off, SEEK_SET);

        if (prepareCAB2write() < 0)
        {
            printf("CABAppender: prepare CAB 2 write failed!\n");
            return -1;
        } // if 

        return 0;
    } 
    else
    {
        // tail CAB is not full, load the tail CAB content
        if (m_file_io->seekContent(m_file_off, SEEK_SET) == (uint64_t)-1)
        {
            printf("CABReader: seek CAB content failed!\n");
            return -1;
        } // if 

        m_layouter->clear();
        if (m_layouter->load(m_cur_info) < 0)
        {
            printf("CABReader::  load CAB content failed!\n");
            return -1;
        } // if 

        CABInfo info = *m_cur_info;
        CABMeta meta = m_cab_meta;
        int buf_used = m_cont_buf->used();
        Buffer *buf = new Buffer();
        buf->initInMemory();
        void* dstbgn = buf->allocate(buf_used, false);
        void* srcbgn = m_cont_buf->getPosition(0);
        memcpy(dstbgn, srcbgn, buf_used);
        m_layouter->clear(); // m_layouter->m_mem_buf is m_cont_buf (@ init)

        BinaryValueArray *bva = BinaryValueArray::create(buf, meta.m_dt);
        meta.m_buf = buf;
        meta.m_bva = bva;

        CABItemInfo::Type type = m_cur_info->getType();
        CAB *tail_cab = new CAB(&meta, &info, buf);
        int s = tail_cab->init2read(type);
        if (s < 0)
        {
            puts("CABReader: init to read next CAB failed!");
            return -1;
        } // if 

        m_cur_cab = new CAB(&m_cab_meta, m_cur_info);
        m_cur_cab->init2write(m_cur_info->m_item_info.m_bgn_recd);
        m_cur_cab->copyContent(tail_cab);

        delete tail_cab; tail_cab = nullptr;
        delete buf; buf = nullptr;
        delete bva; bva = nullptr;

        // overwrite the tail CAB binary content 
        m_file_io->seekContent(m_file_off, SEEK_SET);
    } // if 

    return 0;
} // init2append


} // namespace steed