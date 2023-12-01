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
 * @file AssembleColumn.h
 * @author  Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for AssembleColumn
 */

#include <set> 
#include <queue> 
#include <stdint.h> 

#include "ColumnReader.h"
#include "QueryPathes.h"

#pragma once 



namespace steed {

using std::set;
using std::queue;

class AssembleColumn {
protected:
    QueryPathes             *m_all_path{nullptr}; /**< all related pathes */
    vector<ColumnReader*>    m_all_crds       {}; /**< all related readers*/

    /** all pathes's max depth: including the root node */ 
    uint64_t                 m_max_depth     {0}; 

    QueryPathes             *m_cur_path{nullptr}; /**< current related pathes */
    vector<ColumnReader*>    m_cur_crds       {}; /**< current related readers*/

    queue<uint64_t>          m_reinit_recd_id {}; /**< re init record index   */


public:
    AssembleColumn (void) = default;  
    ~AssembleColumn(void);

public:
    uint64_t               getMaxDepth      (void) { return m_max_depth; }
    QueryPathes           *getCurrentPathes (void) { return m_cur_path ; }
    vector<ColumnReader*> &getCurrentColRead(void) { return m_cur_crds ; }

public:
    /**
     * init function:
     * @param path  all query path 
     * @param crd   all column readers
     */
    void init (QueryPathes*  path, vector<ColumnReader*> &crd);

public:
    /**
     * need to update current assembling columns  
     * @param recd_id    record index to check 
     * @return need to update flag
     */
    bool need2update (uint64_t recd_id);

    /**
     * update the current assembling columns
     * @param recd_id    record index to check 
     * @return need to update flag
     */
    int  updateColumn(uint64_t recd_id);
}; // AssembleColumn










inline
AssembleColumn::~AssembleColumn(void)
{
    delete m_cur_path; 
    
    m_all_path = nullptr;
    m_cur_path = nullptr;
    m_all_crds.clear();
    m_cur_crds.clear();
} // ctor 





inline
void AssembleColumn::init(QueryPathes *path, vector<ColumnReader*> &crd)
{
    m_all_path = path; 
    m_all_crds = crd ;
    m_cur_path = new QueryPathes();
    
    uint32_t pnum = path->size();
    for (uint32_t pi = 0; pi < pnum; ++pi)
    {
        SchemaPath &sp = path->get(pi);
        uint32_t depth = sp.size();
        m_max_depth = (m_max_depth > depth) ? m_max_depth : depth;
    } // for pi  
    m_max_depth += 1; // include the root level 


    set<uint64_t> valid_recd_idx; 
    for (auto & rd : m_all_crds)
    {
        uint64_t ridx = rd->getValidRecdIdx();
        valid_recd_idx.insert(ridx);
    } // for 
    
    for (auto & ridx : valid_recd_idx)
    {   m_reinit_recd_id.emplace(ridx);   }
} // init





inline
bool AssembleColumn::need2update(uint64_t recd_id)
{
    if ((m_reinit_recd_id.empty()) || (m_reinit_recd_id.front() > recd_id))
    {   return false;   }
    
    while ((!m_reinit_recd_id.empty()) && (m_reinit_recd_id.front() <= recd_id))
    {   m_reinit_recd_id.pop();   }
    return true;
} // need2update





inline
int AssembleColumn::updateColumn(uint64_t recd_id)
{
    m_cur_path->clear();
    m_cur_crds. clear();
    
    uint32_t pnum = m_all_path->size();
    for (uint32_t pi = 0; pi < pnum; ++pi)
    {
        ColumnReader *rd = m_all_crds[pi];
        uint64_t valid_rid = rd->getValidRecdIdx();
        bool     not_valid = (valid_rid > recd_id);
        if (not_valid) { continue; } 
        
        SchemaPath &sp = m_all_path->get(pi);
        m_cur_path->emplace_back(sp);
        m_cur_crds .emplace_back(rd);
    } // for 
    
    return 0;
} // updateColumn


} // namespace 
