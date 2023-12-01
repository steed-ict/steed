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
 * @file CompressorNone.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for CompressorNone:
 *     no compress, just copy the content
 */

#pragma once 

#include <string.h>
#include "Compressor.h"

namespace steed {

class CompressorNone : public Compressor {
public: 
    CompressorNone (void) : Compressor(Compressor::none) {}
    ~CompressorNone(void) = default;

public: 
    uint64_t compressBound(uint64_t s) override { return s; }
    int64_t  compress  (void *org, uint64_t org_use, void *cmp, uint64_t &cmp_use) override;
    int64_t  decompress(void *cmp, uint64_t cmp_use, void *org, uint64_t &org_use) override;
}; // CompressorNone



inline
int64_t CompressorNone::
    compress(void *org, uint64_t org_use, void *cmp, uint64_t &cmp_use) 
{
    if (org != cmp)
    {   memcpy(cmp, org, org_use);   }

    return int64_t(cmp_use = org_use);
} // compress



inline
int64_t CompressorNone::
    decompress(void *cmp, uint64_t cmp_use, void *org, uint64_t &org_use) 
{
    if (cmp != org)
    {   memcpy(org, cmp, cmp_use);   }

    return int64_t(org_use = cmp_use);
} // decompress

} // namespace steed  
