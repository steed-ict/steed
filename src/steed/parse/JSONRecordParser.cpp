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
 * @file DataType.cpp
 * @author  Zhiyi Wang  <zhiyiwang@ict.ac.cn>
 * @version 0.4
 * @section DESCRIPTION
 *   definitions and functions for DataType.
 */

#include "JSONRecordParser.h"

namespace steed {

const char *JSONRecordParser::s_idx_str[s_elem_cap] = { nullptr };

int JSONRecordParser::s_init = initStatic();

int JSONRecordParser::initStatic (void)
{
    if (s_idx_str[0] != nullptr)
    {   return 1;   } 

    char buf[128] = {'\0'};
    for (uint32_t i = 0; i < s_elem_cap; ++i) 
    {
        snprintf (buf, 128, "\"%u\"", i);
        s_idx_str[i] = strdup(buf);
    } // for i

    return 1; 
} // initStatic


int JSONRecordParser::uninitStatic(void)
{
    for (uint32_t i = 0; i < s_elem_cap; ++i) 
    {
        free((void*)s_idx_str[i]);
        s_idx_str[i] = nullptr;
    } // for i

    return 0; 
} // uninitStatic

} // namespace steed 
