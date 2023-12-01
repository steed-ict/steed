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
 * @file RecordNestedAssembler.cpp 
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for RecordNestedAssembler 
 */

#include "RecordNestedAssembler.h"



namespace steed {



int RecordNestedAssembler::assemble(void)
{
    void *   recd_bgn   = m_buf->getNextPosition();
    uint32_t before_use = m_buf->used();


    int s = 0;       // state
    ColumnItem citm; // ColumnItem to read 
    uint32_t   cnum = m_crds.size();
    uint32_t   cidx = 0;  // read column index 
    m_build->begin2build(); 

    bool  mv2frt = false; // move to front column in m_crds 
    while (true)
    {
        // None column is valid for the current record id
        if (m_path->empty())
        {   m_build->end2build(); break;   }

        SchemaPath   &sp  = m_path->get(cidx);
        ColumnReader *crd = m_crds[cidx];

        s = crd->readItem(citm);
        if (s < 0)
        {
            printf("RecordNestedAssembler: read from [%u] failed!\n", cidx);
            return -1; 
        } 
        else if (s == 0)
        {   return 0;   } 


        // definition values:
        // SchemaNode level in SchemaTree, including root (def == level == 0)
        uint32_t def = citm.getDef(); 
        if (move2level(def, sp, mv2frt) < 0)
        {
            printf("RecordNestedAssembler: move2level failed!\n");
            return -1;
        } // move2level
         
        
        uint32_t max_def = (sp.size());
        bool got_val = (def == max_def);
        if  (got_val) 
        {  
            SchemaSignature lf_ss = sp.back(); // leaf
            DataType       *lf_dt = m_tree->getDataType(lf_ss);

            const void * src = citm.getBin();
            int   blen = lf_dt->getBinSize(src);
            void *dest = m_buf->allocate  (blen, false);
            lf_dt  ->copy(src, dest);
            m_build->appendLeafValue(blen);
        } // if 
        
     
        // lookup FSM and get next ColumnReader index 
        uint32_t nrep = citm.getNextRep();
        uint32_t nidx = m_fsm->get(cidx, nrep); 
        if (nidx >= cnum) // EOF 
        {
            uint32_t root_com = 0; // root's common
            bool     new_elem = false;
            return2level(root_com, sp, new_elem);
            m_build->end2build(); 
            break;
        } // if 


        // move to front: next column index is smaller than current
        mv2frt = (nidx <= cidx);

        // always return to level no matter got_field is true or false:
        //   true :  return to the common level 
        //   false:  skip to move2level but also need to return2common:
        // 
        // e.g.
        //   index       1  2  3
        //   depth       2  2  1 
        //   rept idx    1  1  3 
        //   got_val     T  F  T
        //   the F in got_val (index == 2) also need to call return2common
        uint32_t tgt_lvl = getCommonLevel(cidx, nidx, nrep);
        return2level(tgt_lvl, sp, mv2frt); 

        cidx = nidx;
    } // while


    uint32_t after_used = m_buf->used();
    uint32_t recd_used  = after_used - before_use;  
    uint32_t recd_size  = *(uint32_t*)recd_bgn;
    if (recd_used != recd_size)
    {
        puts  ("RecordNestedAssembler: assemble error!");
        printf("buffer used:[%u] record size:[%u]\n", recd_used, recd_size);
        m_buf->output2debug();
        abort();
    } // if 

  
    return 1; 
} // assemble



} // namespace
