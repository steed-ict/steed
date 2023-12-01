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
 * @file JSONRecordReader.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 * read one Text record from file 
 */

#pragma once 

#include <assert.h>

#include <fstream>
#include <iostream>
#include <string>

#include "Buffer.h"
#include "Config.h"
#include "RandomValues.h"
#include "InStreamSeeker.h"


namespace steed {

using std::istream;


class JSONRecordReader {
protected: 
    Buffer       *m_buff {nullptr}; /**< json text record buffer */
    istream      *m_strm {nullptr}; /**< records read in stream  */
    uint8_t       m_lines{invalid}; /**< json records used lines */ 

public:    
    enum Mode { 
        invalid = 0,  // invalid mode 
        single  = 1,  // each line is one record
        multiple= 2,  // record in  multi lines 
        max_line= 3,  // max mode id
    }; 

public:
    ~JSONRecordReader(void);
    JSONRecordReader (Buffer *buf, istream *in, uint8_t md);

public:
    /**
     * reset the reader to read from begin 
     * @return 0 success; <0 failed
     */
    void reset(void)
    { m_buff->clear(); m_strm->clear(); m_strm->seekg(0); }

public:
    /**
     * read record function pointer from instream 
     * @param recd_bgn    record bin begin 
     * @param recd_len    record bin content length 
     * @return 0 success; <0 failed
     */
    typedef
    int (JSONRecordReader::*ReadFPtr)(const char* &recd_bgn, uint64_t &recd_len);

    /**
     * read one record from instream 
     * @param recd_bgn    record bin begin 
     * @param recd_len    record bin content length 
     * @return 0 success; <0 failed
     */
    int readRecord  (const char* &recd_bgn, uint64_t &recd_len);

protected:
    /**
     * read record on single line from instream 
     * @param recd_bgn    record bin begin 
     * @param recd_len    record bin content length 
     * @return 0 success; <0 failed
     */
    int readOneLineRecord  (const char* &recd_bgn, uint64_t &recd_len)
    { return readLine(recd_bgn, recd_len); }

    /**
     * read record on multiple lines from instream 
     * TODO : this function is not done yet
     * @param recd_bgn    record bin begin 
     * @param recd_len    record bin content length 
     * @return 0 success; <0 failed
     */
    int readMultiLineRecord(const char* &recd_bgn, uint64_t &recd_len)
    { (void)recd_bgn; (void)recd_len; return -1; }

    /**
     * read one line from instream 
     * @param ln_bgn    line bin begin 
     * @param ln_len    line bin content length 
     * @return 0 success; <0 failed
     */
    int readLine  (const char* &line_bgn, uint64_t &line_len);
}; // JSONRecordReader





inline
JSONRecordReader::~JSONRecordReader(void)
{
    m_buff = nullptr, m_strm = nullptr; m_lines = invalid;
} // dtor 



inline
JSONRecordReader::JSONRecordReader (Buffer *buf, istream *in, uint8_t md)
{
    m_buff = buf, m_strm = in, m_lines = md;
} // ctor  



inline
int JSONRecordReader::readRecord (const char* &recd_bgn, uint64_t &recd_len)
{
    recd_bgn = nullptr, recd_len = 0; 

    int retval = -1;
    switch (m_lines)
    {
        case single:   retval = readOneLineRecord  (recd_bgn, recd_len); break;
        case multiple: retval = readMultiLineRecord(recd_bgn, recd_len); break;
        default: break;
    } // switch

    return retval;
} // readRecord



inline 
int JSONRecordReader::readLine  (const char* &line_bgn, uint64_t &line_len)
{

    uint64_t org_used = m_buff->used();
    bool done = false;
    do
    {
        uint64_t avail_num = m_buff->available();
        void    *avail_bgn = m_buff->getNextPosition();

        m_strm->getline((char*)avail_bgn, avail_num); 
        uint64_t read_num = m_strm->gcount(); 
        if (read_num > 0) // success
        {
            m_buff->allocate(read_num, false); // already with additional '\0'
            done = true;
        }
        else if (m_strm->eof ())
        {   return 0;   } 
        else if (m_strm->fail()) // not finished line 
        {
            uint64_t buf_cap = m_buff->capacity();
            m_buff->allocate(read_num, false); 
            m_buff->reserve (buf_cap * 2);
            m_strm->clear();
        }
        else if (m_strm->bad()) // failed 
        {
            fflush(nullptr);
            return -1;
        }
    } while (!done);

    line_bgn = (const char *)m_buff->getPosition(org_used);
    line_len = (m_buff->used() - org_used);
    return line_len; 
} // readLine

} // namespace steed
