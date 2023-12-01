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
 * @file ColumnTextBuffer.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    definitions and functions for ColumnTextBuffer 
 */

#pragma once 

#include <stdint.h>
#include <vector>


namespace  steed {

using std::vector;

class ColumnTextBuffer {
public:
    /** each Value item in JSON got a Item */
    class Item {
    protected:
        uint32_t    m_rep{0};       /**< rep value   */
        uint32_t    m_def{0};       /**< def value   */
        const char *m_txt{nullptr}; /**< text string */

    public:
        ~Item(void) = default;
        Item (void) = default; 
        Item (const Item &p) = default;
        Item (uint32_t r, uint32_t d, const char *t) :
            m_rep(r), m_def(d), m_txt(t)
        {}

    public:
        uint32_t    getRep(void) { return m_rep; }
        uint32_t    getDef(void) { return m_def; }
        const char *getTxt(void) { return m_txt; }

    public:
        void output2debug(void);
    }; // Item 


protected:
    vector<Item>  m_item_vec; /**< buffer item in vector*/ 

public:
    ColumnTextBuffer (void) = default; 
    ~ColumnTextBuffer(void) = default; 

public:
    vector<Item> &getBuffer(void)       { return m_item_vec;     } 
    Item         &get      (uint32_t i) { return m_item_vec [i]; }
//    Item         &get      (uint32_t i) { return m_item_vec.at(i); }

    void      clear(void)  { m_item_vec.clear(); }
    uint32_t  size (void)  { return m_item_vec.size(); }

    void      reserve      (uint32_t n) { m_item_vec.reserve(n); }
    void      emplace_back (uint32_t r, uint32_t d, const char *t) 
    { m_item_vec.emplace_back(r, d, t); }

    void deleteIns(void)                { m_item_vec.clear(); }
    void copyIns  (ColumnTextBuffer &n)
    { m_item_vec.assign(n.m_item_vec.begin(), n.m_item_vec.end()); }

public:
    void output2debug(void);
}; // ColumnTextBuffer





inline
void ColumnTextBuffer::Item::output2debug(void)
{
    printf("ColumnTextBuffer::Item <%u:%u:%s>\n",
        m_rep, m_def, m_txt ? m_txt : "null");
} // output2debug



inline
void ColumnTextBuffer::output2debug(void)
{
    for (auto & i : m_item_vec)
    {   i.output2debug();   }
} // output2debug



} // steed
