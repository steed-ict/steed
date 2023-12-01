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
 * @file FileIO.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    FileIO is a wrapper for file io operations.
 */

#pragma once

#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <string>

#include "DebugInfo.h"
#include "FileHandler.h"

namespace steed {
using std::string; 

class FileIO {
protected: 
    /** IO type */
    enum IOType { 
        invalid = 0, // invalid
        write   = 1, // write only
        read    = 2, // read  only
        modify  = 3, // read then write to modify
        max     = 4  // maximum type id, invalid 
    }; // enum Type

protected: 
    FileHandler   *m_file_hand{nullptr}; /**< file handler pointer */
    std::string    m_file_name{};        /**< file name string */
    uint64_t       m_file_size{0};       /**< current file size */
    uint64_t       m_offset{0};          /**< file offset to read and write */
    IOType         m_type{invalid};   /**< FileIO type */

public: 
    FileIO (void)             = default;
    FileIO (const FileIO &fb) = delete ;
    virtual ~FileIO(void)  { uninit(); }

public: 
    FileHandler  *getHandler(void)  { return m_file_hand; }
    const string &getNameStr(void)  { return m_file_name; }
    uint64_t      getSize   (void)  { return m_file_size; }
    uint64_t      getOffset (void)  { return m_offset; }

public:
    /**
     * uninit FileIO object
     */
    virtual void uninit(void);

    /**
     * init 2 write the content in m_buffer 
     * @param n        file name string  
     * @return 0 success; <0 failed 
     */
    int init2write(const string &n) 
    { return init(write, n, int(O_CREAT|O_RDWR|O_TRUNC), mode_t(S_IRUSR|S_IWUSR)); }

    /**
     * init 2 read the content in m_buffer 
     * @param n        file name string  
     * @return 0 success; <0 failed 
     */
    int init2read (const string &n)
    { return init(read, n, int(O_RDONLY), mode_t(0));}
    
    /**
     * init 2 modify the content in m_buffer 
     * @param n        file name string  
     * @return 0 success; <0 failed 
     */
    int init2modify(const string &n) 
    { return init(modify, n, int(O_CREAT|O_RDWR), mode_t(S_IRUSR|S_IWUSR)); }

protected:
    /**
     * init FileIO object
     * @param t        IOType
     * @param n        file name string
     * @param flag     open flags
     * @param mt       create file mode
     * @return 0 success; <0 failed 
     */
    virtual int init(IOType t, const string &n, int flag, mode_t mt) = 0;

public: 
    /**
     * write content to this object
     * @param len     content length 
     * @param cont    content begin pointer 2 write 
     * @return >0 write bytes as success; < 0 failed
     */
    virtual int64_t writeContent(uint64_t len, const char* cont) = 0;

    /**
     * read content from file 
     * @param len     content length 
     * @param cont    read the content in m_buffer begins from cont 
     * @return >0 read bytes num, <= m_buf_cap, <= len; 0 EOF; < 0 failed
     */
    virtual int64_t readContent (uint64_t len, char* cont) = 0;

    /**
     * seek content in buffer, the action is similiar to lseek 
     * @param offset    offset in file 
     * @param whence    SEEK_SET and SEEK_CUR
     * @return    offset from the file beginning 
     */
    virtual uint64_t seekContent (uint64_t offset, int whence) = 0; 
  
public: 
    virtual void output2Debug(void);
}; // FileIO 





class FileIOViaOS : public FileIO {
public:
    FileIOViaOS (const FileIOViaOS &f) = delete;
    FileIOViaOS (void) = default;
    ~FileIOViaOS(void) = default;

protected:
    int init(IOType t, const string &n, int flag, mode_t mt) override;

public:
    int64_t  writeContent(uint64_t len, const char* cont) override;
    int64_t  readContent (uint64_t len,       char* cont) override;
    uint64_t seekContent (uint64_t offset,    int whence) override 
    { return (m_offset = m_file_hand->seek(offset, whence)); }
}; // FileIOInOS



inline
int64_t FileIOViaOS::writeContent(uint64_t len, const char *cont)
{
    int64_t wt_size = m_file_hand->write(m_offset, cont, len);
    if (wt_size < 0)
    {
        printf("FileIOViaOS: flush [%ld] Bytes to [%s] failed! errno is [%d]\n",
            wt_size, m_file_name.c_str(), errno);
        DebugInfo::printStackAndExit();
        return -1;
    } // if

    // success
    m_offset += wt_size;
    m_file_size = ((m_offset > m_file_size) ? m_offset : m_file_size);
    return wt_size;
} // writeContent


inline
int64_t FileIOViaOS::readContent(uint64_t len, char *buf)
{
    int64_t rd_size = m_file_hand->read(m_offset, buf, len);
    if (rd_size < 0) 
    {
        printf("FileIOViaOS: read[%ld] Bytes from [%s] failed! errno is [%d]\n",
            rd_size, m_file_name.c_str(), errno);
        DebugInfo::printStackAndExit(); 
        return -1;
    } // if

    // success
    m_offset += rd_size; 
    return rd_size;
} // readContent

} // namespace steed 
