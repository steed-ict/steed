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
 * @file SchemaTree_inline.h
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0 
 * @section DESCRIPTION
 *   SchemaTree inline functions
 */

#pragma once 

namespace steed {


inline
SchemaTree::SchemaTree (const string &db, const string &clt) :
    m_db_name(db), m_clt_name(clt)
{
    // create root node 
    m_nodes = new Container<SchemaNode>();
    m_nodes->appendNew();
    SchemaNode *r = getRoot();
    uint32_t rlvl = 0; 
    Row::ID  rfid = 0; 
    int     rdtid = DataType::s_type_invalid;
    uint8_t rvcat = SchemaNode::s_vcat_single;
    SchemaSignature pss = SchemaSignature(-1); // parent signature
    SchemaSignature rss = SchemaSignature( 0); // root   signature
    r->set(pss, rlvl, rss, rdtid, rfid, rvcat);

    // no need to add root to m_name_map
    m_names     .emplace_back("");
    m_node_valid.emplace_back(1);
} // ctor 



inline
SchemaTree::~SchemaTree(void)
{
    if (m_nodes != nullptr)
    { delete m_nodes; m_nodes = nullptr; }

    m_names     .clear();
    m_node_valid.clear();
    m_name_map  .clear();
} // dtor



inline
uint64_t SchemaTree::getLeafNum(void)
{
    uint64_t leaf = 0;
    uint64_t nnum = this->getNodeNum();
    for (uint64_t i = 0; i < nnum; ++i)
    {   leaf += this->isLeaf(i);   } 
    return leaf;
} // getLeafNum



inline
const string  SchemaTree::getNameWPost(SchemaSignature s)
{
    string ret;
    SchemaNode *sn = getNode(s);
    sn->postfixTypeString(m_names[s], ret);
    return ret;
} // getNameWPost



inline
void SchemaTree::getPath(SchemaSignature l, SchemaPath &p)
{
    SchemaPath  tmp;
    SchemaNode *sn = nullptr;
    do
    {
        tmp.emplace_back (l);
        sn = getNode(l); 
        l  = sn->getParent();
    } while (l != 0); // exclude root 

    uint32_t pdep = tmp.size();
    for(int pi = int(pdep - 1); pi >= 0; --pi)
    {   p.emplace_back(tmp[pi]);   }
} // getPath



inline
uint32_t SchemaTree::getRepeatedNumber(SchemaPath &path)
{
    uint32_t rnum = 0;
    uint64_t pdep = path.size();
    for (uint64_t l = 0; l < pdep; ++l)
    {
        SchemaSignature s = path[l];
        bool mlt = (getCategory(s) == SchemaNode::s_vcat_multi); // multi value 
        bool tmp = (isTemplate (s)); // SchemaTemplate parent 
        rnum += (mlt || tmp) ? 1 : 0;
    } // for 
    return rnum;
} // getRepeatedNumber



inline
uint32_t SchemaTree::getMaxRepeatLevel(SchemaPath &path)
{
    uint32_t depth = path.size(); 
    uint32_t level = depth - 1;
    do {
        SchemaSignature s = path[level];
        bool mlt = (getCategory(s) == SchemaNode::s_vcat_multi); // multi value 
        bool tmp = (isTemplate (s)); // SchemaTemplate parent 
        bool got = (mlt || tmp);
        if  (got)  { break; }
    } while (--level < depth);

    // got : level + 1 for considering the root node  
    // miss: uint32_t(-1) + 1 as 0 for nothing got  
    return level + 1;
} // getMaxRepeatLevel



inline
int SchemaTree::addNode(const char *k, SchemaSignature pidx, int dt_id, uint8_t vcate)
{
    SchemaSignature idx = getNextIndex();
    m_names     .emplace_back( );
    m_node_valid.emplace_back(1);
    SchemaNode *n = m_nodes->appendNew();
    if (n == nullptr)
    {
        printf("SchemaTree: addNode failed!\n");
        return -1; 
    } // if 

    string &nd_name = m_names.back();
    SchemaTree::getNameFromText(nd_name, k);

    SchemaNode::HashKey h(nd_name, pidx);
    m_name_map.emplace (h, idx);

    SchemaNode *p = this->getNode(pidx);
    n->set(p, idx, dt_id, m_next_fid++, vcate);

    return 0;
} // addNode



inline SchemaSignature
    SchemaTree::findNode(const char *k, SchemaSignature psign, int dt_id, uint8_t cate)
{
    SchemaSignature got_sign(-1); 
    auto range = this->findNode(k, psign);
    auto   cur = range.first; 
    auto   end = range.second; 
    while (cur != end)
    {
        SchemaNode* sn = getNode(cur->second);
        bool same_dt = ((int)sn->getDataTypeID() == dt_id);
        bool same_ct = (sn->getCategory() == cate);
        if  (same_dt && same_ct)
        {
            got_sign = cur->second;
            break;
        } // if 
        ++cur;
    } // while 
 
    return got_sign; 
} // findNode




inline
uint32_t SchemaTree::getLowestRepeatedNodeIndex(SchemaPath &path)
{
    uint32_t idx = path.size(); 
    while (--idx != uint32_t(-1))
    {
        SchemaSignature sign = path[idx];
        if  (isRepeated(sign)) { break; }
    } // while
    return idx;
} // getLowestRepeatedNodeIndex



inline
void SchemaTree::getNameFromText(string &name, const char *text)
{
    uint32_t  len = strlen(text);
    if (*text == '"')  { ++text; len -= 2; }

    name.assign(text, len);
} // getNameFromText



inline
int SchemaTree::appendPathName(string &str, SchemaPath &path)
{
    if (path.empty()) { return -1; }
    
    str.append(getName(path.front()));
    
    uint64_t pdep = path.size();
    for (uint64_t l = 1; l < pdep; ++l)
    {
        const string &n = getName(path[l]);
        str.append(1, '.').append(n);
    } // for 
    
    return 0;
} // appendPathName



inline
int SchemaTree::appendPathWPost(string &str, SchemaPath &path)
{
    if (path.empty()) { return -1; }
    
    str.append(getNameWPost(path.front()));
    
    uint64_t pdep = path.size();
    for (uint64_t l = 1; l < pdep; ++l)
    {
        const string &n = getNameWPost(path[l]);
        str.append(1, '.').append(n);
    } // for 
    
    return 0;
} // appendPathWPost



} // namespace
