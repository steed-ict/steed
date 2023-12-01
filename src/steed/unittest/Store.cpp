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


////// Below is the test for steed row

#include "RowInfo.h"
TEST(steedStoreTest, testRow)
{
    using namespace steed::Row;
    EXPECT_EQ(sizeof(ID  ), sizeof(uint16_t));
    EXPECT_EQ(sizeof(Size), sizeof(uint32_t));
    EXPECT_EQ(sizeof(Info), sizeof(uint32_t));

    EXPECT_EQ(Info::calcFlag(0        ),  int(Info::s_flag_8));
    EXPECT_EQ(Info::calcFlag(UINT8_MAX),  int(Info::s_flag_8));
    EXPECT_EQ(Info::calcFlag(UINT8_MAX + 1), int(Info::s_flag_16));
    EXPECT_EQ(Info::calcFlag(UINT16_MAX   ), int(Info::s_flag_16));
    EXPECT_EQ(Info::calcFlag(UINT16_MAX + 1), int(Info::s_flag_32));
    EXPECT_EQ(Info::calcFlag(UINT32_MAX - 1), int(Info::s_flag_32)); 
    EXPECT_EQ(Info::calcFlag(UINT32_MAX    ), int(Info::s_invalid));
} // testConfig                           



#include "RowStructOperator.h"
TEST(steedStoreTest, testRowStructOperator)
{
    using namespace steed::Row;
    RowStructOperator op;
    EXPECT_EQ(op.isInited(), false); 
} // testRowStructOperator


#include "RowObjectOperator.h"
#include "RowArrayOperator.h"
#include "RowStructBuilder.h"
TEST(steedStoreTest, testRowStructBuilder)
{
    using namespace steed::Row;
    RowObjectBuilder ob((steed::Buffer*)nullptr); 
    EXPECT_EQ(ob.isInited(), false); 

    RowArrayBuilder ab((steed::Buffer*)nullptr); 
    EXPECT_EQ(ab.isInited(), false); 
} // testRowStructBuilder


#include "RecordBuilder.h"
TEST(steedStoreTest, testRecordBuilder)
{
    using namespace steed::Row;
    uint64_t max_def = 1;
    RecordBuilder rb(nullptr, nullptr, max_def); 
    EXPECT_EQ(rb.getMaxLevel(), max_def-1);  // exclude root level
} // testRecordBuilder









////// Below is the test for steed column 
namespace steed {
// define global config
steed::Config g_config;
} // namespace s

#include "CABMeta.h"
TEST(steedStoreTest, testCABMeta)
{
    using namespace steed;
    CABMeta meta;
    EXPECT_EQ(meta.m_buf, nullptr);
    EXPECT_EQ(meta.m_dt , nullptr);
    EXPECT_EQ(meta.m_bva, nullptr);
    EXPECT_EQ(meta.m_recd_cap, 0);
    EXPECT_EQ(meta.m_max_rep, 0);
    EXPECT_EQ(meta.m_max_def, 0);
} // testCABMeta

#include "CABInfo.h"
TEST(steedStoreTest, testCABInfo)
{
    using namespace steed;
    CABInfo info;
    info.output2debug();

    CABInfoBuffer info_buf; 
    info_buf.output2debug();
}

#include "CABItemInfo.h"
TEST(steedStoreTest, testCABItemInfo)
{
    using namespace steed;
    CABItemInfo cii;
    EXPECT_EQ(cii.m_bgn_recd, 0);
    EXPECT_EQ(cii.m_recd_num, 0);
    EXPECT_EQ(cii.m_item_num, 0);
    EXPECT_EQ(cii.m_null_num, 0);
    EXPECT_EQ(cii.m_triv_num, 0);
    EXPECT_EQ(cii.getType(), CABItemInfo::trivial);
} // testCABItemInfo

#include "BinaryValueArray.h"
TEST(steedStoreTest, testBinaryValueArray)
{
    using namespace steed;
    Buffer buf;
    FixLengthValueArray fva(&buf, DataType::s_type_ins[DataType::s_type_invalid]);
    EXPECT_EQ (fva.getDataType(), DataType::s_type_ins[DataType::s_type_invalid]);

    VarLengthValueArray vva(&buf, DataType::s_type_ins[DataType::s_type_invalid]);
    EXPECT_EQ (vva.getDataType(), DataType::s_type_ins[DataType::s_type_invalid]);
} // testBinaryValueArray

#include "ColumnItem.h"
#include "ColumnItemArray.h"
TEST(steedStoreTest, testColumnItemArray)
{
    using namespace steed;
    ColumnItemArray cia(0, nullptr, nullptr, nullptr);
    EXPECT_EQ (cia.getType(), CABItemInfo::crucial);
    EXPECT_EQ (cia.getItemNumber(), 0);
} // testColumnItemArray

#include "CAB.h"
#include "CABInfo.h"
TEST(steedStoreTest, testCAB)
{
    using namespace steed;
//    CABMeta meta;
//    CABInfo info;
//    CAB cab(&meta, &info);
//    EXPECT_EQ(cab.valid(), true);


    // cab info buffer
    CABInfo *info = nullptr;
    CABInfoBuffer *info_buf = nullptr;
    uint64_t rbgn = 0, info_num = 0;
    string path("/tmp/steed_store_test_cab_info_buffer");
    {
        // check write full
        info_buf = new CABInfoBuffer();
        info_buf->init2write(path.c_str(), rbgn);

        info = info_buf->getNextInfo2Write(); // empty cab
        info->m_file_off = 4096 * info_num;
        ++info_num;

        info = info_buf->getNextInfo2Write(); // tail full cab
        info->m_item_info.m_recd_num = g_config.m_cab_recd_num;
        info->m_file_off = 4096 * info_num;
        ++info_num;

        delete info_buf; info_buf = nullptr; 
    }

    {
        // check append to full cab
        info_buf = new CABInfoBuffer();
        info_buf->init2append(path.c_str());
        info = info_buf->getTailInfo2Append();
        EXPECT_EQ(info_buf->getNextIndex(), info_num);
        EXPECT_EQ(info_buf->getNextIndex(), info_buf->getUsedNumber()); 
        EXPECT_EQ(info->m_item_info.m_recd_num, g_config.m_cab_recd_num); // full 

        info = info_buf->getNextInfo2Write(); // 2ed cab is full 
        info->m_item_info.m_recd_num = 1;
        info->m_file_off = 4096 * info_num;
        ++info_num;

        delete info_buf; info_buf = nullptr;
    }

    {
        // check append to empty cab
        info_buf = new CABInfoBuffer();
        info_buf->init2append(path.c_str());
        info = info_buf->getTailInfo2Append();
        EXPECT_EQ(info_buf->getNextIndex(), info_num);
        EXPECT_EQ(info_buf->getNextIndex(), info_buf->getUsedNumber());
        EXPECT_EQ(info->m_item_info.m_recd_num, 1); // empty

        delete info_buf; info_buf = nullptr;
    }

} // testCAB


#include "CABLayouter.h"
TEST(steedStoreTest, testCABLayouter)
{
    using namespace steed;
    CABLayouter layouter(nullptr, Compressor::none);
    layouter.output2debug();
} // testCABLayouter/**


#include "ColumnWriter.h"
TEST(steedStoreTest, testColumnWriter)
{
    using namespace steed;
    ColumnWriter writer;
    writer.output2debug();
} // testColumnWriter

#include "ColumnReader.h"
TEST(steedStoreTest, testColumnReader)
{
    using namespace steed;
    ColumnReader reader;
    EXPECT_EQ(reader.getSchemaTree(), nullptr);
} // testColumnReader

