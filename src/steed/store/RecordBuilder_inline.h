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
 * @file RecordBuilder_inline.h
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 0.4 
 * @section DESCRIPTION
 *   RecordBuilder inline functions
 */


#pragma once 

#include <stdlib.h>


namespace steed {
namespace Row {


inline
RecordBuilder::RecordBuilder(SchemaTree *tree, Buffer *buf, uint64_t md) :
    m_tree(tree), m_buf(buf), m_max_level(md - 1) // evict root level
{
    // avoid realloc  
    m_objs.reserve(m_max_level); 
    m_arrs.reserve(m_max_level);
    m_empty_array_elem.resize(m_max_level, 0);

    for (uint32_t i = 0; i < m_max_level; ++i)
    {
        m_objs.emplace_back(m_buf);
        m_arrs.emplace_back(m_buf);
    } // for 
} // ctor



inline
void RecordBuilder::erase(void)
{
    uint64_t used = m_buf->used();
    m_buf->deallocate(used - m_bgn_off);
    this ->clear();
} // erase



inline
void RecordBuilder::clear(void)
{
    for (auto &bld : m_objs) { bld.clear(); }
    for (auto &bld : m_arrs) { bld.clear(); }
    for (auto &empty : m_empty_array_elem)
    { empty = 0; }

    m_bgn_off    = 0; 
    m_recd_size  = 0;
    m_struct_len = 0;
    m_next_idx   = 0;
} // clear



inline
int RecordBuilder::begin2build(void)
{
    m_bgn_off   = m_buf->used();
    m_recd_size = 0;
    m_struct_len= 0;

    assert(m_next_idx == 0); 
    m_objs[m_next_idx++].init(); 
    return 0;
} // begin2build 





inline
void RecordBuilder::appendElement2Array(SchemaPath &sp, uint32_t nidx)
{
//?    // append new element: clear previous struct len 
//?    m_struct_len = 0;

    SchemaSignature s = sp[nidx];
    bool rept = m_tree->isRepeated(s);
    if  (rept)
    {
        assert(m_arrs[nidx].isInited());
        m_arrs[nidx].appendElem();
        m_empty_array_elem[nidx] = 0;
    } // if 
} // appendElement2Array





inline
int  RecordBuilder::appendLeafValue(uint32_t blen)
{
//?    // tail element is not struct:
//?    // clear m_struct_len to avoid double-count in upper level struct
//?    m_struct_len  = 0;

    int got = -1;
    uint32_t bld_idx = m_next_idx - 1;
    if (m_arrs[bld_idx].isInited())
    {
        // append an element to leaf array 
        got = 0;
        m_arrs[bld_idx].appendValue(blen);
    }
    else if (m_objs[bld_idx].isInited())
    {
        // append a member to leaf object
        got = 0;
        m_objs[bld_idx].appendValue(blen);
    } // if 
    
    return got;
} // appendLeafValue





inline
int  RecordBuilder::return2level(uint32_t tgt_lvl, SchemaPath &path, bool app_elem)
{
    int got = -1;

    if (app_elem) 
    {
        uint32_t com_lvl = tgt_lvl;        // common repeated level
        uint32_t com_idx = com_lvl    - 1; // common node index in SchemaPath 
        uint32_t cur_idx = m_next_idx - 1; // current builder's index
        if (cur_idx < com_idx)
        {
            // no need to return:
            // previous move2level did not move to a deeper level
            return 0; 
        } // if  


        // common SchemaNode in SchemaPath : must be repeated
        assert(m_tree->isRepeated( path[com_idx] ) == true);

        // repeated node must appeared; binary struct is inited 
        assert(m_next_idx >= com_lvl); 

        // common rept level = repeated SchemaNode's level 
        got = return2BuildArray(com_lvl, path);
    }
    else
    {
        /**
         * expected index in m_objs and m_arrs builder 
         * tgt_lvl is the last (lowest) existed node in the content should be involved, 
         * we contain the info in the binary row record.
         */
        uint32_t exp_idx = tgt_lvl; 
        uint32_t exp_lvl = exp_idx + 1; 
    
        // current builder index 
        uint32_t cur_idx = m_next_idx - 1;
        if (cur_idx < exp_idx)
        {
            // do not move to a deeper level: no need to return
            return 0;
        } // if 


        got = return2BuildObject(exp_lvl, path);

        // app_elem == false for REPEATED: got all elements in array
        // package array @ exp_idx 
        packageArrayByIndex     (exp_idx);


        // add new member to object builder at expected level
        // also add tail struct len as binary value
        RowObjectBuilder &exp_obj = m_objs[exp_idx]; 
        exp_obj.appendValue(m_struct_len);
        m_struct_len = 0;
    } // if 
    
    return got;
} // return2level 



inline
int RecordBuilder::return2BuildObject(uint32_t tgt_lvl, SchemaPath &sp)
{
    (void)sp;
    uint32_t tgt_idx     = tgt_lvl - 1;
    uint32_t exp_nxt_idx = tgt_idx + 1; // m_next_idx right below low_idx
    while    (m_next_idx > exp_nxt_idx) // m_next_idx > tgt_lvl+1 
    {
        // package the builders @ level (m_next_idx - 1)
        packageByIndex (--m_next_idx);
    } // while 

    return 0;
} // return2BuildObject 





inline
int  RecordBuilder::packageByIndex(uint32_t bld_idx)
{
    packageArrayByIndex (bld_idx);
    packageObjectByIndex(bld_idx);
    return 0; 
} // packageByIndex



inline
int  RecordBuilder::packageArrayByIndex (uint32_t bld_idx)
{
    // child tail struct binary length:
    //   LEAF:     append ==0 for nothing
    //   non-leaf: append !=0 for struct used 

    RowArrayBuilder &arr_bld = m_arrs[bld_idx];
    if  (arr_bld.isInited())
    {
        // current element is empty
        bool empty = (m_empty_array_elem[bld_idx] > 0); 
        if  (empty)
        {
            // append an empty element in array 
            assert (m_struct_len == 0);
            m_arrs[bld_idx].appendElem();
            m_empty_array_elem[bld_idx] = 0;
        } 
        else
        {
            arr_bld.appendValue(m_struct_len);
        } // if 
        
        m_struct_len  = arr_bld.package();
        arr_bld.clear();
    } // isInited
 
    return 0;
} // packageArrayByIndex



inline
int  RecordBuilder::packageObjectByIndex(uint32_t bld_idx)
{
    // child tail struct binary length:
    //   LEAF:     append ==0 for nothing
    //   non-leaf: append !=0 for struct used 

    RowObjectBuilder &obj_bld = m_objs[bld_idx];
    obj_bld.appendValue(m_struct_len); 
    m_struct_len  = obj_bld.package();
    obj_bld.clear();
 
    return 0; 
} // packageObjectByIndex



inline
uint32_t RecordBuilder::end2build(void)
{ 
    assert(m_next_idx == 1);

    packageByIndex(--m_next_idx);
    m_recd_size  = m_struct_len;
    m_struct_len = 0;
    
    return m_recd_size; 
} // end2build



} // namespace Row
} // namespace steed 





