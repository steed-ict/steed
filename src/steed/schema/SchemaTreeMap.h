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
 * @file   SchemaTreeMap.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn> 
 * @version 1.0
 * @section DESCRIPTION
 * definition and functions of Schema Tree Map
 */

#pragma once 

#include <string>
#include <vector>
#include <unordered_map>

#include "Config.h"
#include "Utility.h"
#include "SymbolMap.h"
#include "SchemaTree.h"


namespace steed {

using std::unordered_map;
using std::string;
using std::vector;


/**
 * @class SchemaTreeMap
 * @brief SchemaTreeMap is a map from database name and collection name to SchemaTree
 * @details
 * SchemaTreeMap is a map from database name and collection name to SchemaTree
 */

class SchemaTreeMap {
private: 
    ~SchemaTreeMap(void)  = delete; 
    SchemaTreeMap (void)  = delete; 
    SchemaTreeMap (const SchemaTreeMap &)  = delete; 
    SchemaTreeMap& operator= (const SchemaTreeMap &) = delete; 

private: 
    /**
     * schame tree map
     * key: database name + Config::s_schema_map_sign_delim + collection name
     * value: SchemaTree pointer
     */
    static SymbolMap<SchemaTree*>  s_map;

    /**
     * use database and collection name to get sign
     * @param db database name
     * @param tb collection name
     * @param sign SchemaTree sign returned as map key
     */
    static void getSign (const string &db, const string &tb, string &sign)
    { sign.assign(db).append(1, Config::s_schema_map_sign_delim).append(tb); }


public:
    /**
     * use database and collection name to load SchemaTree
     * @return 1; success 0 not find SchemaTree; <0 failed
     */
    static int  load   (const string &db, const string &tb, SchemaTree* &t);

    /**
     * destory all SchemaTree in the map
     * @return void
     */
    static void destory(void);

public:
    static void emplace(const string &db, const string &tb, SchemaTree *t)
    { string sign; getSign(db, tb, sign); s_map.insert(sign,t); }

    static void erase  (const string &db, const string &tb)
    { string sign; getSign(db, tb, sign); s_map.erase (sign)  ; }

    static SchemaTree* lookup(const string &db, const string &tb); 

public:
    /**
     * get defined SchemaTree pointer by database name and collection name:
     * 1. if SchemaTree is in the map, lookup it and return
     * 2. if SchemaTree is not in the map, lookup the file and load it and return
     * 3. otherwise, the SchemaTree is not defined yet
     * 
     * @param db database name
     * @param tb collection name
     * @param tree SchemaTree pointer returned
     * @return 1 success; 0 not find SchemaTree; <0 failed
     */
    static int getDefinedTree(const string &db, const string &tb, SchemaTree* &tree);

public: 
    static void output2debug(void);
}; // SchemaTreeMap

} // namespace