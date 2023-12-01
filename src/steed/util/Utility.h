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
 * @file Utility.h
 * @author  Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *  Defines a set of methods that perform common
 */

#pragma once

#include <unistd.h>
#include <string> 
#include <vector> 

#include "Config.h"
#include "DebugInfo.h"

#include <assert.h> 
#include <stdint.h> 
#include <dirent.h> // dir
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>



namespace steed {
namespace Utility {

using std::string;
using std::vector;

// bit operation
/**
 * calculate used bit number for value
 * @param val integer value calc bit number
 * @return bits number for value
 */
inline
uint32_t calcUsedBitNum(uint32_t val)
{
    uint32_t bnum = 0;
    while (val > 0)
    { val >>= 1, bnum += 1; }
    return bnum;
} // calcUsedBitNum 

/**
 * calculate bytes used by the bit number
 * @param bit_num   bit number
 * @return bytes number
 */
inline
uint32_t calcBytesUsed (uint32_t bit_num)
{ return (bit_num + 7) / 8; }

/**
 * calculate the align size 
 * @param num   number to align
 * @param align align size
 * @return aligned size
 */
inline
uint32_t calcAlignSize (uint32_t num, uint32_t align)
{ return (num + align - 1) / align * align; }

/**
 * calculate the align size 
 * @param num   number to align
 * @param align align size
 * @return aligned size
 */
inline
uint32_t calcSize2Align(uint32_t num, uint32_t align)
{ return calcAlignSize (num, align) - num; }

/**
 * calculate the align begin 
 * @param num   number to align
 * @param align align size
 * @return aligned begin
 */
inline
uint32_t calcAlignBegin(uint32_t num, uint32_t align)
{ return (num / align * align); }



// file & dir operation
/**
 * remove file by name 
 * @param fn    file name string 
 * @return 0 success; <0 failed
 */
inline
int removeFile(const string &fn)
{ return unlink(fn.c_str()); }

/**
 * create a directory by name 
 * @param dn    directory name string
 * @param m     directory mode, default 0755
 * @return 0 success; <0 failed
 */
int makeDir(string &dn, mode_t m=0755);

/**
 * reomve directory by name 
 * @param dn    directory name string
 * @return 0 success; <0 failed
 */
int removeDir(string &dn);

/**
 * get file name string list by directory name  
 * @param dn      directory name string  
 * @param files   file name strings
 * @param prefix  add directory path as file's prefix
 * @return 0 success; <0 failed
 */
int getFileList(const string &dn, vector<string> &files, bool prefix);

/**
 * check file is existed by name 
 * @param fn    file name string  
 * @return true existed; false not existed
 */
inline
bool checkFileExisted(const string &fn)
{ struct stat s; return (stat(fn.c_str(), &s) == 0); }

/** 
 * get file size by name 
 * @param fn    file name string 
 * @return file size; (uint64_t)-1 as error 
 */
inline
uint64_t getFileSize (const string &fn)
{ struct stat s; return uint64_t( (stat(fn.c_str(), &s) == 0) ? s.st_size : -1); }



// steed path operation (config related)
/**
 * use database name to get data storage directory
 * @param c     config object
 * @param db    database   name string
 * @param path  got path string
 */
inline
void getDataBaseDir(const Config &c, const string &db, string &path)
{   path = c.m_store_base + "/" + db + "/";   }


inline
void getSchemaDir(const Config &c, const string &db, string &path)
{
    getDataBaseDir(c, db, path);
    path += c.m_schema_dir + "/";
} // getSchemaDir

/**
 * use database and collection name to get data storage directory 
 * @param c     config object
 * @param db    database   name string 
 * @param clt   collection mame string
 * @param path  got path string    
 */
inline
void getSchemaPath(const Config &c, const string &db, const string &clt, string &path)
{
    getSchemaDir(c, db, path);
    path += clt;
} // getSchemaPath

/**
 * use database name to get data storage directory
 * @param c     config object
 * @param db    database   name string
 * @param path  got path string
 */
inline
void getDataDir(const Config &c, const string &db, string &path)
{
    getDataBaseDir(c, db, path);
    path += c.m_data_dir + "/";
} // getDataDir

/**
 * use database and collection name to get data storage directory
 * @param c     config object
 * @param db    database   name string
 * @param clt   collection mame string
 * @param path  got path string
 */
inline
void getDataDir (const Config &c, const string &db, const string &clt, string &path)
{
    getDataDir(c, db, path);
    path += clt + "/";
} // getDataDir

/**
 * use data directory and column name to get data storage directory
 * @param data_dir    data directory name string
 * @param col       column name string
 * @param path      got path string
 */
inline
void getDataPath(const string &data_dir, const string &col, string &path)
{   path = data_dir + col;   }

/**
 * use database, collection and column name to get data storage directory
 * @param c     config object
 * @param db    database   name string
 * @param clt   collection mame string
 * @param col   column     name string
 * @param path  got path string
 */
inline
void getDataPath(const Config &c, const string &db, const string &clt,
        const string &col, string &path)
{
    getDataDir (c, db, clt, path); 

    const string &data_dir(path);
    getDataPath(data_dir, col, path);
} // getDataPath

/**
 * use database, collection and column name to get index storage directory
 * @param c     config object
 * @param db    database   name string
 * @param clt   collection mame string
 * @param col   column     name string
 * @param path  got path string
 */
inline
void getIndexPath(const Config &c, const string &db, const string &clt,
        const string &col, string &path)
{
    getDataPath(c, db, clt, col, path);
    path.append(".idx");
} // getIndexPath


// string operation
/**
 * split the string to the piece by the delim
 * @param str      original string content 
 * @param delim    delimiter to split  
 * @param piece    piece as return result 
 */
void splitString(string &str, const string &delim, vector<string> &piece);

} // namespace Utility
} // namespace steed