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

#include "Config.h"
////// Below is the test for steed column 
namespace steed {
// define global config
steed::Config g_config;
} // namespace

std::string json_file("../../examples/c1.json");


#if 0
#include <fstream>
#include "JSONRecordBuffer.h"
#include "JSONRecordNaiveParser.h"
#include "JSONBinTree.h"
TEST(steedParseTest, JSONReader)
{
    using namespace steed;
    std::string jpath(json_file);
    std::ifstream ifs(jpath);
    if (!ifs.is_open())
    {
        printf("ERROR: cannot open [%s]!\n", jpath.c_str());
        return;
    }

    JSONRecordBuffer *jbuf = new JSONRecordBuffer (&ifs);
    JSONRecordParser *jpars= new JSONRecordNaiveParser();
    JSONBinTree      *jtree= new JSONBinTree();
    do
    {
        char    *recd_bgn = nullptr;
        uint64_t recd_len = 0;
        int s =  jbuf->nextRecord(recd_bgn, recd_len);
        if (s <= 0)
        {
            printf("Main: nextRecord got [%d]\n", s);
            break;
        }

#if 1
        static uint64_t recd_cnt = 0;
        printf("\n[%lu]", recd_cnt++);
        printf("\n----------------------------------------\n");
        printf("%s", recd_bgn);
        printf("\n----------------------------------------\n");
#endif 
        s = jpars->parse(jtree, recd_bgn);
        if (s < 0)
        {
            printf("Main: parse got [%d]\n", s);
            break;
        }

#if 1
        jtree->output2debug();
        printf("\n----------------------------------------\n");
#endif

        jtree->clear();
    } while (true);

    ifs.close ();
    delete jbuf ; jbuf = nullptr;
    delete jpars; jpars= nullptr;
    delete jtree; jtree= nullptr;
} // testJSONReader



#include "SchemaTree.h"
#include "CollectionWriter.h"
TEST(steedParseTest, CollectionWriter)
{
    using namespace steed;
    std::string db  = "demo";
    std::string clt = "testParseCollectWriter";
    SchemaTree *t = new SchemaTree(db, clt);
    {
        // add int child
        const char* key = "key";
        uint32_t pidx = 0;
        uint32_t dt_id= DataType::s_type_int_64;
        uint8_t  vcat = SchemaNode::s_vcat_single;
        int s = t->addNode(key, pidx, dt_id, vcat);
        if (s < 0)
        { printf("Main: addNode failed!\n"); return; }

        // add string child
        dt_id= DataType::s_type_string;
        vcat = SchemaNode::s_vcat_multi;
        s = t->addNode(key, pidx, dt_id, vcat);
        if (s < 0)
        { printf("Main: addNode failed!\n"); return; }
    }

    CollectionWriter *cw = new CollectionWriter(t);
    if (cw->init2write() < 0)
    {
        puts("Main: CollectionWriter failed!\n");
        return;
    } // if

    {
        uint64_t tnum = 8;
        const char *txts[] = { "1", "2", "3", "4", "5", "6", "7", "8" };

        for (uint64_t ti = 0; ti < tnum; ++ti)
        { cw->write(SchemaSignature(1), 0, 1, txts[ti]); }
    }

    {
        uint64_t    tnum = 8;
        const char* txts[] = {
            "\"a\"", "\"b\"", "\"c\"", "\"d\"", "\"e\"", "\"f\"", "\"g\"", "\"h\""};

        for (uint64_t ti = 0; ti < tnum; ++ti)
        { cw->write(SchemaSignature(2), ti%2, 1, txts[ti]); }
    }

    delete cw; cw = nullptr;
    delete t ; t  = nullptr;

    DataType::s_init = DataType::uninitStatic();
} // CollectionWriter
#endif 



#include "ColumnParser.h"
TEST(steedParseTest, ColumnParser)
{
    using namespace steed;

    std::string jpath(json_file);
    std::ifstream ifs(jpath);
    if (!ifs.is_open())
    {
        printf("ERROR: cannot open [%s]!\n", jpath.c_str());
        return;
    }

    std::string db ("demo");
    std::string clt("testParseColumnParser");
    steed::ColumnParser *cp = new steed::ColumnParser();
    EXPECT_EQ(cp->init (db, clt, &ifs), 0);
    EXPECT_GT(cp->parseAll(), 0);

    delete cp; cp = nullptr;
    DataType::s_init = DataType::uninitStatic();
} // ColumnParser