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
 * @file CABReader.cpp
 * @author  Zhiyi Wang  <zhiyiwang@ict.ac.cn>
 * @version 0.4
 * @section DESCRIPTION
 *   definitions and functions for CABReader
 */

#include "CABReader.h"

namespace steed {

int CABReader::init2read(const string &base, SchemaTree* tree, SchemaPath &path, uint64_t cap)
{
    if (CABOperator::init(tree, path, cap) < 0)
    {
        printf("CABReader: init base 2 read failed!\n");
        return -1;
    } // init 


    // CAB content file
    string cab_bin(base);
    cab_bin.append(".cab");
    m_cont_buf = m_cab_meta.m_buf; // need one Buffer during read  
    if  (m_cont_buf->init2read(cab_bin) < 0) // init InMemory to read is OK
    {
        printf("CABReader: init Buffer 2 read failed!\n");
        return -1;
    } // if 
    m_file_io = m_cab_meta.m_buf->getFileIO();
    m_layouter = new CABLayouter(m_cont_buf, m_cmp_type);

    
    // CAB info file
    string cab_info(cab_bin);
    cab_info.append(".info");
    m_info_buf = new CABInfoBuffer();
    if  (m_info_buf->init2read(cab_info) < 0)
    {
        printf("CABReader: init CABInfo 2 write failed!\n");
        return -1; 
    } // if 


#ifdef _DEBUG_COLUMN_READER
    path.output2debug();
    size_t nbgn = cab_info.find_last_of ("/");
    assert(nbgn != string::npos);
    string name(cab_info, nbgn + 1); 
    printf("CABReader:init [%p] <%s>\n", this, name.c_str());
    puts  ("");
#endif // _DEBUG_COLUMN_READER


//-    // do not need to prepare the first CAB, prepare later 
//-    if (prepareNextCAB() < 0)

    return 0;
} // init2read





int CABReader::prepareNextCAB(void)
{
    if (m_cur_cab != nullptr)
    {
        m_cab_meta.reset();
        delete m_cur_cab  ;  m_cur_cab = nullptr;
    } // if 

    int s = prepareCABInfo();
    if (s <= 0) { return s; } // failed or EOF


    // read CAB content from file
    if ((!isTrivialCAB()) && (prepareBinCont() < 0))
    {
        puts("CABReader: prepareNextCAB prepareBinCont failed!");
        return -1;
    } // ig 


    CABItemInfo::Type type = getType();
    m_cur_cab = new CAB(&m_cab_meta, m_cur_info);
    s = m_cur_cab->init2read(type);
    if (s < 0)
    {
        puts("CABReader: init to read next CAB failed!");
        return -1;
    } // if 

    m_rep_vec = m_cur_cab->getRepBitsVec(); 

    return 1;
} // prepareNextCAB





int CABReader::prepareBinCont(void)
{
    // seek 2 storage content  
    uint64_t off = m_cur_info->m_file_off;
    if (m_file_io->seekContent(off, SEEK_SET) == (uint64_t)-1)
    {
        printf("CABReader: seek CAB content failed!\n");
        return -1;
    } // if 


    m_layouter->clear();
    if (m_layouter->load(m_cur_info) < 0)
    {
        printf("CABReader::  load CAB content failed!\n");
        return -1;
    } // if 


    return 0;
} // prepareBinCont





void CABReader::output2debug(void) 
{
    if (m_rep_vec != nullptr)
    {   m_rep_vec->output2debug();   }
} // output2debug


}
