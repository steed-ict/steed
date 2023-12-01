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
 * @file FileHandler.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for FileHandler:
 *   base class for system io function call 
 */

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include "DebugInfo.h"

namespace steed {

class FileHandler {
public:
    virtual ~FileHandler       (void) = default; 
    FileHandler                (void) = default; 
    FileHandler(const FileHandler &s) = delete ; 

public:
    /**
     * open file specified by pathname to read, write and append
     * @param p  file pathname to open
     * @param f  open flags
     * @return file descriptor
     */
    virtual int open (const char *p, int f)  = 0;

    /**
     * create file and opens it specified by pathname
     * @param p  file pathname to open
     * @param f  open flags
     * @param m  create file mode  
     * @return file descriptor
     */
    virtual int open (const char *p, int f, mode_t m)  = 0;

    /**
     * closes a file descriptor
     * @return 0 success; -1 errors and errno is set appropriately
     */
    virtual int close(void) = 0;

    /**
     * remove file
     * @param p  file pathname
     * @retrun 0 success; -1 errors and errno is set appropriately
     */
    int remove(const char *p);

public:
    /**
     * Blocking read content from file 
     * @param offset    file offset begin to read 
     * @param buffer    buffer used to read 
     * @param bufsize   read length  
     * @return >=0 success; -1 errors and errno is set appropriately
     */
    virtual int64_t read (uint64_t offset,       void *buffer, uint64_t bufsize) = 0;

    /**
     * Blocking rite content to file 
     * @param offset    file offset begin to write  
     * @param buffer    buffer used to write 
     * @param bufsize   write length  
     * @return >=0 success; -1 errors and errno is set appropriately
     */
    virtual int64_t write(uint64_t offset, const void *buffer, uint64_t bufsize) = 0;

    /**
     * repositions the file offset of the open file description
     * @param offset    file offset   
     * @param whence    the directive whence 
     * @return current location from file begin; (uint64_t)-1 as error
     */
    virtual uint64_t seek (uint64_t offset, int whence) = 0;

public: 
    /**
     * get file size
     * @param n  file pathname
     * @return file size
     */
    static uint64_t getFileSize(const char *n) 
    {   struct stat st; stat(n, &st); return st.st_size;   }

    static uint64_t getFileSize(const std::string &n) 
    {   return getFileSize(n.c_str());   }
}; // FileHandler



class FileHandlerViaOS : public FileHandler {
protected:
    int         m_file_desc{-1};  /**< file descriptor */ 
    uint64_t    m_file_off { 0};  /**< file offset to read and write  */

public:
    ~FileHandlerViaOS(void) = default;
    FileHandlerViaOS (void) = default;
    FileHandlerViaOS (const FileHandlerViaOS &s) = delete;

public:
    int open (const char *p, int f)           override 
    { return (m_file_desc = ::open(p, f   )); }

    int open (const char *p, int f, mode_t m) override
    { return (m_file_desc = ::open(p, f, m)); }

    int close(void) override 
    { return ::close(m_file_desc); }

public:
    int64_t  read (uint64_t offset,       void *buffer, uint64_t bufsize) override;
    int64_t  write(uint64_t offset, const void *buffer, uint64_t bufsize) override;
    uint64_t seek (uint64_t offset, int whence) override
    { return ::lseek(m_file_desc, offset, whence); }

private:
    /**
     * seek to the right offset if needed  
     * @param offset    file offset   
     * @return current location from file begin; -1 as error
     */
    uint64_t seek2offset(uint64_t offset)
    { return (offset == m_file_off) ? offset : this->seek(offset, SEEK_SET); }
}; // FileHandlerViaOS

} // namespace steed
