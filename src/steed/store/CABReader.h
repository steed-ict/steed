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
 * @file   CABReader.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    definitions and functions for CABReader 
 */

#pragma once 

#include "Config.h"
#include "CABOperator.h"



namespace steed {

class CABReader : virtual public CABOperator {
protected:
    BitVector         *m_rep_vec{nullptr}; /**< rep bit value vector */
    uint32_t           m_cab_idx{0};       /**< CAB (info) read idx  */


public:
    CABReader (void) = default;
    ~CABReader(void);

public: 
    /**
     * init class members to read 
     * @param fbase   file base name string (got from setting and SchemaPath)
     * @param tree    related SchemaTree ins
     * @param path    related SchemaPath ins 
     * @param cap     record cap in each CAB  
     * @return 0 success; <0 failed
     */
    int init2read(const string &fbase, SchemaTree* tree, SchemaPath &path, uint64_t cap);

public: 
    CABItemInfo::Type getType(void) { return m_cur_info->getType(); }
    bool       isTrivialCAB  (void) { return getType() == CABItemInfo::trivial; }
    bool       isAllNullCAB  (void) { return getType() == CABItemInfo::allnull; }

    uint64_t   getCABBeginRid(void) { return m_cur_info->getBeginRecdID(); }
    uint64_t   getItemNumber (void) { return m_cur_info->getItemNumber (); }
    
public:
    /**
     * load ColumnAlignBlock by record id  
     * @param ridx    record index 
     * @return 0 success; <0 failed
     */
    int loadCAB4Record(uint64_t ridx);

public:
    /**
     * read one ColumnItem by item index  
     * @param itm_idx    item index in CAB
     * @param ci         column item as result  
     * @return >0 success; 0 is EOF; <0 failed  
     */
    int  read(uint64_t itm_idx, ColumnItem &ci);

    /**
     * get record range 
     * @param bgn    record's item begin index  
     * @param end    record's item end   index 
     */
    void getRecdRange(uint64_t bgn, uint64_t &end);
    
    /**
     * skip some records
     * @param num    skip record number need to be skipped  
     * @param idx    record's begin item index to skip 
     * @return >0 no more records to skip; 0 success; <0 do not have enough items
     */
    int64_t  skipRecds(uint64_t num, uint64_t &idx);

    /**
     * get specific item index using index on each level in current record 
     * @param bgn     record begin item index 
     * @param vidx    selected value index on each level  
     * @return >0 item index; uint64_t(-1) as failed 
     */
    uint64_t getSpecificItemIdx (uint64_t bgn, vector<uint32_t> &vidx);

    /**
     * get record begin item index in current CAB
     * @param cur_ridx    begin  record index
     * @param cur_iidx    begin  item   index
     * @param tgt_ridx    target record index 
     * @return the first item index of target record; uint64_t(-1) as error
     */
    uint64_t getRecdBeginItemIdx(uint64_t cur_ridx, uint64_t cur_iidx, uint64_t tgt_ridx);

    /**
     * get BinaryValueArray ins in current CAB 
     * assert(m_cur_cab != nullptr);
     * @return BinaryValueArray ins 
     */
    BinaryValueArray* getBinValueArray(void) 
    {   return m_cur_cab->getBinValueArray();   } 

    /**
     * get repetition value array in current CAB
     * @return rep array in bit vector 
     */
    BitVector* getRepValueArray(void) { return m_rep_vec; }

    
protected:
    /**
     * calc CAB Index 
     * @param ridx    record index  
     * @return <0 failed; =0 EOF; >0 success 
     */ 
    int calcCABIndex(uint64_t ridx);

    /**
     * compare CAB index using record id  
     * @param ridx    record index  
     * @param cmp     compare CAB index result: <0 front; ==0 current; >0 back 
     * @return 1 got compare result; 0 EOF; <0 failed 
     */
    int compareCABIndex4Record(uint64_t ridx, int &cmp); 

    /**
     * get next info to read  
     * @return <0 failed; =0 EOF; >0 success 
     */ 
    int prepareCABInfo(void);

    /**
     * prepare next CAB ins  
     * @return <0 failed; =0 EOF; >0 success 
     */ 
    int prepareNextCAB(void); 

    /**
     * get bin content for CAB
     * @return 0 success; <0 failed 
     */ 
    int prepareBinCont(void);

//    /**
//     * load CAB content from file
//     * @return 1 success; <0 failed
//     */
//    int loadCABContent(void);

public:
    void output2debug (void);
}; // CABReader


} // namespace



#include "CABReader_inline.h"








#if DEFINE_BLM
public: 
    /**
     * check value is candidate in CAB by blooming or value info
     * @param is_cand     is candidate flag
     * @param cab_rbgn    CAB record begin index 
     * @param chk_bin     binary content to check 
     * @param chk_len     binary length  to check 
     * @return >0 got result; 0 EOF; <0 failed  
     */ 
    int isCandidate(bool &is_cand, uint64_t cab_rbgn, const void *chk_bin, int64_t chk_len); 

protected:
    /**
     * check value is candidate in CAB blooming filter 
     * @param is_cand     is candidate flag
     * @param cab_rbgn    CAB record begin index 
     * @param chk_bin     binary content to check 
     * @param chk_len     binary length  to check 
     * @return >0 got result; 0 EOF; <0 failed  
     */ 
    int candInBloom(bool &is_cand, uint64_t cab_rbgn, const void *chk_bin, int64_t chk_len);

    /**
     * load BloomFilter content by record id  
     * @param cab_rbgn    CAB record begin index 
     * @return >0 success; 0 EOF; <0 failed  
     */
    int loadBloomFilter(uint64_t cab_rbgn);

    /**
     * check value is candidate in CAB blooming filter 
     * @param cand        is candidate flag
     * @param cab_rbgn    CAB record begin index 
     * @param chk_bin     binary content to check 
     * @param chk_len     binary length  to check 
     * @return >0 got result; 0 EOF; <0 failed  
     */ 
    int candByValueInfo(bool &cand, uint64_t cab_rbgn, const void *chk_bin, int64_t chk_len);
#endif


//    /** cab is all null: crucial cab only read rep + def is also allnull */
//    bool isAllNullCAB (void)
//    { return (m_cur_info->getItemNumber() == m_cur_info->getNullNumber()); }
