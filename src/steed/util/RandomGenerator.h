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
 * @file RandomGenerator.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   random value generator definition 
 */

#pragma once

#include <stdint.h>
#include <random>

namespace steed {

class RandomGenerator {
protected:
    std::random_device                      m_dev; /**< seed with a real random value*/
    std::default_random_engine              m_eng; /**< random value engine instance */
    std::uniform_int_distribution<uint64_t> m_dis; /**< uniform random distribution  */

public:
    ~RandomGenerator(void)  = default;
    RandomGenerator (uint64_t min, uint64_t max): 
        m_dev(), m_eng(m_dev()), m_dis(min, max) {}
   
public:
    /**
     * generate a random value
     */ 
    uint64_t generate(void) { return m_dis(m_eng); }
}; // RandomGenerator

} // steed