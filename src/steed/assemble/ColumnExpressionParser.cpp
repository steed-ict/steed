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
 * @file ColumnExpressionParser.cpp
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   ColumnExpressionParser functions
 */

#include "ColumnExpressionParser.h"

namespace steed {


int ColumnExpressionParser::parse(vector<string> &names, uint32_t idx)
{
    // no more strings to parse
    if (names.size() == idx) 
    {   return parseSubTree();   }
    

    string &key = names[idx++]; 
    SchemaSignature psign = m_cur_exp.getBottomSign();

    // parse key as SchemaNode
    int  got_num = 0; 
    auto rng = m_tree->findNode(key.c_str(), psign);
    if  (rng.first != rng.second)
    {
        auto  cur  = rng.first, end = rng.second; 
        while(cur != end)
        {
            SchemaSignature sign = cur->second;
            int s = parseSchemaNode(sign, names, idx);
            if (s < 0)
            { 
                printf("ColumnExpressionParser: parse by Schema failed!");
                return -1;
            } // if 
            got_num += s;  

            ++cur;
        } // while  
    } // if 
    return got_num;
} // parse 





int ColumnExpressionParser::
    parseDefaultNode(SchemaSignature sign, vector<string> &names, uint32_t idx)
{
    SchemaNode *sn = m_tree->getNode(sign);
    assert(!sn->isIndexArray());

    // number is index 
    m_cur_exp.addChild(sign);

    int got_num = 0;
    if (!sn->isLeaf()) 
    {
        // non-leaf SchemaNode
        got_num = parse(names, idx);
    } 
    else if (names.size() == idx) 
    {
            // leaf WO predicate 
            DataType *dt = sn->getDataType();
            emplaceGotExp(dt);

            got_num = 1;
    } // if 
    
    m_cur_exp.removeChild();

    return got_num;
} // parseDefaultNode 







int ColumnExpressionParser::parseSubTree(void)
{
    SchemaSignature sign = m_cur_exp.getBottomSign();
    SchemaNode *node = m_tree->getNode(sign); 
    if (node->isLeaf())
    {
        // leaf WO predicate 
        DataType *dt = node->getDataType();
        emplaceGotExp(dt);
    }
    else
    {
        uint32_t cnum = node->getChildNum(); 
        for (uint32_t ci = 0; ci < cnum; ++ci)
        {
            SchemaSignature css = node->getChild(ci);
            m_cur_exp.addChild(css);

            if (parseSubTree() < 0)
            {
                puts("ColumnExpressionParser: parseSubTree failed!\n");
                return -1;
            } // if 

            m_cur_exp.removeChild();
        } // for ci
    } // if-else 
    
    return 0;
} // parseSubTree

} // namespace
