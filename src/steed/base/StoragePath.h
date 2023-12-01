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
 * @file   StoragePath.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for StoragePath 
 */

#pragma once 

#include <string>

#include "SchemaTree.h"
#include "SchemaPath.h"



namespace steed {

using std::string;


class StoragePath {
public:
    /**
     * get column data storage path 
     * @param dir     directory to save the binary values 
     * @param tree    SchemaTree instance  
     * @param sp      SchemaPath in SchemaTree 
     * @param got     got data 
     * @return 0 succsess; < 0 failed
     */
    static
    int getDataPath(const string &dir, SchemaTree* tree, SchemaPath &sp, string &got)
    {
        got.assign(dir);
        if (got.back() != '/')
        {   got.append(1, '/');   }
        return tree->appendPathWPost(got, sp);
    } // getDataPath
}; // StoragePath





} // namespace 
