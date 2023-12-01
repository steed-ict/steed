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
 * @file JSONBinField.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn> 
 * @version 1.0
 * @DESCRIPTION
 *  Define JSONBinField used to express JSON Field in Binary
 */

#pragma once 

#include <stdint.h>
#include <vector>

#include "JSONType.h"

namespace steed {

using std::vector;

/** 
 * --------------------------------------------------------------------------------
 * JSONBinField could be: 
 *     1. Key Value Pair in Object 
 *     2. Value Element  in Array
 * 
 * Combination for key and value type: U === upper level; L === lower level 
 *               leaf                        non-leaf 
 *   non-rep:    * m_key_ptr == txt_key;     * m_key_ptr == txt_key;
 *               * m_val_ptr == txt_val;       m_val_ptr == nullptr, non txt val for obj 
 *                 child meaningless         * children are nested fields
 *
 *   repeated:    << each elems in [] is saved in U level >>
 *           U: * m_key_ptr == txt_key;     U: * m_key_ptr == txt_key;
 *                m_val_ptr == nullptr;          m_val_ptr == nullptr;
 *              * child is prim elem in []     * child is elem obj in [] 
 *              
 *           L:   m_key_ptr == "index";     L:   m_key_ptr == "index"; // nested object
 *              * m_val_ptr == txt_val;          m_val_ptr == nullptr; // since non-leaf 
 *                child is meaningless         * child means fields appeared in each {} elem
 *   
 *   U present the fields and L for each elems in []: 
 *     JSONBinField level  === JSON bracket level (including {} and [])
 *   * means this attribute is valid 
 * --------------------------------------------------------------------------------
 */

class JSONBinField {
public:
    typedef uint64_t  Index;  /**< Field index in Container */

protected:
    const char       *m_key_ptr   {nullptr};   /**< key c-string   */
    const char       *m_val_ptr   {nullptr};   /**< val c-string   */

    Index             m_parent_idx{Index(-1)}; /**< parent index   */
    vector<Index>     m_child_idx { };         /**< child index    */
    uint32_t          m_cused     {0};         /**< child used num */
    uint8_t           m_val_type  {JSONType::s_invalid};  /**< json val type */


public: 
    ~JSONBinField(void) {} // do nothing: class Container will do everything 
    JSONBinField (void) = default;
    JSONBinField (const JSONBinField &f) { *this = f; }
    JSONBinField& operator = (const JSONBinField &f);

public:  
    void copyIns  (JSONBinField &f)  { *this = f; }
    void deleteIns(void)
    { m_key_ptr = m_val_ptr = nullptr; /*m_child_idx.clear();*/ }

public:  
    void set(uint8_t t, const char* k = nullptr, const char* v = nullptr)
    { m_key_ptr = k, m_val_ptr = v, m_val_type = t; }

    bool hasParent     (void) { return (m_parent_idx != Index(-1)); }
    bool isStruct      (void) { return isArray() || isObject()    ; }
    bool isArray       (void) { return JSONType::isArray    (m_val_type); }
    bool isObject      (void) { return JSONType::isObject   (m_val_type); }
    bool isNumber      (void) { return JSONType::isNumber   (m_val_type); }
    bool isPrimitive   (void) { return JSONType::isPrimitive(m_val_type); }
    bool isNullType    (void) { return JSONType::isNull     (m_val_type); }
    bool isEmptyArray  (void) { return (isArray () && useNonChild()); } 
    bool isEmptyObject (void) { return (isObject() && useNonChild()); }
    bool isNull        (void)
    { return isEmptyArray() || isEmptyObject () || isNullType(); }

    void  clear       (void);
    void  setParent   (Index idx) { m_parent_idx = idx; }
    void  appendChild (Index idx) { return m_child_idx.emplace_back(idx); }
    Index getNextChild(void) 
    { return useAllChild() ? Index(-1) : m_child_idx[m_cused++]; }

    uint32_t    getChildUsedNum(void) { return m_cused     ; }
    bool        useNonChild    (void) { return m_cused == 0; }
    bool        useAllChild    (void) { return m_cused == m_child_idx.size(); }

    void        abandonChild   (void) { m_child_idx.clear()  ; }
    Index       getParent      (void) { return m_parent_idx  ; } 
    Index       getChild (uint32_t i) { return m_child_idx[i]; }

    const char *getKeyPtr   (void) { return m_key_ptr ; }
    const char *getValPtr   (void) { return m_val_ptr ; }
    uint8_t     getValueType(void) { return m_val_type; }

public: 
    void output2debug(uint32_t level);
}; // JSONBinField 





inline
JSONBinField& JSONBinField::operator = (const JSONBinField &f)
{
    if (this != &f)
    {
        m_key_ptr    = f.m_key_ptr   ;
        m_val_ptr    = f.m_val_ptr   ;
        m_parent_idx = f.m_parent_idx;
        m_cused      = f.m_cused     ;
        m_val_type   = f.m_val_type  ;

        m_child_idx.assign(f.m_child_idx.begin(), f.m_child_idx.end());
    } // if 
    return *this;
} // operator =



inline
void JSONBinField::clear(void)
{
    m_cused = 0;
    m_val_type = JSONType::s_invalid;
    m_key_ptr  = m_val_ptr = nullptr;
} // clear 



inline
void JSONBinField::output2debug(uint32_t level)
{
    // output each element  
    for (uint32_t li = 0; li < level; ++li) { printf("\t"); }

    const char *key = ((m_key_ptr != nullptr) ? m_key_ptr : "(nil)");
    const char *val = ((m_val_ptr != nullptr) ? m_val_ptr : "(nil)");
    printf("<%s>:<%s> @[%p] val type:%u parent:[%lu]\n", key, val,
            this, m_val_type, m_parent_idx);
} // output2debug

} // namespace steed 
