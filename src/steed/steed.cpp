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
 * @file steed.cpp
 * @brief steed library
 * @version 1.0
 */

#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

#include "CLI/CLI.hpp"

#include "Config.h"
#include "Utility.h"
#include "ColumnParser.h"
#include "ColumnAssembler.h"
#include "RecordOutput.h"
#include "SchemaTreeMap.h"


namespace steed {
using std::string;
using std::vector;
using std::ifstream;

// steed global config
Config g_config; 


// init steed static data and load config file
void init(const string &cfile)
{
    g_config.init(cfile);
    DataType::initStatic();
    JSONRecordParser::initStatic();
} // initStatic



// uninit steed static data
void uninit(void)
{
    SchemaTreeMap::destory();
    JSONRecordParser::uninitStatic();
    DataType::uninitStatic();
} // uninitStatic



int createDatabase(const string &db)
{
    string s; 
    Utility::getDataBaseDir(g_config, db, s);
    if (Utility::checkFileExisted(s))
    { return 0; }

    string schema_path; Utility::getSchemaDir(g_config, db, schema_path);
    string column_path; Utility::getDataDir  (g_config, db, column_path);
    bool schema = (Utility::makeDir(schema_path) == 0);
    bool column = (Utility::makeDir(column_path) == 0);
    return (schema && column) ? 1 : -1;
} // createDatabase



int dropDatabase(const string &db)
{
    string s;
    Utility::getDataBaseDir(g_config, db, s);
    if (!Utility::checkFileExisted(s))
    { return 0; }

    return (Utility::removeDir(s) < 0) ? -1 : 1;
} // dropDatabase



int createTable(const string &db, const string &table)
{
    std::string path; 
    Utility::getSchemaPath(g_config, db, table, path);
    if (Utility::checkFileExisted(path))
    { return 0; }

    // not yet created: flush an empty SchemaTree 
    SchemaTree *tree = new SchemaTree(db, table);
    int status = tree->flush();
    delete tree; tree = nullptr; 
    if (status < 0) { return -1; }

    string datadir;
    Utility::getDataDir(g_config, db, table, datadir); 
    Utility::makeDir(datadir);
    return 1;
} // createTable



int dropTable  (const string &db, const string &table)
{
    std::string path; 
    Utility::getSchemaPath(g_config, db, table, path);
    if (!Utility::checkFileExisted(path))
    { return 0; }

    SchemaTree *tree = nullptr;
    if (SchemaTreeMap::load(db, table, tree) < 0)
    { return -1; } 


    // delete column files
    string col_dir;
    Utility::getDataDir(g_config, db, table, col_dir);

    vector<string> col_files;
    int nd_num = tree->getNodeNum();
    for (int ni = 0; ni < nd_num; ++ni)
    {
        if (tree->isLeaf(ni))
        {
            SchemaPath sp;
            tree->getPath(ni, sp);

            col_files.emplace_back(col_dir);
            string &file = col_files.back();
            tree->appendPathWPost(file, sp);
        } // if
    } // for ni

    for (auto & fi : col_files)
    {
       Utility::removeFile(fi);

       string info(fi);  info.append(".info");
       Utility::removeFile(info);
    } // for fi


    // delete schema file
    string schema_path;
    Utility::getSchemaPath(g_config, db, table, schema_path);
    Utility::removeFile(schema_path);

    return 1;
} // dropTable



} // steed
