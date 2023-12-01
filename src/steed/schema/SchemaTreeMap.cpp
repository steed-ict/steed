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



/***
 * @file   SchemaTreeMap.cpp
 * @brief  implementation of SchemaTreeMap
 * @details
 * implementation of SchemaTreeMap
 */

#include "SchemaTreeMap.h"

namespace steed {

SymbolMap<SchemaTree*> SchemaTreeMap::s_map(16);


int SchemaTreeMap::
    load(const string &db, const string &tb, SchemaTree* &t)
{
    t = new SchemaTree(db, tb);
    int status = t->load();
    if (status > 0) // success
    {   emplace(db, tb, t);   }
    else  // failed or not defined yet 
    {   delete t; t = nullptr;   }
    return status;
} // load 



void SchemaTreeMap::destory(void)
{
    // erase and return the next iterator 
    for (auto itr = s_map.begin(); itr != s_map.end(); itr = s_map.erase(itr))   
    {
        delete itr->second;
        itr->second = nullptr;
    } // for  
} // destory 



SchemaTree* SchemaTreeMap::lookup(const string &db, const string &tb)
{ 
    string sign;
    getSign(db, tb, sign);

    auto got = s_map.find(sign);
    return (got != s_map.end()) ? got->second : nullptr;
} // lookup



int SchemaTreeMap::
    getDefinedTree(const string &db, const string &tb, SchemaTree* &tree)
{
    string sign;
    getSign(db, tb, sign);

    auto got =  s_map.find(sign);
    if  (got != s_map.end()) 
    {
        tree = got->second;
        return 1;
    }
    else
    {
        return load(db, tb, tree);
    } // if 
} // getDefinedTree



void SchemaTreeMap::output2debug(void) 
{
    puts("STEED SchemaTreeMap::output2debug:");

    auto  cur_itr  = s_map.begin(), end_itr = s_map.end();

    while(cur_itr != end_itr)
    {
        const string &sign = cur_itr->first;
        SchemaTree   *tree = cur_itr->second; 
        printf("[%s] @ [%p]\n", sign.c_str(), tree);
        ++cur_itr;
    } // while  
} // showDirectoryContent


}; // namespace