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
 * @file ColumnReader_inline.h
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0 
 * @section DESCRIPTION
 *   ColumnReader inline functions
 */

#pragma once 

namespace steed {



inline
int ColumnReader::init2read (const string &dir, SchemaTree* tree, SchemaPath &path)
{
    if (init(dir, tree, path) < 0)
    {
        printf("ColumnReader: init failed!\n");
        return -1;
    } // if 
    
    uint64_t rcap = g_config.m_cab_recd_num;
    m_read = new CABReader();
    if (m_read->init2read(m_file_name, m_tree, m_leaf_path, rcap) < 0)
    {
        printf("ColumnReader: init 2 read failed!\n");
        return -1;
    } // if 

    return 0;
} // init2read





inline
int ColumnReader::init(const string &dir, SchemaTree* tree, SchemaPath &path)
{
    m_tree      = tree;
    m_leaf_path = path;

    // debug  
    if (tree == nullptr) { return 0; }

    if (StoragePath::getDataPath(dir, tree, m_leaf_path, m_file_name) < 0)
    {
        printf("ColumnReader: init failed!\n");
        return -1;
    } // if 
    
    return 0;
} // init





inline
int ColumnReader::loadCAB4Record(uint64_t ridx)
{
    // prepare new CAB content and item flags 
    int got = m_read->loadCAB4Record(ridx);
    if (got > 0) // got new CAB
    {
        uint64_t itm_idx = 0;
        uint64_t bgn_rid = m_read->getCABBeginRid();
        m_item_idx = m_read->getRecdBeginItemIdx (bgn_rid, itm_idx, ridx);
        m_recd_idx = ridx;
    } // if 
    return got; 
} // loadCAB4Record





inline
int ColumnReader::prepare2ReadRecord(uint64_t ridx)
{
    uint64_t iidx = m_read->getRecdBeginItemIdx(m_recd_idx, m_item_idx, ridx); 
    if (iidx != uint64_t(-1))
    {
        // got items in CAB
        m_recd_idx = ridx;
        m_item_idx = iidx;
    }
    else
    {
#ifdef _DEBUG_COLUMN_READER
    printf("ColumnReader::m_read@[%p] loadCAB4Record[%lu]\n", m_read, ridx);
    fflush(nullptr);
#endif // _DEBUG_COLUMN_READER

        // m_recd_idx and m_item_idx is updated 
        int got =  loadCAB4Record(ridx);
        if (got <= 0)    { return got; }
    } // if 

    return 1;
} // prepare2ReadRecord





inline
int ColumnReader::readItem(ColumnItem &ci)
{
    int got = m_read->read(m_item_idx++, ci);
    m_recd_idx += (ci.getNextRep() == 0); 
    return got;
} // readItem





inline
void ColumnReader::prepareItemBitmap(BitMap *bitmap)
{
    bitmap->uninit();

    uint64_t num = m_read->getItemNumber();
    bitmap->init (num);
    bitmap->clearAll();
} // prepareItemBitmap





inline
void ColumnReader::output2debug(void)
{
    puts  ("=========================================================");
    puts  ("ColumnReader::output2debug");
    printf("prepare@[%p] ridx[%lu] iidx[%lu]\n", m_read, m_recd_idx,  m_item_idx);

    puts  ("## m_read ");
    m_read->output2debug();

    puts  ("=========================================================");
    fflush(nullptr);
} // output2debug



} // namespace










// Discard  
//    /**
//     * load ColumnAlignBlock rep and def by record id  
//     * @param ridx    record index, maybe got from Index
//     * @return 1 success; 0 EOF; <0 failed
//     */
//    int loadCAB4RepDef (uint64_t ridx);
//
//inline
//int ColumnReader::loadCAB4RepDef(uint64_t ridx)
//{
//    // only used by CABAligner 
//    // assert use the first record to load  
//    assert(ridx % g_config.m_cab_recd_num == 0);
//
//    int got = m_read->loadCAB4RepDef(ridx);
//    if (got > 0) // got new CAB
//    {
//        m_recd_idx = ridx; // assert use the first record to load  
//        m_item_idx = 0;    // reset item index from begin 
//    } // if 
//    return got; 
//} // loadCAB4RepDef


