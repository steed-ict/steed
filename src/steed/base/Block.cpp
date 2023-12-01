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
 * @file Block.cpp
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    Definitions and functions for load2Buffer:
 *    read Block into Buffer
 */

#include "Block.h"

namespace steed {

int block::load2Buffer(Buffer *buf)
{
    // load conent to buffer
    bool     resize = true;
    uint64_t org_use= buf->used  (); // buf used before load 
    uint64_t blk_sz = sizeof(Block); // block size 
    uint64_t rd_num = blk_sz; 
    int  rd_got = buf->load2Buffer(rd_num, resize);
    if  (rd_got <= 0)
    {
        puts ("STEED: load Block size failed!");
        return rd_got;
    } // if 

    Block  *blk = nullptr;
    blk  = (Block*)buf->getPosition(org_use);
    rd_num = blk->m_size - blk_sz; // already read blk_sz bytes
    rd_got = buf->load2Buffer(rd_num, resize); 
    if (rd_got == 0)
    {   return 0;   } // EOF
    else if (rd_got < 0)
    {
        puts ("STEED: load Block content failed!");
        return rd_got;
    } // if 

    blk  = (Block*)buf->getPosition(org_use);
    return int(blk->m_size);
} // load2Buffer

} // namespace steed
