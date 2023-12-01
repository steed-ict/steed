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
 * @file Compressor.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for Compressor 
 */

#pragma once 

#include <stdint.h>

namespace steed {

class Compressor {
public:
    typedef uint16_t Type;

    /** compress type id */
    enum CompressType{
        none = 0,
        lz4  = 1,
    };

public:
    /** Compressor type id */
    const Type m_type{none};

public:
    Compressor(Type t) : m_type(t)    {}
    virtual ~Compressor(void) = default;

public:
    Type type         (void) const { return m_type; }
    bool noCompressBuf(void) const { return m_type == none; }

public:
    /**
     * get the compress bound (max bytes used)
     * @param s    original content size  
     * @return max compressed size 
     */
    virtual uint64_t compressBound(uint64_t s) = 0;

    /**
     * compress binary content 
     * @param org      original content begin 
     * @param org_use  original content used  
     * @param cmp      compress content begin 
     * @param cmp_use  compress content used, init as cap 
     * @return >0 compressed size; <= 0 failed
     */
    virtual int64_t compress(void *org, uint64_t org_use, void *cmp, 
        uint64_t &cmp_use) = 0;

public:
    /**
     * decompress the binary content 
     * @param cmp      decompress content begin
     * @param cmp_use  decompress content used
     * @param org      original  content begin
     * @param org_use  original  content used, init as cap 
     * @return >0 decompressed size; <= 0 failed
     */
    virtual int64_t decompress(void *cmp, uint64_t cmp_use, void *org,  
                uint64_t &org_use) = 0; 
}; // Compressor

} // namespace steed
