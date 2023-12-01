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
 * @file Allocator.h
 * @author  Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    Memory management related funcs
 */

#pragma once


namespace steed {

/**
 * Steed wrapper around ::memalign()
 * If memalign() fails, reports error with stack trace and exit.
 */
void* steedMemalign(size_t align, size_t size);

/**
 * Steed wrapper around ::malloc()
 * If malloc() fails, reports error with stack trace and exit.
 */
void* steedMalloc(size_t size);

/**
 * Steed wrapper around ::realloc().
 * If realloc() fails, reports error with stack trace and exit.
 */
void* steedRealloc(void* ptr, size_t size);

/**
 * Steed wrapper around ::free().
 * Free the memory pointed by ptr.
 */

void steedFree(void* ptr);

} // namespace steed 
