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
 * @file ColumnWriter.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    definitions and functions for ColumnWriter
 */

#pragma once 

#include <string>
#include <stdint.h>

#include "Config.h"
#include "CABWriter.h"
#include "CABAppender.h"
#include "SchemaNode.h"
#include "SchemaTree.h"
#include "StoragePath.h"



namespace steed {

using std::string; 
extern Config g_config;

class ColumnWriter {
protected: 
    SchemaTree      *m_tree{nullptr};  /**< related SchemaTree  */
    SchemaPath       m_leaf_path  {};  /**< leaf node path info */
    string           m_file_name  {};  /**< leaf value file path*/ 

    CABWriter       *m_cab_op  {nullptr};  /**< CAB Operator    */


public:
    ColumnWriter  (void) = default; 
    ~ColumnWriter (void);

public:
    uint64_t          getRecdNum  (void) { return m_cab_op->getRecdNum(); }
    uint32_t          getMaxDefVal(void) { return getPathDepth    (); }
    uint32_t          getPathDepth(void) { return m_leaf_path.size(); }
    SchemaPath       &getLeafPath (void) { return m_leaf_path; } 
    const string     &getFileName (void) { return m_file_name; }
    CAB              *getCurCAB   (void) { return m_cab_op->getCurCAB (); }

    uint64_t  getValidRecdIdx(void) { return m_cab_op->getValidRecdIdx(); }

public:
    /** 
     * init ColumnWriter to write 
     * @param dir     directory to save the binary values 
     * @param tree    SchemaTree instance  
     * @param path    path in SchemaTree 
     * @param rbgn   begin record id (index) to write  
     * @return ==0 succsess; <0 failed 
     */
    int init2write(const string &dir, SchemaTree* tree, SchemaPath &path, uint64_t rbgn);

    /**
     * init ColumnWriter to append
     * @param dir     directory to save the binary values
     * @param tree    SchemaTree instance
     * @param path    path in SchemaTree
     * @return ==0 succsess; <0 failed
     */
    int init2append(const string &dir, SchemaTree* tree, SchemaPath &path);
  
protected:
    /**
     * init ColumnWriter to write 
     * @param dir         directory to save the binary values 
     * @param tree        SchemaTree instance  
     * @param path        path in SchemaTree 
     * @return 0 succsess; < 0 failed
     */
    int init(const string &dir, SchemaTree* tree, SchemaPath &path);

public:
    void copyIns  (ColumnWriter &n);
    void deleteIns(void)     ;

public:
    int writeNull  (uint32_t rep, uint32_t def, uint64_t nnum)
    { return m_cab_op->writeNull(rep, def, nnum); }

    int writeNull  (uint32_t rep, uint32_t def)
    { return m_cab_op->writeNull(rep, def); }

    int writeText  (uint32_t rep, uint32_t def, const char* txt)
    { return m_cab_op->writeText(rep, def, txt); }

public:
    void output2debug(void);
}; // ColumnWriter

} // namespace steed 



#include "ColumnWriter_inline.h"
