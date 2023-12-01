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
 * @file JSONType.h
 * @author Zhiyi  Wang <zhiyiwang@ict.ac.cn>
 * @version 0.3
 * @section DESCRIPTION
 *     definitions and functions for JSON data types.
 */

#pragma once 

#include <stdint.h>

namespace steed {

class JSONType {
private:
    JSONType (void) = delete;
    ~JSONType(void) = delete;
 
public:
    static const uint8_t s_invalid = 0;
    static const uint8_t s_object  = 1;
    static const uint8_t s_array   = 2;
    static const uint8_t s_string  = 3;
    static const uint8_t s_number  = 4;
    static const uint8_t s_true    = 5;  
    static const uint8_t s_false   = 6; 
    static const uint8_t s_null    = 7;
    static const uint8_t s_max     = 8;

    static const uint8_t s_prim_bgn = 3; 
    static const uint8_t s_prim_end = 7; // exclude null: not valid type 

public:
    /**
     * get JSON data type using JSON text value
     * @param txt    text value content begin 
     * @return json type id
     */
    static uint8_t type(const char *txt);

    static bool isNull     (uint8_t t) { return (t == s_null  ); }
    static bool isArray    (uint8_t t) { return (t == s_array ); }
    static bool isObject   (uint8_t t) { return (t == s_object); }
    static bool isNumber   (uint8_t t) { return (t == s_number); }
    static bool isPrimitive(uint8_t t)
    { return ((t >= s_prim_bgn) && (t < s_prim_end)); }
}; // JSONType



inline
uint8_t JSONType::type(const char *txt)
{
    int got =  s_invalid;
    if (txt == nullptr)   { return s_null; }

    switch (*txt)
    {
        case '{':    got = s_object;  break;
        case '[':    got = s_array ;  break;
        case '"':    got = s_string;  break;
        case 't':    got = s_true  ;  break; 
        case 'f':    got = s_false ;  break;
        case 'n':    got = s_null  ;  break;

        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
        case '8': case '9': case '-':
                     got = s_number;  break;
        default:                      break;
    }
    return got;
} // type

} // namespace steed 
