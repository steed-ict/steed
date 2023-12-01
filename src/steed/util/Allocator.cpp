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
 * @file Allocator.cpp
 * @author  Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    Memory management related funcs 
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "Allocator.h"
#include "DebugInfo.h"

namespace steed {

void* steedMemalign(size_t align, size_t size)
{
    void* x = memalign(align, size);
    if (x == nullptr)
    {
        puts ("steedMemalign: failed!");
        DebugInfo::printStackAndExit();
    }

    return x;
} // steedMemalign


void* steedMalloc(size_t size)
{
    void* x = malloc(size);
    if (x == nullptr)
    {
        puts ("steedMalloc: failed!");
        DebugInfo::printStackAndExit();
    }

    memset(x, 0x00, size);
    return x;
} // steedMalloc


void* steedRealloc(void* ptr, size_t size)
{
    void* x = realloc(ptr, size);
    if (x == nullptr)    
    {
        printf("steedRealloc: failed!");
        DebugInfo::printStackAndExit();
    }

    return x;
} // steedRealloc 


void steedFree(void* ptr)
{
    free(ptr);
} // steedFree

} // namespace steed
