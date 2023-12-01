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
 * @file Buffer.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for memory Buffer
 */

#pragma once

#include <stdint.h>  // uint64_t 
#include <string>    // string 

#include "Config.h"
#include "Utility.h"
#include "Allocator.h"
#include "FileIO.h" 

namespace steed {

class Buffer {
protected:
    char    *m_buffer{nullptr}; /**< buffer in memory  */
    uint32_t m_used{0};         /**< buffer used bytes */
    uint32_t m_cap {0};         /**< buffer capability */

    FileIO  *m_file_io{nullptr}; /**< read and write with file   */
    uint8_t  m_io_type{invalid}; /**< buffer mode: default in mem*/

public: 
    const uint32_t m_align{0}; /**< memory aligned base */ 
    
    /** buffer mode */
    enum BufferMode { 
        invalid = 0, // BufferMode invalid
        write   = 1, // write only
        read    = 2, // read  only
        modify  = 3, // read then write to modify
        inmem   = 4, // in memory without IO
        max     = 5  // maximum type id, invalid
    }; // enum Type

public:
    ~Buffer(void);
    Buffer (uint32_t cap = 0);
    Buffer (const Buffer&) = delete;

public:
    /**
     * init the instance in memory 
     * @param n        file name string  
     * @return 0 success; <0 failed 
     */
    int initInMemory(void)
    { m_io_type = inmem; return 0; }

    /**
     * init 2 write the content in m_buffer 
     * @param n        file name string 2 write 
     * @return 0 success; <0 failed 
     */
    int init2write (const std::string &n);

    /**
     * init 2 read the content in file 2 memory assigned 
     * @param n        file name string  
     * @return 0 success; <0 failed 
     */
    int init2read  (const std::string &n); 
  
    /**
     * init 2 read and write the content with file 
     * @param n        file name string  
     * @return 0 success; <0 failed 
     */
    int init2modify(const std::string &n); 

    /**
     * set FileIO to this instance,
     *   which means this buffer is in memory instance
     * @param md    io mode
     * @param fb    FileIO instance 
     * @return 0 success; <0 failed 
     */
    void    setFileIO(FileIO *fb)
    { m_io_type = inmem; m_file_io = fb; }
  
    FileIO *getFileIO(void)
    { return m_file_io; }

public: 
    void     clear     (void)  { m_used = 0;      }
    void*    data      (void)  { return m_buffer; }
    bool     valid     (void)  { return (m_io_type != invalid); }

    uint64_t used      (void)  { return m_used; }        
    uint64_t capacity  (void)  { return m_cap ; }        
    uint64_t available (void)  { return m_cap - m_used; }

    void*    allocate  (uint64_t len, bool resize);
    int      deallocate(uint64_t len);
    int      reserve   (uint64_t cap);
    int      append    (const void *src, uint64_t len);

public:
    /**
     * get align size
     */
    uint32_t getAlignSize(void) { return m_align; }

    /**
     * get position using the offset  
     * @param off    offset need to get the position
     * @return  position with offset off
     */
    void *getPosition(uint64_t off)
    { return ((off < m_used) ? (m_buffer + off) : nullptr); } 

    /**
     * get next available position in buffer when write 
     * @return    next available position
     */
    void *getNextPosition(void) { return (m_buffer + m_used); }
 
public:
    /**
     * flush block content from buffer 2 file 
     * @param bgn    begin offset write to file: default is 0  as begin 
     * @param end    end   offset write to file: default is -1 as end
     * @return >0 write bytes number; <0  error 
     */
    int flush2File(uint64_t bgn = 0, uint64_t end = (uint64_t)-1);

    /**
     * load content from file 2 buffer
     * @param len    content length 
     * @param resize  allow realloc flag to resize buffer 
     * @return >0 read-in block size; 0 EOF; <0 error
     */
    int load2Buffer(uint64_t len, bool resize);

public:
    void output2debug(void) ;
}; // Buffer

} // namespace steed 

#include "Buffer_inline.h"