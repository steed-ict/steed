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
 * @file   RowStructBuilder.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for RowStructBuilder 
 */

#pragma once 

#include <array>
#include <vector>

#include "Buffer.h"
#include "Row.h"
#include "RowInfo.h"
#include "RowArrayOperator.h"
#include "RowObjectOperator.h"

namespace steed {
namespace Row {

using std::array ;
using std::vector;

class RowObjectBuilder {
protected:
    Buffer             *m_buf{nullptr}; /**< buffer bin object content  */
    RowObjectOperator  *m_obj{nullptr}; /**< buffer to write bin content*/
    vector<Row::ID>     m_ids       {}; /**< buffered ids in row object */
    vector<uint32_t>    m_offs      {}; /**< buffered offset in row obj */
    uint32_t            m_vlen     {0}; /**< binary value length in obj */
    bool                m_init {false}; /**< current builder is inited  */

public:
    ~RowObjectBuilder(void);
    RowObjectBuilder (Buffer *buf) : m_buf(buf)
    { m_obj = new RowObjectOperator(); }

public:
    void     clear    (void);
    bool     isInited (void)      { return m_init; }
    uint32_t getValueLength(void) { return m_vlen; } 

    bool     haveIDs  (void)      { return !m_ids.empty(); }
    Row::ID  getBackID(void)      { return  m_ids.back (); }


    /**
     * init to build object from scratch
     * @return 0 success; <0 failed 
     */
    int init(void);

    /**
     * init to build object from the existed object
     *  (append to the existed object)
     * @param bgn    object begin offset in m_buf
     * @return 0 success; <0 failed 
     */
    int init(uint32_t bgn);

public:
    void appendElem (Row::ID id)
    { m_ids .emplace_back(id), m_offs.emplace_back(m_vlen); }

    void appendValue(uint32_t blen)
    { m_vlen += blen; }

    uint32_t package(void);
}; // RowObjectBuilder 





class RowArrayBuilder {
protected:
    Buffer            *m_buf {nullptr}; /**< buffer bin array  content  */
    RowArrayOperator  *m_arr {nullptr}; /**< RowArrayOperator  to write */
    vector<uint32_t>   m_offs       {}; /**< buffered offset in row arr */
    uint32_t           m_vlen      {0}; /**< binary value length in arr */
    bool               m_init  {false}; /**< current builder is inited  */

public:
    ~RowArrayBuilder(void);
    RowArrayBuilder (Buffer *buf) : m_buf(buf) 
    { m_arr  = new RowArrayOperator(); }

public:
    void     clear   (void);
    bool     isInited(void)       { return m_init; }
    uint32_t getValueLength(void) { return m_vlen; } 

    int      init       (void);
    void     appendElem (void)  { m_offs.emplace_back(m_vlen); }
    void     appendValue(uint32_t blen)      { m_vlen += blen; }
    uint32_t package    (void);
}; // RowArrayBuilder

} // namespace Row
} // namespace steed


#include "RowStructBuilder_inline.h"