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
 * @file ColumnWriter_inline.h
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0 
 * @section DESCRIPTION
 *   ColumnWriter inline functions
 */

#pragma once 



namespace steed {

extern Config g_config; /**< global config */


inline
ColumnWriter::~ColumnWriter(void)
{
    m_tree = nullptr;

    if (m_cab_op != nullptr)
    {   delete m_cab_op; m_cab_op = nullptr;   }
} // dtor 



inline
int ColumnWriter::init2write(const string &dir, SchemaTree* tree, SchemaPath &path, uint64_t rbgn)
{
    if (init(dir, tree, path) < 0)
    {
        printf("ColumnWriter: init failed!\n");
        return -1;
    } // if 
    

    uint64_t rcap = g_config.m_cab_recd_num;
    m_cab_op = new CABWriter();
    if  (m_cab_op ->init2write(m_file_name, m_tree, m_leaf_path, rcap, rbgn) < 0)
    {
        printf("ColumnWriter: init 2 write failed!\n");
        return -1;
    } // if 
    
    return 0;
} // init2write



inline
int ColumnWriter::init2append(const string &dir, SchemaTree* tree, SchemaPath &path)
{
    if (init(dir, tree, path) < 0)
    {
        printf("ColumnWriter: init failed!\n");
        return -1;
    } // if 
    
    // CABAppender is child of CABWriter 
    uint64_t rcap = g_config.m_cab_recd_num;
    CABAppender *appender = new CABAppender();
    if  (appender->init2append(m_file_name, m_tree, m_leaf_path, rcap)  < 0)
    {
        printf("ColumnWriter: init 2 append failed!\n");
        return -1;
    } // if 
    m_cab_op = appender;
    
    return 0;
} // init2append



inline
int ColumnWriter::init(const string &dir, SchemaTree* tree, SchemaPath &path)
{
    m_tree = tree;
    m_leaf_path = path;

    if (StoragePath::getDataPath(dir, tree, m_leaf_path, m_file_name) < 0)
    {
        printf("ColumnWriter: init failed!\n");
        return -1;
    } // if 
  
    return 0;
} // init



inline
void ColumnWriter::copyIns(ColumnWriter &n)
{
    m_tree      = n.m_tree     ;
    m_leaf_path = n.m_leaf_path;      
    m_file_name = n.m_file_name;      

    m_cab_op    = n.m_cab_op   ; n.m_cab_op  = nullptr;     
} // copyIns 



inline
void ColumnWriter::deleteIns(void)
{
    m_tree     = nullptr;
    m_leaf_path.clear();
    m_file_name.clear();
    if (m_cab_op != nullptr)
    {
        delete m_cab_op;
        m_cab_op  = nullptr;
    } // if 
} // deleteIns



inline
void ColumnWriter::output2debug(void)
{   
    printf("ColumnFile info:\n");
    printf("Bin file path:[%s]\n", m_file_name.c_str());
    printf("SchemaTree@[%p]\n", m_tree);
    
    printf("Path SchemaSignature:\n\t");
    uint32_t pdep = m_leaf_path.size();
    for (uint32_t i = 0; i < pdep; ++i)
    {   printf("[%u]%s", m_leaf_path[i], i + 1 < pdep ? "->" : "");   } 
    puts("");
} // output2debug



} // namespace steed
