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
 * @file CollectionWriter_inline.h
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0 
 * @section DESCRIPTION
 *    CollectionWriter inline functions
 */

#pragma once 

namespace steed {


inline
ColumnParser::~ColumnParser(void)
{
    // always flush SchemaTree when updated
    if ((m_tree != nullptr) && (m_tree->flush() < 0))
    {
        printf("ColumnParser: flush SchemaTree failed!\n");
        DebugInfo::printStackAndExit();
    } // if 

    delete m_item_gen; m_item_gen = nullptr;
    delete m_clt_wt  ; m_clt_wt   = nullptr;

    // SchemaTree is managed by SchemaTreeMap, just release the pointer
    m_tree = nullptr;

    for (uint32_t i = 0; i < s_jtree_cap; ++i)
    {
        delete m_jtree[i];
        m_jtree[i] = nullptr;
    } // for 
 
    delete m_jparser; m_jparser = nullptr;
    delete m_jbuffer; m_jbuffer = nullptr;
} // dtor  



inline
int64_t ColumnParser::parseOne(void)
{
    // read samples
    uint32_t rnum = 1; 
    int bnum = readRecds2TreeInBatch( &JSONRecordReader::readRecord, rnum); 
    if (bnum < 0)
    {
        printf("ColumnParser: parse got [%d]\n", bnum);
        return bnum;
    } 
    else if (bnum == 0)
    {   return 0;   }

    int s = m_item_gen->generate(m_jtree_used, m_jtree);
    if (s < 0)
    {
        printf("ColumnParser: update SampleTree got [%d]\n", s);
        return s;
    } // if 

    return 1;
} // parseOne





inline
int64_t ColumnParser::parseAll(void)
{
    int64_t bat_cnt = 0, recd_cnt = 0;
    uint32_t rnum = s_jtree_cap;
    do {
        // read samples
        int bnum = readRecds2TreeInBatch( &JSONRecordReader::readRecord, rnum); 
        if (bnum < 0)
        {
            printf("ColumnParser: parse got [%d]\n", bnum);
            return bnum;
        } 
        else if (bnum == 0)
        {   break;   }
        recd_cnt += bnum;

        int s = m_item_gen->generate(m_jtree_used, m_jtree);
        if (s < 0)
        {
            printf("ColumnParser: update SampleTree got [%d]\n", s);
            return s;
        } // if 

        if (bat_cnt++ % 1000 == 0) 
        {
            DebugInfo::printTime();
            printf("ColumnParser parsed [%lu * %u] = %lu records\n",
                (bat_cnt - 1),  g_config.m_text_recd_num,
                (bat_cnt - 1) * g_config.m_text_recd_num);
        } // if 
    } while (true);

//    printf("STEED: parsed [%lu] records in total\n", recd_cnt);
    return recd_cnt;  
} // parse



inline
int64_t ColumnParser::parseOne(const char *recd, uint32_t len)
{
    if (recd == nullptr) { return -1; }

    m_jbuffer->reset();
    m_jbuffer->appendOneRecd(recd, len);

    JSONBinTree* jtree = m_jtree[0];
    uint64_t recd_len = 0; 
    char*    recd_bgn = nullptr;
    int ns = m_jbuffer->nextRecord(recd_bgn, recd_len);
    if (ns <= 0)
    { return ns; }

    int ps = m_jparser->parse(jtree, recd_bgn);
    if (ps < 0)
    { return ps; }
    m_jtree_used = 1;

    int gs = m_item_gen->generate(m_jtree_used, m_jtree);
    if (gs < 0)
    { return gs; }

    return 1;
} // parserOne



inline
int ColumnParser::readRecds2TreeInBatch(ReadFPtr fptr, uint32_t rnum)
{
    // read records in batch
    array<char*, s_jtree_cap> bgns;
    int rs = m_jbuffer->readRecords(fptr, rnum, bgns);
    if (rs <  0) { printf("ColumnParser: nextRecord got [%d]\n", rs); } 
    if (rs <= 0) { return rs; }
//    m_jbuffer->output2debug();

    // parse records in batch  
    uint32_t pidx = 0; 
    while (pidx < rnum)
    {
        int ps = m_jparser->parse(m_jtree[pidx], bgns[pidx]);
        if (ps < 0)
        {
            printf("ColumnParser: parse got [%d]\n", ps);
            return ps;
        }

        ++pidx;
    } // while 

    m_jtree_used =  rnum;
    return (rnum = pidx) ? int(rnum) : -1;
} // readRecds2TreeInBatch



} // namespace steed

