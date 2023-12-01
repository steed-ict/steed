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
 * @file CABOperator.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    definitions and functions for CABOperator
 */

#pragma once 

#include <stdint.h>

#include "Config.h"
#include "Buffer.h"
#include "FileIO.h"
#include "SchemaTree.h"

#include "RepetitionType.h"
#include "BinaryValueArray.h"
#include "CompressorFactory.h"

#include "CAB.h"
#include "CABLayouter.h"


namespace steed {

extern Config g_config;

class CABOperator {
protected:
    // my initial members: binary content in memory 
    RepetitionType   *m_rept{nullptr}; /**< rep type ins  */
    CABMeta           m_cab_meta   {}; /**< CAB meta info */

    // create and used by child class: read or write
    Buffer           *m_cont_buf{nullptr}; /**< cab bin cont buf */
    FileIO           *m_file_io {nullptr}; /**< bin IO with file */
    CABLayouter      *m_layouter{nullptr}; /**< content layouter */
    CABInfoBuffer    *m_info_buf{nullptr}; /**< CAB info heads   */

    // current CAB in use
    CABInfo          *m_cur_info{nullptr}; /**< current CAB info */
    CAB              *m_cur_cab {nullptr}; /**< current CAB ins  */
    uint64_t          m_recd_num{0};       /**< column record num*/

    /** bin value content compress type */
    Compressor::Type  m_cmp_type{Compressor::none};
 
public:
    CABOperator(void) = default; 
    virtual ~CABOperator (void);

protected:
    /**
     * init class members
     * @param tree    SchemaTree instance
     * @param path    path in SchemaTree
     * @param cap     cab record capacity 
     * @return 0 success; <0 failed
     */
    int init(SchemaTree* tree, SchemaPath &path, uint64_t cap);

public:
    CAB*      getCurCAB  (void) { return m_cur_cab ; }
    uint64_t  getRecdNum (void) { return m_recd_num; }
    DataType* getDataType(void) { return m_cab_meta.m_dt; }

    RepetitionType* getReptType(void) { return m_rept; }

    uint64_t getValidRecdIdx(void)
    { return m_info_buf->getValidRecdIdx(); }
}; // CABOperator





inline
CABOperator::~CABOperator(void)
{
    delete m_rept;  
    m_rept = nullptr;

    delete m_cab_meta.m_bva;
    delete m_cab_meta.m_buf;
    m_cab_meta.m_bva = nullptr;
    m_cab_meta.m_buf = nullptr;
    m_cab_meta.m_dt  = nullptr;

    m_cont_buf = nullptr;
    m_file_io  = nullptr;
    m_layouter = nullptr;

    m_info_buf = nullptr;
    m_cur_info = nullptr;
    m_cur_cab  = nullptr;
    m_recd_num = 0;
    m_cmp_type = Compressor::none;
} // dtor 



inline
int CABOperator::init(SchemaTree* tree, SchemaPath &path, uint64_t cap)
{
    uint32_t max_rep = tree->getMaxRepeatLevel(path);
    uint32_t rep_num = tree->getRepeatedNumber(path);
    m_rept = RepetitionType::create(rep_num);
    if (rep_num == RepetitionType::single)
    {
        // set the rept value for single rept type
        static_cast<RepeatSingle*>(m_rept)->setReptLevel(max_rep);
    } // if 

    SchemaSignature ls = path.back();
    m_cab_meta.m_dt  = tree->getDataType(ls);
    m_cab_meta.m_buf = new Buffer(0); 
    m_cab_meta.m_buf-> initInMemory (); // always success
    m_cab_meta.m_bva = // no need to setBeginOffset
        BinaryValueArray::create (m_cab_meta.m_buf, m_cab_meta.m_dt); 
    m_cab_meta.m_recd_cap = cap;
    m_cab_meta.m_max_rep  = m_rept->getReptBits(max_rep);
    m_cab_meta.m_max_def  = path.size();
   
    //TODO: set m_cmp_type 
   
    return 0;
} // init

} // namespace





#if DEFINED_BLM
    BloomFilter      *m_bloom   {nullptr}; /**< bloom filter in CABInfoBuffer */
    bool              m_use_bloom {false}; /**< column use bloom filter flag  */
#endif

#if DEFINED_BLM
{
    // in dtor func 
    delete m_bloom; m_bloom = nullptr;
}
#endif

 #if DEFINED_BLM
 {
    // In init func

    // set bloom flag  
    m_use_bloom = false;
    uint64_t depth = path.size();
    bool    enough = (depth > 1);
    if (enough && Config::s_kav_key_blm)
    {
        // SchemaTemplate has 2 level  
        SchemaSignature ss  = path[depth - 2]; // template root candidate
        SchemaSignature css = path[depth - 1]; // template key  candidate
        bool parentIsTemp   = tree->isTemplate(ss); 
        bool leafIsTempKey  = (css == tree->getTempKeySign(ss));
        m_use_bloom = parentIsTemp && leafIsTempKey;    
    } // if 

 }
#endif