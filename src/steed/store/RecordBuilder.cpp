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
 * @file RecordBuilder.cpp 
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for RecordBuilder 
 */

#include "RecordBuilder.h"

namespace steed {
namespace Row {



int RecordBuilder::move2level(uint32_t tgt_lvl, SchemaPath &path, bool app_elem)
{
    // binary row builder is inited  
    assert(m_next_idx > 0);

    uint32_t cur_idx = m_next_idx - 1; // current builder index for m_objs and m_arrs
    uint32_t tgt_idx = tgt_lvl    - 1; // target  builder index to move 


    // Nothing: no binary struct needs to append:
    //   Builder must move to a target field (tgt_lvl > 0, def value);
    //   Otherwise, this path has nothing needs to append.
    bool nothing = (tgt_lvl == 0); 

    // Existed: struct is already appended by its sibling columns:
    //   current level (cur_idx) is below target level in SchemaTree:
    //   this column's bin value is 'nil' and has nothing append to RecordBuilder 
    bool existed = (cur_idx > tgt_idx);
    if  (nothing || existed)
    {   return 0;   } 


    // append array element (at current index) to the builder
    if (app_elem)
    {
        // SchemaNode path[cur_idx] is repeated, 
        // the first element in array is built with param app_elem = false, 
        // the following elements     is built with param app_elem = true . 
        assert(m_arrs[cur_idx].isInited());


        // set as an empty element
        m_empty_array_elem[cur_idx] = 1;

        // append primitive value 2 leaf array 
        // m_next_idx is already returned to the common level, which is repeated level 
        // tgt_idx is the target level move to 
        uint32_t depth  = path.size();
        uint32_t leaf_idx = depth - 1; 
        bool append_2_leaf= (cur_idx == leaf_idx);
        if  (append_2_leaf) // avoid duplicately append ids
        {
            m_arrs[cur_idx].appendElem();  
            m_empty_array_elem[cur_idx] = 0;
            return 0;
        }  // if 
    } 
    else 
    {
        // For the SchemaNode path  [cur_idx], which is SN: 
        //   If SN is single  , m_arrs[cur_idx] is not inited yet.
        //   If SN is repeated, m_arrs[cur_idx] is packaged in return2level;
        //
        // Check and append the SchemaNode @ cur_idx in SchemaPath:
        //     cur_idx == m_next_idx - 1 
        appendField2Struct(path, cur_idx);
    } // if 

    
    // build deeper level binary struct:
    // add element in current binary array, if needed 
    if (cur_idx < tgt_idx)
    {   appendElement2Array(path, cur_idx);   }


    // deeper level 
    while (cur_idx < tgt_idx)
    {
        // init deeper level  
        cur_idx = m_next_idx++;
        RowObjectBuilder &obj_bld = m_objs[cur_idx];
        assert (!obj_bld.isInited());
        obj_bld.init(); 

        appendField2Struct(path, cur_idx);
        if (cur_idx < tgt_idx)
        {
            // do not append element into array until necessary
            appendElement2Array(path, cur_idx);
        } // if 
    } // while 


    uint32_t depth = path.size();
    uint32_t leaf_idx = depth - 1; 
    bool append_2_leaf = (cur_idx == leaf_idx);
    if  (append_2_leaf)
    {
        appendElement2Array(path, cur_idx);
    } // if 


    return 0;
} // move2level





void RecordBuilder::appendField2Struct(SchemaPath &sp, uint32_t nidx)
{
//?    // append new field: clear previous struct len 
//?    m_struct_len = 0;

    SchemaSignature s =  sp[nidx];
    Row::ID id = m_tree->getFieldID(s);
    bool  rept = m_tree->isRepeated(s);

    RowArrayBuilder  &arr_bld = m_arrs[nidx];
    RowObjectBuilder &obj_bld = m_objs[nidx];

    assert(obj_bld.isInited());
    if    (obj_bld.haveIDs ()) 
    {
        // field appeared in object  
        bool appeared = (obj_bld.getBackID() == id);
        if  (appeared)
        {
            // Any id in a binary object struct appears only once: 
            // if field's id is already appended: do nothing
            // previous SchemaPath (shares the same parent) is 'nil' 
            return;
        }
        else 
        {
            // package previous field's child array
            packageArrayByIndex(nidx);

            // update field's child array to parent 
            obj_bld.appendValue(m_struct_len); 
            m_struct_len = 0; // reset for new field 
        } // if 
    } // if 


    obj_bld.appendElem(id);

    if (rept)
    {
        assert(!arr_bld.isInited());

        // array @ deeper level: always init 
        arr_bld.init(); 
        m_empty_array_elem[nidx] = 1;
    } // if 
} // appendField2Struct





int RecordBuilder::return2BuildArray(uint32_t com_lvl, SchemaPath &sp)
{
    // root SchemaNode is single, not repeated
    assert(com_lvl != 0);

    // repeated @ leaf node 
    uint32_t lf_lvl = sp.size (); 
    uint32_t lf_idx = lf_lvl - 1;
    bool leaf = (lf_idx + 1 == m_next_idx); // current is building leaf 
    bool rept = (lf_lvl == com_lvl);        // repeated at leaf node 
    if  (leaf && rept)
    {
        // m_next_idx - 1 == leaf index
        assert( m_arrs[lf_idx].isInited() );
    
        // since repeated @ the leaf node:
        //   next def == nrep must be true 

        return 0;
    } // if 


    // repeated @ non-leaf node:
    // package lower content and prepare the next element 
    return2BuildObject(com_lvl, sp);


    // add new elements to array builder at common level
    // also add tail struct len as binary value
    uint32_t com_idx = com_lvl - 1; // com_idx == m_next_idx - 1 
    RowArrayBuilder &com_arr = m_arrs [com_idx]; 
    assert (com_arr.isInited());

    bool empty = (m_empty_array_elem[com_idx] > 0); 
    if  (empty)
    {
        // append an empty element in array 
        assert (m_struct_len == 0);
        com_arr.appendElem();
        m_empty_array_elem[com_idx] = 0;
    } // if 

    com_arr.appendValue(m_struct_len);
    m_struct_len = 0;


    return 0;
} // return2BuildArray 


} // namespace Row
} // namespace 
