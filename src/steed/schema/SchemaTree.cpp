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
 * @file SchemaTree.cpp
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0 
 * @section DESCRIPTION
 *   SchemaTree functions
 */

#include "Block.h"
#include "SchemaTree.h"

namespace steed {

extern steed::Config g_config;

//extern
//int steed::block::load2Buffer(Buffer *buf);


int SchemaTree::getAlignBasePath(SchemaPath &sp, SchemaPath &base,
            uint32_t &same_rep, uint32_t &same_def)
{
    assert (base.empty()); 
    assert (isLeaf(sp.back()));

    // look up the same deepest parent level @ nidx in SchemaPath 
    SchemaNode *sn= nullptr;
    uint32_t nnum = sp.size();
    uint32_t nidx = nnum;
    while (--nidx < nnum)
    {
        SchemaSignature ss = sp[nidx];
        sn = getNode(ss);
        uint32_t cnum = sn->getChildNum();
        bool new_temp = ((isTemplate(ss)) && (cnum == 2));
        bool no_sib   = (cnum <= 1);
        if  (no_sib || new_temp)
        {   continue;   } 
        else
        {   break   ;   }
    } // for

    if (nidx > nnum) 
    {
        // iterate done: none path can be the base path
        // all parents have single child, which is just created with empty content
        // all previous CABs are s_trivial (none parent is shared) 
        same_rep = same_def = 0; 
        return 0;
    } // if 


    // prepare base and calc same rep and def value  
    uint32_t nlvl = nidx + 1; // trans index to node level
    same_rep = 0, same_def = nlvl; 
    for (uint32_t ni = 0; ni <= nidx; ++ni)
    {
        SchemaSignature ss = sp[ni];
        base.emplace_back(ss);  
        same_rep = isRepeated(ss) ? ni + 1 : same_rep; 
    } // for
 
        
    // get a base SchemaPath from the same parent 
    uint32_t cidx = nidx + 1; // child index in SchemaPath
    uint32_t cnum = sn->getChildNum();
    SchemaSignature css = sp[cidx]; // child SchemaSignature
    for (uint32_t ci = 0; ci < cnum; ++ci)
    {
        SchemaSignature s = sn->getChild(ci);
        if (css == s) { continue; } // ignore my parents
        else    { css = s; break; } // TODO: better
    } // for 
    base.emplace_back (css);
    assert(css != sp[cidx]);


    // get child SchemaNode until leaf 
    do {
        SchemaNode *csn = getNode(css); 
        cnum = csn->getChildNum();
        if (cnum == 0)  { break; } // leaf 

        // TODO: always choice the first child  
        css = csn->getChild(0);
        base.emplace_back(css); 
    } while (true);

    return 0;
} // getAlignBasePath





int  SchemaTree::addTemplate(SchemaSignature pidx, int dt_id, uint8_t vcate)
{
    // Template's root SchemaNode 
    SchemaSignature temp_sign = getNextIndex();
    int         temp_dtid = dt_id;
    uint8_t     temp_cate = vcate;
    const char *temp_name = g_config.m_schema_temp_name.c_str();
    if (addNode(temp_name, pidx, temp_dtid, temp_cate) < 0)
    {
        printf("SchemaTree: addTemplate failed!\n");
        return -1; 
    }
    SchemaNode *temp_sn = getNode(temp_sign);
    temp_sn->setTemplateFlag(); 

    // Template's Key Child SchemaNode
    int         tkey_dtid  = DataType::s_type_string;
    uint8_t     tkey_vcate = SchemaNode::s_vcat_single;
    const char *tkey_name  = g_config.m_schema_temp_key.c_str();
    if (addNode(tkey_name, temp_sign, tkey_dtid, tkey_vcate) < 0)
    {
        printf("SchemaTree: addTemplate Key failed!\n");
        return -1; 
    }

    // Template's Value Child SchemaNode
    int         tval_dtid  = dt_id;
    uint8_t     tval_vcate = vcate;
    const char *tval_name  = g_config.m_schema_temp_val.c_str();
    if (addNode(tval_name, temp_sign, tval_dtid,  tval_vcate) < 0)
    {
        printf("SchemaTree: addTemplate Key failed!\n");
        return -1; 
    }

    return 0;
} // addTemplate 





SchemaSignature SchemaTree::findTemplate(SchemaSignature psign, int dt_id, uint8_t cate)
{
    int         temp_dtid = dt_id; // the same dt_id to Template Value
    uint8_t     temp_cate = cate;  // the same cate  to Template Value 
    const char *temp_name = g_config.m_schema_temp_name.c_str();
    return findNode(temp_name, psign, temp_dtid, temp_cate);
} // findTemplate





int SchemaTree::flush(void)
{
    string path; 
    Utility::getSchemaPath(g_config, m_db_name, m_clt_name, path);

//printf("SchemaTree::[%s]\n", path.c_str());
    
    Buffer *fb = new Buffer();
    if (fb->init2write(path) < 0)
    {
        printf("SchemaTree: init Buffer write failed!\n");
        return -1; 
    } // if 
    
    
    // block header 
    uint64_t blk_use = sizeof(block::Block); // block used size  
    fb->allocate(blk_use, false);

    // SchemaNode array  
    int64_t used = m_nodes->flush2buffer(fb);
    if (used < 0)
    {
        printf("SchemaTree: flush failed !\n");
        return -1;
    }
    else
    {   blk_use += used;   }
    
    // name string array
    for (auto & n : m_names)
    {
        uint32_t s = n.size() + 1; // with '\0'
        void  *bgn = fb->allocate(s, true); 
        memcpy(bgn, n.data(), s);
        blk_use += s;
    }
    
    // SchemaNode valid array 
    {
        uint32_t nnum = getNodeNum();
        uint32_t s = sizeof(uint8_t) * nnum;
        void  *bgn = fb->allocate(s, true); 
        memcpy(bgn, m_node_valid.data(), s);
        blk_use += s;
    }

    // update size info 
    block::Block *blk = (block::Block*)fb->getPosition(0);
    blk->m_size = blk_use;

    // flush content 2 file 
    if (fb->flush2File() < 0)
    {
        printf("SchemaTree:: flush Buffer flush2File failed!\n");
        return -1;
    }
    
    delete fb; fb = nullptr;
    return 0;
} // flush





int SchemaTree::load(void)
{
    // clear this to prepare to load from file  
    m_nodes->clear();
    m_names .clear();
    m_node_valid.clear();


    // begin to build from file  
    string path; 
    Utility::getSchemaPath(g_config, m_db_name, m_clt_name, path);
    
    Buffer *lb = new Buffer();
    if (lb->init2read(path) < 0)
    {   return 0;   } 
    

    block::load2Buffer(lb);
    uint64_t rd_off = sizeof(block::Block);

    // SchemaNode array 
    int64_t used = m_nodes->load2buffer(lb, rd_off);
    if (used < 0)
    {
        printf("SchemaTree: load2buffer failed!\n");
        return -1;
    }
    rd_off += used; 
    uint32_t nnum = getNodeNum();

    // update DataType pointers in SchemaNode 
    for (uint32_t ni = 0; ni < nnum; ++ni)
    {
        SchemaNode *sn = getNode(ni);
        int dt_id = sn-> getDataTypeID();
        DataType   *dt = DataType::getDataType (dt_id);
        sn->setDataType(dt);
    } // for
 
    // name string array
    for (uint32_t ni = 0; ni < nnum; ++ni)
    {
        char *n = (char*)lb->getPosition(rd_off); 
        m_names.emplace_back(n);
        rd_off += m_names.back().size() + 1;
    } // for 
    
    // SchemaNode valid array 
    uint8_t *rd_vld = (uint8_t*)lb->getPosition(rd_off); 
    for (uint32_t ni = 0; ni < nnum; ++ni)
    {   m_node_valid.emplace_back(rd_vld[ni]);   }
    rd_off += sizeof(uint8_t) * nnum;

    // other set member vars
    SchemaNode *tail = m_nodes->get(nnum-1);
    Row::ID tid = tail->getFieldID(); 
    m_next_fid = tid + 1;

    // build the between relation of SchemaNode and its hashtable
    for (uint32_t ni = 1; ni < nnum; ++ni) 
    {
        SchemaNode *sn = m_nodes->get(ni);
        uint32_t  pidx = sn->getParent();
        SchemaNode *pn = m_nodes->get(pidx);
        pn->addChild(ni);
        
        SchemaNode::HashKey h(m_names[ni], pidx);
        m_name_map.emplace(h, ni);
    } // for 

    delete lb; lb = nullptr;
    return 1; 
} // load



void SchemaTree::getDirPath(string &path)
{
    Utility::getSchemaPath(g_config, m_db_name, m_clt_name, path);
} // getDirPath






void SchemaTree::output2debug(void)
{
    printf("\n\n>> SchemaTree output2debug:");
    printf("\n0 ========================================\n");

    printf("db:[%s] table:[%s] Node #: %lu next id:%u\n",
        m_db_name.c_str(), m_clt_name.c_str(), m_names.size(), m_next_fid);

    printf("\n0 ========================================\n");

#if 1
    uint32_t nnum = m_nodes->size(); 
    for (uint32_t ni = 0; ni < nnum; ++ni)
    {
        SchemaNode *n = m_nodes->get(ni);
        printf("SchemaNode @ [%p][%u] [%s] valid:[%s]\n",
            n, ni, m_names[ni].c_str(), isEnabled(ni) > 0 ? "true" : "false");
        
        n->output2debug(); 
    } // for
    puts("\n\n1 ========================================\n\n");
#endif 

#if 0
    printf("\n\n>> SchemaNode HashTable:");
    printf("\n2 ========================================\n");
    for (auto & slot : m_name_map)
    {
        printf("[%u]\n", slot.second);
        slot.first.output2debug();
        puts("");
        this->getNode(slot.second)->output2debug();
    } // for 
    puts("\n\n\n");
#endif

    puts("all candidate path expressions are:");
    outputPathes();
} // output2debug





void SchemaTree::outputPathes4Node(SchemaPath &sp, SchemaSignature ss)
{
    SchemaNode *sn = getNode(ss); 
    uint32_t  cnum = sn->getChildNum();
    for (uint32_t ci = 0; ci < cnum; ++ci)
    {
        SchemaSignature csign = sn->getChild(ci);
        sp.emplace_back(csign);
        if (!isLeaf(csign))
        {
            outputPathes4Node(sp, csign);
        } 
        else
        {
            string s1, s2; 
            appendPathWPost(s1,sp);
            appendPathName (s2,sp);

            sp.output2debug();
//-            printf("%s\n",  s1.c_str());
            printf("%s\n",  s2.c_str());
            puts("");
        } // if 
        
        sp.pop_back();
    } // for 
} // outputPathes4Node





void SchemaTree::output2tree(const string &name, SchemaSignature ss)
{
    SchemaNode *sn = getNode(ss);
    sn->output2tree(name);
    
    uint32_t  cnum = sn->getChildNum();
    for (uint32_t ci = 0; ci < cnum; ++ci)
    {
        SchemaSignature css = sn->getChild(ci);
        const string &cnm = this->getName(css); 
        output2tree(cnm, css);
    } // for ci
} // output2tree



} // namespace steed
