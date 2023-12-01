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
 * @file JSONBinTree.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn> 
 * @version 1.0
 * @DESCRIPTION
 *    Define JSONBinTree used to express one JSON text Record to a binary tree
 */

#pragma once 

#include "Container.h"
#include "JSONBinField.h"

namespace steed {

class JSONBinTree {
public:
    typedef JSONBinField::Index Index; /**< JSONBinField Index in tree */

protected:
    Container<JSONBinField>    *m_nodes{nullptr}; /**< nodes Container */

public:
    JSONBinTree (void);
    ~JSONBinTree(void)   { delete m_nodes ; m_nodes = nullptr; }

public:
    /** clear all record related info, rather the struct info */
    void          clear  (void);
    JSONBinField *getRoot(void)     { return this->getNode(0); }
    JSONBinField *getNode(Index i)  { return m_nodes->get (i); }

    /**
     * get next child of JSONBinField
     * @param pidx    parent JSONBinField index in JSONBinTree
     * @return child index in JSONBinTree; -1 as error 
     */
    Index getNextChild(uint32_t pidx);

    /**
     * set new child JSONBinField
     * @param pidx    parent JSONBinField index in JSONBinTree
     * @param t       child type
     * @param k       child key   c-string 
     * @param v       child value c-string
     * @return child index in JSONBinTree; -1 as error 
     */
    Index setNewChild(Index pidx, uint8_t t, const char* k, const char* v);

public:
    bool  useNonChild    (Index i);
    bool  isMatrix       (Index i);
    bool  isObjectInArray(Index i); // Ambiguity Array: indexed or repeated ?
    bool  isLeafField    (Index i); 
    bool  isRepeatedArray(Index i); // share the same type 
    bool  isIndexedArray (Index i); // index decide types
    bool  isArrayHasRepeatedObject(Index i); // Repeated Objects in Array

public: 
    void output2debug(Index i = 0, uint32_t level = uint32_t(0));
    void outputAttributes2debug(Index i, uint32_t level = uint32_t(0));
}; // JSONBinTree





inline
JSONBinTree::JSONBinTree(void)
{
    m_nodes  = new Container<JSONBinField>(1024);
    uint64_t ridx = m_nodes->getNextIndex (); // root 
    m_nodes->appendNew();

    JSONBinField *rt = m_nodes->get(ridx);
    rt->set(JSONType::s_object); // root is an object
} // ctor



inline
void JSONBinTree::clear(void) 
{
    uint64_t nnum = m_nodes->size(); 
    for (uint64_t ni = 0; ni < nnum; ++ni) 
    {
        JSONBinField *bf = m_nodes->get(ni);
        bf->clear();
    } // for ni 


    JSONBinField *rbf = m_nodes->get(0);
    rbf->set(JSONType::s_object);
} // clear



inline
JSONBinTree::Index JSONBinTree::getNextChild(uint32_t pidx)
{
    JSONBinField *p = m_nodes->get(pidx);
    if (p->useAllChild())
    {
        uint64_t cidx = m_nodes->getNextIndex();
        m_nodes->appendNew  ();

        // m_nodes maybe realloc
        p = m_nodes->get(pidx);
        p-> appendChild (cidx);

        JSONBinField *c = nullptr;
        c = m_nodes->get(cidx);
        c-> setParent   (pidx);
    } // if  

    return p->getNextChild();
} // getNextChild



inline
JSONBinTree::Index JSONBinTree::
    setNewChild(Index pidx, uint8_t t, const char* k, const char* v)
{
    Index cidx = this->getNextChild(pidx);
    if (cidx != Index(-1))
    {
        JSONBinField* c = m_nodes->get(cidx);
        c->set(t, k, v);
    }
    return cidx;
} // setNewChild 



inline
bool JSONBinTree::useNonChild(Index i)
{
    JSONBinField *n = m_nodes->get(i);
    return n->useNonChild();
} // useNonChild



inline
bool JSONBinTree::isMatrix(Index i)
{
    JSONBinField *n = m_nodes->get(i);
    bool   is_array = n->isArray  ();
    bool  pis_array = false;
    if (n->hasParent())
    {
        Index       pidx = n->getParent();
        JSONBinField *pn = m_nodes->get(pidx);
        pis_array = pn->isArray();
    }
    return is_array && pis_array;  
} // isMatrix



inline
bool  JSONBinTree::isObjectInArray (Index i)
{
    JSONBinField *n= m_nodes->get(i);
    bool is_object = n->isObject ( );
    bool pis_array = false;   
    if  (n->hasParent())
    {
        Index       pidx = n->getParent();
        JSONBinField *pn = m_nodes->get(pidx);
        pis_array = pn->isArray();
    } // if  
    return is_object && pis_array;
} // isObjectInArray



inline
bool  JSONBinTree::isLeafField(Index i)
{
    JSONBinField  *f = m_nodes->get(i);
    if (isRepeatedArray(i))
    {
        Index cidx = f->getChild (0);
        f = m_nodes->get(cidx);
    } // if 
    return f->isPrimitive();
} // isLeafField



inline
bool JSONBinTree::isRepeatedArray(Index i)
{
    JSONBinField *n = m_nodes->get(i);
    if (!n->isArray()) { return false; }

    bool same_type = false;
    if (!n->isEmptyArray())
    {
        Index      fcidx = n->getChild (0);
        JSONBinField *fc = m_nodes->get(fcidx);
        uint8_t      ftp = fc->getValueType (); // first type  

        uint32_t cnum = n->getChildUsedNum();
        for (uint32_t i = 0; i < cnum; ++i) // single elem: begins from 0
        {
            Index       cidx = n->getChild (i);
            JSONBinField *cn = m_nodes->get(cidx);
            uint8_t   val_tp = cn->getValueType();

            if (ftp != val_tp) { break; } // diff type 
            if (i == cnum - 1) { same_type = true; }
        } // for i 
    } // if 
    
    return same_type; 
} // isRepeatedArray



inline
bool JSONBinTree::isIndexedArray (Index i)
{
    JSONBinField *n = m_nodes->get(i);
    return !(n->isNull()) && (n->isArray()) && !(isRepeatedArray(i)); 
} // isIndexedArray



inline
bool  JSONBinTree::isArrayHasRepeatedObject(Index i)
{
    bool     retval = false;
    JSONBinField *n = m_nodes->get   (i);
    bool is_rept    = isRepeatedArray(i);
    if  (is_rept)
    {
        Index      cidx = n->getChild(0);
        JSONBinField *c = m_nodes->get(cidx);
        retval = c->isObject();
    } 
    return retval; 
} // isArrayHasRepeatedObject



inline
void JSONBinTree::output2debug(Index i, uint32_t level)
{
    JSONBinField* e = m_nodes->get(i);
    e->output2debug(level); 
//    outputAttributes2debug (i, level);

    // DFS for each child node  
    level += 1; 
    uint32_t cnum = e->getChildUsedNum ();
    for (uint32_t ci = 0; ci < cnum; ++ci) 
    {
        Index cidx = e->getChild (ci);
        JSONBinTree::output2debug(cidx, level);
    } // for ci
} // output2debug



inline
void JSONBinTree::outputAttributes2debug(Index i, uint32_t level)
{
    for (uint32_t li = 0; li < level; ++li) { printf("\t"); }
//    printf("  ->Lf:[%d] Rept:[%d] Idx:[%d] [Obj]:%d Mtx:%d ArrayHasObject:%d \n",
    printf("[%d][%d][%d][%d][%d][%d]\n",
            isLeafField(i), isRepeatedArray(i), isIndexedArray(i),
            isObjectInArray(i), isMatrix(i), isArrayHasRepeatedObject(i));
} // outputAttributes2debug



} // namespace steed
