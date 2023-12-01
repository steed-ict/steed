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



#include "gtest/gtest.h"

#include "SchemaNode.h"
TEST(steedSchemaTest, testSchemaBase)
{
    using namespace steed;
    SchemaNode n;
    EXPECT_EQ(n.getDataType(), nullptr); // not init yet
} // testSchemaBase



#include "Config.h"
#include "SchemaTree.h"
#include "SchemaTreePrinter.h"
namespace steed {
Config g_config;
} // namespace steed

TEST(steedSchemaTest, testSchemaTree)
{
    using namespace steed;
    using namespace steed::Row;
    std::string db("debug"), col("clt"); 
    SchemaTree *t  = new SchemaTree(db, col);

    const char* keystr= "\"key\""; 
    const char* keynm = "key"; 

    // level 1 test  
    {
        uint32_t pidx = 0; 
        int dts[] = {
            DataType::s_type_int_64,     
            DataType::s_type_string,
            DataType::s_type_invalid }; 
        uint32_t vcats[] = {
            SchemaNode::s_vcat_single,
            SchemaNode::s_vcat_multi,
            SchemaNode::s_vcat_single };

        uint32_t nnum = sizeof(dts)/sizeof(dts[0]);
        for (uint32_t i = 0; i < nnum; ++i)
        {
            int s = t->addNode(keystr, pidx, dts[i], vcats[i]);
            EXPECT_EQ(s, 0);
        } // for i

        for (uint32_t i = 0; i < nnum; ++i)
        {
            SchemaSignature s = t->findNode(keynm, pidx, dts[i], vcats[i]);
            EXPECT_EQ(s, i+1);
        } // for i
    }

    // level 2 test 
    {
        // parent node index: the last node in level 1
        uint32_t pidx = 3;  
        int dts[] = {
            DataType::s_type_double,   // 3.4
            DataType::s_type_string }; // 3.5 
        uint32_t vcats[] = {
            SchemaNode::s_vcat_multi,
            SchemaNode::s_vcat_multi };

        uint32_t nnum = sizeof(dts)/sizeof(dts[0]);
        for (uint32_t i = 0; i < nnum; ++i)
        {
            int s = t->addNode(keystr, pidx, dts[i], vcats[i]);
            EXPECT_EQ(s, 0);
        } // for i

        for (uint32_t i = 0; i < nnum; ++i)
        {
            SchemaSignature s = t->findNode(keynm, pidx, dts[i], vcats[i]);
            EXPECT_EQ(s, i+1+pidx);
        } // for i
    }

    // test SchemaTreePrinter
    if (1)
    {
        SchemaTreePrinter::output2debug(t);
        SchemaTreePrinter::output2tree (t);
        SchemaTreePrinter::output2dot  (t);
    }

    // test flush and load 
    {
        std::string path;
        const char *base = "/tmp/steed";
        steed::g_config.m_store_base.assign(base);
        Utility::getSchemaPath(g_config, db, string(""), path);
        EXPECT_EQ(Utility::makeDir(path), 0);

        EXPECT_EQ (t->flush(), 0);
        delete t; t = nullptr;

        t = new SchemaTree(db, col);
        EXPECT_EQ (t->load(), 1);
        t->output2debug();

        EXPECT_EQ (Utility::removeDir(steed::g_config.m_store_base), 0);
    }

    delete t;
    t = nullptr;
} // testSchemaTree 
