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
 * @file ColumnAssembler.cpp
 * @version 1.0
 * @section DESCRIPTION
 *  assemble column from records
 */

#include "ColumnAssembler.h"

namespace steed {


int ColumnAssembler::init(const string &db, const string &tb, vector<string> cols)
{
    int got = SchemaTreeMap::getDefinedTree(db, tb, m_tree);
    if (got <= 0)
    {
        printf( "ColumnAssembler: SchemaTree [%s:%s] is missing!\n", db.c_str(), tb.c_str());
        return got;
    } // if

    m_buf = new Buffer(g_config.m_assemble_buf_cap);
    m_parser.init(m_tree, &m_exps);

    // parse column names into m_exps,
    // then sort and put the unique columns into m_fields
    for (auto & col : cols)
    {
        vector<string> names; // field names in path
        Utility::splitString(col, Config::s_field_delim, names);
        int got = m_parser.parse(names); // parsed into m_exps
        if (got == 0)
        {
            printf("ColumnAssembler: parse column [%s] failed!\n", col.c_str());
            return -1;
        } // if
    } // for 


    std::sort( m_exps.begin(), m_exps.end() );
    m_exps.erase( std::unique( m_exps.begin(), m_exps.end() ), m_exps.end() );
    for (auto & exp : m_exps)
    { m_fields.checkAndAppend(exp.getPath()); }

    // create column readers
    string dir;
    Utility::getDataDir(g_config, db, tb, dir);

    uint32_t num = m_fields.size();
    for (uint32_t i = 0; i < num; ++i)
    {
        SchemaPath   &sp = m_fields.get(i);
        ColumnReader *rd = new ColumnReader();
        rd->init2read(dir, m_tree, sp); 
        m_col_rds.emplace_back(rd);
    } // for

    return this->init(&m_fields, m_col_rds);
} // init 



int32_t ColumnAssembler::getNext(char* &rbgn)
{
    rbgn = nullptr;

    if (m_buf_rnum == 0)
    {
        if (m_buf != nullptr)
        {   m_buf->clear ();   }
        m_next_rbgn = 0;

        int32_t got = bufferMore();
        if (got <= 0) { return got; }

        m_buf_rnum    = got;
        m_total_rnum += got;
    } // if

    --m_buf_rnum;
    rbgn = (char*)m_buf->getPosition(m_next_rbgn);
    m_next_rbgn += *(Row::Size*)rbgn;
    return 1;
} // getNext



int32_t ColumnAssembler::bufferMore(void)
{
    if (m_dbl_buf)
    {
        m_dbl_buf = false;
        if (doubleBuffer() < 0)
        {
            puts("CFO: double buffer for space failed!");
            return -1;
        } // if
    } // if

    int32_t rnum = 0, rd_got = 0, anum = 0;
    while  (rnum < int32_t(g_config.m_recd_cap))
    {
        // check avail space is enough
        uint64_t  avail = m_buf->available();
        m_dbl_buf = (avail < g_config.m_recd_max_len);
        if (m_dbl_buf) { break; }


        // prepare and assemble
        rd_got = prepareColumnReader();
        if      (rd_got <  0) { rnum = rd_got; break; } // failed
        else if (rd_got == 0) { break; }                // EOF

        anum = m_assemble->assemble();
        assert(anum == 1);

        m_cur_recd_idx += anum;  // assemble one: update record index
        ++rnum;
    } // rnum


    return rnum;
} // bufferMore



int ColumnAssembler::prepareColumnReader(void)
{
    // some columns become valid: reinit assembler
    if (m_columns->need2update (m_cur_recd_idx))
    {
        m_columns ->updateColumn(m_cur_recd_idx);
        m_assemble->reinit(m_columns);
    } // if

    // prepare each column reader:
    // next read got the first ColumnItem in record m_cur_recd_idx
    vector<ColumnReader*> &crds = m_columns->getCurrentColRead();
    for (auto & rd : crds)
    {
        int got = rd->prepare2ReadRecord(m_cur_recd_idx);
        if (got <= 0)
        {   return got;   }
    } // for

#ifdef _DEBUG_COLUMN_READER
    puts("");
    for (auto & rd : crds)
    {   rd->output2debug();   }
    puts("");
#endif // _DEBUG_COLUMN_READER

    return 1;
} // prepareColumnReader


} // namespace steed