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
 * @file CABWriter.cpp
 * @author  Zhiyi Wang  <zhiyiwang@ict.ac.cn>
 * @version 0.4
 * @section DESCRIPTION
 *   definitions and functions for CABWriter
 */

#include "CABWriter.h"

namespace steed {

int CABWriter::init2write(const string &base, SchemaTree* tree, SchemaPath &path,
        uint64_t cap, uint64_t rbgn)
{
    if (CABOperator::init(tree, path, cap) < 0)
    {
        printf("CABWriter: init base 2 write failed!\n");
        return -1;
    } // init 


    // CAB content file
    string cab_bin(base);
    cab_bin.append(".cab");
    m_cont_buf = new Buffer(0); // buffered full content 
    if (m_cont_buf->init2write(cab_bin) < 0)
    {
        printf("CABWriter: init buffer @ [%s] 2 write failed!\n", cab_bin.c_str());
        return -1;
    }
    m_file_io = m_cont_buf->getFileIO();
    m_layouter = new CABLayouter(m_cont_buf, m_cmp_type);
    

    // CAB info file  
    string cab_info(cab_bin);
    cab_info.append(".info");
    m_info_buf = new CABInfoBuffer();
    if  (m_info_buf->init2write(cab_info, rbgn) < 0)
    {
        printf("CABWriter: init CABInfo 2 write failed!\n");
        return -1;
    } // info


    // Set current m_recd_num as CAB begin record index:
    //   need to write null ColumnItems to align records within CAB.
    m_recd_num = Utility::calcAlignBegin (rbgn, g_config.m_cab_recd_num);
    initValueInfo( m_info_buf->getValueInfo() );
 
    // always prepare the next CAB 2 write:
    //    (must be sequential writer)
    if (prepareCAB2write() < 0)
    {
        printf("CABWriter: init 2 prepareCAB2write failed!\n");
        return -1;
    } // if  


    return 0;
} // init2write



int CABWriter::writeNull(uint32_t rep, uint32_t def, uint64_t nnum)
{
    int got = 0;
    for (uint64_t ni = 0; ni < nnum; ++ni)
    {
        if ((got = writeNull(rep, def)) < 0)
        {
            puts("CABWriter:: write null failed!\n");
            break;
        } // if 
    } // for 

    return got;
} // writeNull



int CABWriter::writeNull(uint32_t rep, uint32_t def)
{
    // update repetition to storage
    rep = m_rept->encode(rep);

    int got =  m_cur_cab->writeNull(rep, def);
    if (got == 0)
    {
        // CAB is full  
        bool istail = false;
        if ((got = flush     (istail)) < 0) { return got; }
        if ((got = prepareCAB2write()) < 0) { return got; }

        // write 2 new CAB 
        got = m_cur_cab->writeNull(rep, def);
        if (got < 0)
        {
            printf("CABWriter: writeNull to CAB failed!\n");
            return -1;
        } // if 
    } // if 

    m_recd_num += ((rep == 0) ? 1 : 0);

    return got;
} // writeNull



int CABWriter::writeText(uint32_t rep, uint32_t def, const char* txt)
{
//printf("CABWriter:: writeText: [%s]\n", txt);
    // update repetition to storage
    rep = m_rept->encode(rep);

    const void *bin = nullptr; 
    int retval =  m_cur_cab->writeText(rep, def, txt, bin);
    if (retval == 0)
    {
        // CAB is full  
        mergeValueInfo(
            &(m_cur_info->m_value_info), m_info_buf->getValueInfo());

        bool istail = false; 
        if ((retval = flush     (istail)) < 0) { return retval; }
        if ((retval = prepareCAB2write()) < 0) { return retval; }
   
        // write 2 new CAB 
        retval = m_cur_cab->writeText(rep, def, txt, bin);
        if (retval < 0)
        {
            printf("CABWriter: writeText to CAB failed!\n");
            return -1;
        } // if 
    } // if 
    
    // write success: update value info 
    updateValueInfo(bin, &(m_cur_info->m_value_info)); // CAB

#if DEFINE_BLM
    if (m_bloom)
    {   updateBloom(bin, len);   } 
#endif

    m_recd_num += ((rep == 0) ? 1 : 0);

    return retval;
} // writeText   



int CABWriter::writeBinVal(uint32_t rep, uint32_t def, const void* bin, uint32_t len)
{
    // update repetition to storage
    rep = m_rept->encode(rep);

    int retval = m_cur_cab->writeBinVal(rep, def, bin, len);
    if (retval == 0)
    {
        // CAB is full  
        mergeValueInfo(
            &(m_cur_info->m_value_info), m_info_buf->getValueInfo());

        bool istail = false;  
        if ((retval = flush     (istail)) < 0) { return retval; }
        if ((retval = prepareCAB2write()) < 0) { return retval; }
   
        // write 2 new CAB 
        retval = m_cur_cab->writeBinVal(rep, def, bin, len);
        if (retval < 0)
        {
            printf("CABWriter: writeBinVal to CAB failed!\n");
            return -1;
        } // if 
    } // if 
    

    // write success: update value info 
    updateValueInfo(bin, &(m_cur_info->m_value_info)); // CAB

#if DEFINE_BLM
    if (m_bloom)
    {   updateBloom(bin, len);   }
#endif 

    m_recd_num += ((rep == 0) ? 1 : 0);

    return retval;
} // writeBinVal



int CABWriter::flush(bool tail)
{
    // flush CAB from mem 2 disk
    if (m_layouter->flush(tail, m_cur_info, m_cur_cab) < 0)
    {
        puts("CABWriter:: CABLayouter flush CAB failed!");
        return -1;
    } // flush
    
#if DEFINE_BLM
    // flush blooming content of CABInfo file 
    if (m_bloom)
    {
        char    *blmbin = (char*)m_bloom->getData();
        uint64_t memlen = m_bloom->sizeInBytes();
        uint64_t dsklen = UTILITY_CALC_ALIGN_SIZE(memlen, Config::s_mem_alignment);
        int64_t  got =
            m_info_buf->flushBloomContent(m_cur_info, blmbin, memlen, dsklen);
        if (got < 0)
        {
            puts("CABWriter:: flush flushBloomContent failed!");
            return -1;
        } // flush
   
        this->resetBloom();
    } // if 
#endif 
    
    // update and clear  
    m_file_off += m_cur_info->m_strg_size;
    if (m_file_io->seekContent(m_file_off, SEEK_SET) == (uint64_t)-1)
    {
        printf("CABWriter: seek CAB content failed!\n");
        return -1;
    } // if 
    
    m_cab_meta.reset ();
    m_cont_buf->clear();
    m_layouter->clear();


    delete m_cur_cab;
    m_cur_cab  = nullptr;
    m_cur_info = nullptr; 
    
    return 0;
} // flush



} // namespace
