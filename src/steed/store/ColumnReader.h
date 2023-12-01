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
 * @file ColumnReader.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    definitions and functions for ColumnReader
 */

#pragma once 

#include <string>
#include <stdint.h>

#include "BitMap.h"
#include "Config.h"

#include "CABReader.h"
#include "ColumnItem.h"

#include "SchemaNode.h"
#include "SchemaTree.h"
#include "StoragePath.h"



namespace steed {

extern Config g_config;

class ColumnReader {
protected:
    SchemaTree        *m_tree{nullptr};  /**< related SchemaTree  */
    SchemaPath         m_leaf_path  {};  /**< leaf node path info */
    string             m_file_name  {};  /**< leaf value file path*/ 

    CABReader         *m_read{nullptr};  /**< CAB read Operator   */
    uint64_t           m_recd_idx  {0};  /**< read record index */
    uint64_t           m_item_idx  {0};  /**< read item   index */


public:
    ColumnReader (void) = default;
    ~ColumnReader(void)
    { delete m_read; m_read = nullptr; m_tree = nullptr; }

public:
    SchemaTree       *getSchemaTree   (void) { return m_tree; } 
    uint32_t          getPathDepth    (void) { return m_leaf_path.size(); }
    SchemaPath       &getLeafPath     (void) { return m_leaf_path; } 
    const string     &getFileName     (void) { return m_file_name; }

    uint64_t          getRecdIndex    (void) { return m_recd_idx ; }
    uint64_t          getItemIndex    (void) { return m_item_idx ; }
    uint64_t          getCABItemNum   (void) { return m_read->getItemNumber  (); }
    uint64_t          getValidRecdIdx (void) { return m_read->getValidRecdIdx(); }

    CABReader        *getCABReader    (void) { return m_read; }
    DataType         *getDataType     (void) { return m_read->getDataType(); }
    BinaryValueArray *getBinValueArray(void) { return m_read->getBinValueArray(); } 
    BitVector        *getRepValueArray(void) { return m_read->getRepValueArray(); }

public:
    /**
     * init ColumnWriter to read ALL values
     * @param dir         directory to save the binary values 
     * @param tree        SchemaTree instance  
     * @param path        path in SchemaTree 
     * @return ==0 succsess; <0 failed 
     */ 
    int init2read(const string &dir, SchemaTree* tree, SchemaPath &path);

protected:
    /**
     * init ColumnWriter to write 
     * @param dir         directory to save the binary values 
     * @param tree        SchemaTree instance  
     * @param path        path in SchemaTree 
     * @return 0 succsess; < 0 failed
     */
    int init(const string &dir, SchemaTree* tree, SchemaPath &path);

#if DEFINE_BLM
public:
    /**
     * check the right value in CAB blooming filter 
     * @param is_cand   is candidate flag
     * @param cab_rbgn  CAB record begin index 
     * @param chk_bin   binary content to check blooming filter 
     * @param chk_len   binary length  to check blooming filter 
     * @return >0 got is candidate flag; 0 EOF; <0 failed  
     */ 
    int isCandidate(bool &is_cand, uint64_t cab_rbgn, const void *chk_bin, int64_t chk_len)
    {   return m_read->isCandidate(is_cand, cab_rbgn, chk_bin, chk_len);   } 
#endif

public:
    /**
     * load ColumnAlignBlock by record id
     * @param ridx    record index, maybe got from Index
     * @return 1 success; 0 EOF; <0 failed
     */
    int loadCAB4Record (uint64_t ridx);

public:
    /**
     * prepare to read record  
     * @param ridx    record index 
     * @return >0 success; 0 is EOF; <0 failed  
     */
    int prepare2ReadRecord(uint64_t ridx);

    /**
     * read next column item 
     * @param ci       column item got from CAB
     * @return >0 success; 0 is EOF; <0 failed  
     */
    int readItem(ColumnItem &ci);

public:
    /**
     * prepare bitmap content before predicate compares
     * @param num    item number in CAB 
     */
    void prepareItemBitmap(BitMap *bitmap);

public:
    void output2debug(void);
}; // ColumnReader

} // namespace



#include "ColumnReader_inline.h"
