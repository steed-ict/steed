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
 * @file QueryPathes.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    QueryPathes definition to the QueryPathes in SQL statement,
 *    which are SchemaPathes
 */

#pragma once 

#include <algorithm>

#include "SchemaPath.h"



namespace steed {


class QueryPathes {
protected:
    vector<SchemaPath> m_pathes; /**< QueryPathes in SQL statement */

public:
    ~QueryPathes(void) = default; 
    QueryPathes (void) = default; 
    QueryPathes (const QueryPathes &pathes) = default; 

public:
    bool     empty (void) const   { return m_pathes.empty(); }
    void     clear (void)         { return m_pathes.clear(); }
    uint32_t size  (void)         { return m_pathes.size (); }
    void     resize(uint32_t n)   { return m_pathes.resize(n); }

    void checkAndAppend(SchemaPath &p);
    void emplace_back  (SchemaPath &p) { m_pathes.emplace_back(p); }

    SchemaPath       &get        (uint32_t n)       { return m_pathes[n]; }
    SchemaPath       &operator[] (uint64_t n)       { return m_pathes[n]; } 
    const SchemaPath &operator[] (uint64_t n) const { return m_pathes[n]; }



    /**
     * get SchemaPath's index in m_pathes 
     * @return -1 as failed; otherwise, index 
     */
    uint32_t getIndex(SchemaPath &p);
 
    /**
     * sort m_pathes by parent SchemaNode's Signature: 
     *     Pathes sharing the same parent are next to each other in m_pathes
     */
    void sortByParent(void) 
    { std::sort(m_pathes.begin(), m_pathes.end(), SchemaPath()); }

public:
    void output2debug(void);
}; // QueryPathes





inline
void QueryPathes::checkAndAppend(SchemaPath &p)
{
    if (uint32_t(-1) == getIndex(p))
    {   emplace_back(p);   } 
} // checkAndAppend 



inline
uint32_t QueryPathes::getIndex(SchemaPath &p)
{
    uint32_t i = 0, num = m_pathes.size();
    while (i < num)
    {
        if (p == m_pathes[i])
        { break; }
        else
        { ++i  ; }
    }
    return (i != num) ? i : uint32_t(-1);
} // getIndex



inline
void QueryPathes::output2debug(void)
{
    puts("QueryPathes ------------------------");
    for (auto & p : m_pathes)
    { p.output2debug(); } 
    puts("------------------------------------");
} // output2debug


} // namespace steed
