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
 * @file ColumnItemGenerator.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *     ColumnItemGenerator generates ColumnItems from JSONBinTree by SchemaTree
 */

#pragma once 

#include <array>

#include "TreeCounter.h"

#include "SchemaTree.h"
#include "JSONBinTree.h"
#include "JSONTypeMapper.h"
#include "CollectionWriter.h"


namespace steed {
using std::array;

class ColumnItemGenerator {
protected:
    /**
     * SchemaNode appeared counter
     * It is a pre-count counter  
     */
    TreeCounter         m_nd_cnt       {};
    SchemaTree         *m_tree  {nullptr}; /**< related SchemaTree instance */
    CollectionWriter   *m_clt_wt{nullptr}; /**< collection writer 4 columns */
    
    /**< record used in m_jtree */
    static const uint64_t s_jtree_cap{16};


public:
    ColumnItemGenerator (SchemaTree *st, CollectionWriter *cw);
    ~ColumnItemGenerator(void);


public:
    /** resize member flag */
    void resizeCounter(void)
    { m_nd_cnt.resize (m_tree->getNodeNum()); }

    /**
     * get SchemaNode appeared counter
     * @return TreeCounter instance 
     */
    TreeCounter *getCounter(void)
    { return &m_nd_cnt; }

    /**
     * generate JSONBinTree instances in array to SchemaTree
     * @param s    array used size
     * @param bts  JSONBinTree array 
     * @return 0 success; <0 failed
     */
    int generate(uint32_t s, array<JSONBinTree*, s_jtree_cap> &bts);

private: // generate ColumnItem
    /**
     * generate ColumnItems from one JSONBinField using SchemaTree 
     * @param bt        JSONBinTree instance 
     * @param bt_idx    field index in JSONBinTree
     * @param sign      SchemaNode signature in SchemaTree 
     * @param rep       repeated value calc by silbing path 
     * @return 0 success; <0 failed
     */
    int generate(JSONBinTree* bt, JSONBinField::Index bt_idx,
            SchemaSignature sign, uint32_t rep);

    /**
     * generate Field JSONBinField in JSONBinTree to SchemaTree
     * NOTE:Field includes K:V pair and Indexed Array  
     * 
     * @param bt         JSONBinTree instance 
     * @param bt_idx     field index in JSONBinTree
     * @param sign       SchemaNode signature in SchemaTree 
     * @param rep        repeated value calc by silbing path 
     * @return 0 success; <0 failed
     */
    int generateByField(JSONBinTree* bt, JSONBinField::Index bt_idx, 
            SchemaSignature sign, uint32_t rep);

    /**
     * generate Array JSONBinField to SchemaTree
     * NOTE:Array is short for REPEATED Array
     * 
     * @param bt         JSONBinTree instance 
     * @param bt_idx     field index in JSONBinTree
     * @param sign       SchemaNode signature in SchemaTree 
     * @param rep        repeated value calc by silbing path 
     * @return 0 success; <0 failed
     */
    int generateByArray(JSONBinTree* bt, JSONBinField::Index bt_idx, 
            SchemaSignature sign, uint32_t rep);

    /**
     * generate Naive Array JSONBinField to SchemaTree
     * Naive Array == REPEATED Array
     * 
     * @param bt          JSONBinTree instance 
     * @param bt_idx      field index in JSONBinTree
     * @param sign        SchemaNode signature in SchemaTree 
     * @param rep         repeated value calc by silbing path 
     * @return 0 success; <0 failed
     */
    int generateByNaiveArray(JSONBinTree* bt, JSONBinField::Index bt_idx, 
            SchemaSignature sign, uint32_t rep);

    /**
     * generate matrix Array JSONBinField to SchemaTree
     * Matrix Array == [[]], the parent got single child  
     * 
     * @param bt          JSONBinTree instance 
     * @param bt_idx      field index in JSONBinTree
     * @param sign        SchemaNode signature in SchemaTree 
     * @param rep         repeated value calc by silbing path 
     * @return 0 success; <0 failed
     */
    int generateByMatrix(JSONBinTree* bt, JSONBinField::Index bt_idx, 
            SchemaSignature sign, uint32_t rep);

    /**
     * check child appeared and output null for alignment 
     * @param sign    parent SchemaNode signature
     * @param rep     repetition value for alignment
     * @param def     definition value for alignment
     * @return 0 success; <0 failed
     */
    int checkChildAppeared(SchemaSignature psign, uint32_t rep, uint32_t def);


private:
    /**
     * update Schema: SchemaNode appearance and new appeared  
     * @param bt         JSONBinTree instance 
     * @param cbf_idx    child field index in JSONBinTree
     * @param psign      parent    SchemaNode signature in SchemaTree 
     * @param rep        repetition value 
     * @return SchemaNode SchemaSignature
     */
    SchemaSignature updateSchema  (JSONBinTree* bt, JSONBinField::Index cbf_idx,
            SchemaSignature psign, uint32_t &rep);

    /**
     * using JSONBinField to lookup in SchemaTree 
     * @param bt        JSONBinTree instance 
     * @param cbf_idx   child field index in JSONBinTree
     * @param psign     parent    SchemaNode signature in SchemaTree 
     * @param dt_id     SchemaNode data type id 
     * @param cate      possible SchemaNode  category
     * @return SchemaNode SchemaSignature
     */
    SchemaSignature lookupSchema  (JSONBinTree* bt, JSONBinField::Index cbf_idx,
            SchemaSignature psign, int dt_id, uint32_t vcate);

    /**
     * create SchemaNode in SchemaTree 
     * @param key       SchemaNode key name string 
     * @param psign     parent    SchemaNode signature in SchemaTree 
     * @param dt_id     SchemaNode data type id 
     * @param cate      possible SchemaNode  category
     * @return SchemaNode SchemaSignature
     */
    SchemaSignature createNode(const char *key, SchemaSignature psign,
            int dt_id, uint32_t vcate);

    /**
     * calculate the DataType from JSONBinField node 
     * @param bt         JSONBinTree instance 
     * @param bt_idx     field index in JSONBinTree
     * @return 0 success; <0 failed
     */
    int     calcType    (JSONBinTree *bt, JSONBinField::Index bt_idx);

    /**
     * calculate the category from JSONBinField node 
     * @param bt         JSONBinTree instance 
     * @param bt_idx     field index in JSONBinTree
     * @return 0 success; <0 failed
     */
    uint8_t calcCategory(JSONBinTree *bt, JSONBinField::Index bt_idx);


private: // debug 2 output 
    /**
     * output field info 
     * @param bt         JSONBinTree instance 
     * @param bt_idx     field index in JSONBinTree
     * @param sign       SchemaNode Signature 
     * @param tag        output tag string  
     * @return 0 success; <0 failed
     */
    void outputFieldInfo(JSONBinTree* bt, JSONBinField::Index bt_idx,
                uint32_t sign, const char *tag);

    /**
     * output array info 
     * @param bt         JSONBinTree instance 
     * @param bt_idx     field index in JSONBinTree
     * @param sign       SchemaNode Signature 
     * @param pt_tag     parent output tag string  
     * @param cd_tag     child  output tag string  
     * @return 0 success; <0 failed
     */
    void outputArrayInfo(JSONBinTree* bt, JSONBinField::Index bt_idx,
                uint32_t sign, const char *pt_tag, const char *cd_tag);
}; // ColumnItemGenerator



} // namespace steed


#include "ColumnItemGenerator_inline.h"
