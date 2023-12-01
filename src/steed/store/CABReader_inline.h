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
 * @file   CABReader_inline.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    CABReader inline functions
 */

#pragma once 

namespace steed {



inline
CABReader::~CABReader(void)
{
    m_rep_vec  = nullptr;

    m_cont_buf = nullptr;
    m_file_io  = nullptr;
    m_cur_info = nullptr;

    delete m_layouter; m_layouter = nullptr;
    delete m_info_buf; m_info_buf = nullptr;
    delete m_cur_cab ; m_cur_cab  = nullptr;
} // dtor





inline
int CABReader::loadCAB4Record(uint64_t ridx)
{
    // check target CAB is loaded 
    if (m_cur_info != nullptr)
    {
        uint64_t rbgn = m_cur_info->getBeginRecdID();
        uint64_t rend = m_cur_info->getRecordNum() + rbgn;
        bool loaded = ((ridx >= rbgn) && (ridx < rend));
        if  (loaded) { return 1; } 
    } // if 

    int s = calcCABIndex  (ridx);
    if (s > 0)
    {   s = prepareNextCAB();   }
    return s;
} // loadCAB4Record



inline
int  CABReader::read(uint64_t itm_idx, ColumnItem &ci)
{
    assert(m_cur_cab != nullptr);

    int  got =  m_cur_cab->read(itm_idx, ci);
    bool sgl = (m_rept->type() == RepetitionType::single);
    if ((sgl) && (got > 0))
    {
        ci.setRepVal ( m_rept->decode(ci.getRep    ()) );
        ci.setNextRep( m_rept->decode(ci.getNextRep()) );
    } // if 

#ifdef _DEBUG_COLUMN_READER
    if (got > 0)
    {
        printf("[rd %p][recd:%lu itm:%lu]  ", this, m_recd_num, itm_idx);
        uint32_t max_def = m_cab_meta.m_max_def;
        DataType *dt = getDataType(); 
        ci.output2debug(max_def, dt);
        puts("");
        
        m_recd_num += (ci.getRep() == 0);
    } // if 
#endif // _DEBUG_COLUMN_READER

    return got;
} // read 



inline
void CABReader::getRecdRange(uint64_t bgn, uint64_t &end)
{
    // no bit content for check:
    // m_rept is RepetitionType::none
    if (m_rep_vec == nullptr)
    {
        assert((m_rept->type() == RepetitionType::none) /* isNonRept */ ||
            isTrivialCAB() /* trivial */);

        end = bgn + 1;
        return;
    } // if  

    
    // check the bit content to get rep value 0
    assert(m_rep_vec->get(bgn) == 0);

    end = bgn + 1;

    uint32_t bnd = m_rept->encode(0); // 0 is record boundary
    uint64_t cap = getItemNumber  ();
    while  ((end < cap) && (m_rep_vec->get(end) != bnd))
    {   ++end;   } 
} // getRecdRange



inline
int64_t CABReader::skipRecds(uint64_t num, uint64_t &idx)
{
    // not init CAB yet
    if (m_cur_info == nullptr)
    {   return int64_t(-1);   }

    uint64_t cap = getItemNumber();
    if (idx >= cap)
    {   return int64_t(-1);   } 


    uint64_t end = 0;
    while  ((num > 0) && (end < cap))
    {
        getRecdRange(idx, end);

        idx = end, --num; 
    } // while 

    return (end < cap) ? num : int64_t(-1);
} // skipRecds



inline
uint64_t CABReader::getSpecificItemIdx(uint64_t bgn, vector<uint32_t> &vidx)
{
    uint32_t dep = vidx.size(); 
    uint32_t lvl = 0; 
    while   (lvl < dep)
    {
        uint32_t cnt = 0;         // child elem counter  
        uint32_t tgt = vidx[lvl]; // target value by index 
        uint32_t exp = lvl + 1;   // expect rept  by level
        while   (cnt < tgt)
        {
            // at least skip 1 item  
            uint64_t bit =  m_rep_vec->get(++bgn);
            uint64_t rep =  m_rept->decode(bit);
            if      (rep <  exp)  // no more enough item 
            {   return uint64_t(-1);   } 
            else if (rep == exp)  // skip items at lower level
            {   ++cnt;   }
        } // while 

        ++lvl;
    } // for lvl 
    
    return bgn; 
} // getSpecificItemIdx



inline
uint64_t CABReader::
    getRecdBeginItemIdx(uint64_t cur_ridx, uint64_t cur_iidx, uint64_t tgt_ridx)
{
    uint64_t dis = tgt_ridx - cur_ridx;
    int64_t  num = skipRecds (dis, cur_iidx);
    return  (num >= 0) ? cur_iidx : uint64_t(-1);
} // getRecdBeginItemIdx



inline
int CABReader::calcCABIndex(uint64_t ridx)
{
    int cmp = 0, got = 0;
    do {
        got = compareCABIndex4Record(ridx, cmp); 
        m_cab_idx = int64_t(m_cab_idx) + cmp; 
    } while ((got == 1) && (cmp != 0));
    return got;
} // calcCABIndex



inline
int CABReader::compareCABIndex4Record(uint64_t ridx, int &cmp)
{
    CABInfo *info = m_info_buf->getCABInfo(m_cab_idx);
    if (info == nullptr)  { return 0; }

    uint64_t rbgn = info->getBeginRecdID();
    uint64_t rnum = info->getRecordNum  ();

    //  [0,rbgn) -> -1;  [rbgn, rbgn+rnum) -> 0;  [rbgn+rnum, +oo) ->1
    cmp = (ridx < rbgn) ? -1 : (ridx < rbgn + rnum) ? 0 : 1; 

    return 1;
} // compareCABIndex4Record



inline
int CABReader::prepareCABInfo(void) 
{
    m_cur_info = m_info_buf->getCABInfo(m_cab_idx++);
    return m_cur_info == nullptr ? 0 : 1;
} // prepareCABInfo



} // namespace










