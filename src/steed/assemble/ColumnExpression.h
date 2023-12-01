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
 * @file ColumnExpression.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    ColumnExpression definition to the predicate's field in Query 
 */

#pragma once 

#include <stdint.h>
#include <vector>
#include <string>

#include "DataType.h"
#include "SchemaPath.h"
#include "SchemaTree.h"
#include "SchemaSignature.h"


namespace steed {

using std::vector; 
using std::string; 

class ColumnExpression {
protected:
    SchemaTree *m_tree{nullptr};   /**< related SchemaTree  */
    DataType   *m_dt  {nullptr};   /**< leaf node data type */
    SchemaPath  m_path{};          /**< query SchemaPath    */

public:
    ~ColumnExpression(void) = default;
    ColumnExpression (void) = default;
    ColumnExpression (const ColumnExpression &) = default;

public:
    bool   empty(void) const   { return m_path.empty(); }
    int    getDataTypeID(void) { return m_dt->getTypeID(); }
    DataType        *getDataType  (void)  { return m_dt  ; }
    SchemaTree      *getTree      (void)  { return m_tree; }
    SchemaPath      &getPath      (void)  { return m_path; } 
    SchemaSignature &frontSign    (void)  { return m_path.front(); }
    SchemaSignature &backSign     (void)  { return m_path.back (); }
    SchemaSignature  getBottomSign(void)  { return empty() ? 0 : m_path.back(); }
    
public:
    bool operator<  (const ColumnExpression& ce) const
    { return (m_path < ce.m_path); }

    bool operator== (const ColumnExpression& ce) const
    { return (m_path == ce.m_path); }

    bool operator() (const ColumnExpression& l, const ColumnExpression& r) const
    { return l < r; }

public:
    void setInfo(SchemaTree *tree, DataType *dt)
    { m_tree = tree, m_dt = dt; }

    /**
     * add child from current field and update value number 
     * @param sign    child field's SchemaSignature
     * @param vidx    selected value index compared in predicate 
     */
    void addChild(SchemaSignature sign)
    { m_path.emplace_back(sign); }

    /**
     * remove child from current field and update value number 
     */
    void removeChild(void)
    { m_path.pop_back(); } 

public:
    void output2debug(uint32_t lvl = 0) const;
}; // ColumnExpression





inline
void ColumnExpression::output2debug(uint32_t lvl) const
{
    for (uint32_t i = 0; i < lvl; ++i) { printf("\t"); }
    puts("---------------- ColumnExpression Begin ----------------");

    m_path.output2debug(lvl);

    for (uint32_t i = 0; i < lvl; ++i) { printf("\t"); }
    puts("---------------- ColumnExpression Done ----------------");
} // output2debug



} // namespace
