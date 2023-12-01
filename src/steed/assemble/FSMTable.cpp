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
 * @file FSMTable.cpp 
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for FSMTable in RowNestedAssembler 
 */

#include "FSMTable.h"



namespace steed {



void FSMTable::initCommonLevel(SchemaTree * tree, QueryPathes &path)
{
    m_column_num  = path.size();
    for (uint32_t pi = 0; pi + 1 < m_column_num; ++pi)
    {
        SchemaPath &p1 = path[pi  ];
        SchemaPath &p2 = path[pi+1];
        m_pathes.emplace_back(p1);

        // the lowest same level in SchemaPath 
        uint32_t low_same = SchemaPath::getLowestSameLevel(p1, p2); 
        m_low_same_lvl.emplace_back(low_same);

        // common repetition level: 
        // repetition level of their lowest common ancestor ->
        //   must be a repeated SchemaNode or root  
        uint32_t com_rept = low_same; 
        while   (com_rept > 0)
        {
            // SchemaPath excludes the root node: 
            //   trans candidate com_rept to SchemaPath index 
            uint32_t com_pidx = com_rept - 1;    
            SchemaSignature sign = p1[com_pidx]; // parent's sign in SchemaTree 
            if (tree->isRepeated(sign))
            {
                // got repeated SchemaNode: can repeated @ this level 
                // multiple value array OR SchemaTemplate root  
                break;
            } 
            else
            {
                // try upper SchemaNode  
                --com_rept;
            } // if 
        } // while 
        m_com_rep_lvl.emplace_back(com_rept);
    } // for 
    
    // the last path's common level is 0
    SchemaPath &last = path.get(m_column_num-1);
    m_pathes.emplace_back(last);
    m_low_same_lvl.emplace_back(0);
    m_com_rep_lvl .emplace_back(0);
} // initCommonLevel





int FSMTable::initTransTable(QueryPathes &path, vector< vector<uint32_t> > &trans)
{
    // alloc space for each SchemaPath in QueryPathes
    for (uint32_t pi = 0; pi < m_column_num; ++pi)
    {
        SchemaPath &sp = path [pi];
        uint32_t splen = sp.size(); 
        uint32_t trlen = splen + 1; // one extra for root's value (rep = 0)
        trans.emplace_back(trlen, uint32_t(-1)); 
        m_max_size = (m_max_size > trlen) ? m_max_size : trlen;
    } // for 


    // fill transition table ///////////////////////////////////
    for (uint32_t pidx = 0; pidx < m_column_num; ++pidx)
    {
        uint32_t barrier      = pidx + 1;            // next or final FSM state
        uint32_t barrierLevel = m_com_rep_lvl[pidx]; // common rep with barrier
        vector<uint32_t> &trans_col =   trans[pidx];


        // 1. update common level <= barrierLevel:
        //   FSM must jump to the barrier
        for (uint32_t li = 0; li <= barrierLevel; ++li)
        {   trans_col[li] = barrier;   } 
        

        // 2. update common level > barrierLevel 
        //   set the transition to jump
        SchemaPath   &sp = path.get(pidx); 
        for (uint32_t fi = pidx; fi != uint32_t(-1); --fi) 
        {
            // common level between pidx and fi 
            uint32_t com = getCommonReptLevel(pidx, sp, fi);
            if (com > barrierLevel)  // to the most head 
            {   trans_col[com] = fi;   }
        } // for 
        
        
        // 3. set the result -1 in transition column 
        uint32_t  sp_len  = sp.size ();
        uint32_t  max_tab_idx = sp_len; // max index in trans table 
        uint32_t  bgn_idx = max_tab_idx - 1;
        for (uint32_t li = bgn_idx; (li!=uint32_t(-1)) && (li>barrierLevel); --li)
        {
            // lacks trans info: copy from lower level 
            bool lack_info = (trans_col[li] == uint32_t(-1));
            if  (lack_info)
            {   trans_col[li] = trans_col[li + 1];   } 
        } // for 


        // 4. erase the invalid jump info:
        // Only keep the repeated SchemaNode's jump info; 
        // optional SchemaNode cannot jump
        for (uint32_t sp_idx = 0; sp_idx < sp_len; ++sp_idx)
        {
            SchemaSignature sign = sp[sp_idx]; 
            if (!m_tree->isRepeated(sign))
            {
                uint32_t  tr_idx  = sp_idx + 1;
                trans_col[tr_idx] = uint32_t(-1);
            } // if 
        } // for 
    } // for 

    ////////////////////////////////////////////////////////////

    return 0;
} // initTransTable 





void FSMTable::initFSMTable(vector< vector<uint32_t> > &trans)
{
    // alloc m_fsm_table space 
    uint32_t path_num  = trans.size ();
    uint32_t elem_num  = path_num * m_max_size;
    uint32_t byte_used = elem_num * sizeof (uint32_t);
    m_fsm_table  = (uint32_t*)malloc (byte_used);
    memset(m_fsm_table, uint32_t(-1), byte_used); // init as -1 


    // fill m_fsm_table
    uint32_t *bgn = m_fsm_table;
    for (uint32_t pidx = 0; pidx < path_num; ++pidx)
    {
        vector<uint32_t> &cur = trans.at(pidx);
        uint32_t len = cur.size();
        memcpy  (bgn,  cur.data(), len << 2); // sizeof(uint32_t) == *4 = 2^2
        bgn += m_max_size;
    } // for pidx 
} // initFSMTable





void FSMTable::output2debug(void)
{
    if (m_fsm_table == nullptr)
    {
        puts("FSMTable: nothing to output2debug");
        return ;
    } // if 


    if (1)
    {
        puts("FSMTable_cont | low_same | com_rept | schema_path");

        uint32_t pnum = m_pathes.size();
        for (uint32_t pi = 0; pi < pnum; ++pi)
        {
            // trans table 
            printf("<%3u> : ", pi);
            for (uint32_t ei = 0; ei < m_max_size; ++ei)
            {
                uint32_t rep = get(pi,ei);
                bool invalid = (rep == uint32_t(-1));
                if  (invalid)
                { printf("[N.A]"); }
                else
                { printf("[%3u]", rep); }
            } // for 
        
            // the lowest same level 
            uint32_t low_same = m_low_same_lvl[pi];
            printf (" <-> [%u]", low_same);
        
            // common repetition level
            uint32_t com_rept = m_com_rep_lvl[pi];
            printf (" <-> [%u]", com_rept);

            printf (" <-> ");
            m_pathes[pi].outputPath2Debug();

            printf (" <-> ");
            string exp;
            m_tree->appendPathName(exp, m_pathes[pi]); 
            printf("%s\n", exp.c_str());
        } // for 
    } // if 

    
    if (0) 
    {
        // print by members
        puts("FSMTable output pathes:");
        for (auto &p : m_pathes)
        {   p.output2debug();   }
        puts("");
        
        puts("FSMTable output FSM table:");
        for (uint32_t ci = 0; ci < m_column_num; ++ci)
        {
            printf("\t<%u> : ", ci);
            for (uint32_t ei = 0; ei < m_max_size; ++ei)
            {   printf("[%2d]", int32_t(get(ci,ei)));   }
            puts("");
        } // for ci
    
        puts("FSMTable output m_com_rep_lvl:");
        for (auto &c : m_com_rep_lvl)
        {   printf("\t[%2u]\n", c);   }
        puts("");
        
        puts("========================================================\n\n");
    } // if 
} // output2debug



} // namespace 
