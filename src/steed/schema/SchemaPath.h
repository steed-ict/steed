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
 * @file SchemaPath.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   SchemaPath definition to index the SchemaNodes in tree 
 */

#pragma once 

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>

#include "SchemaSignature.h"

namespace steed {
using std::vector;

/**
 * Path Signature in SchemaTree
 *     the nodes on the path can be SchemaNode 
 */
class SchemaPath {
protected:
    vector<SchemaSignature> m_signs{}; /**< nodes on each level in path */

public:
    ~SchemaPath(void) = default; 
    SchemaPath (void) = default;
    SchemaPath (const SchemaPath &path) = default;

public: 
    void      clear   (void)        { m_signs.clear(); }
    bool      empty   (void) const  { return m_signs.empty(); }
    uint64_t  size    (void) const  { return m_signs.size (); }
    void      pop_back(void)        { m_signs.pop_back(); }
    void      emplace_back (SchemaSignature s)
    { m_signs.emplace_back (s); }

    SchemaSignature& front(void)  { return m_signs.front(); }
    SchemaSignature& back (void)  { return m_signs.back (); }
    SchemaSignature  leaf (void)  { return m_signs.back (); }

    SchemaSignature* data (void)  { return m_signs.data (); }
    const SchemaSignature* data(void) const noexcept
    { return m_signs.data(); }

    SchemaSignature  operator[](uint64_t n) const 
    { return m_signs[n]; } 

public:
    /** compare two SchemaPathes */ 
    bool operator() (const SchemaPath &l, const SchemaPath &r);
    bool operator== (const SchemaPath &p) const;
    bool operator!= (const SchemaPath &p) { return !(*this == p); }
    bool operator<  (const SchemaPath &p) const;

public:
    void output2debug    (uint32_t lvl = 0) const;
    void outputPath2Debug(void)             const;


public: // static function 
    /**
     * get the lowest same level between SchemaPathes
     * p1 and p2 should contain leaf SchemaNode
     * @return the lowest same level: root is level 0
     */
    static uint32_t getLowestSameLevel(SchemaPath &p1, SchemaPath &p2);
};  // SchemaPath 



inline
bool SchemaPath::operator() (const SchemaPath &l, const SchemaPath &r)
{
    uint32_t llen = l.size();
    uint32_t rlen = r.size();
    uint32_t len  = llen < rlen ? llen : rlen;  
    for (uint32_t i = 0; i < len; ++i)
    {
        SchemaSignature lsign = l[i];
        SchemaSignature rsign = r[i];
        if (lsign != rsign)
        {
            // got diff SchemaSignature on path  
            return (lsign < rsign); 
        }
    } // for i 
    
    // longer path is larger 
    return (llen < rlen); 
} // operator()


inline
bool SchemaPath::operator== (const SchemaPath &p) const
{
    uint32_t len  = size  ();
    uint32_t plen = p.size();
    if (len != plen) { return false; }

    uint32_t n = len * sizeof(SchemaSignature);
    return  (memcmp(data(), p.data(), n) == 0);
} // operator == 


inline
bool SchemaPath::operator<  (const SchemaPath &p) const
{
    uint32_t mylen = m_signs.size(), plen = p.size();
    uint32_t len = mylen < plen ? mylen : plen; 
    uint32_t cmp = 0; 
    while ((m_signs[cmp] == p.m_signs[cmp]) && (++cmp < len));
    
    return (cmp < len) ? (m_signs[cmp] < p.m_signs[cmp]) : (mylen < plen); 
} // operator<


inline
void SchemaPath::output2debug(uint32_t lvl) const
{
#if 1
    (void)lvl;
    puts("SchemaPath:");
    for (auto &s : m_signs)
    { printf("[%10u] ", s); }
    puts("");
#else
    for (uint32_t i = 0; i < lvl; ++i) { printf("\t"); }
    printf("SchemaPath: ");
    puts("SchemaPath -----------------");

    for (uint32_t i = 0; i < lvl; ++i) { printf("\t"); }
    printf("|  ");
    for (auto &s : m_signs)
    { printf("[%u]", s); }
    puts("  |");

    for (uint32_t i = 0; i < lvl; ++i) { printf("\t"); }
    puts("SchemaPath Done ------------");
    puts("");
#endif
} // output2debug


inline
void SchemaPath::outputPath2Debug(void) const
{
    for (auto &s : m_signs)
    { printf("[%u]", s); }
} // outputPath2Debug


inline
uint32_t SchemaPath::getLowestSameLevel(SchemaPath &p1, SchemaPath &p2)
{
    uint32_t len1 = p1.size();
    uint32_t len2 = p2.size();
    uint32_t min  = (len1 < len2) ? len1 : len2; 

    // the lowest same level on pathes  
    // the nodes in the differnt sub-tree cannot be the same
    uint32_t lowest = 0, lvl = 0;
    do
    {
        lowest += (p1[lvl] == p2[lvl]);
    } while (++lvl < min);

    return lowest;
} // getLowestSameLevel

} // namespace steed