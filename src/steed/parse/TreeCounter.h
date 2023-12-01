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
 * @file   TreeCounter.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn> 
 * @version 1.0
 * @section DESCRIPTION
 *    definition and functions of TreeCounter 
 */

#pragma once 

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>


namespace steed {

using std::vector; 

class TreeCounter {
protected: // record parser counter 
    uint64_t           m_root_count{0}; /**< root node appeared number   */
    vector<uint32_t>   m_node_count {}; /**< SchemaNode appeared counter */

protected: // Statistics
    /**
     * Statistics: SchemaNode is appeared within record
     * If a node is appeared in one record, no matter it is optional or repeated, 
     * m_stat_is_appeared[node_index] = 1;  
     */
    vector<uint8_t >   m_stat_is_appeared {};
    vector<uint64_t>   m_stat_appear_count{}; /**< Statistics: total appeared counter*/


public:
    TreeCounter (void) = default; 
    ~TreeCounter(void) = default;

public:
    void     updateNode(uint32_t i)  { ++m_node_count[i]  ; m_stat_is_appeared[i] = 1; }
    void     setAppear (uint32_t i)  { m_node_count[i] = 1; m_stat_is_appeared[i] = 1; }

    uint64_t getRootCnt(void)        { return m_root_count; }
    void     setRootCnt(uint64_t v)  { m_root_count = v   ; } // append mode
    void     updateRoot(void)        { ++m_root_count     ; }
    void     clear     (uint32_t i)  { m_node_count[i] = 0; }
    void     resize    (uint32_t n);

    bool     appeared  (uint32_t i)  { return m_node_count[i] > 0; }
    uint32_t get       (uint32_t i)  { return m_node_count[i]    ; }

    uint32_t size        (void)      { return m_node_count.size(); }
    void     clear       (void); 
    void     emplace_back(uint32_t v)
    {   m_node_count.emplace_back (v);   }

public:
    void updateAppearStatistic(void); 
    void outputAppearStatistic(void); 

public: // TODO: parse opt  
    bool setByRange(uint32_t bgn, uint32_t len); 
    bool getByRange(uint32_t bgn, uint32_t len, vector<uint32_t> &gots);
}; // TreeCounter





inline
void TreeCounter::resize(uint32_t n)
{
    m_node_count.resize(n, 0);
    m_stat_is_appeared .resize(n, 0);
    m_stat_appear_count.resize(n, 0);
} // resize



inline
void TreeCounter::clear(void)
{
    m_node_count.clear ();
    m_stat_is_appeared .clear();
    m_stat_appear_count.clear();
} // clear



inline
void TreeCounter::updateAppearStatistic(void)
{
    assert(m_stat_is_appeared.size() == m_stat_appear_count.size());

    uint64_t num = m_stat_is_appeared.size();
    for (uint64_t idx = 0; idx < num; ++idx)
    {
        m_stat_appear_count[idx] += m_stat_is_appeared[idx];
        m_stat_is_appeared [idx]  = 0; 
    } // for 
} // updateAppearStatistic



inline
void TreeCounter::outputAppearStatistic(void)
{
    puts("STEED Info: SchemaNodes appeared statistic by TreeCounter:");
    
    uint64_t idx= 0; 
    for (auto & cnt : m_stat_appear_count)
    {
        printf("%lu, %lu\n", idx++, cnt);
    } // for 
    
    puts("------------------------------------------\n\n\n");
} // outputAppearStatistic



inline
bool TreeCounter::setByRange(uint32_t bgn, uint32_t len)
{
    if (bgn + len > m_node_count.size()) { return false; }
    
    for (uint32_t idx = bgn, i = 0; i < len; ++i, ++idx)
    { m_node_count[idx] = 1; } 
    
    return true; 
} // setByRange 



inline bool TreeCounter::
    getByRange(uint32_t bgn, uint32_t len, vector<uint32_t> &gots)
{
    if (bgn + len > m_node_count.size()) { return false; }

    for (uint32_t idx = bgn, i = 0; i < len; ++i, ++idx)
    {
        if (m_node_count[idx] > 0)
        { gots.emplace_back(idx); }
    } // for idx

    return true;
} // getByRange


} // namespace steed



#if 0
// debug used 
//    void     updateNode(uint32_t i) // debug 
//    { assert(i < m_node_count.size()); ++m_node_count.at(i); }
//
//    void     setAppear (uint32_t i) 
//    { assert(m_node_count[i] == 0); m_node_count[i] = 1; }
#endif 
