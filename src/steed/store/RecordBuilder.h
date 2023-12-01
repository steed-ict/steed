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
 * @file   RecordBuilder.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 * definitions and functions for RecordBuilder 
 */

#pragma once 

#include <array>
#include <vector>

#include "Buffer.h"
#include "SchemaPath.h"
#include "SchemaTree.h"
#include "RowStructBuilder.h"



namespace steed {
namespace Row {

using std::array ;
using std::vector;

/**
 * Level Relationship between RowBuilder, SchemaPath and Def value:
 * 
 *   Idx   Row Builders       SchemaPath         Level   Def Value
 *         vector:            vector:                    SchemaNode level
 *                                                 0        O root
 *                                                          |
 *                                                          |
 *   0     RowObjectBuilder   SchemaSignature      1        O field
 *         RowArrayBuilder                                  |
 *                                                          |
 *   1     RowObjectBuilder   SchemaSignature      2        O field
 *         RowArrayBuilder                            
 */

class RecordBuilder {
protected:
    SchemaTree       *m_tree{nullptr};  /**< related SchemaTree      */
    Buffer           *m_buf {nullptr};  /**< buffer bin record cont  */

    vector<RowObjectBuilder> m_objs{};  /**< nested builder on lvls  */
    vector<RowArrayBuilder > m_arrs{};  /**< multi values on levels  */
    vector<char> m_empty_array_elem{};  /**< empty elem flag: 1 true, 0 false */
    uint32_t          m_max_level {0};  /**< record max nest level   */

    uint32_t          m_bgn_off   {0};  /**< recd bin begin offset   */
    uint32_t          m_recd_size {0};  /**< record bin content size */
    uint32_t          m_struct_len{0};  /**< tail struct length @ lower struct */

    /**
     * next level needs to init in binary row object:
     *   m_next_idx = definitions value = current SchemaNode level in SchemaTree 
     *
     *   Usage:
     *   SchemaPath[m_next_idx - 1] -> current SchemaNode Signature 
     *
     *   Explain:
     *   For the builders in m_objs[i] and m_arrs[i]:
     *     i     <  m_next_idx:  builders are inited (binary struct is beginned)
     *     i + 1 == m_next_idx:  the level need to init next 
     *     i + 1 >  m_next_idx:  deepper level which is not used 
     */
    uint32_t          m_next_idx{0};


public:
    /**
     * ctor 
     * @param tree    SchemaTree instance 
     * @param buf     Buffer ins for row content  
     * @param md      max depth of path from root to leaf, including root 
     */
    RecordBuilder (SchemaTree *tree, Buffer *buf, uint64_t md);

    /**
     * dtor 
     * reset ptrs to nullptr;
     * vector are automaticly deallocated
     */
    ~RecordBuilder(void) = default; 
 
    /** get max level in RecordBuilder (Explain the root) */
    uint32_t getMaxLevel(void)  { return m_max_level; } 

    /** erase incomplete content in buffer*/
    void     erase(void);

    /** clear the record builder  */
    void     clear(void);


public:
    /**
     * begin to build new row record 
     * @return 0 success; <0 failed
     */
    int  begin2build(void);

    /**
     * move to destination index in SchemaPath  
     * @param tgt_lvl   target level in SchemaTree to build (def value)
     *            NOTE: target index in SchemaPath is tgt_idx
     *                  tgt_lvl = tgt_idx + 1 = m_next_idx
     * @param path       related SchemaPath 
     * @param app_elem   append new element in array flag 
     * @return 0 success; <0 failed
     */
    int  move2level (uint32_t tgt_lvl, SchemaPath &path, bool app_elem);

protected:
    /**
     * append field to binary row struct 
     * @param sp        related SchemaPath ins
     * @param nidx      node index on SchemaPath 
     * @return 0 success; <0 failed
     */
    void appendField2Struct (SchemaPath &sp, uint32_t nidx);

    /**
     * append element to binary array struct 
     * @param sp        related SchemaPath ins
     * @param nidx      node index on SchemaPath 
     * @return 0 success; <0 failed
     */
    void appendElement2Array(SchemaPath &sp, uint32_t nidx);


public:
    /**
     * append leaf value 2 content  
     * @param blen    binary value length 
     * @return 0 success; <0 failed
     */
    int  appendLeafValue    (uint32_t blen);

    
public:
    /**
     * return to the target level, which is SchemaNode level in SchemaTree
     *     The SchemaNode @ level l is path[l-1].
     * 
     * The target level should be involved in the binary row content:
     *     so the expected level is tgt_lvl + 1
     * 
     * @param tgt_lvl   SchemaNode repeated @ the common level 
     *            NOTE: target index in SchemaPath is tgt_idx
     *            DONE: tgt_lvl = tgt_idx + 1 = m_next_idx
     * @param path      related SchemaPath 
     * @param app_elem  append new element in array flag 
     * @return 0 success; <0 failed
     */
    int  return2level(uint32_t tgt_lvl, SchemaPath &path, bool app_elem);

protected:
    /**
     * return to build array 
     * @param com_lvl    common level (rep value) in SchemaTree to build:
     *                   the SchemaNode @ com_lvl must be repeated
     * @param sp         SchemaPath to build  
     * @return 0 success; <0 failed
     */
    int  return2BuildArray (uint32_t com_lvl, SchemaPath &sp);

    /**
     * package lower objects: keeps tgt_lvl object builder to append 
     * @param tgt_lvl    target level in SchemaTree to build
     * @param sp         SchemaPath to build  
     * @return 0 success; <0 failed
     */
    int  return2BuildObject  (uint32_t tgt_lvl, SchemaPath &sp);

    /**
     * package builder by SchemaPath and builder index 
     * @param bld_idx    builder index for m_objs and m_arrs 
     * @return 0 success; <0 failed
     */
    int  packageByIndex      (uint32_t bld_idx);
    int  packageArrayByIndex (uint32_t bld_idx);
    int  packageObjectByIndex(uint32_t bld_idx);


public: 
    /**
     * end to build (package) current binary row content record:
     * m_next_idx must return to 0 (root level)
     * @return bin record used; uint32_t(-1) failed
     */
    uint32_t end2build(void);


public: // debug
    RowObjectBuilder &getObjectBuilder(uint32_t idx) { return m_objs[idx]; }
    RowArrayBuilder  &getArrayBuilder (uint32_t idx) { return m_arrs[idx]; }
}; // RecordBuilder 

} // namespace row
} // namespace steed



#include "RecordBuilder_inline.h"
