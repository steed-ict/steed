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
 * @file BinaryValueArray_inline.h
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0 
 * @section DESCRIPTION
 *   BinaryValueArray inline functions
 */


#pragma once 

namespace steed {


inline
BinaryValueArray *BinaryValueArray::create(Buffer *buf, DataType *dt)
{
    return (dt->getDefSize() > 0) ?
           static_cast<BinaryValueArray*>(new FixLengthValueArray(buf, dt)) :
           static_cast<BinaryValueArray*>(new VarLengthValueArray(buf, dt));
} // create 



inline
void BinaryValueArray::output2debug(void)
{
    puts("== BinaryValueArray ==============================");
    printf("dt:[%d], bgn@[%p] cap:[%lu] num:[%lu]\n",
          m_dt->getTypeID(), m_cont_bgn, m_val_cap, m_val_num);
} // output2debug





inline
FixLengthValueArray::
FixLengthValueArray(Buffer *buf, DataType *dt) : BinaryValueArray(buf, dt)
{   m_length = dt->getDefSize();   }



inline
void FixLengthValueArray::uninit(void)
{
    m_cont_bgn = nullptr; 
    m_val_cap  = 0, m_val_num = 0; 
} // uninit



inline
int FixLengthValueArray::init2read (uint64_t len, void* bgn, uint64_t num)
{
    m_cont_bgn = (char*)bgn;    
    m_val_cap  = len / m_length;
    m_val_num  = num;
    return (m_val_num > m_val_cap) ? -1 : 0;
} // init2read



inline
int FixLengthValueArray::init2write(uint64_t len, void* bgn)
{
    m_cont_bgn = (char*)bgn;
    m_val_cap  = len / m_length;

    // init all values as null
    m_dt->fillNull(m_cont_bgn, m_val_cap);

    return 0;
} // init2write



inline
int64_t FixLengthValueArray::copyContent(BinaryValueArray *src)
{
    if  (this == src)   { return  0; }

    // valid check
    auto *flva  = dynamic_cast<FixLengthValueArray*>(src);
    bool  diff  = (flva == nullptr);
    bool  empty = (m_val_num ==  0);
    bool  small = (m_val_cap < flva->m_val_cap);
    if   (small || !empty || diff) { return -1; }

    // copy content
    m_val_num = flva->m_val_num; 
    uint64_t cp_len  = m_val_num * m_length; 
    memcpy(m_cont_bgn, flva->m_cont_bgn, cp_len);

    return int64_t(cp_len);
} // copyContent



inline
int FixLengthValueArray::read(uint64_t idx, const void* &bin, uint32_t &len)
{
    bin  = (const void *)read(idx);
    return (bin == nullptr) ? (len = 0, 0) : (len = m_length, 1);
} // read



inline
int FixLengthValueArray::writeText(const char *txt, const void* &bin) 
{
    if (m_val_num == m_val_cap)
    {
        bin = nullptr;
        return 0;
    }  

    bin = m_cont_bgn + m_val_num * m_length;
    int used = m_dt->transTxt2Bin(txt, const_cast<void*>(bin), m_length);
    if (used < 0)  { abort(); } // always success

    ++m_val_num;
    return 1; 
} // writeText



inline
int FixLengthValueArray::writeBinVal(uint64_t len, const void *bin)
{
    (void)len;
    if (m_val_num == m_val_cap) { return 0; }  // full 

    void *dst = m_cont_bgn + m_val_num * m_length;
    m_dt->copy(bin, dst);

    ++m_val_num;
    return 0;
} // writeBinVal



//inline
//int FixLengthValueArray::getWrittenBin(const void* &bin, uint32_t &len)
//{
//    uint64_t idx = m_val_num - 1;
//    len  = m_length; 
//    bin  = m_cont_bgn + idx * m_length;
//    return 1;
//} // getWrittenBin



inline
int64_t FixLengthValueArray::appendValues (Buffer *buf)
{
    uint64_t vlen = getWriteValueArrayUsed();
    void    *dest = buf->allocate(vlen, false);
    memcpy  (dest, m_cont_bgn, vlen);
    return int64_t(vlen);
}  // appendValues



inline
void FixLengthValueArray::output2debug(void)
{
    BinaryValueArray::output2debug();
    printf("FIXED value array:[%u]\n", m_length);
    for(uint32_t i = 0; i < m_val_num; ++i)    
//    {   printf("<%lf>\n", *(((double*)m_cont_bgn) + i));   }
    {   printf("<%ld>\n", *(((int64_t*)m_cont_bgn) + i));   }

    puts("\n==================================================\n");
} // output2debug










inline
VarLengthValueArray::~VarLengthValueArray(void)
{
    m_offsets = nullptr, m_rd_vbgn = nullptr, m_rd_vlen = 0; 
    m_cur_off = 0, m_nxt_buf_idx = 0, m_cur_buf = nullptr; 
    for (auto &b : m_buf_vec)
    {
        delete b; b = nullptr;
    }
    m_buf_vec.clear();
} // dtor 



inline
void VarLengthValueArray::uninit(void)
{
    m_cont_bgn = nullptr; 
    m_val_cap  = m_val_num = 0; 

    m_offsets = nullptr;
    m_rd_vbgn = nullptr; 
    m_rd_vlen = 0;

    m_cur_buf = nullptr;
    for (uint64_t bi = 0; bi < m_nxt_buf_idx; ++bi)
    { m_buf_vec[bi]->clear(); }

    m_cur_off = m_nxt_buf_idx = 0;
} // uninit 



inline
int VarLengthValueArray::init2read (uint64_t len, void* bgn, uint64_t num)
{
    m_cont_bgn = (char*)bgn; 
    m_val_cap  = m_val_num  = num;

    uint64_t off_len = m_val_num * s_offset_size;
    m_rd_vlen = len - off_len; 
    m_offsets = (uint32_t*)m_cont_bgn;
    m_rd_vbgn = m_cont_bgn +  off_len;

    return 0;
} // init2read



inline
int  VarLengthValueArray::init2write(uint64_t len, void* bgn)
{
    m_cont_bgn = (char*)bgn; 
    m_val_cap  = len / s_offset_size;
    m_offsets  = (uint32_t*)m_cont_bgn;

    // init all offset as null
    for (uint64_t i = 0; i < m_val_cap; ++i)
    {   m_offsets[i] = s_invalid_off;   }

    // init buffer for binary values
    if (m_nxt_buf_idx == m_buf_vec.size())
    {
        Buffer *buf = new Buffer(s_buffer_size);
        m_buf_vec.emplace_back  (buf);
    } // if 

    m_cur_buf = m_buf_vec[m_nxt_buf_idx++];

    return 0;
} // init2write



inline
int64_t VarLengthValueArray::copyContent(BinaryValueArray *src) 
{ 
    if  (this == src)   { return  0; }

    // valid check
    auto *vlva  = dynamic_cast<VarLengthValueArray*>(src);
    bool  diff  = (vlva == nullptr);
    bool  empty = (m_val_num ==  0);
    bool  small = (m_val_cap < vlva->m_val_cap);
    if   (small || !empty || diff) { return -1; }

    // copy content
    m_val_num = vlva-> m_val_num; 
    uint64_t off_len = m_val_num * s_offset_size; 
    memcpy(m_offsets, vlva->m_offsets, off_len);
    m_cur_off = vlva->m_rd_vlen;

    uint64_t val_len = vlva->getReadValueArrayUsed();
    void *bgn = m_cur_buf->allocate(val_len, true);
    memcpy(bgn, vlva->m_rd_vbgn, vlva->m_rd_vlen);

    return int64_t(off_len + val_len);
} // copyContent



inline
uint64_t VarLengthValueArray::getWriteValueArrayUsed(void)
{
    uint64_t total = 0;
    for(auto &bf : m_buf_vec)
    {   total += bf->used();  }
    return total;
} // getWriteValueArrayUsed



inline
const void *VarLengthValueArray::read(uint64_t idx)
{
    if (idx >= m_val_num) { return nullptr; } 

    uint32_t off =  m_offsets[idx];
    return  (off == s_invalid_off) ? nullptr : (char*)(m_rd_vbgn + off);
} // read



inline
int VarLengthValueArray::read(uint64_t idx, const void* &bin, uint32_t &len) 
{
    bin = (const void*)read(idx);
    if (bin == nullptr)
    {   len = 0; return 0;   }

    bool tail = (idx + 1 == m_val_num);
    uint32_t my_nxt = (tail ? m_cur_off : m_offsets[idx+1]);
    uint32_t my_bgn = m_offsets[idx];
    len = my_nxt - my_bgn;    
    return 1;
} // read 



inline
int VarLengthValueArray::writeText(const char *txt, const void* &bin) 
{
    if (m_val_num == m_val_cap) { return 0; }  // full 

    int used = trans2Bin(txt, bin);
    if (used < 0)
    {
        // current buffer is not enough
        uint32_t blen = m_dt->getBinSizeByTxt(txt);
        uint32_t cap  = (blen > s_buffer_size) ?
            Utility::calcAlignSize(blen, m_align) : s_buffer_size;

        // no more available buffer 
        if (m_nxt_buf_idx == m_buf_vec.size())
        {
            m_cur_buf = new Buffer(cap);
            m_buf_vec.emplace_back(m_cur_buf);
        } // if 

        m_cur_buf = m_buf_vec[m_nxt_buf_idx++];
        m_cur_buf->reserve(blen);
        assert(m_cur_buf->used() == 0);
        
        used = trans2Bin(txt, bin);
        if (used < 0)
        {
            puts   ("VarLengthValueArray:: trans2Bin failed!\n");
            printf ("VarLengthValueArray::[%s]\n", txt);
            abort  ();
            return used; 
        } // if 
    } // if

    // already written  
    m_cur_buf->allocate(used, false); 

    m_offsets[m_val_num] = m_cur_off;
    m_cur_off += used;
    ++m_val_num; // write one more

    return 1; 
} // writeText



inline
int VarLengthValueArray::writeBinVal(uint64_t len, const void *bin) 
{
    if (m_val_num == m_val_cap) { return 0; }  // full 

    uint64_t  avail = m_cur_buf->available();
    if (len > avail)
    {
        uint64_t cap = (len > s_buffer_size) ? 
            Utility::calcAlignSize(len, m_align) : s_buffer_size;

        // no more available buffer 
        if (m_nxt_buf_idx == m_buf_vec.size())
        {
            m_cur_buf = new Buffer(cap);
            m_buf_vec.emplace_back(m_cur_buf);
        } // if 

        m_cur_buf = m_buf_vec[m_nxt_buf_idx++];
        m_cur_buf->reserve(len);
        assert(m_cur_buf->used() == 0);
    } // if 
    
    void *dst = m_cur_buf->allocate(len, false); 
    m_dt->copy(bin, dst);

    m_offsets[m_val_num] = m_cur_off;
    m_cur_off += len;
    ++m_val_num; // write one more

    return 1; 
} // writeBinVal



//inline
//int VarLengthValueArray::getWrittenBin(const void* &bin, uint32_t &len)
//{
//    uint64_t idx =  m_val_num - 1;
//    len = m_cur_off - m_offsets[idx]; 
//
//    uint64_t used = m_cur_buf->used();
//    bin = (const void*)m_cur_buf->getPosition(used - len);
//    return 1;
//} // getWrittenBin



inline
int VarLengthValueArray::trans2Bin(const char *txt, const void* &bin)
{
    bin = m_cur_buf->getNextPosition();
    uint64_t avail = m_cur_buf->available (); 
    return m_dt->transTxt2Bin(txt, const_cast<void*>(bin), avail);
} // try2trans 



inline
int64_t VarLengthValueArray::appendOffsets(Buffer *buf)
{
    uint64_t olen = getOffsetArrayUsed();
    void    *dest = buf->allocate(olen, false);
    memcpy(dest, m_offsets, olen);
    return int64_t(olen);
} // appendOffsets



inline
int64_t VarLengthValueArray::appendValues (Buffer *buf)
{
    int64_t total = 0; 
    for (auto & cb : m_buf_vec)
    {
        void    *vars = cb->data();
        uint64_t used = cb->used();
        void    *dest = buf->allocate(used, false);
        if (dest == nullptr) { total = -1; break; } 

        memcpy  (dest, vars, used); 
        total += used;
    } // for 
    
    return total;
} // appendVarLengthValue



inline
void VarLengthValueArray::output2debug(void)
{
    BinaryValueArray::output2debug();
    printf("Var value offsets@[%p]\n", m_offsets);
    printf("read value begin @[%p] offset:[%u]\n", m_rd_vbgn, m_cur_off);

    if (m_rd_vbgn != nullptr) 
    {
        for(uint32_t i = 0; i < m_val_num; ++i)    
        {
            uint32_t off = m_offsets[i];
            char   *next = (off == s_invalid_off) ? nullptr : (char*)(m_rd_vbgn + off);
            printf("<%s>\n", next != nullptr ? next : "nil");
        } // for i
    } // if 

    for (auto & buf : m_buf_vec)
    {   buf->output2debug();   }

    puts("\n==================================================\n");
} // output2debug



} // namespace steed
