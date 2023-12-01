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
 * @file InStreamSeeker.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   seek char in std::istream
 */

#pragma once

#include <assert.h>
#include <fstream>
#include <iostream>

#include "Config.h"

namespace steed {

extern Config g_config;
using std::istream;

class InStreamSeeker {
public:
    static const char s_lf{0x0A}; // LF (NL line feed, new line)

private:
    InStreamSeeker (void) = delete;
    ~InStreamSeeker(void) = delete;

public:
    /**
     * seek to the next line  
     * move to the offset in istream, then read until new line 
     * @param is     input stream 
     * @param off    offset before the next line 
     * @return 0 success; -1 failed
     */
    static void seek2NextLine(istream *is, uint64_t off);
}; // InStreamSeeker



inline
void InStreamSeeker::seek2NextLine(istream *is, uint64_t off)
{
    is->seekg(off); 
    
    uint64_t rmax = g_config.m_text_recd_avg_len * 2;
    char     delm = s_lf; 
    is->ignore(rmax, delm);
} // seek2NextLine

} // namespace
