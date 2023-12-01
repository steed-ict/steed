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
 * @file ColumnItemArray_inline.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   ColumnItemArray inline functions definitions.
 */

#pragma once

namespace steed {


inline
ColumnItemArray::~ColumnItemArray(void)
{
    m_buffer = nullptr, m_dt = nullptr, m_values = nullptr; 
    delete m_reps; m_reps = nullptr;
    delete m_defs; m_defs = nullptr;
    m_item_cap = 0, m_item_num = 0;
} // dtor



inline ColumnItemArray::
ColumnItemArray (uint32_t cap, DataType *dt, Buffer *buf, BinaryValueArray *bva):
    m_buffer(buf), m_dt(dt), m_values(bva), m_item_cap(cap)
{}



inline
uint64_t ColumnItemArray::getRepDefUsed(void)
{
    uint64_t rep_use = m_reps->getUsedSize();
    uint64_t def_use = m_defs->getUsedSize();
    return  (rep_use + def_use);
} // getRepDefUsed



inline
uint64_t ColumnItemArray::getValueUsed(void)
{
    uint64_t off_use = m_values->getOffsetArrayUsed    ();
    uint64_t val_use = m_values->getWriteValueArrayUsed();
    return  (off_use + val_use);
} // getValueUsed



inline
uint64_t ColumnItemArray::getContentUsed(void)
{
    // rep and def array used  
    uint64_t rda_use = getRepDefUsed();
    uint64_t val_use = getValueUsed ();
    uint64_t total = (rda_use + val_use);
    return   total;
} // getContentUsed





inline
int ColumnItemArray::init2write(uint32_t max_rep, uint32_t max_def)
{
    uint32_t rep_bnum = Utility::calcUsedBitNum(max_rep); 
    uint32_t def_bnum = Utility::calcUsedBitNum(max_def);
    uint32_t rep_size = Utility::calcBytesUsed (rep_bnum * m_item_cap);
    uint32_t def_size = Utility::calcBytesUsed (def_bnum * m_item_cap);
    uint32_t fix_size = m_values->getFixSize(m_item_cap); // fix bin OR VAR offset
    uint32_t buf_size = rep_size + def_size + fix_size; 

    // init ptrs
    char *bin = (char*)m_buffer->allocate(buf_size, true);
    m_reps = new BitVector(rep_bnum);
    m_reps-> init2write   (rep_size, bin);
    bin += rep_size;

    m_defs = new BitVector(def_bnum);
    m_defs-> init2write   (def_size, bin);
    bin += def_size;

    m_values->init2write  (fix_size, bin);

    return 0;
} // init2write





inline
int ColumnItemArray::init2read(CABItemInfo::Type type,
        uint32_t max_rep, uint32_t max_def, uint64_t item_num)
{
    m_type = type;
    m_item_num = item_num;

    // trivial CAB read nothing
    if (m_type == CABItemInfo::trivial)  { return 0; }

    uint32_t rep_bnum = Utility::calcUsedBitNum(max_rep); 
    uint32_t def_bnum = Utility::calcUsedBitNum(max_def);
    uint32_t rep_size = Utility::calcBytesUsed (rep_bnum * m_item_num);
    uint32_t def_size = Utility::calcBytesUsed (def_bnum * m_item_num);

    // init ptrs 
    uint64_t offset = 0;
    void    *cbin   = nullptr;

    cbin   = m_buffer->getPosition(offset);
    m_reps = new BitVector(rep_bnum);
    m_reps->init2read(m_item_num, rep_size, cbin);
    offset += rep_size;

    cbin   = m_buffer->getPosition(offset);
    m_defs = new BitVector(def_bnum);
    m_defs->init2read(m_item_num, def_size, cbin);
    offset += def_size;


    // all null CAB read rep and def array  
    if (m_type == CABItemInfo::allnull)  { return 0; }

    assert(m_type == CABItemInfo::crucial);
    uint64_t total = m_buffer->used();
    uint64_t val_used = total - offset; 
    cbin = m_buffer->getPosition(offset);
    m_values->init2read(val_used, cbin, m_item_num);

    return 0;
} // init2read





inline
int64_t ColumnItemArray::copyContent (ColumnItemArray *cia)
{
    if (this == cia) { return 0; }

    // clear previous Buffer usage  
    m_buffer->clear();

    CABItemInfo::Type  type = cia->getType();
    bool trivial = (type == CABItemInfo::trivial);
    bool crucial = (type == CABItemInfo::crucial);

    // update item info
    uint64_t itm_num = Utility::calcAlignSize(cia->m_item_num, 8);
    m_item_cap = (itm_num > m_item_cap) ? itm_num : m_item_cap;  
    m_item_num = cia->m_item_num;

    // calc my buffer size    
    uint32_t rep_bnum = m_reps->getMaskSize(); 
    uint32_t def_bnum = m_defs->getMaskSize(); 
    uint32_t rep_size = Utility::calcBytesUsed(rep_bnum * m_item_cap);  
    uint32_t def_size = Utility::calcBytesUsed(def_bnum * m_item_cap);  
    uint32_t fix_size = m_values->getFixSize(m_item_cap);
    uint32_t total_size = rep_size + def_size + fix_size; 
    m_buffer->reserve(total_size);

    // update content 
    void   *bcont = nullptr;
    int64_t total = 0, used = 0;

    bcont = m_buffer->allocate(rep_size, false); 
    m_reps->init2write(rep_size, bcont);
    if (!trivial)
    {   used = m_reps->copyContent(cia->m_reps);   }
    else
    {
        m_reps->resizeElemUsed(cia->m_item_num);
        used = rep_size;
    } // if 
    if (used < 0)
    {
        printf("ColumnItemArray: copy rep failed!\n");
        return -1;
   }
    total += used;
    
    bcont = m_buffer->allocate(def_size, false); 
    m_defs->init2write(def_size, bcont);
    if (!trivial)
    {   used = m_defs->copyContent(cia->m_defs);   }
    else
    {
        m_defs->resizeElemUsed(cia->m_item_num);
        used = def_size;
    } // if 
    if (used < 0)
    {
        printf("ColumnItemArray: copy def failed!\n");
        return -1;
    }
    total += used;

    bcont = m_buffer->allocate(fix_size, false); 
    m_values->init2write(fix_size, bcont);
    used = crucial ?
        m_values->copyContent   (cia->m_values) : 
        m_values->resizeElemUsed(cia->m_item_num);
    if (used < 0)
    {
        printf("ColumnItemArray: copy values failed!\n");
        return -1;
    }
    total += used;
    
    return total;
} // copyContent





inline
int ColumnItemArray::read(uint64_t idx, ColumnItem &ci)
{
    // overflow
    if (idx >= m_item_num) { return 0; } 

    uint64_t nidx = idx + 1;
    uint32_t rep  = 0, def = 0, nrep = 0; 
    const void *bin = nullptr; 
    switch  (m_type)
    {
        case CABItemInfo::crucial: 
                bin = m_values->read(idx);
                [[fallthrough]];
        
        case CABItemInfo::allnull: 
                rep = m_reps-> get(idx);
                def = m_defs-> get(idx);
                nrep = (nidx < m_item_num) ? m_reps->get(nidx) : 0; 
                [[fallthrough]];
        
        case CABItemInfo::trivial: 
        default:
                ci.set(rep, def, bin, nrep);
                break; 
    } // type
//printf("ColumnItemArray: read idx:<%lu> rep:<%u> def:<%u> nrep:<%u>\n", idx, rep, def, nrep); 

    return 1;
} // read





inline
int ColumnItemArray::writeNull(uint32_t rep, uint32_t def)
{
    if (m_item_num == m_item_cap) { return 0; }

    if (writeRepDef(rep, def) < 0)
    {
        printf("ColumnItemArray: write rep and def failed!\n");
        return -1;
    }

    if (m_values->writeNull() < 0)
    {
        printf("ColumnItemArray: write null failed!\n");
        return -1;
    }

    ++m_item_num; 
    
    return 1;
} // writeNull



inline
int ColumnItemArray::
    writeText(uint32_t rep, uint32_t def, const char *txt, const void* &bin)
{
    if (m_item_num == m_item_cap) { return 0; }

    if (writeRepDef(rep, def) < 0)
    {
        printf("ColumnItemArray: write rep and def failed!\n");
        return -1;
    }
    
    if (m_values->writeText(txt, bin) < 0)
    {
        printf("ColumnItemArray: write next text failed!\n");
        return -1;
    }

    ++m_item_num; 
    
    return 1;
} // writeText



inline int ColumnItemArray::
    writeBinVal(uint32_t rep, uint32_t def, const void *bin, uint32_t len)
{
    if (m_item_num == m_item_cap) { return 0; }

    if (writeRepDef(rep, def) < 0)
    {
        printf("ColumnItemArray: write rep and def failed!\n");
        return -1;
    }
    
    if (m_values->writeBinVal(len, bin) < 0)
    {
        printf("ColumnItemArray: write next bin failed!\n");
        return -1;
    }

    ++m_item_num; 
    
    return 1;
} // writeBinVal



inline
int ColumnItemArray::writeRepDef(uint32_t rep, uint32_t def)
{
    if (m_reps->append(rep) < 0)
    {
        printf("ColumnItemArray: write next rep failed!\n");
        return -1;
    }
    
    if (m_defs->append(def) < 0) 
    {
        printf("ColumnItemArray: write next def failed!\n");
        return -1;
    }
    
    return 0;
} // writeRepDef







inline
int64_t ColumnItemArray::appendBitsContent(BitVector *bv, Buffer *buf)
{
    const void *bbgn = bv ->getContent ();
    int64_t     buse = bv ->getUsedSize();
    void       *dest = buf->allocate(buse, false); 
    memcpy(dest, bbgn, buse);
    return buse;
} // appendBitsContent 





inline
void ColumnItemArray::output2debug(void)
{
    puts("ColumnItemArray::output2debug");
    printf("data type:[%d]\n", m_dt->getTypeID());
    printf("item cap:<%lu> num:<%lu>\n", m_item_cap, m_item_num);
    
    printf("m_reps:  [%p]\n", m_reps);
    if (m_reps)   { m_reps  ->output2debug(); }

    printf("m_defs:  [%p]\n", m_defs);
    if (m_defs)   { m_defs  ->output2debug(); }

    printf("m_values:[%p]\n", m_values); 
    if (m_values) { m_values->output2debug(); }
    
    if (m_buffer != nullptr)
    {
        puts("m_buffer:");
        m_buffer->output2debug();
    }
    puts("");
} // output2debug




} // namespace steed
