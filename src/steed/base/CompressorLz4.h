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
 * @file CompressorLz4.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for CompressionLz4: 
 *
 * @Usage
 *    compress memory content using zlib4
 *    install the following package ($ sudo apt-get install ...):
 *        liblz4-1     - Fast LZ compression algorithm library - runtime
 *        liblz4-dev   - Fast LZ compression algorithm library - development files
 *    
 *    compilte with the lz4 lib:
 *        -llz4
 */

#pragma once 

#include "Compressor.h"
#include "lz4.h"

namespace steed {

class CompressorLz4 : public Compressor {
public: 
    CompressorLz4 (void) : Compressor(Compressor::lz4) {}
    ~CompressorLz4(void) = default;

public: 
    uint64_t compressBound(uint64_t s) override
    { return LZ4_compressBound(s); }

    int64_t  compress  (void *org, uint64_t org_use, void *cmp, uint64_t &cmp_use) override
    { return cmp_use = LZ4_compress_default((const char*)org, (char*)cmp, org_use, cmp_use); }

    int64_t  decompress(void *cmp, uint64_t cmp_use, void *org, uint64_t &org_use) override
    { return org_use = LZ4_decompress_safe ((const char*)cmp, (char*)org, cmp_use, org_use); }
}; // CompressorLz4

} // namespace steed  
