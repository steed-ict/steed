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

extern Config g_config; /**< global config */


inline
CollectionWriter::CollectionWriter(SchemaTree *tree) : m_tree(tree)
{
    if (m_tree == nullptr) { return; }

    m_db_name .assign(tree->getDBName ());
    m_clt_name.assign(tree->getCltName());
    Utility::getDataDir(g_config, m_db_name, m_clt_name, m_strg_path);
} // ctor





inline
CollectionWriter::~CollectionWriter(void)
{
    if (m_tree != nullptr)
    {   flush();  }

#ifdef _DEBUG_PARSER
    puts("\n\n\n\n\n\n\n\n\n");
    puts("#################### _DEBUG_PARSER #####################");
    puts("## Debug: output every columns' parsed record number. ##");
    output2debug();
    puts("#################### _DEBUG_PARSER #####################");
#endif // _DEBUG_PARSER

    delete m_col_wts; 
    delete m_txt_buf; 

    m_tree    = nullptr; 
    m_col_wts = nullptr;
    m_txt_buf = nullptr;
} // dtor 





inline
int CollectionWriter::init2append(TreeCounter *tc)
{
    uint64_t nnum = m_tree->getNodeNum();
    m_col_wts = new Container<ColumnWriter>(sizeof(ColumnWriter) * nnum);
    m_col_wts->resize(nnum);
    m_txt_buf = new Container<ColumnTextBuffer>(sizeof(ColumnTextBuffer) * nnum);
    m_txt_buf->resize(nnum);

    SchemaSignature first_leaf = 0; 
    for (SchemaSignature ni = 0; ni < nnum; ++ni)
    {
        bool isleaf = m_tree->isLeaf(ni);
        if (!isleaf)  { continue; }

        first_leaf = (first_leaf != 0) ? ni : first_leaf;

        // init column to append 
        if (initColumnAppender(ni) < 0)
        {
            puts("CollectionWriter::init2append initColumnAppender failed!");
            return -1;
        } // if
    } // for ni

    if (first_leaf != 0)
    {
        ColumnWriter *first_leaf_wt = m_col_wts->get(first_leaf);
        uint64_t recd_bgn = first_leaf_wt->getRecdNum();
        tc->setRootCnt(recd_bgn);
    } // if 

    return 0;
} // init2append





inline
int CollectionWriter::initColumnAppender(SchemaSignature ls)
{
    bool isleaf = m_tree->isLeaf(ls);
    if (!isleaf)
    {
        puts("CollectionWriter:: initColumnAppender init non-leaf");
        return -1;
    } // if 


    SchemaPath sp;
    m_tree->getPath(ls, sp);

    if (!m_col_wts->initElem(ls))
    {
        puts("CollectionWriter::initColumnAppender m_col_wts initElem failed!");
        return -1;
    } // if

    ColumnWriter *c = m_col_wts->get(ls);
    if (c->init2append(m_strg_path, m_tree, sp) < 0)
    {
        puts("CollectionWriter::initColumnAppender init ColumnWriter failed!");
        return -1;
    } // if

    if (!m_txt_buf->initElem(ls))
    {
        puts("CollectionWriter::initColumnAppender m_txt_buf initElem failed!");
        return -1;
    } // if


    return 0;
} // initColumnAppender





inline
int CollectionWriter::init2write(void)
{
    uint64_t nnum = m_tree->getNodeNum();
    m_col_wts = new Container<ColumnWriter>(sizeof(ColumnWriter) * nnum);
    m_col_wts->resize(nnum);
    m_txt_buf = new Container<ColumnTextBuffer>(sizeof(ColumnTextBuffer) * nnum);
    m_txt_buf->resize(nnum);

    for (SchemaSignature ni = 0; ni < nnum; ++ni)
    {
        bool isleaf = m_tree->isLeaf(ni);
        if (!isleaf)  { continue; }

        // init leaf ColumnWriter ins 
        uint64_t recd_bgn = 0;   // record begin index 
        if (initColumnWriter(ni, recd_bgn) < 0)
        {
            puts("CollectionWriter::init2write initColumnWriter failed!");
            return -1;
        } // if 
    } // for ni

    return 0;
} // init2write 





inline
int CollectionWriter::initColumnWriter(SchemaSignature ls, TreeCounter &cnt)
{
    uint64_t rnum = cnt.getRootCnt();
    uint64_t ridx = rnum - 1;
    if (initColumnWriter(ls, ridx) < 0)
    {
        puts("CollectionWriter::initColumnWriter W TreeCounter failed!");
        return -1;
    } // if 

    if (alignColumnWriter(ls, cnt) < 0)
    {
        puts("CollectionWriter::initColumnWriter alignColumnWriter ");
        return -1;
    } // alignColumnWriter

    return 0;
} // initColumnWriter





inline
int CollectionWriter::initColumnWriter(SchemaSignature ls, uint64_t rbgn)
{
    bool isleaf = m_tree->isLeaf(ls);
    if (!isleaf)
    {
        puts("CollectionWriter:: initColumnWriter init non-leaf");
        return -1;
    } // if 


    SchemaPath sp; 
    m_tree->getPath(ls, sp);

    if (!m_col_wts->initElem(ls))
    {
        puts("CollectionWriter::initColumnWriter m_col_wts initElem failed!");
        return -1;
    } // if 

    ColumnWriter *c = m_col_wts->get(ls);
    if (c->init2write(m_strg_path, m_tree, sp, rbgn) < 0)
    {
        puts("CollectionWriter::initColumnWriter init ColumnWriter failed!");
        return -1;
    } // if 

    if (!m_txt_buf->initElem(ls))
    {
        puts("CollectionWriter::initColumnWriter m_txt_buf initElem failed!");
        return -1;
    } // if 


    return 0;
} // initColumnWriter





inline void CollectionWriter::
    write(SchemaSignature ls, uint32_t r, uint32_t d, const char *txt)
{
    ColumnTextBuffer *ctb = m_txt_buf->get(ls);
    ctb->emplace_back(r, d, txt);
} // write 





inline
void CollectionWriter::output2debug(void)
{
    const char *dbname = m_db_name .c_str();
    const char *cltname= m_clt_name.c_str();
    printf("CollectionWriter [%s] [%s]\n", dbname, cltname);
    
    puts("Record Number in each ColumnWriter:");
    uint32_t nnum = m_tree->getNodeNum();
    for (uint32_t ni = 0; ni < nnum; ++ni)
    {
        ColumnWriter *wt = m_col_wts->get(ni);
        if (wt == nullptr) { continue; }

        uint64_t rnum = wt->getRecdNum();
        printf("leaf [%u]: %lu recds\n", ni, rnum);
    } // for ni
} // output2debug



} // namespace steed