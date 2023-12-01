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



/***
 * @file ColumnParser.cpp
 * @brief ColumnParser class implementation
 * @version 1.0
 * @section DESCRIPTION
 *  ColumnParser class implementation
 */

#include "ColumnParser.h"

namespace steed {


int ColumnParser::init (const string &db, const string &clt, istream *is)
{
    m_jbuffer = new JSONRecordBuffer(is);
    m_jparser = new JSONRecordNaiveParser();

    for (uint32_t i = 0; i < s_jtree_cap; ++i)
    {   m_jtree[i] = new JSONBinTree();   } 

    // get SchemaTree from SchemaTreeMap
    int status = SchemaTreeMap::getDefinedTree(db, clt, m_tree);
    if (status < 0)
    {
        printf("ColumnParser: get SchemaTree failed!\n");
        return -1; 
        DebugInfo::printStackAndExit();
    } 
    else if (status == 0)
    {
        m_append = false;

        // create a new SchemaTree
        m_tree = new SchemaTree(db, clt);

        // move the SchemaTree ownership to SchemaTreeMap
        SchemaTreeMap::emplace(db, clt, m_tree);
    }
    else 
    {
        // status > 0, got a defined SchemaTree
        m_append = true;
    } // if
    
    m_clt_wt   = new CollectionWriter(m_tree);
    m_item_gen = new ColumnItemGenerator(m_tree, m_clt_wt);


    // reset the buffer and reader
    m_jbuffer->reset();   

    // resize to the same size as SchemaTree
    m_item_gen->resizeCounter(); 

    if (m_append)
    {
        TreeCounter *tc = m_item_gen->getCounter();
        if (m_clt_wt->init2append(tc) < 0)
        {
            printf("ColumnParser::CollectionWriter init2append failed!\n");
            return -1;
        } // if 
    }
    else
    {
        if (m_clt_wt->init2write () < 0)
        {
            printf("ColumnParser::CollectionWriter init2write failed!\n");
            return -1;
        } // if 
    } // if 

    return 0; 
} // init


} // namespace steed