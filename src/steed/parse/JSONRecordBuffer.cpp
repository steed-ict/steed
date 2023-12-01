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
 * @file JSONRecordBuffer.cpp
 * @brief JSONRecordBuffer class implementation
 * @version 1.0
 * @section DESCRIPTION
 *   This file implements the JSONRecordBuffer class.
 */

#include "JSONRecordBuffer.h"

namespace steed {

const uint32_t JSONRecordBuffer::s_invalid_offset = UINT32_MAX; 


void JSONRecordBuffer::clearOffsetArray(void)
{
    m_offset_array.fill(JSONRecordBuffer::s_invalid_offset);
    m_elem_idx = 0, m_elem_used = 0;
} // clearOffsetArray


} // namespace steed