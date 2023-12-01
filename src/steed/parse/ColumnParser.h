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
 * @file ColumnParser.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *     ColumnParser:
 *         1. use SchemaGenerator to infer a SchemaTree  
 *         2. parse JSONBinTree to binary column format  
 */

#pragma once 

#include <stdint.h>
#include <array>
#include <string>

#include "Config.h"
#include "DebugInfo.h"
#include "JSONRecordBuffer.h"
#include "JSONRecordNaiveParser.h"

#include "SchemaTree.h"
#include "SchemaTreeMap.h"
#include "CollectionWriter.h"
#include "ColumnItemGenerator.h"

#define COLUMNPARSER_PRINT_LOG 1

namespace steed {

using std::array ; 
using std::string; 

class ColumnParser {
protected:
    /** record reader function pointer */
    typedef JSONRecordReader::ReadFPtr ReadFPtr; 

protected:
    /**< record used in m_jtree */
    static const uint64_t s_jtree_cap{JSONRecordBuffer::s_recd_num};

    JSONRecordBuffer    *m_jbuffer{nullptr}; /**< JSON text record buffer */
    JSONRecordParser    *m_jparser{nullptr}; /**< JSON text record parser */

    /** binary records in m_jbuffer by JSONBinTree */
    array <JSONBinTree*, s_jtree_cap> m_jtree{};
    uint64_t             m_jtree_used  {0}; /**< m_jtree used number */

    SchemaTree          *m_tree    {nullptr}; /**< inferred SchemaTree ins */
    CollectionWriter    *m_clt_wt  {nullptr}; /**< CollectionWriter write txt 2 bin*/
    ColumnItemGenerator *m_item_gen{nullptr}; /**< bond JSONBinField to SchemaTree */

    bool    m_append{false}; /**< append records to existed collection */


public:
    /**
     * ctor 
     * @param db   database   name string 
     * @param clt  collection name string 
     */
    ColumnParser (void) = default;
    ~ColumnParser(void);

public:
    /**
     * init to parse the JSON text records in stream
     *   steed will infer the SchemaTree from SampleTree 
     * @param db   database   name string
     * @param clt  collection name string
     * @param is   JSON text records input stream
     * @return 0 success; <0 failed 
     */
    int init (const string &db, const string &clt, istream *is);

    /**
     * parse one text record from JSON in stream
     * @return 1 done; 0 EOF; <0 failed
     */
    int64_t parseOne(void);

    /**
     * parse ALL text records from JSON in stream  
     * @return >0 done num; 0 EOF; <0 failed
     */
    int64_t parseAll(void);

public:
    /*
     * Example:
     *   ColumnParser cp;
     *   cp.init("test", "test");
     *   const char *recd = "{\"a\":1, \"b\":2}";
     *   uint32_t len = strlen(recd);
     *   int64_t s = parseOne(recd, len);
     *   cp.parseOne();
     */

    /**
     * init to parse to parse text records in cstring
     * @param db   database   name string
     * @param clt  collection name string
     * @retrun 0 success; <0 failed
     */
    int init (const string &db, const string &clt)
    {   return init(db, clt, nullptr);   }

    /**
     * parse one text record from JSON in cstring
     * @param recd  text record in cstring
     * @param len   text record length
     */
    int64_t parseOne(const char *recd, uint32_t len);

protected:
    /**
     * read text records and trans to JSONBinTree in batch
     * @param fptr     read function pointer: read or sample  
     * @param rnum     records number need to read
     * @return >0 done num; 0 EOF; <0 failed
     */
    int readRecds2TreeInBatch(ReadFPtr fptr, uint32_t rnum);
}; // ColumnParser

} // namespace steed

#include "ColumnParser_inline.h"
