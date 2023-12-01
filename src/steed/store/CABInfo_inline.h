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
 * @file ColumnBlock_inline.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   FixedBitVector inline functions 
 */

#pragma once

namespace steed {


inline
void CABInfo::output2debug (void)
{
    printf("-------- CAB Info --------\n");
    printf("CAB : offset@[%lu]\n", m_file_off);
    printf("Size: strg[%u] disk[%u] mem[%u]\n", m_strg_size, m_dsk_size, m_mem_size);
    printf("Type: rep [%u] type[%u]\n", m_rep_type, m_cmp_type);
#ifdef DEFINE_BLM
    printf("Blooming: begin[%lu] mem len[%u] dsk len[%u]\n",
                m_blm_bgn_off, m_blm_mem_len, m_blm_dsk_len);
#endif
    m_item_info.output2debug();
    printf("--------------------------\n\n");
} // output2debug





inline
CABInfoBuffer::~CABInfoBuffer(void)
{
    for (uint32_t i = 0; i < m_foot.m_info_used; ++i)
    {   m_infos[i].~CABInfo();   }

    if ((m_io_tp == write) || (m_io_tp == modify))
    {
        appendFooter();
        m_buf->flush2File();
    } // if 

    delete m_buf; m_buf = nullptr;

    m_infos    = nullptr;
    m_file_size= 0;
    m_next_idx = 0;
    m_io_tp  = invalid;
} // dtor



inline
int CABInfoBuffer::emplaceTailBack(void)
{
    bool  resize = false;
    void *got = m_buf->allocate(s_info_size, resize);
    if   (got == nullptr)
    {
        resize= true; 
        got   = m_buf->allocate(s_info_size, resize);
        if (got == nullptr)
        {
            printf("CABInfoBuffer: emplaceTailBack resize failed!\n");
            return -1;
        } // if 

        // resized: need to reset pointers 
        this->updateMemberPtr(); 
    } // if 

    // placement new to construct the ins 
    got = new (got) CABInfo();
    m_foot.m_info_used += 1;

    return 0;
} // emplaceTailBack



inline 
int CABInfoBuffer::init2write(const string &n, uint64_t rbgn) 
{
    m_buf = new Buffer(s_init_size); 
    if (m_buf->init2write(n) < 0)
    {  
        printf("CABInfoBuffer: init buffer 2 write failed!\n");
        return -1;  
    } // if 

    m_infos = (CABInfo*)m_buf->getNextPosition(); // not used yet  
    m_foot.m_valid_recd = rbgn;
    m_io_tp = write;
  
    return 1; 
} // init2write 




inline
int CABInfoBuffer::appendFooter(void)
{
    bool  resize = false;
    void *ptr = m_buf->allocate(s_foot_size, resize);
    if   (ptr == nullptr)
    {
        resize = true; 
        ptr     = m_buf->allocate(s_foot_size, resize);
        if (ptr == nullptr)
        {
            printf("CABInfoBuffer: appendFooter resize failed!\n");
            return -1;
        } // if 

        // resize: reset pointers 
        this->updateMemberPtr(); 
    } // if 

    *(Footer*)ptr = m_foot; 
    m_file_size += s_info_size * m_foot.m_info_used; 
    m_file_size += s_foot_size;  

    return 0;
} // appendFooter



inline
int CABInfoBuffer::init2read(const string &n) 
{
    m_buf = new Buffer(s_init_size); 
    if (m_buf->init2read(n) < 0)
    {
        printf("CABInfoBuffer: init buffer 2 read failed!\n");
        return -1;
    } // if 
    m_io_tp = read;

    // read CABInfo file
    this->readFile();

    // prepare to read blooming content @ the beginning of file in the future
    FileIO *fb = m_buf->getFileIO();
    fb->seekContent(0, SEEK_SET);
  
    return 1;
} // init2read 



inline 
int CABInfoBuffer::init2append(const string &n) 
{
    m_buf = new Buffer(s_init_size); 
    if (m_buf->init2modify(n) < 0)
    {  
        printf("CABInfoBuffer: init 2 append failed!\n");
        return -1;  
    } // if 
    m_io_tp = modify;

    // read CABInfo file
    this->readFile();

    // prepare to read blooming content @ the beginning of file in the future
    FileIO *fb = m_buf->getFileIO();
    fb->seekContent(0, SEEK_SET);

    return 1; 
} // init2append 



inline
CABInfo *CABInfoBuffer::getTailInfo2Append(void)
{
    if (m_io_tp != modify)
    {
        printf("CABInfoBuffer: getTailInfo2Append invalid io type!\n");
        return nullptr;
    } // if

    // check current tail info is full or not
    m_next_idx = m_foot.m_info_used;
    return getCABInfo(m_next_idx - 1);
} // getTailInfo2Append



inline
void CABInfoBuffer::readFile(void)
{
    // load Footer from file tail 
    uint64_t foot_off = uint64_t(-s_foot_size); // man lseek: off_t is a signed integer
    FileIO *fb = m_buf->getFileIO();
    fb->seekContent(foot_off, SEEK_END);

    m_buf->load2Buffer(s_foot_size, false);
    m_foot = *(Footer*)(m_buf->getPosition(0));
    m_buf->clear();


    // seek and load CABInfo array 
    uint64_t info_size = s_info_size * m_foot.m_info_used;
    uint64_t info_off  = uint64_t( -(s_foot_size + info_size) );
    fb->seekContent(info_off, SEEK_END);

    m_buf->load2Buffer(info_size, true);
    this ->updateMemberPtr();
} // readFile



inline
void CABInfoBuffer::output2debug (void)
{
    if (m_buf == nullptr)
    {
        printf("CABInfoBuffer: output2debug buffer is nullptr!\n");
        return;
    } // if

    printf("Buffer @ [%p] Mem @ [%p]\n", m_buf, m_buf->data()); 
    m_buf->output2debug();
    printf("info @ [%p] next idx:%lu\n", m_infos, m_next_idx);
    printf("------------------------------------------------------------\n");
    for (uint64_t i = 0; i < m_foot.m_info_used; ++i)
    {   m_infos[i].output2debug();   }
    printf("------------------------------------------------------------\n");

    printf("CABInfoBuffer::Footer {valid:%lu, used #:%lu}\n",
                m_foot.m_valid_recd, m_foot.m_info_used);
    printf("File Size [%lu]\n", m_file_size); 
} // output2debug 


} // namespace steed







