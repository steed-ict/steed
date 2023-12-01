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
 * @file JSONRecordBuffer.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 * JSON text record buffer 
 */

#pragma once 

#include <array>
#include <iostream>
#include <stdint.h>

#include "Config.h"
#include "Buffer.h"
#include "JSONRecordReader.h"


namespace steed {

using std::array;
using std::istream;

extern Config g_config;

class JSONRecordBuffer {
protected:
    /** record reader function pointer */
    typedef JSONRecordReader::ReadFPtr ReadFPtr; 

public:
    /** invalid offset value in m_offset_array  */ 
    static const uint32_t s_invalid_offset; 
    static const uint32_t s_recd_num = 16; 

protected:
    /**
     * records read in stream
     * when m_strm == nullptr, direct append the text json records to buffer
     */
    istream           *m_strm   {nullptr};
    Buffer            *m_buff   {nullptr}; /**< json text record buffer*/
    JSONRecordReader  *m_recd_rd{nullptr}; /**< json text record reader*/ 

    /** record begin offset in buffer */ 
    array<uint32_t, s_recd_num> m_offset_array; 
    uint32_t    m_elem_idx {0};  /**< elements index visited */ 
    uint32_t    m_elem_used{0};  /**< elements used in array */ 


public:
    JSONRecordBuffer (istream *i);
    ~JSONRecordBuffer(void);
                       
public:
    /**
     * get next text json record from buffer  
     * @param recd_bgn  record begin position 
     * @param recd_len  record binary length 
     * @return 1 return number; 0 EOF; <0 failed
     */
    int  nextRecord(char* &recd_bgn, uint64_t &recd_len);

    /**
     * read records in batch  
     * @param fptr     read function pointer: read or sample  
     * @param rnum     records number need to read 
     * @param recds    records begin position 
     * @return >0 success; 0 EOF; <0 failed
     */
    int  readRecords(ReadFPtr fptr, uint32_t rnum, array<char*, s_recd_num> &recds);

    /** reset the reader and buffer to read from stream begin */
    void reset(void);

private:
    /**
     * clear m_offset_array content: 
     * set all elements as s_invalid_offset
     */
    void clearOffsetArray(void);

    /**
     * read one json record to buffer from input stream
     * @param fptr     read function pointer: read or sample  
     * @return >0 return number; 0 EOF; <0 failed
     */
    int  read2buffer (ReadFPtr fpt, uint32_t rnum = s_recd_num);

public:
    /**
     * append one text json record to buffer
     * @param recd  text json record begin position
     * @param len   text json record length
     * @return 1 success; <0 failed
    */
    int appendOneRecd(const char *recd, uint32_t len);

public:
    /** output buffer content to debug */
    void output2debug(void);
}; // JSONRecordBuffer





inline
JSONRecordBuffer::JSONRecordBuffer (istream *i): m_strm(i)
{
    uint64_t size = g_config.m_text_buffer_number * g_config.m_text_recd_avg_len;
    m_buff = new Buffer(size);
    m_buff->initInMemory();  

    if (i != nullptr)
    {
        uint8_t mod = JSONRecordReader::single;
        m_recd_rd = new JSONRecordReader(m_buff, m_strm, mod);
    } // if 

    this->clearOffsetArray();
} // ctor 



inline
JSONRecordBuffer::~JSONRecordBuffer(void)
{
    m_strm = nullptr;
    this->clearOffsetArray();
    if (m_buff    != nullptr) { delete m_buff;    m_buff    = nullptr; }
    if (m_recd_rd != nullptr) { delete m_recd_rd; m_recd_rd = nullptr; }
} // dtor 



inline
int JSONRecordBuffer::nextRecord(char* &recd_bgn, uint64_t &recd_len)
{
    bool all_used = (m_elem_idx == m_elem_used);
    if  (all_used)
    {
        int got_num =  read2buffer(&JSONRecordReader::readRecord);
        if (got_num <= 0) { return got_num; }
    } // if 

    uint64_t offset = m_offset_array[m_elem_idx];
    recd_bgn = (char*)m_buff->getPosition(offset);
    
    bool     not_tail = (m_elem_idx + 1 < m_elem_used);
    uint64_t next_bgn = not_tail ? m_offset_array[m_elem_idx+1] : m_buff->used();
    recd_len =  next_bgn - offset;

    ++m_elem_idx; 

    return 1;
} // nextRecord



inline
int JSONRecordBuffer::
    readRecords(ReadFPtr fptr, uint32_t rnum, array<char*, s_recd_num> &recds)
{
    if (rnum > s_recd_num)
    {   rnum = s_recd_num;   }

    recds.fill(nullptr);

    int got = read2buffer(fptr, rnum);
    if (got <= 0)  { return got; }

    for (m_elem_idx = 0; m_elem_idx < m_elem_used; ++m_elem_idx)
    {
        uint32_t off = m_offset_array[m_elem_idx];
        recds[m_elem_idx] = (char*)m_buff->getPosition(off);
    } // for

    return m_elem_used;
} // readRecords



inline
void JSONRecordBuffer::reset(void)
{
    this->clearOffsetArray();

    if (m_recd_rd != nullptr)
    {   m_recd_rd->reset();   }
} // reset



inline
int JSONRecordBuffer::read2buffer (ReadFPtr fptr, uint32_t rnum)
{
    m_buff->clear(), this->clearOffsetArray(); 

    if (m_recd_rd == nullptr) { return 0; }
    
    uint64_t    rlen = 0;
    const char *rbgn = nullptr; 
    uint32_t rcap = (rnum < s_recd_num) ? rnum : s_recd_num;
    while (m_elem_used < rcap)
    {
        uint32_t offset = m_buff->used();
        if ((m_recd_rd->*fptr)(rbgn, rlen) <= 0)
        {   break;   }

        m_offset_array[m_elem_used++] = offset;
    } // while 
   
    return m_elem_used;
} // read2buffer



inline
int JSONRecordBuffer::appendOneRecd(const char *recd, uint32_t len)
{
    if (m_buff == nullptr) { return -1; }

    uint32_t offset = m_buff->used();
    m_buff->append(recd, len);
    m_offset_array[m_elem_used++] = offset;

    return 1;
} // appendOneRecd



inline
void JSONRecordBuffer::output2debug(void)
{
    printf("JSONRecordBuffer:m_buff cap:[%lu]\tused:[%lu]\n",
        m_buff->capacity(), m_buff->used());

    puts("JSONRecordBuffer: the records in buffer:");
    for (uint32_t i = 0; i < m_elem_used; ++i)
    {
        printf("\n[%u] ----------------------------------------\n", i);
        uint32_t off = m_offset_array[i];
        char *recd = (char*)m_buff->getPosition(off);
        printf("%s", recd);
    } // for
    printf("\n----------------------------------------\n");
} // output2debug


} // namespace steed