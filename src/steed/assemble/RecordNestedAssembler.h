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
 * @file   RecordNestedAssembler.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for RecordNestedAssembler 
 */

#pragma once 

#include <vector>

#include "Buffer.h"

#include "FSMTable.h"
#include "RecordBuilder.h"

#include "SchemaSignature.h"
#include "SchemaTree.h"
#include "AssembleColumn.h"



namespace steed {

using std::vector;

class RecordNestedAssembler {
protected: 
    Buffer                *m_buf  {nullptr}; /**< Buffer 4 assembled row    */
    SchemaTree            *m_tree {nullptr}; /**< related SchemaTree ins    */

    QueryPathes           *m_path {nullptr}; /**< assemble query pathes     */
    vector<ColumnReader*>  m_crds {};        /**< ColumnReader to get items */

    FSMTable              *m_fsm  {nullptr}; /**< FSMTable to get jump info */
    Row::RecordBuilder    *m_build{nullptr}; /**< Row layout builder        */

public:
    /**
     * ctor 
     * @param buf   Buffer for assembled binary row  
     * @param t     SchemaTree used    to assemble 
     */
    RecordNestedAssembler (Buffer *buf, SchemaTree *t): m_buf(buf), m_tree(t) {}
    ~RecordNestedAssembler(void);

public:
    /**
     * init assembler to assemble
     * @param path  related SchemaPath to assemble 
     * @param crds  ColumnReader ins to read ColumnItems
     */
    int  init  (AssembleColumn *cols);

    /**
     * reinit assembler to assemble: assemble columns are changed  
     * @param path  related SchemaPath to assemble 
     * @param crds  ColumnReader ins to read ColumnItems
     */
    int  reinit(AssembleColumn *cols);

    /**
     * reset the state 
     * clear buffer used and discard assembled binary content  
     */
    void reset  (void) { m_build->erase(); }

    /**
     * assemble column items to binary row layout
     * @return 1 assmble num; ==0 EOF; <0 failed 
     */
    int assemble(void);

protected: 
    /**
     * get common ancestor level between cur_idx and nxt_idx
     * @param cur_idx   current column index 
     * @param nxt_idx   next    column index
     * @param nrep      next rept value  
     * @return common level between cur_idx and nxt_idx
     */
    uint32_t getCommonLevel(uint32_t cur_idx, uint32_t nxt_idx, uint32_t nrep);

    /**
     * move to the nested level according to def value 
     * @param def       def value to write got from ColumnReader
     * @param path      writing SchemaPath ins 
     * @param mv2frt    move to front reader: append new elem in array flag
     * 
     */
    int move2level (uint32_t def, SchemaPath &path, bool mv2frt)
    {   return m_build->move2level(def, path, mv2frt);   }

    /**
     * return the binary row layout to the level 
     * @param tgt_lvl    common level to return 
     * @param path       writing SchemaPath ins 
     * @param mv2frt     move to front reader: append new elem in array flag
     */
    int return2level(uint32_t tgt_lvl, SchemaPath &path, bool mv2frt)
    {   return m_build->return2level(tgt_lvl, path, mv2frt);   }
}; // RecordNestedAssembler










inline
RecordNestedAssembler::~RecordNestedAssembler(void)
{
    m_tree = nullptr;  m_path = nullptr; 

    m_crds.clear()  ;
    m_buf->clear()  ;  m_buf  = nullptr;

    delete m_fsm    ;  m_fsm  = nullptr;
    delete m_build  ;  m_build= nullptr; 
} // dtor



inline
int  RecordNestedAssembler::init  (AssembleColumn *cols)
{
    m_fsm = new FSMTable();

    uint32_t max_depth = cols->getMaxDepth();
    m_build = new Row::RecordBuilder(m_tree, m_buf, max_depth); 

    m_path = cols->getCurrentPathes ();
    m_crds = cols->getCurrentColRead();

    return 0; 
} // ctor 



inline
int RecordNestedAssembler::reinit(AssembleColumn *cols)
{
    m_path = cols->getCurrentPathes ();
    m_crds = cols->getCurrentColRead();

    m_fsm->uninit();
    m_fsm->init  (m_tree, *m_path);

#ifdef _DEBUG_COLUMN_READER
//    // output SchemaTree
//    m_tree->output2debug();
    
    // output FSM content 
    m_fsm->output2debug();
    fflush(nullptr);
//exit(0);
#endif // _DEBUG_COLUMN_READER

    return 0;
} // reinit



inline
uint32_t RecordNestedAssembler::
            getCommonLevel(uint32_t cur_idx, uint32_t nxt_idx, uint32_t nrep)
{
    // same column:
    // next repetition value decides which level to return 
    if (cur_idx == nxt_idx)
    {   return nrep;   }

    // diff columns:
    // always return to the same level of the neighbouring columns
    uint32_t tgt_lvl = uint32_t(-1);
    uint32_t large = 0, small = 0;
    bool mv2frt = (nxt_idx <= cur_idx);
    if  (mv2frt)
    {
        large = cur_idx, small = nxt_idx;
        SchemaPath &sp = m_path->get(large);
        tgt_lvl = m_fsm->getCommonReptLevel(large, sp, small);

        // repetition value decides the target level:
        // repeated @ upper SchemaTemplate 
        tgt_lvl = (nrep < tgt_lvl) ? nrep : tgt_lvl; 
    }
    else
    {
        large = nxt_idx, small = cur_idx;
        SchemaPath &sp = m_path->get(large);
        tgt_lvl = m_fsm->getLowestSameLevel(large, sp, small);
    } // if 
    
    return tgt_lvl;
} // getCommonLevel





} // namespace 
