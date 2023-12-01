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
 * @file JSONRecordParser.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 * JSON text record parser: parse json record to json element
 */

#pragma once 

#include "JSONType.h"
#include "JSONBinTree.h"


namespace steed {

class JSONRecordParser {
public:
    JSONRecordParser         (void) = default;
    virtual ~JSONRecordParser(void) = default;

public:
    /**
     * parse one json text record to JSONBinTree
     * @param bt    json binary tree 
     * @param c     json text record begin  
     * @return >0 success; 0 EOF; <0 failed 
     */
    virtual int parse(JSONBinTree* &bt, char* &c) = 0;

public:
    // treat array as anonymous object:
    //     use Element Index as field name string   
    static const uint32_t s_elem_cap = 4096;
    static const char    *s_idx_str[s_elem_cap];
    static int   s_init; // = 1 after initialization
    static int   initStatic  (void);
    static int   uninitStatic(void);
}; // JSONRecordParser

} // namespace steed
