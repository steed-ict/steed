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
 * @file ColumnAssembler.h
 * @version 1.0
 * @section DESCRIPTION
 *   ColumnAssembler definition to assemble Column according to SchemaPathes
 */

#pragma once

#include <stdint.h>

#include "Row.h"
#include "Buffer.h"
#include "Config.h"
#include "Utility.h"
#include "SchemaTree.h"
#include "SchemaTreeMap.h"

#include "ColumnExpressionParser.h"

#include "AssembleColumn.h"
#include "RecordNestedAssembler.h"


namespace steed {

using std::string;
extern Config g_config;


class ColumnAssembler {
protected: // init by column name strings 
    vector<ColumnExpression> m_exps   {}; /**< column expressions to assemble */
    ColumnExpressionParser   m_parser {}; /**< column expression parser */
    QueryPathes              m_fields {}; /**< query pathes for fields */
    vector<ColumnReader*>    m_col_rds{}; /**< column readers for fields */

protected: // init by QueryPathes and ColumnReader 
    Buffer        *m_buf  {nullptr}; /**< binary row buffer  */
    SchemaTree    *m_tree {nullptr}; /**< related SchemaTree */

    AssembleColumn          *m_columns {nullptr}; /**< assemble columns  */
    RecordNestedAssembler   *m_assemble{nullptr}; /**< bin row assembler */
    uint64_t                 m_cur_recd_idx  {0}; /**< current recd idx  */

    uint64_t       m_total_rnum {0}; /**< total record number*/
    uint64_t       m_next_rbgn  {0}; /**< next record begin  */
    uint32_t       m_buf_rnum   {0}; /**< record buffer num  */
    bool           m_dbl_buf{false}; /**< double buffer flag */

    
public:
    ~ColumnAssembler(void);
    ColumnAssembler (void) = default;

public: 
    /**
     * init function:
     *   use string text column name to init: 
     *   1. use SchemaTree to get related QueryPathes
     *   2. use each path in QueryPathes to create reader in vector<ColumnReader*>
     * @param cols  column name strings in vector
     * @return 0 success; <0 failed
     */
    int init(const string &db, const string &tb, vector<string> cols);

    /**
     * init function:
     *   use each path in QueryPathes to create reader in vector<ColumnReader*>
     *
     * @param  path    related query path
     * @param  crd     related column readers
     */
    int init(QueryPathes *path, vector<ColumnReader*> &crd);

public:
    SchemaTree* getSchemaTree(void) { return m_tree; }

public:
    /**
     * get next buffered data in m_buf
     * @param rbgn  next record  begin address
     * @return  <0 error code; ==0 EOF; >0 number of next record get, ==1
     */
    int32_t getNext(char* &rbgn);

private:
    /**
     * buffer more records from lower operator:
     *    operator buffers more records by itself
     * @return >0 buffered record number; 0 EOF; <0 read error
     */
    int32_t bufferMore(void);


    /**
     * double m_buf content capacity
     * @return 0 success; <0 failed
     */
    int doubleBuffer(void);

    /**
     * prepare ColumnReader for the record
     * @return 1 prepare success; 0 EOF; <0 failed;
     */
    int prepareColumnReader(void);
}; // ColumnAssembler



inline
ColumnAssembler::~ColumnAssembler(void)
{
    m_tree = nullptr;
    delete m_buf;  m_buf = nullptr; 
    delete m_columns ; m_columns  = nullptr;
    delete m_assemble; m_assemble = nullptr;
    m_cur_recd_idx = 0;
    m_total_rnum = 0;
    m_next_rbgn  = 0;
    m_buf_rnum   = 0;
    m_dbl_buf    = false;
} // dtor 



inline
int ColumnAssembler::init(QueryPathes *path, vector<ColumnReader*> &crd)
{
    assert(m_cur_recd_idx == 0);

    m_columns = new AssembleColumn();
    m_columns->init(path, crd);              // app pathes
    m_columns->updateColumn(m_cur_recd_idx); // get current

    // use current pathes to init assembler
    m_assemble = new RecordNestedAssembler(m_buf, m_tree);
    return m_assemble->init(m_columns);
} // init



inline
int32_t ColumnAssembler::doubleBuffer(void)
{
    uint64_t cap = m_buf->capacity();
    return   m_buf->reserve(cap * 2);
} // doubleBuffer



} // namespace steed