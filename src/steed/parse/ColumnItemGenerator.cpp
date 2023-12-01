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
 * @file ColumnItemGenerator.cpp
 * @author  Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   Defines funcs to class ColumnItemGenerator related funcs 
 */

#include "ColumnItemGenerator.h"


namespace steed {



int ColumnItemGenerator::
    generateByField(JSONBinTree* bt, JSONBinField::Index bt_idx,
        SchemaSignature sign, uint32_t rep)
{
    SchemaSignature csign = SchemaTree::s_invalid_sign;
    uint32_t   org = rep;   // original rep

    JSONBinField *bf = bt->getNode(bt_idx);
    uint32_t   eused = bf->getChildUsedNum();
    for (uint32_t ei = 0; ei < eused; ++ei)
    {
        JSONBinField::Index  cbf_idx = bf->getChild(ei);
        JSONBinField  *cbf = bt->getNode(cbf_idx);
        if (cbf->isNull()) { continue; }

        // get SchemaNode in SchemaTree 
        csign = updateSchema(bt, cbf_idx, sign, rep);

        // write 
        bool is_leaf = m_tree->isLeaf (csign);   // SchemaTree 
        bool nochild = bt->useNonChild(cbf_idx); // JSON text content 
        bool wt2col  = is_leaf && nochild; 
        if  (wt2col)
        {
            uint32_t    def = m_tree->getLevel(csign);
            const char *val = cbf->getValPtr ();
            m_clt_wt->write(csign, rep, def, val);
        }
        else if (generate(bt, cbf_idx, csign, rep) < 0)
        {
            puts("CIG::updateField failed!");
            return -1; 
        } // if 

        rep = org;
    } // for 


    // use parent def value to output nulls  
    uint32_t def = m_tree->getLevel(sign);
    if (checkChildAppeared(sign, rep, def) < 0)
    {
        puts("CIG:: checkChildAppeared failed!\n");
        return -1;
    } // checkChildAppeared

    return 0;
} // generateByField





int ColumnItemGenerator::
    generateByNaiveArray(JSONBinTree* bt, JSONBinField::Index bt_idx, 
        SchemaSignature sign, uint32_t rep)
{
    // generate repeated JSONBinField in arrays to one SchemaNode
    bool is_leaf = m_tree->isLeaf (sign); 
    JSONBinField *bf = bt->getNode(bt_idx);
    uint32_t   eused = bf->getChildUsedNum();
    for (uint32_t ei = 0; ei < eused; ++ei)
    {
        // set the repeated node appeared 
        // NOTE:
        // m_nd_cnt count from parent to child (appeared number)
        if (ei > 0)
        {   m_nd_cnt.updateNode(sign);   }

        JSONBinField::Index cbf_idx = bf->getChild(ei);
        JSONBinField *cbf = bt->getNode(cbf_idx);
        bool nochild = bt->useNonChild (cbf_idx);
        bool wt2col  = is_leaf && nochild; 
        if  (wt2col)
        {
            // primitive leaf  
            uint32_t    def = m_tree->getLevel(sign);
            const char *txt = cbf->getValPtr  ();
            m_clt_wt->write(sign, rep, def, txt);
        }
        else if (generate(bt, cbf_idx, sign, rep) < 0)
        {
            printf("CIG::updateField failed!\n");
            return -1; 
        } // if 

        rep = m_tree->getLevel(sign);
    } // for 

    return 0;
} // generateByNaiveArray





int ColumnItemGenerator::
    generateByMatrix(JSONBinTree* bt, JSONBinField::Index bt_idx,
        uint32_t sign, uint32_t rep)
{
    uint32_t org_rep = rep;   // original rep

    // get matrix schema node: return when [] 
    JSONBinField *bf = bt->getNode(bt_idx);
    if (bf->useNonChild()) { return 0; }


    auto cbf_idx = bf->getChild(0);

    // update counter in schema
    SchemaSignature csign = updateSchema(bt, cbf_idx, sign, rep); 


    // check JSONBinField 
    uint32_t def = m_tree->getLevel(csign);
    bool is_leaf = m_tree->isLeaf  (csign); 
    bool nochild = bt->useNonChild   (cbf_idx);
    bool wt2col  = is_leaf && nochild;
    if  (wt2col)
    {
        JSONBinField *cbf = bt ->getNode(cbf_idx);
        const char   *val = cbf->getValPtr ();
        m_clt_wt->write(csign, rep, def, val);
    }
    else 
    {
        uint32_t nrep = m_tree->getLevel(sign);
        uint32_t used = bf->getChildUsedNum ();
        for (uint32_t  ei = 0; ei < used; ++ei)
        {
            cbf_idx = bf->getChild(ei);
            if (generate(bt, cbf_idx, csign, rep) < 0)
            {
                printf("SampleTree::updateField failed!\n");
                return -1; 
            } // if 

            rep = nrep; 
        } // for
    } // if 


    // use original rep to output nulls
    rep =  org_rep;
    def =  m_tree->getLevel(sign);
    return checkChildAppeared(sign, rep, def);
} // generateByMatrix 





int ColumnItemGenerator::
    checkChildAppeared(SchemaSignature sign, uint32_t rep, uint32_t def)
{
    SchemaNode *sn = m_tree->getNode(sign);
    uint32_t  cnum = sn->getChildNum();
    for (uint32_t ci = 0; ci < cnum; ++ci)
    {
        SchemaSignature csign = sn->getChild(ci);
        if (m_nd_cnt.appeared(csign)) 
        {
            m_nd_cnt.clear(csign);
            continue;
        } // if 

    
        if (m_tree->isLeaf(csign))
        {
            m_clt_wt->write(csign, rep, def);
            continue;
        } // if 
    

        int s = checkChildAppeared(csign, rep, def); 
        if (s < 0)
        {
            puts("CIG: checkChildAppeared failed");
            return s;
        } // if 
    } // for ci


    // root node check child appeared 
    if (sign == 0)
    {   m_nd_cnt.updateAppearStatistic();   }


    return 0; 
} // checkChildAppeared





SchemaSignature ColumnItemGenerator::
    updateSchema(JSONBinTree* bt, JSONBinField::Index cbf_idx,
        SchemaSignature psign, uint32_t &rep)
{
    (void)rep;

    // lookup defined SchemaNode from SchemaTree 
    int     dt_id = calcType    (bt, cbf_idx);
    uint8_t vcate = calcCategory(bt, cbf_idx);
    SchemaSignature csign = SchemaTree::s_invalid_sign;
    csign = lookupSchema (bt, cbf_idx, psign, dt_id, vcate); 

    bool invalid = (csign == SchemaTree::s_invalid_sign);
    bool mtl_cat = (vcate == SchemaNode::s_vcat_multi  );
    if  (invalid && mtl_cat)
    {
        // text content is an array::
        //   lookup as a multiple value array failed,  
        //   try to lookup as an indexed value array.  
        int     invlt_id = DataType::s_type_invalid;
        uint8_t idx_cat  = SchemaNode::s_vcat_index;
        csign = lookupSchema(bt, cbf_idx, psign, invlt_id, idx_cat);
    } // if 



    // new appeared field: either SchemaNode, 
    // here we infer as a SchemaNode. 
    if (csign == SchemaTree::s_invalid_sign)
    {
        JSONBinField *cbf = bt ->getNode(cbf_idx);
        const char   *key = cbf->getKeyPtr();
        csign = createNode(key, psign, dt_id, vcate);
    } // if 

    if (csign == SchemaTree::s_invalid_sign) 
    {
        puts("CIG::updateSchema got SchemaNode failed!");
        return csign;
    } // if 


    m_nd_cnt.setAppear(csign);

    return csign;
} // updateSchema





SchemaSignature ColumnItemGenerator::
    createNode(const char *key, SchemaSignature psign, int dt_id, uint32_t vcate)
{
    // update SchemaTree: add SchemaNode 
    SchemaSignature sign = m_tree->getNextIndex(); 
    if (m_tree->addNode(key, psign, dt_id, vcate) < 0)
    {
        puts("CIG::createNode by addNode failed!");
        return SchemaTree::s_invalid_sign;
    } // if 

    uint64_t  nnum = m_tree->getNodeNum();
    m_nd_cnt.resize(nnum); 


    // update CollectionWriter:
    //   add column file 4 the new SchemaNodes
    bool leaf = m_tree->isLeaf(sign);
    if  (leaf)
    {
        if (m_clt_wt->initColumnWriter(sign, m_nd_cnt) < 0)
        {
            puts("CIG::createNode initColumnWriter failed!");
            return SchemaTree::s_invalid_sign;
        } // if 
    } // if 

    return sign;
} // createNode



} // namespace steed 
