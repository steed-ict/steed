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
 * @file Container_inline.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    Container inline functions
 */


#pragma once 

namespace steed {


template <class T> inline
Container<T>::Container (uint32_t cap)
{
    m_ptrs.reserve(cap);
    m_size = sizeof (T);
    m_buf  = new Buffer(m_size * cap);
} // ctor 


template <class T> inline 
Container<T>::~Container(void)
{
    for (auto & p : m_ptrs)
    {
        if (p == nullptr)
        { continue; }

        p->deleteIns(); // do free  
        p->~T();        // do nothing
        operator delete (p, nullptr); // placement delete: does nothing
    } 
    m_ptrs.clear();

    if (m_buf != nullptr)
    { 
        delete m_buf;
        m_buf = nullptr;
    }
} // dtor


template <class T> inline
bool Container<T>::initElem(uint64_t i)
{
    if (i >= this->size())
    { m_ptrs.resize(i+1, nullptr); }

    // already inited
    if (notNull(i)) { return false; }
    
    // init faile if no enough memory
    T*  ptr = this->alloc();
    if (ptr == nullptr) { return false; }
    
    m_ptrs[i] = ptr;
    return true;
} // initElem


template <class T> inline
T* Container<T>::appendNew(void)
{
    // must get a new element since call alloc
    T* n = this->alloc();
    if (n == nullptr)
    { return nullptr;}

    m_ptrs.emplace_back(n);
    return n; 
} // appendNew 


template <class T> inline
T* Container<T>::alloc(void)
{
    uint64_t len = m_buf->available();
    if ((m_size > len) && (doubleCap() < 0))
    {
        printf("Container: doubleCap failed!\n");
        return nullptr;
    } // if 

    bool resize = false;
    T* got = (T*)m_buf->allocate(m_size, resize);
    return (got == nullptr) ? got : new (got) T(); 
} // alloc


template <class T> inline
int Container<T>::doubleCap (void)
{
    // swap to double the pointer and buffer 
    vector<T*> org_ptrs;
    org_ptrs.swap(m_ptrs);

    Buffer  *org_buf = m_buf; 
    uint64_t buf_cap = m_buf->capacity();
    m_buf = new Buffer(buf_cap * 2);

    // copy org_ptrs to m_ptrs
    for (auto &p : org_ptrs)
    {
        // org element is nullptr
        if (p == nullptr)
        {
            m_ptrs.emplace_back(nullptr);
            continue;
        } // if 
        
        // copy and delete org element
        T*  n =  this->appendNew();
        if (n == nullptr)
        {
            printf("Container: appendNew failed!\n");
            return -1;
        } // if
        
        n->copyIns(*p);
        p->deleteIns(); // do free  
        p->~T();        // do nothing
        operator delete (p, nullptr); // placement delete: does nothing
    } // for 

    delete org_buf;
    org_buf = nullptr;
    org_ptrs.clear();

    return 0;
} // doubleCap



template <class T> inline
int64_t Container<T>::flush2buffer(Buffer *fb)
{
    bool alc = true;
    uint64_t org_used = fb->used();
    
    // T binary content 
    // [elem_num] | [elem content]
    uint64_t    elem_use  = m_buf->used(); // in Bytes
    uint64_t    elem_num  = elem_use / sizeof(T); 
    uint64_t   *cp_elem_num = (uint64_t*)fb->allocate(sizeof(uint64_t), alc);
    *(uint64_t*)cp_elem_num = elem_num;
    
    T* org_elem = (T*)m_buf->getPosition(0);
    T* cp_elem  = (T*)fb->allocate(elem_use, alc);
    for (uint32_t ei = 0; ei < elem_num; ++ei)
    { cp_elem[ei] = org_elem[ei]; }

    // pointers
    // [ptr_num] | [ptr content]
    uint64_t    ptr_num    = m_ptrs.size(); 
    uint64_t   *cp_ptr_num = (uint64_t*)fb->allocate(sizeof(uint64_t), alc);
    *(uint64_t*)cp_ptr_num = ptr_num;
    
    T* ptr_bgn = m_ptrs[0];
    uint32_t  psize  = ptr_num * sizeof(uint64_t);
    uint64_t *cp_ptr = (uint64_t*)fb->allocate(psize, alc);
    for (uint32_t pi = 0; pi < ptr_num; ++pi)
    {
        cp_ptr[pi] = (m_ptrs[pi] == nullptr) ?
            uint64_t(-1) : m_ptrs[pi] - ptr_bgn;
    } // for 
   
    uint64_t cur_used = fb->used();
    return int64_t(cur_used - org_used);
} // flush2buffer



template <class T> inline
int64_t Container<T>::load2buffer(Buffer *lb, uint64_t off)
{
    bool alc = true;
    char *org = (char*)lb->getPosition(off);
    char *ptr = org;
    
    // Container content read from file 
    m_buf->reserve( lb->used() );

    // T binary content 
    // [elem_num] | [elem content]
    uint64_t elem_num = *(uint64_t*)ptr;
    ptr += sizeof(uint64_t);
    
    uint64_t elem_use = elem_num * sizeof(T);
    T* org_elem = (T*)ptr;
    T* cp_elem  = (T*)m_buf->allocate(elem_use, alc);
    for (uint32_t ei = 0; ei < elem_num; ++ei)
    {
        T* e = new (cp_elem + ei) T(); // placement new 
        *e = org_elem[ei];  
    } // for 
    ptr += elem_use;
    
    // pointers
    // [ptr_num] | [ptr content]
    uint64_t  ptr_num = *(uint64_t*)ptr;
    ptr += sizeof(uint64_t);
    
    uint64_t *ptr_idx = (uint64_t*)ptr;
    for (uint32_t pi = 0; pi < ptr_num; ++pi)
    {
        uint64_t idx = ptr_idx[pi];
        T* eptr = (idx == uint64_t(-1)) ?
            nullptr : cp_elem + idx;
        m_ptrs.emplace_back(eptr);
    } // for 
    
    ptr += ptr_num * sizeof(uint64_t);
    
    return int64_t(ptr - org); 
} // load2buffer


} // namespace steed 