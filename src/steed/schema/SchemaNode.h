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
 * @file SchemaNode.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   Schema Node definition to build Schame tree
 */

#pragma once 

#include <stdint.h>

#include <string>
#include <vector>
#include <functional>  // hash
#include <unordered_map>

#include "Row.h"
#include "Config.h"
#include "DataType.h"
#include "SchemaSignature.h"
#include "SchemaPath.h"


namespace steed {

using std::hash;  // hash template func 
using std::pair;   
using std::string; 
using std::vector;
using std::unordered_multimap; 



class SchemaNode  {
public: // Schema node hash table
    class HashKey {
    public:
        string           m_name{""}; /**< field name string  */
        SchemaSignature  m_pidx {0}; /**< parent Schema sign */
    
    public:
        ~HashKey(void) = default; 
        HashKey (void) = default; 
        HashKey (const HashKey& h) : m_name(h.m_name), m_pidx(h.m_pidx) {}
        HashKey (const string & n, SchemaSignature pidx) :
                m_name(n), m_pidx(pidx) {}
    
        void output2debug(void) const
        { printf("[%s] parent@[%u]", m_name.c_str(), m_pidx); }
    }; // HashKey
    
    class Hasher {
    protected:
        hash<string>           m_hash_str ;
        hash<SchemaSignature>  m_hash_sign;
    
    public:
        Hasher (void) = default; 
        ~Hasher(void) = default; 
        size_t operator() (HashKey const& h) const
        { return (m_hash_str(h.m_name) + m_hash_sign(h.m_pidx)); } 
    }; // Hasher
    
    class KeyEqual {
    public:
        bool operator() (const HashKey & l, const HashKey & r) const
        { return ((l.m_pidx == r.m_pidx) && (l.m_name.compare(r.m_name) == 0)); }
    }; // KeyEqual
    
    /**
     * Hash Table Define
     * @param HashKey          hash key  : struct Key
     * @param SchemaSignature  hash value: SampleNode index in tree 
     * @param Hasher           hash func : use user default function 
     * @param KeyEqual         overload operator () to calc key equal 
     */
    typedef unordered_multimap<HashKey, SchemaSignature, Hasher, KeyEqual> HashTable;
    typedef HashTable::const_iterator   hash_citr;
    typedef pair<hash_citr, hash_citr>  hash_crange;


protected:
    DataType           *m_dt {nullptr};  /**< data type ins    */
    int                 m_dt_id{DataType::s_type_invalid}; /**< type id*/
    SchemaSignature     m_index    {0};  /**< node index tree  */

    Row::ID             m_field_id {0};  /**< unique field id  */
    SchemaSignature     m_parent   {0};  /**< parent index     */ 
    vector<SchemaSignature>  m_child{};  /**< child  index     */

    uint16_t  m_level{0};    /**< nested level == def value*/

    /** 
     * field's value category:
     *     s_vcat_invld, s_vcat_single, s_vcat_multi, s_vcat_index 
     */
    uint8_t   m_vcate{s_vcat_invld};  

    /**
     * this SchemaNode is root of SchemaTemplate:
     * SchemaTemplate is a multiple object SchemaNode, which means:
     *     m_dt    should be DataType::s_type_invalid; 
     *     m_vcate should be SchemaNode::s_vcat_multi;
     * but use m_dt and m_vcate to distinguish the SchemaTemplates
     *   from the same parent 
     */
    bool      m_temp {false};


public:
    static const uint8_t s_vcat_invld  = 0;  /**< invalid  value category */
    static const uint8_t s_vcat_single = 1;  /**< single   value category */
    static const uint8_t s_vcat_multi  = 2;  /**< multiple value category */
    static const uint8_t s_vcat_index  = 3;  /**< indexed  value category */
    static const uint8_t s_vcat_max    = 4;  /**< max      value category */
 
public:
    ~SchemaNode (void) = default; 
    SchemaNode  (void) = default;
    SchemaNode  (const SchemaNode &n)  { *this = n; }
    SchemaNode& operator= (const SchemaNode &n);

public:
    /** parent is SchemaNode */
    void set(SchemaNode  *p, SchemaSignature idx, int dt_id, Row::ID fid,
            uint8_t val_cat);

    /** set root node */
    void set(SchemaSignature ps, uint32_t lvl, SchemaSignature idx, int dt_id,
            Row::ID fid, uint8_t val_cat);

    SchemaSignature  getNodeIndex   (void) { return m_index ; }
    SchemaSignature  getParent      (void) { return m_parent; }
    SchemaSignature  getChild(uint32_t  i) { return m_child.at(i); }

    void      addChild     (SchemaSignature c) { m_child.push_back (c); }
    void      clearChild   (void) { m_child.clear      (); }
    uint32_t  getChildNum  (void) { return m_child.size(); }

    void      setDataType  (DataType *dt) { m_dt =dt; }
    DataType *getDataType  (void) { return m_dt; }
    Row::ID   getFieldID   (void) { return m_field_id; }
    int       getDataTypeID(void) { return m_dt_id; }
    uint32_t  getLevel     (void) { return m_level; }
    uint32_t  getDefValue  (void) { return getLevel(); }
    uint8_t   getCategory  (void) { return m_vcate; }
    bool      isIndexArray (void) { return m_vcate == s_vcat_index; }
    bool      isMultiArray (void) { return m_vcate == s_vcat_multi; }
    bool      isTemplate   (void) { return m_temp ; }
    bool      isLeaf       (void)
    { return ((!m_temp) && (m_dt->isPrimitive())); }

    void      setTemplateFlag(void) { m_temp = true; } 
    bool      getTemplateFlag(void) { return m_temp; }

public:
    void copyIns  (SchemaNode &n) { *this = n; m_child = n.m_child; }
    void deleteIns(void)          { m_child.clear(); }

public:
    /** 
     * append storage name as postfix
     * @param n  field name string  
     * @param s  storage path string 
     */
    void postfixTypeString(const string &n, string &s);

public: 
    void output2debug(void);

public:
    /** output in tree format */
    void output2tree(const string &name);
}; // SchemaNode

} // namespace steed


#include "SchemaNode_inline.h"