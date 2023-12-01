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
 * @file SchemaNode_inline.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *     definitions and functions for SchemaNode inline funcs 
 */

#pragma once 

namespace steed {

inline
SchemaNode& SchemaNode::operator= (const SchemaNode &n)
{
    if (this != &n)
    {
        m_dt = n.m_dt; 
        m_dt_id = n.m_dt_id; 
        m_index = n.m_index; 
    
        m_field_id = n.m_field_id; 
        m_parent   = n.m_parent  ; 
        // copy m_child is invalid; use duplicate instead 
    
        m_level = n.m_level; 
        m_vcate = n.m_vcate; 
        m_temp  = n.m_temp ;
    }
    return *this;
} // operator=


inline
void SchemaNode::
    set(SchemaNode *p, SchemaSignature idx, int dt_id, Row::ID fid, uint8_t val_cat)
{
    SchemaSignature ps = p->getNodeIndex();
    uint32_t clvl = p->getLevel() + 1;
    this->set(ps, clvl, idx, dt_id, fid, val_cat);

    assert(m_index == idx);
    p->addChild(m_index);
} // set 


inline
void SchemaNode::set (SchemaSignature ps, uint32_t lvl, SchemaSignature idx,
        int dt_id, Row::ID fid, uint8_t val_cat)
{
    m_dt = DataType::getDataType(dt_id);
    m_dt_id = dt_id;
    m_index = idx;

    m_field_id = fid;
    m_parent   = ps;

    m_level = lvl;
    m_vcate = val_cat;
} // set



inline
void SchemaNode::postfixTypeString(const string &n, string &s)
{
    s.append(n);
    s.append(1, '#').append(1, ('0' + m_vcate));
    s.append(1, '#').append(1, ('A' + m_dt_id));
} // postfixTypeString



inline 
void SchemaNode::output2debug(void)
{
    printf("--------------------------------------\n");
    printf("  m_dt_id:[%s][%d]:%c[%p]\n",
        DataType::s_type_desc[m_dt_id].name, m_dt_id, 'A'+m_dt_id, m_dt); 

    printf("  m_index    : [%u]\n", m_index   ); 
    printf("  m_field_id : [%u]\n", m_field_id);
    printf("  m_parent   : [%u], OX[%X]\n", m_parent, m_parent); 

    printf("  m_level    : [%u]\n", m_level); 
    printf("  m_vcate    : [%u]\n", m_vcate); 
    printf("  m_temp     : [%d]\n", m_temp ); 

    printf("  m_child [%lu]\n:", m_child.size());
    for (auto & ci : m_child)
    {   printf("[%u] ", ci);   }
    printf("\n--------------------------------------\n\n");
} // output2debug



inline
void SchemaNode::output2tree(const string &name)
{
    // output parent key:value pair  
    for (uint32_t li = 0; li < m_level; ++li) { printf("    "); }

    const char *key = name.c_str();
    int   dtid = m_dt_id;

//    printf("\"%s\": idx[%u] parent[%u] fid[%u] dt:[%s] cat[%u] temp[%s]\n", 
//        key, m_index, m_parent, m_field_id, DataType::s_type_desc[dtid].name, m_vcate,
//        m_temp ? "true" : "false");

    printf("\"%s\": idx[%u] parent[%u] dt:[%s] cat[%u]\n", 
        key, m_index, m_parent, DataType::s_type_desc[dtid].name, m_vcate);

} // output2tree



} // namespace steed
