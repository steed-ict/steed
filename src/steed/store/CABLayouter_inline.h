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
 * @file CABLayouter_inline.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   CABLayouter inline functions 
 */

#pragma once 

namespace steed {


inline CABLayouter::
CABLayouter(Buffer *buf, Compressor::Type t) : m_mem_buf(buf), m_dsk_buf(buf) 
{
    m_cmp = CompressorFactory::create(t); 
    if (!m_cmp->noCompressBuf()) 
    {
        m_dsk_buf = new Buffer(s_buf_init_size);
        m_dsk_buf->initInMemory ();

        FileIO *fio = m_mem_buf->getFileIO();
        m_dsk_buf->setFileIO(fio);
    } // if 
} // ctor



inline CABLayouter::~CABLayouter(void)
{
    if (!m_cmp->noCompressBuf()) 
    {   delete m_dsk_buf;   }

    m_mem_buf = m_dsk_buf = nullptr;

    delete m_cmp; m_cmp = nullptr;
} // dtor



inline void CABLayouter::output2debug(void)
{
    printf("\nCABLayouter::output2Debug         \n");
    printf("\n----------------------------------\n");
    printf("\nMemory buffer @[%p] <<<\n", m_mem_buf);
    if (m_mem_buf != nullptr)
    { m_mem_buf->output2debug(); }

    printf("\nDisk   buffer @[%p] <<<\n", m_dsk_buf);
    if (m_dsk_buf != nullptr)
    { m_dsk_buf->output2debug(); }
    printf("\n----------------------------------\n");
} // output2Debug 



} // namespace steed
