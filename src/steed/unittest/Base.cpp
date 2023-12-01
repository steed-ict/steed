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

#include "DataType.h"
TEST(steedBaseTest, testDataTypeString)
{
    int dt_id = steed::DataType::s_type_string;
    steed::DataType *dt = steed::DataType::getDataType(dt_id);
    const char *input = "\"hello world!\""; 
    const int bufcap = 4096;
    char bufin [bufcap] = {0}; 
    char bufout[bufcap] = {0}; 

    EXPECT_EQ(dt->getBinSizeByTxt(input), 13);
    EXPECT_EQ(dt->transTxt2Bin(input, bufin, bufcap), 13);
    EXPECT_EQ(dt->compareEqual(bufin, "hello world!"), 1);
    EXPECT_EQ(dt->transBin2Txt(bufin, bufout, bufcap), 15);
    EXPECT_EQ(dt->compareEqual(input, bufout), 1);
} // testConfig


#include "BinaryValueArray.h"
namespace steed {
// define global config
steed::Config g_config;
} // namespace steed

TEST(steedBaseTest, testBinaryValueArray)
{
    using namespace steed; 
    DataType *dt = DataType::getDataType(DataType::s_type_string);
    Buffer *buf = new Buffer(4096);
    BinaryValueArray *bva = new VarLengthValueArray(buf, dt);
    EXPECT_EQ(bva->inited(), false);
    delete bva; bva = nullptr;
    delete buf; buf = nullptr;
} // testBinaryValueArray


#include "CompressorFactory.h"
TEST(steedBaseTest, testCompressor)
{
    using namespace steed;
    const uint32_t bufcap = 4096;
    char orgbuf[bufcap] = {0}, cmpbuf[bufcap] = {0}, decbuf[bufcap] = {0}; 
    for (uint32_t i = 0; i < bufcap; ++i)
        orgbuf[i] = i % 256;

    for (Compressor::Type type = Compressor::none; 
            type <= Compressor::lz4; 
            type = Compressor::Type(type + 1))
    {
        memset(cmpbuf, 0, bufcap);
        memset(decbuf, 0, bufcap);

        Compressor *cmp = CompressorFactory::create(Compressor::none);
        EXPECT_EQ(cmp->type(), Compressor::none);
        uint64_t cmpsize = 0, decsize = 0; 
        EXPECT_EQ(cmp->compress  (orgbuf, bufcap, cmpbuf, cmpsize), bufcap);
        EXPECT_EQ(cmpsize, bufcap);
        EXPECT_EQ(cmp->decompress(cmpbuf, cmpsize, decbuf, decsize), bufcap);
        EXPECT_EQ(decsize, bufcap);
        delete cmp; cmp = nullptr;

        EXPECT_EQ(memcmp(orgbuf, decbuf, bufcap), 0);
    } // for 

} // testCompressor


#include "RepetitionType.h"
TEST(steedBaseTest, testRepetitionType)
{
    using namespace steed;
    for (RepetitionType::Type type = RepetitionType::none; 
            type < RepetitionType::max; 
            type = RepetitionType::Type(type + 1))
    {
        RepetitionType *rt = RepetitionType::create(type);
        EXPECT_EQ(rt->type(), type);

        uint32_t val = 0;
        EXPECT_EQ(val, rt->decode( rt->encode(val) ));
        delete rt; rt = nullptr;
    } // for
} // testRepetitionType