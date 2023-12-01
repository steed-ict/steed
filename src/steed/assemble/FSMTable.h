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
 * @file   FSMTable.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for FSMTable in RowNestedAssembler 
 */

#pragma once 

#include <stdint.h> 
#include <string.h> // memset

#include <algorithm>
#include <vector>

#include "SchemaTree.h"
#include "QueryPathes.h"



namespace steed {

using std::vector; 

class FSMTable {
public:
    /** SchemaTree to check repeated SchemaNode */
    SchemaTree        *m_tree  {nullptr};

    /** SchemaPathes to build FSMTable */
    vector<SchemaPath> m_pathes{}; 

    /** 
     * the lowest same parent's level index, including the root level
     * Definition:
     *     level of lowest parent between next SchemaPath 
     *
     * m_low_same_lvl[idx] === level of lowest parent between [idx] and [idx+1]
     */
    vector<uint32_t>  m_low_same_lvl{};

    /** 
     * next common repetition level, including the root level
     * Definition:
     *     level of lowest parent between next SchemaPath 
     *
     * m_com_rep_lvl[idx] === level of lowest REPEATED parent between [idx] and [idx+1]
     * the SchemaNode must be a repeated SchemaNode or SchemaTemplate root  
     */
    vector<uint32_t>  m_com_rep_lvl{};

    uint32_t         *m_fsm_table {nullptr}; /**< transition array content   */
    uint32_t          m_column_num{0};       /**< column number in FSM table */
    uint32_t          m_max_size  {0};       /**< max size in all SchemaPath */

public:
    FSMTable (void) = default; 
    ~FSMTable(void) { uninit(); }

public:
    /**
     * init FSMTable by QueryPathes 
     * @param tree    SchemaTree to decide the common repetition level
     * @param path    all related SchemaPathes in QueryPathess,
     *                which are already sorted by QueryPathess::sortByParent
     * @return 0 success; <0 failed 
     */
    int init(SchemaTree * tree, QueryPathes &path);

    /**
     * uninit FSM table content 
     */
    void uninit(void);


protected:
    /**
     * init common level vector 
     * @param tree    SchemaTree to decide the common repetition level
     * @param path    assemble related SchemaPathes in QueryPathes 
     */
    void initCommonLevel(SchemaTree * tree, QueryPathes &path);

    /**
     * init QueryPathes in FSMTable 
     * @param path    assemble related QueryPathes in QueryPathes 
     * @param trans   FSM transition table in vector 
     * @return 0 success; <0 failed
     */
    int initTransTable(QueryPathes &path, vector< vector<uint32_t> > &trans);

    /**
     * init FSM table in array
     * @param trans   FSM transition table in vector 
     */
    void initFSMTable(vector< vector<uint32_t> > &trans);


public:
    /**
     * get common repetition level between target and current
     * NOTE: target index >= current index  
     *
     * @param tgt    target  column index 
     * @param tsp    target SchemaPathe in SchemaTree
     * @param cur    current column index
     * @return common rept value between i and j
     */
    uint32_t getCommonReptLevel(uint32_t tgt, SchemaPath &tsp, uint32_t cur);

    /**
     * get the lowest same level between target and current
     * NOTE: target index >= current index  
     *
     * @param tgt    target  column index 
     * @param tsp    target SchemaPathe in SchemaTree
     * @param cur    current column index
     * @return the lowest same level between i and j
     */
    uint32_t getLowestSameLevel(uint32_t tgt, SchemaPath &tsp, uint32_t cur);

    /**
     * get max path size in QueryPathes
     * @return max path size 
     */
    uint32_t getMaxPathSize(void) { return m_max_size; }

    /**
     * get transition value from FSMTable
     * @param col    current column index in FSM table 
     * @param rep    repetition value to get next column index 
     * @param next column index 
     */
    uint32_t get(uint32_t col, uint32_t rep)
    { return *(m_fsm_table + m_max_size * col + rep); }


public:
    void output2debug(void);
}; // FSMTable





inline
int FSMTable::init(SchemaTree * tree, QueryPathes &path)
{
    m_tree = tree;
    initCommonLevel(tree, path); 

    /**
     * FSM transition table in vector
     *     trans[column_idx][next_rep] = next_column_idx
     */
    vector<vector<uint32_t>> trans;  
    if (initTransTable(path, trans) < 0)
    {
        puts("FSMTable: initTransTable failed!\n");
        return -1; 
    } // initTransTable

    initFSMTable(trans);

    return 0;
} // init



inline
void FSMTable::uninit(void)
{
    m_tree = nullptr;
    m_pathes      .clear();
    m_low_same_lvl.clear();
    m_com_rep_lvl .clear();

    free (m_fsm_table);
    m_fsm_table  = nullptr;
    m_column_num = 0;
    m_max_size   = 0;
} // dtor



inline
uint32_t FSMTable::getCommonReptLevel(uint32_t tgt, SchemaPath &tsp, uint32_t cur)
{
    // NOTE: tgt >= cur 
    uint32_t rep_lvl = uint32_t(-1);
    if (tgt == cur)
    {
        uint32_t  rep_idx = m_tree->getLowestRepeatedNodeIndex(tsp);
        rep_lvl = rep_idx + 1;
        return rep_lvl;
    } // if 
 
    for (uint32_t ci = cur; ci < tgt; ++ci)
    {
        // common repetition level between ci and ci+1
        uint32_t tmp = m_com_rep_lvl.at(ci);

        // common_level(a, b, c) ==>
        // min( common_level(a, b), common_level(b, c))
        rep_lvl = (rep_lvl > tmp) ? tmp : rep_lvl;
    } // for
    return rep_lvl; 
} // getCommonReptLevel



inline
uint32_t FSMTable::getLowestSameLevel(uint32_t tgt, SchemaPath &tsp, uint32_t cur)
{
    if (tgt == cur)
    {
        return tsp.size();
    } // if 
 
    uint32_t same_lvl = uint32_t(-1);
    for (uint32_t ci = cur; ci < tgt; ++ci)
    {
        // low same level between ci and ci+1
        uint32_t tmp = m_low_same_lvl.at(ci);

        // low_same_level(a, b, c) ==>
        // min( low_same_level(a, b), low_same_level(b, c))
        same_lvl = (same_lvl > tmp) ? tmp : same_lvl;
    } // for
    return same_lvl; 
} // getLowestSameLevel



} // namespace steed
