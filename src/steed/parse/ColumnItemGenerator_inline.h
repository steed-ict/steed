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
 * @file ColumnItemGenerator_inline.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *     definitions and functions for ColumnItemGenerator inline funcs 
 */


#pragma once 

namespace steed {



inline
ColumnItemGenerator::ColumnItemGenerator(SchemaTree *st, CollectionWriter *cw):
        m_tree(st), m_clt_wt(cw)
{   resizeCounter();   }





inline
ColumnItemGenerator::~ColumnItemGenerator(void)
{
//    m_nd_cnt.outputAppearStatistic();
    m_nd_cnt.clear();
    
    m_tree   = nullptr;
    m_clt_wt = nullptr;
} // ctor 





inline int ColumnItemGenerator::
    generate(uint32_t s, array<JSONBinTree*, s_jtree_cap> &bts)
{
    int retval = 0; 
    for (uint32_t i = 0; i < s; ++i)
    {
        JSONBinTree *bt = bts[i];

#if _DEBUG_PARSER
        bt->output2debug();
        puts("");
#endif // _DEBUG_PARSER


        // got a record to generate ColumnItems:
        // pre-count the record number (tree's root counter)
        m_nd_cnt.updateRoot(); 

        retval =
            generate(bt, JSONBinField::Index(0), SchemaSignature(0), uint32_t(0));
        if (retval < 0)
        {
            printf("CIG: generate record failed!\n");
            bt->output2debug();
            break;
        } // if 

        bt->clear();
    } // for

    // flush the parsed content
    m_clt_wt->flush(); 

    return retval;
} // generate 





inline
int ColumnItemGenerator::generate(JSONBinTree* bt, JSONBinField::Index bt_idx,
        SchemaSignature sign, uint32_t rep)
{
    uint8_t     cate = m_tree->getCategory(sign);
    bool mtl = (cate == SchemaNode::s_vcat_multi);

    int   retval = -1;
    bool is_rept = bt->isRepeatedArray(bt_idx);
    if  (is_rept && mtl) // repeated array 
    {   retval = generateByArray(bt, bt_idx, sign, rep);   }
    else  // object + indexed array
    {   retval = generateByField(bt, bt_idx, sign, rep);   }

    return retval;
} // generate





inline
int ColumnItemGenerator::generateByArray(JSONBinTree* bt, JSONBinField::Index bt_idx,
        SchemaSignature sign, uint32_t rep)
{
    int  retval = -1;
    JSONBinField *bf  = bt->getNode(bt_idx);
    if (bf->useNonChild())
    {
        uint32_t pdef = m_tree->getLevel(sign);
        return checkChildAppeared(sign, rep, pdef);
    } // if 

    JSONBinField::Index cidx = bf->getChild(0);
    bool child_is_matrix = bt->isMatrix (cidx);
    if  (child_is_matrix) // matrix: repeated array in array
    {   retval = generateByMatrix    (bt, bt_idx, sign, rep);   }
    else // naive array: repeated other types in array 
    {   retval = generateByNaiveArray(bt, bt_idx, sign, rep);   }
    return retval;
} // generateByArray





inline
SchemaSignature ColumnItemGenerator::lookupSchema(JSONBinTree* bt,
        JSONBinField::Index cbf_idx, SchemaSignature psign, int dt_id, uint32_t vcate)
{
    JSONBinField *cbf = bt ->getNode(cbf_idx);
    const char   *key = cbf->getKeyPtr     ();

    string nd_name;
    SchemaTree::getNameFromText(nd_name, key); 
    const char      *nm = nd_name.c_str();
    SchemaSignature got = m_tree->findNode(nm, psign, dt_id, vcate); 
    return got; 
} // lookupSchema





inline
int ColumnItemGenerator::calcType(JSONBinTree *bt, JSONBinField::Index bt_idx)
{
    int   dt_id = DataType::s_type_invalid; 
    JSONBinField *bf = bt->getNode(bt_idx);
    if (bt->isRepeatedArray(bt_idx))
    {
        // elements in array have the same type  
        uint32_t cidx = bf->getChild(0);
        bf = bt->getNode(cidx); 
    } // if 
    
    // only primitive type  
    uint8_t jtype = bf->getValueType();
    dt_id = JSONTypeMapper::mapType (jtype);
    return  dt_id; 
} // calcType





inline
uint8_t ColumnItemGenerator::calcCategory(JSONBinTree *bt, JSONBinField::Index bt_idx)
{
    JSONBinField *n = bt->getNode (bt_idx);
    uint8_t cat = SchemaNode::s_vcat_invld;
    if (!n->isArray())
    {   cat = SchemaNode::s_vcat_single;   }
    else if (bt->isRepeatedArray(bt_idx))
    {   cat = SchemaNode::s_vcat_multi ;   }
    else
    {   cat = SchemaNode::s_vcat_index ;   }
    return cat;
} // calcCategory





inline
void ColumnItemGenerator::outputFieldInfo(JSONBinTree* bt, JSONBinField::Index bt_idx,
            uint32_t sign, const char *tag)
{
    uint32_t lvl = m_tree->getLevel(sign);
    for (uint32_t i = 0; i < lvl; ++i) { printf("\t"); }

    printf("[%s]: ", bt->isLeafField(bt_idx) ? "LF" : "NL");
    printf("%s <<sign:%X>>: ", tag, uint32_t(sign));
    JSONBinField *cbf = bt->getNode(bt_idx);
    cbf->output2debug(0);
} // outputFieldInfo 





inline
void ColumnItemGenerator::outputArrayInfo(JSONBinTree* bt, JSONBinField::Index bt_idx,
                uint32_t sign, const char *pt_tag, const char *cd_tag)
{
    JSONBinField *bf = bt->getNode(bt_idx); 
    if (!bf->isArray()) { return; }

    uint32_t eused = bf->getChildUsedNum();
    for (uint32_t ei = 0; ei < eused; ++ei)
    {
        JSONBinField::Index cbf_idx = bf->getChild(ei);
        outputFieldInfo(bt, cbf_idx, sign, pt_tag);

        JSONBinField *lf = bt->getNode(cbf_idx);
        uint32_t lfused = lf->getChildUsedNum();
        for (uint32_t li = 0; li < lfused; ++li)
        {
            JSONBinField::Index lbf_idx = lf->getChild(li);
            outputFieldInfo(bt, lbf_idx, sign, cd_tag);
        }
    } // for leaf  
} // outputArrayInfo





} // namespace steed 
