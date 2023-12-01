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
 * @file ColumnExpressionParser.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    ColumnExpressionParser definition to parse text Path Expression 
 */

#pragma once 

#include <vector>
#include <string>

#include "DataType.h"
#include "SchemaTree.h"

#include "QueryPathes.h"
#include "ColumnExpression.h"


namespace steed {

using std::vector;
using std::string;

class ColumnExpressionParser {
protected: 
    SchemaTree               *m_tree{nullptr}; /**< exp related SchemaTree */
    vector<ColumnExpression> *m_exps{nullptr}; /**< got exp ins by parsing */
    ColumnExpression          m_cur_exp{};      /**< current exp during parsing*/

public:
    ColumnExpressionParser (void) = default;
    ~ColumnExpressionParser(void) = default; 
    
public:
    /**
     * init parser to parse
     * @param tree   SchemaTree ins to get expression info  
     * @param exps   result exps : maybe multiple exps
     * @param preds  result preds: maybe multiple preds
     */
    void init(SchemaTree *tree, vector<ColumnExpression> *exps)
    { m_tree = tree, m_exps = exps; }

public:
    /**
     * parse text path expression: (original) names = path + names
     * @param names    child name strings to parse 
     * @return 1 success; 0 got nothing; <0 failed
     */
    int parse(vector<string> &names)
    { uint32_t idx{0}; return parse(names, idx); }

protected:
    /**
     * emplace the got expression to m_exps
     */ 

    void emplaceGotExp(DataType *lf_dt)
    {
        m_cur_exp.setInfo(m_tree, lf_dt);
        m_exps->emplace_back(m_cur_exp);
    } // emplaceGotExp

    /**
     * parse text path expression: (original) names = path + names
     * @param names    child name strings to parse 
     * @param idx      name strings index 
     * @return 1 success; 0 got nothing; <0 failed
     */
    int parse(vector<string> &names, uint32_t idx);

    /**
     * parse ColumnExpression field as SchemaNode
     * @param sign     SchemaNodes' SchemaSignature
     * @param names    child name strings to parse 
     * @param idx      name strings index 
     * @return 1 success; 0 got nothing; <0 failed
     */
    int parseSchemaNode (SchemaSignature sign, vector<string> &names, uint32_t idx)
    { return parseDefaultNode(sign, names, idx); }

    /**
     * parse default field by Schema: field can be single or multiple value
     * @param sign     single value SchemaNode's SchemaSignature
     * @param names    child name strings to parse 
     * @param idx      name strings index 
     * @return 1 success; 0 got nothing; <0 failed
     */
    int parseDefaultNode(SchemaSignature sign, vector<string> &names, uint32_t idx);

    /**
     * parse the subtree in SchemaTree:
     * add all possible SchemaPath from subtree to m_exps
     * current path is stored in m_cur_exp
     * @param sign    sub tree root SchemaSignature in m_tree
     * @return 1 success; 0 got nothing; <0 failed
     */
    int parseSubTree    (void);
}; // ColumnExpressionParser



} // namespace
