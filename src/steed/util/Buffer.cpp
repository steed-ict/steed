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
 * @file Buffer.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *     Buffer related functions
 */

#include "Buffer.h"

namespace steed {

// must define a Config when using the buffer
extern steed::Config g_config;

Buffer::Buffer(uint32_t cap) : m_align(g_config.m_mem_align_size)
{
    m_cap = ((cap < m_align) ? m_align : Utility::calcAlignSize(cap, m_align));
    m_buffer = (char*)steedMalloc(m_cap);
} // constructor

} // namespace steed
