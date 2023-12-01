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
 * @file CABLayouter.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for CABLayouter 
 */

#pragma once 

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Config.h"
#include "Buffer.h"
#include "CompressorFactory.h"

#include "CAB.h"


namespace steed {

/**
 * CAB content:
 *   Memory(m_mem_buf): original   CAB content (rep + def + bin value array)
 *   Disk  (m_dsk_buf): compressed CAB content by Compressor 
 */
class CABLayouter {
protected:
    Compressor  *m_cmp    {nullptr}; /**< bin value compressor  */
    Buffer      *m_mem_buf{nullptr}; /**< bin content in memory */
    Buffer      *m_dsk_buf{nullptr}; /**< bin content on disk   */

    static const uint64_t s_buf_init_size;; /** buffer init size */

public:
    /**
     * ctor 
     * @param buf  buffer used to IO with file 
     * @param t    compress type 
     */
    CABLayouter (Buffer *buf, Compressor::Type t);
    ~CABLayouter(void);

public:
    void clear        (void)  { clearMemory(), clearDisk(); }
    void clearMemory  (void)  { m_mem_buf->clear(); }
    void clearDisk    (void)  { m_dsk_buf->clear(); }

    uint64_t getMemoryUsed(void)  { return m_mem_buf->used(); }
    uint64_t getDiskUsed  (void)  { return m_dsk_buf->used(); }
 

public: 
    /**
     * encoding memory content to disk layout and flush 2 file
     * @param tail    tail cab flag  
     * @param info    cab info to flush 
     * @param cab     cab ins  to flush 
     * @return >0 success, used bytes; <0 failed
     */
    int64_t flush(bool tail, CABInfo *info, CAB *cab);

    /**
     * load disk layout content from file and decode to memory layout 
     * @param info    cab info to flush 
     * @return >0 success, used bytes; <0 failed
     */
    int64_t load (CABInfo *info);

public: 
    /** print content to debug */
    void output2debug(void);
}; // CABLayouter

} // namespace steed



#include "CABLayouter_inline.h"