// Discard  
//    /**
//     * load ColumnAlignBlock by record id  
//     * @param ridx    record index 
//     * @return 0 success; <0 failed
//     */
//    int loadCAB4RepDef(uint64_t ridx);
//inline
//int CABReader::loadCAB4RepDef(uint64_t ridx)
//{
//    // only used by CABAligner 
//    assert(ridx % Config::s_cab_recd_num == 0);
//
//    int s = calcCABIndex (ridx);
//    if (s > 0)
//    {   s = prepareNextCAB();   }
//    return s;
//} // loadCAB4RepDef



  
#ifdef DEFINE_BLM
inline
int CABReader::
    isCandidate(bool &is_cand, uint64_t cab_rbgn, const void *chk_bin, int64_t chk_len)
{
    // invalid state can not be a candidate
    uint64_t valid_recd_bgn = getValidRecdIdx(); 
    if (cab_rbgn < valid_recd_bgn)
    {   is_cand = false; return 1;   } 


    int retval = -1;
    if (m_use_bloom)
    {   retval = candInBloom    (is_cand, cab_rbgn, chk_bin, chk_len);   }
    else
    {   retval = candByValueInfo(is_cand, cab_rbgn, chk_bin, chk_len);   }
    return retval;
} // isCandidate



inline
int CABReader::
    candInBloom(bool &is_cand, uint64_t cab_rbgn, const void *chk_bin, int64_t chk_len)
{
    if (m_bloom == nullptr)
    {   m_bloom = new BloomFilter(Config::s_cab_recd_num);   } 

    bool loaded = (m_cur_info != nullptr) && (m_cur_info->getBeginRecdID() == cab_rbgn);
    if (!loaded)
    {
        int got = loadBloomFilter(cab_rbgn);
        if (got <= 0)
        {
            // failed or EOF
            is_cand = true;
            return got;
        } // if 
    } // if 
    
    is_cand = m_bloom->testBytes((const char *)chk_bin, chk_len);
    return 1;
} // candInBloom



inline
int CABReader::loadBloomFilter(uint64_t cab_rbgn)
{
    int got = calcCABIndex(cab_rbgn);
    if (got <= 0) { return got; }

    got = prepareCABInfo();
    if (got <= 0) { return got; } // failed or EOF

    // assert bloom filter has the same size in memory and disk
    assert( m_cur_info->m_blm_mem_len == m_bloom->sizeInBytes() );

    char *bin = (char*)m_bloom->getData();
    return m_info_buf->loadBloomContent(m_cur_info, bin);
} // loadBloomFilter





inline
int CABReader::
    candByValueInfo(bool &is_cand, uint64_t cab_rbgn, const void *chk_bin, int64_t chk_len)
{
    bool loaded = (m_cur_info != nullptr) && (m_cur_info->getBeginRecdID() == cab_rbgn);
    if (!loaded)
    {
        int got = calcCABIndex(cab_rbgn);
        if (got <= 0) { return got; }
    
        got = prepareCABInfo();
        if (got <= 0) { return got; } // failed or EOF
    } // if 


    DataType *dt = this->getDataType ();
    if (dt->getDefSize() == 0)
    {
        // var-length value  
        is_cand = true; return 1;
    } // if 


    // use cab value info to check is candidate 
    void *min = m_cur_info->getMinBin();
    void *max = m_cur_info->getMaxBin();
    is_cand = (dt->compareNotLess   (chk_bin, min) > 0)
           && (dt->compareNotGreater(chk_bin, max) > 0);

    return 1;
} // candByValueInfo 
#endif