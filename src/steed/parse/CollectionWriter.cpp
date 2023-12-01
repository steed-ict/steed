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
 * @file CollectionWriter.cpp
 * @author  Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   Defines funcs to class CollectionWriter related funcs 
 */

#include "CollectionWriter.h"

namespace steed {



int CollectionWriter::flush(void)
{
    uint64_t cnum = m_txt_buf->size();
    for (uint64_t ci = 0; ci < cnum; ++ci)
    {
        ColumnTextBuffer *ctb = m_txt_buf->get(ci);
        if (ctb == nullptr) { continue; } 

        ColumnWriter   *col = m_col_wts->get(ci);
        uint32_t maxd = col->getMaxDefVal     ();
        uint64_t tnum = ctb->size(); 
        for (uint64_t ti = 0; ti < tnum; ++ti)
        {
            ColumnTextBuffer::Item &cti = ctb->get(ti);
            uint32_t     r = cti.getRep();
            uint32_t     d = cti.getDef();
            const char  *t = cti.getTxt();
            bool    isnull = (d < maxd);
            int s = isnull ? col->writeNull(r, d) : col->writeText(r, d, t);
            if (s < 0)
            {
                puts("CollectionWriter: writeText failed!");
                return -1;
            } // if 

#ifdef _DEBUG_PARSER
            printf("CollectionWriter::flush [%lu] rep[%u] def[%u] value[%s]\n", ci, r, d, t);
#endif // _DEBUG_PARSER
        } // for ti
        ctb->clear();
    } // for ci

    return 0;
} // flush





int CollectionWriter::alignColumnWriter(SchemaSignature ls, TreeCounter &cnt)
{
    ColumnWriter *c = m_col_wts->get(ls);


    // align records in current CAB
    uint64_t recd_num = cnt.getRootCnt(); 
    uint64_t recd_idx = recd_num - 1;
    uint64_t cab_rbgn = Utility::calcAlignBegin(recd_idx, g_config.m_cab_recd_num);

    uint32_t rep = 0, def = 0;  
    uint64_t null_num = recd_idx - cab_rbgn; 
    if (null_num > 0)
    {
        if (c->writeNull(rep, def, null_num) < 0)
        {
            puts("CollectionWriter: alignColumnWriter write record null failed!");
            return -1;
        } // if 
    } // if 
    assert(c->getRecdNum() == c->getValidRecdIdx());

    
    // align column items in current record 
    bool first = true; // record's first ColumnItem 
    SchemaPath sp; 
    m_tree->getPath(ls, sp);
    uint32_t depth = sp.size(); // schema path depth
    for (uint32_t idx = 0; idx < depth; ++idx)
    {
        SchemaSignature ss = sp[idx];
        null_num = cnt.get(ss);          // null   number: TreeCounter is pre-counter
        uint32_t out_num = null_num - 1; // output number
        if      (null_num == 0) { break; } // just created: waiting first item, no need to align
        else if (out_num  == 0) { continue; } // no null to out @ this level 


        // use TreeCounter to get the field's first appeared ColumnItem within record 
        uint32_t lvl = idx + 1; // level = index + 1
        if (first)
        {
            // buffer then write the first item to column 
            rep = 0, def = lvl; 
            write(ls, rep, def, nullptr);

            // written one null
            first = false, --out_num; 
        } // if 
        

        // buffer then write 
        rep = lvl, def = lvl;  
        for (uint32_t i = 0; i < out_num; ++i)
        {   write(ls, rep, def, nullptr);   }
    } // for idx 


    return 0;
} // alignColumnWriter



} // namespace 
