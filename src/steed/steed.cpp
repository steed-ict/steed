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





// call libsteed.so from python using ctypes
// directly call C++ function in C
extern "C" {

void make_clean(void)
{
    // remove python generated files
    std::string pycache = "__pycache__";
    if (steed::Utility::checkFileExisted(pycache))
    {   steed::Utility::removeDir(pycache);   }

    std::string pycfile = "steed.pyc";
    if (steed::Utility::checkFileExisted(pycfile))
    {   steed::Utility::removeFile(pycfile);   }

    // remove store base dir 
    std::string s = steed::g_config.m_store_base;
    steed::Utility::removeDir(s);
} // make_clean


void free_string(char *str)
{
//    printf("STEED: free string\n----\n%s\n----\n", str);
    delete [] str;
} // free_string


void init(const char *cfile)
{
    printf("STEED: init static data\n"); 
    const std::string conf_file(cfile); 
    steed::init(conf_file);    
} // init


void uninit(void)
{
    printf("STEED: uninit static data\n");
    steed::uninit();
} // uninit


int create_database(const char *db)
{
    printf("STEED: create database [%s]\n", db);
    const std::string database(db);
    return steed::createDatabase(database);
} // create_database


int drop_database  (const char *db)
{
    printf("STEED: drop database [%s]\n", db);
    const std::string database(db);
    return steed::dropDatabase(database);
} // drop_database


int create_table(const char *db, const char *table)
{
    printf("STEED: create table [%s.%s]\n", db, table);
    const std::string database(db), tname(table);
    return steed::createTable(database, tname);
} // create_table


int drop_table  (const char *db, const char *table)
{
    printf("STEED: drop table [%s.%s]\n", db, table);
    const std::string database(db), tname(table);
    return steed::dropTable(database, tname);
} // drop_table


int parse_file(const char *db, const char *table, const char *jpath)
{
    printf("STEED: parse json [%s.%s] from [%s]\n", db, table, jpath);
    const std::string database(db), tname(table), jfile(jpath);
    std::ifstream ifs(jfile);
    if (!ifs.is_open())
    {
        printf("STEED: cannot open [%s]!\n", jfile.c_str());
        return -1;
    } // ifs

    steed::ColumnParser *cp = new steed::ColumnParser();
    std::istream *is = &ifs;
    if (cp->init(database, tname, is) < 0)
    {
        printf("STEED: ColumnParser init failed!\n");
        return -1;
    } // if

    int status = 0, cnt = 0;
    while ((status = cp->parseOne()) > 0)
    {
        ++cnt;
        if (cnt % 100000 == 0)
        {   printf("STEED: parsed %d records\n", cnt);   }
    } // while

    delete cp; cp = nullptr;
    ifs.close();

    if (status < 0)
    {
        printf("STEED: insert failed!\n");
        return -1;
    } 
    else
    {
        printf("STEED: parsed %d records\n", cnt);
    } // if

    return 1;
} // parse_file


int assemble_to_file(const char *db, const char *table, char **cols, int ncol, const char *jpath)
{
    printf("STEED: assemble json [%s.%s] to [%s]\n", db, table, jpath);
    const std::string database(db), tname(table), jfile(jpath);
    std::ofstream ofs(jfile);
    if (!ofs.is_open())
    {
        printf("STEED: cannot open [%s]!\n", jfile.c_str());
        return -1;
    } // ofs

    std::vector< std::string > cols_vec;
    for (int ci = 0; ci < ncol; ++ci)
    {   cols_vec.emplace_back(cols[ci]);   } // for ci

    steed::ColumnAssembler *ca = new steed::ColumnAssembler();
    if (ca->init(database, tname, cols_vec) < 0)
    {
        printf("STEED: ColumnAssembler init failed!\n");
        return -1;
    } // if

    char *rbgn = nullptr;
    std::ostream *ostrm = &ofs;
    steed::RecordOutput ro( ca->getSchemaTree() );
    while (ca->getNext(rbgn) > 0)
    {   ro.outJSON2Strm(ostrm, rbgn);   } // while

    delete ca; ca = nullptr;
    ofs.close();

    return 1;
} // assemble_to_file 


const char *assemble_to_string(const char *db, const char *table, const char **cols, int ncol)
{
    printf("STEED: assemble json [%s.%s] to string\n", db, table);
    const std::string database(db), tname(table);

    std::vector< std::string > cols_vec;
    for (int ci = 0; ci < ncol; ++ci)
    {   cols_vec.emplace_back(cols[ci]);   } // for ci

    steed::ColumnAssembler *ca = new steed::ColumnAssembler();
    if (ca->init(database, tname, cols_vec) < 0)
    {
        printf("STEED: ColumnAssembler init failed!\n");
        return nullptr;
    } // if


    char *rbgn = nullptr;
    steed::RecordOutput ro( ca->getSchemaTree() );

    bool first = true;
    std::stringstream ss;
    ss << "[";
    while (ca->getNext(rbgn) > 0)
    {
        if (first) { first = false; }
        else { ss << ","; }

        ro.outJSON2Strm(&ss, rbgn);
    } 
    ss << "]";

    delete ca; ca = nullptr;

    std::string str = ss.str();
    char* cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    return cstr; // NOTE: free this memory in PYTHON
} // assemble_to_string


steed::ColumnParser *open_parser(const char *db, const char *table)
{
    printf("STEED: open column parser [%s.%s]\n", db, table);
    const std::string database(db), tname(table);
    steed::ColumnParser *cp = new steed::ColumnParser();
    if (cp->init(database, tname) < 0)
    {
        printf("STEED: ColumnParser init failed!\n");
        return nullptr;
    } // if

    return cp;
} // create_column_parser


int insert_parser(steed::ColumnParser *cp, const char *recd, uint32_t len)
{
    if (recd == nullptr) { return -1; }

    int64_t s = cp->parseOne(recd, len);
    if (s < 0)
    {
        printf("STEED: insert record failed!\n");
        return -1;
    } // if

    printf("STEED: insert record success!\n");

    return 1;
} // insert_parser


void close_parser(steed::ColumnParser *cp)
{
    printf("STEED: close column parser\n");
    delete cp; cp = nullptr;
} // close_parser


} // extern "C



