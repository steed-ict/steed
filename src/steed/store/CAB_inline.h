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
 * @file CAB_inline.h
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0 
 * @section DESCRIPTION
 *   CAB inline functions
 */

#pragma once 

namespace steed {

class CAB::CABItemUnit {
public:
    ColumnItemArray  *m_cia{nullptr}; /**< object to read and write  */ 
    Buffer           *m_buf{nullptr}; /**< binary content in buffer  */

    /** whether this unit own buffer and BinaryValueArray in ColumnItemArray */
    bool    m_own{false}; 

public:
    CABItemUnit (ColumnItemArray *cia, Buffer* buf, bool own);
    ~CABItemUnit(void);

public:
    void output2debug(void);
}; // CABItemUnit



inline CAB::CABItemUnit::
CABItemUnit(ColumnItemArray *cia, Buffer* buf, bool own) :
    m_cia(cia), m_buf(buf), m_own(own)
{}


inline CAB::CABItemUnit::
~CABItemUnit(void)
{
    BinaryValueArray *bva = m_cia->getValueArray();
    if (m_cia != nullptr)
    {
        // ColumnItemArray does not delete m_buf and m_values
        delete m_cia; m_cia = nullptr;
    } // if 
    
    if (m_own)
    { 
        // minor unit owns buffer and BinaryValueArray
        if (m_buf != nullptr) { delete m_buf; }
        if (bva   != nullptr) { delete bva;   }
    } // if 
    
    m_buf = nullptr, 
    bva   = nullptr; 
} // dtor 


inline
void CAB::CABItemUnit::output2debug(void)
{
    printf("CIUnit::m_own@[%d]\n", m_own); 
    printf("CIUnit::m_cia@[%p]\n", m_cia); m_cia->output2debug();
    printf("CIUnit::m_buf@[%p]\n", m_buf); m_buf->output2debug();
} // output2debug





inline CAB::
CAB(CABMeta *mt, CABInfo *info, Buffer *buf):
    m_meta(mt), m_info(info),
    m_align_size(g_config.m_mem_align_size), 
    m_buf_size  (g_config.m_max_bin_val_len),
    m_recd_cap  (g_config.m_cab_recd_num)
{
    /** item num got from CAB info */
    uint64_t info_num = m_info->m_item_info.m_item_num; 
    uint64_t meta_cap = m_meta->m_recd_cap; /** expecting item num (== rnum) */
    uint32_t cap = info_num > meta_cap ? info_num : meta_cap;
    BinaryValueArray *bva = m_meta->m_bva;
    ColumnItemArray  *cia = createCIA(buf, bva, cap);

    // buf and bva is owned by CABMeta 
    bool own = false;
    m_major_unit = new CABItemUnit(cia, buf, own);
    m_cur_unit = m_major_unit;
} // ctor 



inline
CAB::~CAB(void)
{
    m_meta = nullptr, m_info = nullptr;

    if (m_major_unit != nullptr)
    {
        delete m_major_unit;
        m_major_unit = nullptr;
        m_cur_unit   = nullptr;
    }

    for (auto &ciu : m_minor_units)
    {   delete ciu; ciu = nullptr;   } 
    m_minor_units.clear();
} // dtor



inline
uint64_t CAB::getMergedUsed(bool tail)
{
    uint64_t total = 0;
    CABItemInfo::Type type = tail ? 
        CABItemInfo::Type(CABItemInfo::crucial) : m_item_info.getType();

       
    switch (type)
    {
        // crucial CAB output full content:
        // rep + def + [offset array] + bin value array  
        case CABItemInfo::crucial:
            total += getValueUsed(tail);
            [[fallthrough]];

        // allnull CAB output rep + def
        case CABItemInfo::allnull:
            total += getRepDefUsed(tail); 
            [[fallthrough]];

        // trivial CAB output nothing
        case CABItemInfo::trivial:
            break;

        default: break;
    } // switch

    return total; 
} // getMergedUsed



inline
uint64_t CAB::getRepDefUsed(bool tail)
{
    // trivial CAB does not have rep + def array
    bool nobits = (m_item_info.getType() >= CABItemInfo::trivial);
    if ((nobits) && (!tail)) { return 0; }

    // calc rep + def size
    ColumnItemArray *cia = m_major_unit->m_cia;
    uint64_t total = cia-> getRepDefUsed();
    for (auto &u : m_minor_units)
    {
        cia = u->m_cia;
        total += cia->getRepDefUsed();
    } // for  
    return total; 
} // getRepDefUsed



inline
uint64_t CAB::getValueUsed(bool tail)
{
    // trivial and allnull CAB does not have bin value array 
    bool novalue = (m_item_info.getType() >= CABItemInfo::allnull);
    if ((novalue) && (!tail)) { return 0; }

    // calc value size  
    ColumnItemArray *cia = m_major_unit->m_cia;
    uint64_t total = cia-> getValueUsed();
    for (auto &u : m_minor_units)
    {
        cia = u->m_cia;
        total += cia->getValueUsed();
    } // for  
    return total; 
} // getValueUsed





inline
int CAB::init2write (uint64_t bgn_rid)
{ 
    m_item_info.m_bgn_recd = bgn_rid; 
    
    uint32_t rep = m_meta->m_max_rep;
    uint32_t def = m_meta->m_max_def;
    ColumnItemArray *cia = m_cur_unit->m_cia;
    return cia->init2write(rep, def);
} // init2write



inline
int CAB::writeNull(uint32_t rep, uint32_t def)
{
    if (checkFull(rep)) { return 0; }

    ColumnItemArray *cia = m_cur_unit->m_cia;
    int retval = cia->writeNull(rep, def);
    if (retval == 0) // current is full 
    { 
        m_cur_unit = createMinorUnit();
        m_minor_units.emplace_back(m_cur_unit);
        cia = m_cur_unit->m_cia;
        retval = cia->writeNull(rep, def);
    } // if  

    if (retval < 0)
    {
        printf("CAB: writeNull failed!\n");
        return retval; 
    } // if 

    m_item_info.update(rep, def, m_meta->m_max_def);

    return retval;
} // writeNull



inline
int CAB::writeText(uint32_t rep, uint32_t def, const char *txt, const void* &bin)
{
    if (checkFull(rep)) { return 0; }

    ColumnItemArray *cia = m_cur_unit->m_cia;
    int retval = cia->writeText(rep, def, txt, bin);
    if (retval == 0) // current is full 
    { 
        m_cur_unit = createMinorUnit();
        m_minor_units.emplace_back(m_cur_unit);

        cia = m_cur_unit->m_cia;
        retval = cia->writeText(rep, def, txt, bin);
    } // if  

    if (retval < 0)
    {
        printf("CAB: writeText failed!\n");
        return retval; 
    } // if 

    m_item_info.update(rep, def, m_meta->m_max_def);

    return retval; 
} // writeText



inline
int CAB::writeBinVal (uint32_t rep, uint32_t def, const void *bin, uint32_t len)
{
    if (checkFull(rep)) { return 0; }

    ColumnItemArray *cia = m_cur_unit->m_cia;
    int retval = cia->writeBinVal (rep, def, bin, len);
    if (retval == 0) // current is full 
    { 
        m_cur_unit = createMinorUnit();
        m_minor_units.emplace_back  (m_cur_unit);

        cia    = m_cur_unit->m_cia;
        retval = cia->writeBinVal  (rep, def, bin, len);
    } // if  

    if (retval < 0)
    {
        printf("CAB: writeBinVal failed!\n");
        return retval;
    } // if 

    m_item_info.update(rep, def, m_meta->m_max_def);

    return retval; 
} // writeBinVal

//inline
//int CAB::getWrittenBin(const void* &bin, uint32_t &len)
//{
//    return m_cur_unit->m_cia->getWrittenBin(bin, len);
//} // getWrittenBin



inline
CAB::CABItemUnit *CAB::createMinorUnit(void)
{
    DataType *dt = m_meta->m_dt; 
    Buffer  *buf = new Buffer(); // use Buffer default init len  
    buf->initInMemory();

// calc minor CAB item capacity 
#if 0
    // fixed item num in minor CABs
    uint64_t  cap = Config::s_cab_minor_unit_item_num; // Config::s_cab_recd_num / 8
#else    
    // IMPLEMENTATION 2: calc  item num in minor CABs
    uint32_t item_done = m_item_info.m_item_num; 
    uint32_t recd_done = m_item_info.m_recd_num;
    double   itm_pre_recd  = double(item_done) / recd_done;
    uint64_t cab_recd_cap  = m_meta->m_recd_cap;
    uint64_t rest_recd_num = cab_recd_cap - recd_done + 1; // rest record number 
    uint64_t exp_item_num  = g_config.m_reserve_factor * itm_pre_recd * rest_recd_num; 
    uint64_t minor_itm_cap = Utility::calcAlignSize(exp_item_num, 8);
    uint64_t cap = minor_itm_cap;
#endif

    BinaryValueArray *pre = m_cur_unit->m_cia->getValueArray();
    m_bva_bgn_off +=  pre-> getWriteValueArrayUsed();
    BinaryValueArray *bva = BinaryValueArray::create(buf, dt);
    bva->setBeginOffset(m_bva_bgn_off);

// TODO:
// add CAB binary Directory Encoding here
// may be defined in (m_major_unit->m_cia->m_values)

    ColumnItemArray *cia = createCIA(buf, bva, cap);
    uint32_t rep = m_meta->m_max_rep;
    uint32_t def = m_meta->m_max_def;
    cia->init2write(rep, def);

    bool   own = true;
    return new CABItemUnit(cia, buf, own);
} // createMinorUnit










inline
int CAB::init2read(CABItemInfo::Type type)
{
    m_item_info = m_info->m_item_info;
    uint32_t rep  = m_meta->m_max_rep;
    uint32_t def  = m_meta->m_max_def;
    ColumnItemArray *cia = m_cur_unit->m_cia;
    return cia->init2read(type, rep, def, m_item_info.m_item_num);
} // init2read



inline
int CAB::read(uint64_t idx, ColumnItem &ci)
{   return m_major_unit->m_cia->read(idx, ci);   }



inline
BitVector* CAB::getRepBitsVec(void)
{   return m_major_unit->m_cia->getRepBitsVec();   }



inline
BinaryValueArray* CAB::getBinValueArray(void)
{   return m_major_unit->m_cia->getValueArray();   }



inline
int64_t CAB::copyContent(CAB *cab)
{
    m_item_info = cab->m_item_info; 

    // got content 
    CABItemUnit     *cp_ciu = cab->m_cur_unit;
    ColumnItemArray *cp_cia = cp_ciu->m_cia;
    ColumnItemArray *my_cia = m_cur_unit->m_cia;
    return my_cia->copyContent(cp_cia);
} // copyContent 


} // namespace steed
