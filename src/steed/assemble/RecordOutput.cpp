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
 * @file RecordOutput.cpp 
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for RecordOutput 
 */

#include "RecordOutput.h"

namespace steed {



int RecordOutput::
    outJSONArr2Strm(ostream *ostrm, char *bgn, uint32_t lvl, SchemaSignature ss)
{
    assert(lvl < m_lvl_exp.size());

    (*ostrm) << '[';
    
    RowArrayOperator &arr_op = m_lvl_exp[lvl].m_arr;
    arr_op.init2read (bgn);
    
    bool comma = false;
    bool leaf  = m_tree->isLeaf(ss);
    uint32_t i = 0, num = arr_op.getElemNum();
    while   (i < num)
    {
        if (comma) 
        {   (*ostrm) << ',';   }

        bool empty = (arr_op.getBinSize(i) == 0);
        if  (empty)
        {
            (*ostrm) << (leaf ? "null" : "{}"); 
            comma = true, ++i;
            continue;
        } // if 
    
        char *bin = (char*)arr_op.getBinVal(i);
        if (leaf)
        {
            // use but no need to allocate buffer content 
            DataType *dt  = m_tree->getDataType  (ss);
            uint64_t  len = m_tbuf->available();
            char     *txt = (char*)m_tbuf->getNextPosition();
            if (dt->transBin2Txt(bin, txt, len) < 0)
            {
                puts("RecordOutput:: outJSONArr2Strm to transBin2Txt failed!\n");
                abort();
                return -1;
            } // if 

            (*ostrm) << txt; 
        }
        else if (outJSONObj2Strm(ostrm, bin, lvl + 1, ss) < 0)
        {
            printf("RecordOutput::outJSONObj2Strm failed\n");
            return -1;
        } // if 


        comma = true;
        ++i;
    } // while  
    
    arr_op.uninit();
    (*ostrm) << ']';

    return 0;    
} // outJSONArr2Strm 





int RecordOutput::
    outJSONObj2Strm(ostream *ostrm, char *bgn, uint32_t lvl, SchemaSignature ss)
{
    (void) ss;
    assert(lvl < m_lvl_exp.size());

    (*ostrm) << '{';

    RowObjectOperator &obj_op = m_lvl_exp[lvl].m_obj;
    obj_op.init2read  (bgn);

    bool comma = false;
    uint32_t i = 0, num = obj_op.getElemNum();
    while   (i < num)
    {
        // skip empty member 
        if (obj_op.getBinSize(i) == 0)
        {   ++i; continue;   }

        if (comma) 
        {   (*ostrm) << ',';   }

        char   *bin = (char*)obj_op.getBinVal(i);
        Row::ID         id = obj_op.getRowID (i);
        SchemaSignature ss = m_tree->getSignByID(id);
        if (!m_tree->isDefined(id))
        {
            printf("RecordOutput::outJSONObj2Strm [%u] failed!\n", id);
            return -1;
        } // if 


        // SchemaNode is defined in SchemaTree
        const string &key = m_tree->getName(ss);
        (*ostrm)  << "\"" << key << "\"" << ":";

        if (m_tree->isRepeated(ss))
        {
            if (outJSONArr2Strm(ostrm, bin, lvl, ss) < 0)
            {
                printf("RecordOutput::outArr4Debug failed\n");
                return -1;
            } // if 
        }
        else
        {
            if (m_tree->isLeaf(ss))
            {
                // use but no need to allocate buffer content 
                DataType *dt  = m_tree->getDataType  (ss);

                if (outJSONValue2Strm(ostrm, dt, bin) < 0)
                {
                    puts("RecordOutput::outJSONObj2Strm to outJSONValue2Strm failed!\n");
                    return -1;
                } // if 
            }
            else if (outJSONObj2Strm(ostrm, bin, lvl + 1, ss) < 0)
            {
                printf("RecordOutput::outObj4Debug failed\n");
                return -1;
            } // if 
        } // if 


        comma = true;
        ++i;
    } // while 

    obj_op.uninit();
    (*ostrm) << '}';

    return 0;    
} // outJSONObj2Strm









int RecordOutput::outArr4Debug(char *bgn, uint32_t lvl, SchemaSignature ss, uint32_t num)
{
    assert(lvl < m_lvl_exp.size());

    RowArrayOperator &arr_op = m_lvl_exp[lvl].m_arr;
    arr_op.init2read (bgn);

    bool     leaf  = m_tree->isLeaf(ss);
    uint32_t asize = arr_op.getElemNum();
    output4alignment (num);
    printf(">> Array begin: [%u] elems\n", asize);

    for (uint32_t i = 0; i < asize; ++i)
    {
        output4alignment (num);

        uint32_t off = arr_op.getOffset (i);
        uint32_t len = arr_op.getBinSize(i);
        printf("[%u]: off[%u] len[%u]\n", i, off, len);

        /** empty array may got an elements with length is 0  */
        if (len == 0) { continue; } 


        char *bin = (char*)arr_op.getBinVal(i);
        if  (leaf)
        {
            uint64_t buf_len = 4096;
            char buf[buf_len] = {0};
            DataType *dt = m_tree->getDataType(ss);
            dt->transBin2Txt(bin, buf, buf_len); 

            output4alignment (num);
            printf("leaf bin @ [%p] : [%s]\n", bin, buf);
        }
        else if (outObj4Debug(bin, lvl + 1, ss, num + 1) < 0)
        {
            printf("RecordOutput::outObj4Debug failed\n");
            return -1;
        }
    } // for 
        
    arr_op.uninit();

    output4alignment (num);
    puts(">> Array end ");
    
    return 0;
} // outArr4Debug





int RecordOutput::outObj4Debug(char *bgn, uint32_t lvl, SchemaSignature ss, uint32_t num)
{
    (void)ss;  
    assert(lvl < m_lvl_exp.size());

    RowObjectOperator &obj_op = m_lvl_exp[lvl].m_obj;
    obj_op.init2read  (bgn);

    uint32_t osize = obj_op.getElemNum();
    output4alignment (num);
    printf(">> Object begin: [%u] elems\n", osize);

    for (uint32_t i = 0; i < osize; ++i)
    {
        output4alignment (num);

        Row::ID  id  = obj_op.getRowID  (i);
        uint32_t off = obj_op.getOffset (i);
        uint32_t len = obj_op.getBinSize(i);
        char    *bin = (char*)obj_op.getBinVal(i);

        SchemaSignature ss = m_tree->getSignByID(id);
        const string & key = m_tree->getName(ss);
        printf("[%u]: id[%u] off[%u] len[%u] key:[%s]\n", i, id, off, len, key.c_str());

        /** empty array may got an elements with length is 0  */
        if (len == 0) { continue; } 


        bool leaf = m_tree->isLeaf(ss);
        bool rept = m_tree->isRepeated(ss);
        if  (rept)
        {
            if (outArr4Debug(bin, lvl, ss, num + 1) < 0)
            {
                printf("RecordOutput::outArr4Debug failed\n");
                return -1;
            } // if 
        }
        else
        {
            if  (leaf)
            {
                uint64_t buf_len = 4096;
                char buf[buf_len] = {0};
                DataType *dt = m_tree->getDataType(ss);
                dt->transBin2Txt(bin, buf, buf_len); 
    
                output4alignment (num);
                printf("leaf bin @ [%p] : [%s]\n", bin, buf);

//                output4alignment (num);
//                printf("leaf bin @ [%p]\n", bin);
            }
            else if (outObj4Debug(bin, lvl + 1, ss, num + 1) < 0)
            {
                printf("RecordOutput::outObj4Debug failed\n");
                return -1;
            } // if 
        } // if 
    } // for 
        
    obj_op.uninit();

    output4alignment (num);
    puts(">> Object end ");
    
    return 0;
} // outObj4Debug





} // namespace
