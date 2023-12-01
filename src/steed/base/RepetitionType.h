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
 * @file RepetitionType.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for RepetitionType  
 */

#include <stdint.h>

#pragma once 

namespace steed {

class RepetitionType {
public:
    typedef enum Type {
        none    = 0, // none   rept
        single  = 1, // single rept
        multi   = 2, // multi  rept
        max     = 3, // maximum type id, invalid
        invalid = uint8_t(-1)
    } Type; // enum Type

protected:
    uint8_t m_type{invalid}; /**< rept type */

public:
    RepetitionType(uint8_t tp) : m_type(tp) {}
    virtual ~RepetitionType(void) = default;  

public:
    /** get type id */
    uint8_t type    (void) { return m_type; }

    /** simple path */
    uint8_t isSimple(void) { return m_type <= single; }

    /**
     * get max rept value in bits 
     * @param rl rept level
     * @return
     */
    virtual uint32_t getReptBits(uint32_t rl) = 0; 

public:
    /**
     * encode the value to binary 
     * @param val   value full content 
     * @return encode bin content  
     */
    virtual uint32_t encode(uint32_t val) = 0;

    /**
     * decode the binary to value 
     * @param bin   binary content 
     * @return encode bin content  
     */
    virtual uint32_t decode(uint32_t bin) = 0;

public:
    /**
     * create a RepetitionType object
     * @param tp    type id
     * @return RepetitionType object
     */
    static RepetitionType *create(uint8_t tp);
}; // RepetitionType



/**
 * repeated at none level 
 * all rept values are 0 
 */
class RepeatNone : public RepetitionType {
public: 
    RepeatNone (void) : RepetitionType(none) {}
    ~RepeatNone(void) = default;

public:
    uint32_t getReptBits(uint32_t rl) override { (void)rl; return 0; }

public:
    uint32_t encode(uint32_t val) override { (void)val; return 0; }
    uint32_t decode(uint32_t bin) override { (void)bin; return 0; }
}; // RepeatNone



/**
 * repeated at a single level
 * rept value can be 0 or m_rep
 */
class RepeatSingle : public RepetitionType {
protected:
    uint32_t  m_rep{UINT32_MAX}; /**< repeated level */

public: 
    RepeatSingle (void) : RepetitionType(single) {}
    ~RepeatSingle(void) = default;

public:
    void     setReptLevel (uint32_t rep) { m_rep = rep; }

    // get rept bit number: 0 or 1 for m_rep
    uint32_t getReptBits(uint32_t rl) override { return (rl == m_rep); }

public:
    uint32_t encode(uint32_t val) override { return m_rep == val; }
    uint32_t decode(uint32_t bin) override { return bin ? m_rep : 0; }
}; // RepeatSingle
 


/**
 * repeated at multi levels
 * rept value is the original value
 */
class RepeatMulti : public RepetitionType {
public: 
    RepeatMulti (void) : RepetitionType(multi) {}
    ~RepeatMulti(void) = default;

public:
    uint32_t getReptBits(uint32_t rl) override { return rl; }

public:
    uint32_t encode(uint32_t val) override { return val; }
    uint32_t decode(uint32_t bin) override { return bin; }
}; // RepeatMulti



inline
RepetitionType *RepetitionType::create(uint8_t tp)
{
    RepetitionType *got = nullptr;
    switch (tp)
    {
        case none:   got = new RepeatNone  (); break; 
        case single: got = new RepeatSingle(); break; 
        default:     got = new RepeatMulti (); break; 
    }
    return got; 
} // create 


} // namespace
