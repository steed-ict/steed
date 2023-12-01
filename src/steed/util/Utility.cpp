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
 * @file Utility.cpp
 * @author  Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    Utility functions
 */

#include "Utility.h"

namespace steed {
namespace Utility {


int makeDir(string &dn, mode_t m)
{
    if (dn.back() != '/') { dn +=  '/'; }

    int     got  = -1;
    size_t  dpos =  0; // delimiter position 
    while ((dpos = dn.find_first_of('/', dpos)) != string::npos)
    {
        string path = dn.substr(0, dpos++);
        if (path.size() == 0) { continue; }

        got = mkdir(path.c_str(), m);
        if  (errno == EEXIST)
        { got = 0; }
        else if (got < 0)
        { break; }
    } // while   

    if (got < 0)
    {
        printf("Utility: makeDir [%s] failed [%d]!\n", dn.c_str(), errno);
        DebugInfo::printStackAndExit(); 
    } // if 

    return got;
} // makeDir 



int removeDir(string &dn)
{
    if (dn.empty())
    {
        puts("Utility: remove no name directory failed!"); 
        DebugInfo::printStackAndExit(); 
    } // if

    if (dn.back() != '/') { dn +=  '/'; }

    // remove all in dn 
    const char *pstr =  dn.c_str();
    DIR *dir =  opendir(pstr);
    if  (dir == nullptr)
    {
        printf ("Utility: open dir [%s] failed!\n", pstr); 
        DebugInfo::printStackAndExit();
    } // if

    int got = 0;
    struct dirent * enptr = nullptr;          // entry pointer
    while ((enptr = readdir(dir)) != nullptr) // read entry from directory 
    {   
        const char *name = enptr->d_name;
        bool current = (strcmp(name, "." ) ==  0); // current directory
        bool parent  = (strcmp(name, "..") ==  0); // parent  directory 
        bool is_link = (enptr->d_type      == 10); // link file 
        if (current || parent || is_link)
        {   continue;   }

        string fn = dn + name; 
        bool is_dir =  (enptr->d_type ==  4); // sub-directory 
        if  (is_dir)
        {   got = removeDir (fn);   } 
        else
        {   got = removeFile(fn);   }

        if (got < 0)   { break; }
    } // while 

    closedir(dir);
 
    // remove dn
    if (got == 0) { got = rmdir(dn.c_str()); }
    if (got <  0)
    {   printf("Utility::remove dir [%s] failed!\n", dn.c_str());   }
    return got;
} // removeDir



int getFileList(const string &dn, vector<string> &files, bool prefix)
{
    files.clear(); 
    string path(dn); 
    if (path.back() !='/')  { path += '/'; }

    const char *pstr = path.c_str();
    DIR *dir =  opendir(pstr);
    if  (dir == nullptr)
    {
        printf("Utility: open dir [%s] failed!\n", pstr);
        DebugInfo::printStackAndExit();
    } // if


    struct dirent * enptr = nullptr;          // entry pointer
    while ((enptr = readdir(dir)) != nullptr) // read from directory 
    {   
        const char *name = enptr->d_name;
        bool is_current = (strcmp(name, "." ) == 0); // current directory
        bool is_parent  = (strcmp(name, "..") == 0); // parent  directory 
        bool is_link    = (enptr->d_type == 10); // link file 
        bool is_dir     = (enptr->d_type ==  4); // sub-directory 
        bool is_sub_dir = is_dir && !is_current && !is_parent; 
        bool is_file    = !(is_current || is_parent || is_link || is_sub_dir);
        if  (is_file)
        {
            if (prefix)
            {
                files.emplace_back (path);
                string &fn = files.back();
                fn.append(name);
            }   
            else
            {   files.emplace_back (name);   } 
        }
        else if (is_sub_dir)
        {
            string sub_dir(path + name);
            getFileList(sub_dir, files, prefix);
        } // if-else 
    } // while 

    closedir(dir);
    return 0;
} // getFileList



void splitString(string &str, const string &delim, vector<string> &piece)
{
    piece.clear(); 

    uint64_t bgn = 0, end = 0;
    while  ((end = str.find_first_of(delim, bgn)) != string::npos)
    {
        if (bgn != end)
        {   piece.emplace_back(str, bgn, end - bgn);   }

        bgn = end + 1;
    } // while 

    uint64_t  cap = str.size();
    if (bgn < cap)
    {   piece.emplace_back(str, bgn, cap - bgn);   } 
} // splitString


} // namespace Utility 
} // namespace steed