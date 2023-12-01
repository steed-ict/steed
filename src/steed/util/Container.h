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
 * @file Container.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    malloc several instances on one continuous memory,
 *    the memory is maintained by Container 
 * 
 * NOTE:     
 *   class T MUST define the following functions: 
 *   1. T(void);
 *      construction func with no param  
 *   2. void copyIns  (T &n);
 *      copy the instance to double the buffer memory
 *   3. void deleteIns(void);
 *      delete the instance in my dtor 
 * @see SchemaNode class
 */

#pragma once 

#include <stdint.h>
#include <vector>

#include "Buffer.h"

namespace steed {

using std::vector;

template <class T>
class Container {
protected:
    vector<T*>   m_ptrs{};         /**< element ptrs */
    Buffer      *m_buf {nullptr};  /**< ins content  */
    uint32_t     m_size{0};        /**< type T size  */

    const static uint32_t s_cap = 2; /**< default cap */

public: 
    Container (uint32_t cap = s_cap);
    ~Container(void);

public:
    uint64_t typeSize     (void) { return m_size; }

    uint64_t size         (void) { return m_ptrs.size(); }
    uint64_t getNextIndex (void) { return this-> size(); }

    T*       get    (uint64_t i) { return m_ptrs [i]; }
    void     reserve(uint64_t n) { m_ptrs.reserve(n); }
    void     resize (uint64_t n) { m_ptrs.resize (n, nullptr);  }
    bool     notNull(uint64_t i) { return m_ptrs[i] != nullptr; }

    /**
     * init i-th element in Container
     * @param i    element index 
     * @return true success; false failed
     */
    bool initElem(uint64_t i);

    /**
     * create new element in the tail of container  
     * @return new element; nullptr for failed 
     */
    T* appendNew(void);

    /**
     * clear all members in this container 
     */
    void clear(void)
    {   m_ptrs.clear(); m_buf->clear();   }


protected:
    /**
     * allocate new instance
     * @return instance pointer; nullptr for failed
     */
    T*  alloc(void);

    /**
     * double the cap of container
     * @return 0 success; <0 failed 
     */
    int doubleCap(void); 


public:
    /**
     * flush binary content to buffer
     * @param fb   flush buffer 
     * @return >0 flushed size; <0 failed 
     */
    int64_t flush2buffer(Buffer *fb);

    /**
     * flush binary content to buffer
     * @param lb    load buffer 
     * @param off   content begin offset in load buffer 
     * @return >0 flushed size; <0 failed 
     */
    int64_t load2buffer (Buffer *lb, uint64_t off);
}; // Container


} // namespace steed



#include "Container_inline.h"