#if DEFINED_BLM
inline
int64_t CABInfoBuffer::
    flushBloomContent(CABInfo *info, char* blmbin, uint64_t memlen, uint64_t dsklen)
{
    assert(dsklen >= memlen);
    assert(dsklen %  g_config.m_mem_align_size == 0);

    // set blooming storage info
    info->m_blm_bgn_off = m_file_size;
    info->m_blm_mem_len = memlen;
    info->m_blm_dsk_len = dsklen;


    // assert file fb @ offset m_file_size: write directly 
    FileIO *fb = m_buf->getFileIO();
    int64_t got = fb->writeContent(memlen, blmbin);
    if (got < 0)
    {
        printf("CABInfoBuffer: flushBloomContent writeContent failed!\n");
        return got;
    } // if 

    uint64_t delta  = dsklen - memlen; 
    uint64_t offset = fb->seekContent(delta, SEEK_CUR);
    if (offset == uint64_t(-1))
    {
        printf("CABInfoBuffer: flushBloomContent seekContent failed!\n");
        return -1;
    } // if 

    m_file_size += dsklen;  

    // written dsklen size: blooming content + delta align gap
    return got + delta;  
} // flushBloomContent
#endif



#if DEFINED_BLM
inline
int64_t CABInfoBuffer::loadBloomContent(CABInfo *info, char* blmbin)
{
    uint64_t blm_off = info->m_blm_bgn_off; // blooming begin offset in file 
    uint32_t blm_len = info->m_blm_mem_len; // blooming content size 

    FileIO *fb = m_buf->getFileIO();
    uint64_t   got = fb->seekContent(blm_off, SEEK_SET);
    if (got == uint64_t(-1))
    {
        printf("CABInfoBuffer: loadBloomContent seekContent failed!\n");
        return -1;
    } // if 

    return fb->readContent(blm_len, blmbin);
} // loadBloomContent 
#endif