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
 * @file RandomValues.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   random values in vector
 */

#pragma once

#include <algorithm> // std::sort
#include <vector>
#include "RandomGenerator.h"

namespace steed {
using std::vector;

class RandomValues {
protected:
    vector<uint64_t>  m_values{} ; /**< random values to read */

public:
    RandomValues (uint64_t num, uint64_t min, uint64_t max);
    ~RandomValues(void) = default; 

public:
    const vector<uint64_t> &values(void) const { return m_values; }
}; // RandomValues



inline RandomValues::
    RandomValues (uint64_t num, uint64_t min, uint64_t max) : m_values(num)
{
    RandomGenerator gen(min, max); 
    for (auto & v : m_values)
    {   v = gen.generate();   } 
    
    std::sort(m_values.begin(), m_values.end());
    m_values.erase( unique(m_values.begin(), m_values.end()), m_values.end() );
} // ctor 

} // namespace 
