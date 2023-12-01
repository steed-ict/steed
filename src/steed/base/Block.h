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
 * @file Block.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    Definitions and functions for Block:
 *    Binary content for storage, can be read and writen in Buffer.
 *    Each Block should be on a CONTINUOUS space 
 */

#pragma once

#include <stdint.h>

#include "Buffer.h" 

namespace steed {
namespace block {

typedef struct Block 
{
    uint64_t  m_size;     /**< block content size */  
    char      m_cont[0];  /**< var length content */ 
} Block;

/**
 * @brief load Block into Buffer
 * @param buf Buffer to load Block
 * @return 0 for EOF, -1 for error, >0 for bytes loaded
 */
int load2Buffer(Buffer *buf);

} // namespace block

} // namespace steed