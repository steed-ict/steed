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
 * @file CABLayouter.cpp
 * @author  Zhiyi Wang  <zhiyiwang@ict.ac.cn>
 * @version 0.4
 * @section DESCRIPTION
 *   definitions and functions for CABLayouter
 */

#include "CABLayouter.h"


namespace steed {

extern Config g_config;

/** buffer init size */
const uint64_t CABLayouter::s_buf_init_size = 1024 * 1024; 


int64_t CABLayouter::
    flush(bool tail, CABInfo *info, CAB *cab)
{
    // calc merged CAB binary content to m_mem_buf
    uint64_t mem_size = cab->getMergedUsed(tail);
    bool trivial = (mem_size == 0);
    if  (trivial)
    {
        // trivial cab flush nothing 
        info->m_strg_size = 0;
        info->m_dsk_size  = 0;
        info->m_mem_size  = 0;
        return 0;
    } // if 


    // CAB merge its content 
    m_mem_buf->reserve(mem_size);
    int64_t merged_used = cab->merge2Buffer(m_mem_buf, tail);
    if (mem_size != uint64_t(merged_used))
    {
        puts ("CABLayouter:: CAB merge2Buffer failed");
        abort();
        return -1;
    } // if 


    // TODO:
    //  put the following if statements into a function of class Compressor
    // int compressBuffer(Buffer *org, Buffer *cmp);

    // encoding m_mem_buf to m_dsk_buf 
    uint64_t dsk_size = 0;  
    uint32_t max_used = m_cmp->compressBound(mem_size); // max bound
    m_dsk_buf->reserve(max_used);

    // encode CAB Layout by Compressor 
    if (m_cmp->m_type == Compressor::none) 
    {
        assert(m_mem_buf == m_dsk_buf);

        // no compression & no need to copy
        dsk_size = mem_size;
    }
    else
    {
        uint32_t cab_bgn_offset = 0;
        void *mem_cab = m_mem_buf->getPosition(cab_bgn_offset); // org cont
        void *dsk_cab = m_dsk_buf->getPosition(cab_bgn_offset); // cmp cont
        if (m_cmp->compress(mem_cab, mem_size, dsk_cab, dsk_size) <= 0)
        {
            printf("CABLayouter: reserve disk buffer failed!\n");
            return -1;
        } // if 

        // m_cmp->compress update dsk_size 
        void  *got =  m_dsk_buf->allocate(dsk_size, false);
        if ((got == nullptr) || (got != dsk_cab))
        {
            printf("CABLayouter: reserve disk buffer failed!\n");
            return -1;
        }
    } // if 

    // update cab info 
    info->m_strg_size = Utility::calcAlignSize(dsk_size, g_config.m_mem_align_size);
    info->m_dsk_size  = dsk_size; 
    info->m_mem_size  = mem_size; 

    // flush all content in m_dsk_buf to file 
    int64_t flushed = m_dsk_buf->flush2File(); 
    assert(dsk_size == uint64_t(flushed));
    return int64_t(flushed);
} // flush





int64_t CABLayouter::load(CABInfo *info)
{
    // get cab info 
    uint64_t dsk_size = info->m_dsk_size;
    uint64_t mem_size = info->m_mem_size;

    // trivial cab has no content
    if (info->m_strg_size == 0) { return 0; }

    // load m_dsk_buf from file and prepare m_mem_buf 
    int got = m_dsk_buf->load2Buffer(dsk_size, true);
    if (uint32_t(got) != dsk_size)
    {
        puts("CABLayouter:: load disk content failed!");
        return -1;
    } // if

    if (m_mem_buf->reserve(mem_size) < 0)
    {
        puts("CABLayouter: reserve memory buffer failed!");
        return -1;
    } // if 


    // TODO:
    //  put the following if statements into a function of class Compressor
    // int compressBuffer(Buffer *org, Buffer *cmp);

    // decode the compressed CAB binary content 
    if (m_cmp->m_type != Compressor::none) 
    {
        assert(m_dsk_buf != m_mem_buf);

        // decompress 
        uint32_t cab_bgn_offset = 0;
        void   *dsk_cab = m_dsk_buf->getPosition(cab_bgn_offset); // cmp cont 
        void   *mem_cab = m_mem_buf->allocate   (mem_size, false); // org cont 
        int64_t got = m_cmp->decompress(dsk_cab, dsk_size, mem_cab, mem_size); 
        if  (got <= 0)
        {
            printf("CABLayouter: reserve disk buffer failed!\n");
            return -1;
        } // if 
    } // if 

    return mem_size; 
} // load 



} // namespace
