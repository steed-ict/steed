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
 * @file Buffer_inline.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   Buffer inline functions definitions.
 */

#pragma once

namespace steed {

inline
Buffer::~Buffer (void)
{
    if (m_buffer != nullptr)
    { free(m_buffer); m_buffer = nullptr; } 
    m_used = 0, m_cap = 0;

    bool is_rd  = (m_io_type == read  );
    bool is_wt  = (m_io_type == write );
    bool is_md  = (m_io_type == modify);
    bool own_fb = is_rd || is_wt || is_md; // own FileIO instance 
    if ((own_fb) && (m_file_io != nullptr))
    { delete m_file_io; m_file_io = nullptr; }

    m_io_type  = invalid;
} // dtor

inline
int Buffer::init2write(const std::string &n)
{
    m_io_type = write;
    m_file_io = new FileIOViaOS();
    return m_file_io->init2write(n);
} // init2write
 
inline
int Buffer::init2read (const std::string &n)
{
    m_io_type = read;
    m_file_io = new FileIOViaOS(); 
    return m_file_io->init2read(n); 
} // init2read

inline
int Buffer::init2modify (const std::string &n)
{
    m_io_type = modify;
    m_file_io = new FileIOViaOS(); 
    return m_file_io->init2modify(n); 
} // init2modify 


inline
void* Buffer::allocate(uint64_t len, bool resize) 
{
    uint64_t nu = m_used + len; // next used
    if (nu > m_cap)
    {
        if (!resize) 
        {
            printf("Buffer: rest is not enough without resize!\n");
            return nullptr;
        } // if 

        if (reserve(nu * 2) < 0) 
        {
            printf("Buffer: resize to allocator failed!\n");
            return nullptr;
        } // if 
    } // if 

//printf("Buffer [%p]: allocate [%lu] @ [%p]\n",
//    this, len, getNextPosition());

    void* got = getNextPosition(); 
    m_used += len;
    return got; 
} // allocate


inline
int Buffer::deallocate(uint64_t len) 
{
    int  ret_val  = 0;
    bool underflow = (len > m_used);  
    if  (underflow)
    {
        printf("Buffer: return to deallocate underflow!\n");
        ret_val = -1;
    }
    else 
    {   m_used -= len; }

    return ret_val;  
} // deallocate


inline
int Buffer::reserve(uint64_t cap)
{
    if (m_cap >= cap) { return 0; }

    cap = Utility::calcAlignSize(cap, this->m_align);
    int   ret_val = 0; 
    char *buf_got = (char*)steedRealloc(m_buffer, cap); 
    if   (buf_got == nullptr)
    {
        printf("Buffer: reserve failed!\n");
        ret_val = -1;
    }
    else  
    {
        m_buffer = buf_got;
        m_cap    = cap;
        memset(m_buffer + m_used, 0, m_cap - m_used);
    }  // if 
    return ret_val; 
} // reserve


inline
int Buffer::append(const void *src, uint64_t len)
{
    bool resize = (m_used + len > m_cap);
    if ((resize) && (reserve(m_used + len) < 0))
    {
        printf("Buffer: reserve to append failed!\n");  
        return -1; 
    } //if 

    void *dst = this->getNextPosition();
    memcpy(dst, src, len);
    m_used += len;
    return  0;
} // append


inline
int Buffer::flush2File(uint64_t bgn, uint64_t end)
{
    if (m_file_io == nullptr)
    {
        printf("Buffer: FileBuffer not init 2 flush!\n");
        return -1;
    } // if

    bool     fl_all = (end == (uint64_t)-1);
    uint64_t fl_bgn =  bgn; 
    uint64_t fl_end = (fl_all ? this->used() : end);
    uint64_t len    = fl_end - fl_bgn;
    void    *src = this->getPosition(fl_bgn);

    // system call 2 write into file
    return m_file_io->writeContent(len, (char*)src);
} // flush2File


inline
int Buffer::load2Buffer(uint64_t len, bool resize)
{
    if (m_file_io == nullptr)
    {
        printf("Buffer: FileBuffer not init 2 load!\n");
        return -1;
    }

    int   ret_val = 0;
    char* rd_pos  = (char*)allocate(len, resize);
    if   (rd_pos == nullptr)
    {
        printf("Buffer: allocate 2 load failed!\n"); 
        ret_val = -1;
    } 
    else
    {   ret_val = m_file_io->readContent(len, rd_pos);   } 

    return ret_val;
} // load2Buffer


inline
void Buffer::output2debug(void)
{
    printf("Buffer output2debug:\n");
    printf("m_buffer:[%p] m_used:[%u] m_cap:[%u]\n", m_buffer, m_used, m_cap);
    printf("available size:[%lu]\n\n", available());

    // print buffer content to debug in hex and char 
    uint64_t w = 8; // width
    uint64_t l = (m_used + 7) / 8; 
    for (uint64_t i = 0; i < l; ++i)
    {
        for (uint64_t j = 0; j < w; ++j)
        { printf("%2X ", (uint8_t)m_buffer[i*w+j]); } 
    
        printf("\t | \t");
    
        for (uint64_t j = 0; j < w; ++j)
        { printf("%2c ", (char   )m_buffer[i*w+j]); } 
    
        puts("");
    } // for i
    
    puts("\n\n"); 

    return;
} // output2debug







} // namespace steed
