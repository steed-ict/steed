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
 * @file CollectionWriter.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *     definitions and functions for CollectionWriter
 */

#pragma once 

#include <string>
#include <stdint.h>

#include "Config.h" 
#include "SchemaNode.h" 
#include "SchemaPath.h" 
#include "SchemaTree.h" 

#include "ColumnWriter.h"
#include "Container.h"
#include "TreeCounter.h" 
#include "ColumnTextBuffer.h"



namespace steed {

class CollectionWriter {
protected:
    string    m_db_name  {""}; /**< database name string */
    string    m_clt_name {""}; /**< collection name str  */
    string    m_strg_path{""}; /**< storage file path    */

    SchemaTree                  *m_tree   {nullptr}; /**< related tree */
    Container<ColumnWriter>     *m_col_wts{nullptr}; /**< mem columns  */
    Container<ColumnTextBuffer> *m_txt_buf{nullptr}; /**< text buffer  */ 


public:
    CollectionWriter (SchemaTree *tree);
    ~CollectionWriter(void);

public:
    /**
     * init to append using existed SchemaTree
     *   some ColumnWriters are valid from record index != 0
     * @param cnt   tree nodes appeared counter
     * @return 0 success; <0 failed
     */
    int init2append(TreeCounter *cnt); 

    /**
     * init to append using existed SchemaTree
     * @
    */
    int initColumnAppender(SchemaSignature ls);

public:
    /**
     * init to write using existed SchemaTree
     *   all ColumnWriters are valid from record index = 0
     * @return 0 success; <0 failed 
     */
    int init2write(void);

    /**
     * init ColumnWriter when the SchemaTree add new fields 
     *   the ColumnWriter gets valid record index from cnt, called by ColumnItemGenerator
     * @param ls    leaf SchemaNode SchemaSignature 
     * @param cnt   tree nodes appeared counter  
     * @return 0 success; <0 failed 
     */
    int initColumnWriter(SchemaSignature ls, TreeCounter &cnt);

protected:
    /**
     * init ColumnWriter using leaf SchemaSignature  
     *   the ColumnWriter is valid from record index = 0
     * @param ls    leaf SchemaNode SchemaSignature 
     * @param rbgn   begin record id (index) to write  
     * @return 0 success; <0 failed 
     */
    int initColumnWriter (SchemaSignature ls, uint64_t rbgn);

    /**
     * align the column items in the new created record
     * @param ls    leaf SchemaNode SchemaSignature 
     * @param cnt   tree nodes appeared counter  
     * @return 0 success; <0 failed 
     */
    int alignColumnWriter(SchemaSignature ls, TreeCounter &cnt);


public:
    /**
     * write the ColumnWriter Text Item to ColumnTextBuffer
     * @param ls    SchemaNode SchemaSignature
     * @param r    repetition value  
     * @param d    definition value 
     * @param txt  text value content  
     */
    void write(SchemaSignature ls, uint32_t r, uint32_t d,
            const char *txt = nullptr);

    /**
     * flush ColumnTextBuffer::Item to ColumnWriter binary content
     * @return 0 success; <0 failed 
     */
    int flush(void);


public:
    /** output 2 debug */
    void output2debug(void);
}; // CollectionWriter


} // namespace steed



#include "CollectionWriter_inline.h"
