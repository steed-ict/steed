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

#include <string>
#include <string.h>

#include "Utility.h"
#include "FileHandler.h"
TEST(steedUtilTest, testFileHandler) {
    std::string dname("/tmp/steed/");
    steed::Utility::makeDir(dname, 0777);

    const char *fname = "/tmp/steed/test.txt";
    const char *fcont = "hello";
    size_t fsize = strlen(fcont);
    size_t bgnoff = 16;
    int wt_flag =  O_RDWR | O_CREAT | O_TRUNC;
    int mt = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    steed::FileHandlerViaOS fileHandler;
    EXPECT_GT (fileHandler.open(fname, wt_flag, mt), 2); // 0, 1, 2 are reserved for stdin, stdout, stderr
    EXPECT_EQ (fileHandler.write(bgnoff, fcont, fsize), fsize);
    EXPECT_EQ (fileHandler.close(), 0);
    EXPECT_EQ (bgnoff + fsize, fileHandler.getFileSize(fname));
    EXPECT_EQ (bgnoff + fsize, fileHandler.getFileSize(std::string(fname)));
    
    char fbuff[1024] = {0};
    int rd_flag = O_RDONLY;
    EXPECT_GE (fileHandler.open (fname, rd_flag, mt), 0);
    EXPECT_EQ (fileHandler.read (bgnoff, fbuff, fsize), fsize);
    EXPECT_EQ (fileHandler.close(), 0);
    EXPECT_STREQ (fcont, fbuff);

    EXPECT_EQ (fileHandler.remove(fname), 0);
} // testFileHandler


#include "FileIO.h"
TEST(steedUtilTest, testFileIO) {
    const char *fname = "/tmp/steed/test.txt";
    const char *fcont1 = "hello";
    const char *fcont2 = "world";
    size_t fsize1 = strlen(fcont1);
    size_t fsize2 = strlen(fcont2);
    steed::FileIOViaOS fileIO;

    EXPECT_EQ (fileIO.init2write(fname), 0); 
    EXPECT_EQ (fileIO.writeContent(fsize1, fcont1), fsize1);
    fileIO.uninit();

    char fbuff[1024] = {0};
    EXPECT_EQ (fileIO.init2read(fname), 0);
    EXPECT_EQ (fileIO.readContent(fsize1, fbuff), fsize1); 
    EXPECT_STREQ (fcont1, fbuff);
    fileIO.uninit();

    EXPECT_EQ (fileIO.init2modify(fname), 0); 
    EXPECT_EQ (fileIO.writeContent(fsize2, fcont2), fsize2);
    fileIO.uninit();

    EXPECT_EQ (fileIO.init2read(fname), 0);
    EXPECT_EQ (fileIO.readContent(fsize2, fbuff), fsize2); 
    EXPECT_STREQ (fcont2, fbuff);
    fileIO.uninit();
} // testFileIO


#include "Buffer.h"
namespace steed {
// define global config
steed::Config g_config;
} // namespace steed

TEST(steedUtilTest, testBuffer) {
    const char *fn= "/tmp/steed/test.txt";
    steed::Utility::removeFile (fn); 
    int char_size = 5;
    std::string str{"Test Content: hello world"}; 
    int strlen = str.size() + 1; // include '\0'
    const char *strcont = str.c_str();

    // test Buffer to write
    {
        steed::Buffer buf(10);
        EXPECT_EQ (buf.init2write(fn), 0 );
        EXPECT_EQ (buf.valid(), true);

        void * got = buf.allocate(char_size, false);
        EXPECT_NE (got, nullptr);
        for (int i = 0; i < char_size; ++i)
        { ((char *)got)[i] = 'A' + i; }

        EXPECT_EQ (buf.data(), got);
        EXPECT_EQ (buf.used(), char_size);
        EXPECT_EQ (buf.append(strcont, strlen), 0);
        EXPECT_EQ (buf.used(), char_size + strlen);
        EXPECT_EQ (buf.flush2File(), char_size + strlen);
    }

    // test Buffer to read
    {
        size_t fsize = steed::Utility::getFileSize(fn);
        EXPECT_EQ (fsize, char_size + strlen);

        steed::Buffer buf(10);
        EXPECT_EQ (buf.init2read(fn), 0 );
        EXPECT_EQ (buf.valid(), true);
        bool resize = false;
        EXPECT_EQ (buf.load2Buffer(fsize, resize), fsize); 
        EXPECT_EQ (buf.used(), char_size + strlen);

        char * got = (char *)buf.data();
        for (int i = 0; i < char_size; ++i)
        { EXPECT_EQ (got[i], 'A' + i); }
        got = (char *)buf.getPosition(char_size);
        EXPECT_EQ (strncmp(got, strcont, strlen), 0);

        EXPECT_EQ (buf.getPosition(fsize), nullptr);
    }
} // testBuffer


#include "Utility.h"
TEST(steedUtilTest, testUtilityBits) {
    uint64_t word_len = sizeof(uint16_t) * 8;

    EXPECT_EQ (steed::Utility::calcUsedBitNum(0), 0);
    for (uint64_t i = 1; i < word_len; ++i)
    {
        uint64_t chk = 1 << (i - 1);
        uint64_t max = 1 << i; 
        while (chk < max)
        {
            EXPECT_EQ (steed::Utility::calcUsedBitNum(chk), i);
            ++chk;   
        } // while
    } // for

    EXPECT_EQ (steed::Utility::calcBytesUsed(0), 0);
    for (uint32_t i = 1; i < word_len; ++i)
    {
        uint64_t chk = 1 << (i - 1);
        uint64_t max = 1 << i;  // total move 64 bits
        while (chk < max)
        {
            uint64_t bnum = (chk / 8) + (chk % 8 == 0 ? 0 : 1);
            EXPECT_EQ (steed::Utility::calcBytesUsed(chk), bnum);
            ++chk;   
        } // while
    } // for

    uint32_t base = 4096;

    // test steed::calcAlignSize
    EXPECT_EQ (steed::Utility::calcAlignSize(0, base), 0);
    EXPECT_EQ (steed::Utility::calcAlignSize(base, base), base);
    for (uint32_t i = 1; i < base; ++i)
    {
        uint64_t aligned = steed::Utility::calcAlignSize(i, base);
        uint32_t mod = aligned / base;
        EXPECT_EQ (aligned % base, 0);
        EXPECT_GE (aligned, mod * base);
        EXPECT_LE (aligned, (mod + 1) * base);
    } // for

    // test steed::calcSize2Align
    EXPECT_EQ (steed::Utility::calcSize2Align(0,    base), 0);
    EXPECT_EQ (steed::Utility::calcSize2Align(base, base), 0);
    for (uint32_t i = 1; i < base; ++i)
    {
        uint64_t s = steed::Utility::calcSize2Align(i, base);
        EXPECT_EQ (i + s, base);
    } // for

    // test steed::calcAlignBegin
    EXPECT_EQ (steed::Utility::calcAlignBegin(0, base), 0);
    EXPECT_EQ (steed::Utility::calcAlignBegin(base, base), base);
    for (uint32_t i = 1; i < base; ++i)
    {
        uint64_t bgn = steed::Utility::calcAlignBegin(i, base);
        EXPECT_EQ (bgn, 0);
    } // for
} // testUtility


TEST(steedUtilTest, testUtilityFile) {
    std::string fn{"/tmp/steed/hello.txt"};
    EXPECT_EQ (steed::Utility::checkFileExisted(fn), false); 
    if (system ("echo \"hello\" > /tmp/steed/hello.txt") < 0)
    {   printf("system call failed!\n");   }
    EXPECT_EQ (steed::Utility::checkFileExisted(fn), true); 

    EXPECT_EQ (steed::Utility::getFileSize(fn), 6); // "hello\n"
    EXPECT_EQ (steed::Utility::removeFile (fn), 0); 
    EXPECT_EQ (steed::Utility::checkFileExisted(fn), false); 

    std::string dn{"/tmp/steed/test/"};
    EXPECT_EQ (steed::Utility::checkFileExisted(dn), false); 
    EXPECT_EQ (steed::Utility::makeDir(dn, 0777), 0);
    EXPECT_EQ (steed::Utility::checkFileExisted(dn), true); 
    EXPECT_EQ (steed::Utility::removeDir(dn), 0);
    EXPECT_EQ (steed::Utility::checkFileExisted(dn), false); 
} // testUtility


#include "BitVector.h"
TEST(steedUtilTest, testBitVector) {
    const uint32_t buflen = 4096;
    char bufcont[buflen] = {0};

    uint32_t max_mask_len = 32;
    for (uint32_t l = 1; l < max_mask_len; ++l) {
        uint32_t elem_cap = buflen * 8 / l;

        // steed::BitVector append 
        {
            steed::BitVector bv(l);
            EXPECT_EQ (bv.init2write(buflen, bufcont), 0);

            EXPECT_EQ (bv.getContent(), bufcont);
            EXPECT_EQ (bv.getContCap(), buflen);
            EXPECT_EQ (bv.getMaxValue(), (uint64_t(1) << l) - 1);
            EXPECT_EQ (bv.getElementUsed(), 0);
            EXPECT_EQ (bv.getElementCap(), elem_cap);

            uint32_t mask = bv.getMask();
            for (uint32_t i = 0; i < bv.getElementCap(); ++i)
            {
                EXPECT_EQ (bv.getElementUsed(), i);
                EXPECT_EQ (bv.append(i % mask), 0);
                EXPECT_EQ (bv.getElementUsed(), i+1);
            } // for
        } 

        // steed::BitVector get 
        {
            steed::BitVector bv(l);
            EXPECT_EQ (bv.init2read(elem_cap, buflen, bufcont), 0);

            EXPECT_EQ (bv.getContent(), bufcont);
            EXPECT_EQ (bv.getContCap(), buflen);
            EXPECT_EQ (bv.getMaxValue(), (uint64_t(1) << l) - 1);
            EXPECT_EQ (bv.getElementUsed(), elem_cap);
            EXPECT_EQ (bv.getElementCap(), elem_cap);

            uint32_t mask = bv.getMask();
            for (uint32_t i = 0; i < bv.getElementCap(); ++i)
            {
                uint64_t got = bv.get(i);
                EXPECT_EQ (got, i % mask);
            } // for
        }

        // steed::BitVector set & get
        {
            steed::BitVector bv(l);
            EXPECT_EQ (bv.init2read(elem_cap, buflen, bufcont), 0);

            EXPECT_EQ (bv.getContent(), bufcont);
            EXPECT_EQ (bv.getContCap(), buflen);
            EXPECT_EQ (bv.getMaxValue(), (uint64_t(1) << l) - 1);
            EXPECT_EQ (bv.getElementUsed(), elem_cap);
            EXPECT_EQ (bv.getElementCap(), elem_cap);

            uint32_t mask = bv.getMask();
            for (uint32_t i = 0; i < bv.getElementCap(); ++i)
            { EXPECT_EQ (bv.set(i, i+10 % mask), 0); }

            for (uint32_t i = 0; i < bv.getElementCap(); ++i)
            { EXPECT_EQ (bv.get(i), (i+10 % mask) & mask); } 
        }
    } // for
} // testBitVector


#include "RandomValues.h"
#include "BoolVector.h"
TEST(steedUtilTest, testBoolVector) {
    const uint32_t buflen = 4096;
    char  bufcont[buflen] = {0};
    steed::BoolVector bv;
    bv.init(buflen * 8, buflen, bufcont);
    bv.clearAll();


    uint64_t bitnum = buflen * 8;
    uint64_t setmin = 0;
    uint64_t setmax = buflen * 8 - 1;
    steed::RandomValues rvals(buflen * 3, setmin, setmax);
    for (auto &v : rvals.values()) {
        bv.set(v);
    } // for
    EXPECT_EQ (bv.getPopCount(), rvals.values().size());

    uint64_t setidx = 0;
    for (auto &v : rvals.values()) {
        uint64_t expect = v; 
        uint64_t got = bv.getNextSetBit(setidx);
        EXPECT_EQ (expect, got);
        setidx = got + 1; // update search start index
    } // for

    auto cbgn = rvals.values().cbegin();
    auto cend = rvals.values().cend();
    for (uint64_t i = 0; i < bitnum; ++i) {
        uint64_t nextset = (cbgn != cend ? *cbgn : bitnum);
        bool val = (i == nextset);
        EXPECT_EQ (bv.get(i), val);
        EXPECT_EQ (bv.calcBitAnd(i, true), val);
        EXPECT_EQ (bv.calcBitOr (i, false), val);

        cbgn += val;
    } // for

    uint64_t minrand = *(rvals.values().begin());
    uint64_t maxrand = *(rvals.values().end());
    bv.clearAll();
    bv.setBitByRange(minrand, maxrand);
    for (uint64_t i = 0; i < bitnum; ++i) {
        bool val = (i >= minrand && i < maxrand);
        EXPECT_EQ (bv.get(i), val);
        EXPECT_EQ (bv.calcBitAnd(i, true), val);
        EXPECT_EQ (bv.calcBitOr (i, false), val);
    } // for

    bv.setAll();
    bv.clearBitByRange(minrand, maxrand);
    for (uint64_t i = 0; i < bitnum; ++i) {
        bool val = (i < minrand || i >= maxrand);
        EXPECT_EQ (bv.get(i), val);
        EXPECT_EQ (bv.calcBitAnd(i, true), val);
        EXPECT_EQ (bv.calcBitOr (i, false), val);
    } // for

} // testBoolVector


#include "BitMap.h"
TEST(steedUtilTest, testBitMap) {
    const uint32_t size = 64;
    const uint32_t itemnum = 8 * size;
    steed::BitMap  bm(size);
    bm.init(itemnum);
    EXPECT_EQ (bm.getSetBitNum(), 0);

    bm.setAll();
    EXPECT_EQ (bm.getSetBitNum(), itemnum);
} // testBoolVector


#include "Container.h"
class CElem {
public:
    CElem(void) = default;
    void copyIns(CElem &c) { *this = c; }
    void deleteIns(void) {}
}; // CElem

TEST(steedUtilTest, testContainer) {
    steed::Container <CElem> c; 

    EXPECT_EQ (c.size(), 0);
} // testContainer



#if 0
#include "DebugInfo.h"
TEST(steedUtilTest, testLog) {
// Only print debug info:
//   no need to test 
    steed::DebugInfo::printStack();
} // testLog
#endif 


#include "Allocator.h"
TEST(steedUtilTest, testAllocator) {
    void *ptr1 = steed::steedMalloc(1024);
    ASSERT_TRUE(ptr1 != nullptr);

    size_t align = 16; 
    void *ptr2 = steed::steedMemalign(align, 1024);
    ASSERT_TRUE (ptr2 != nullptr);

    steed::steedFree(ptr1); 
    steed::steedFree(ptr2); 
    ptr1 = nullptr, ptr2 = nullptr;
} // testAllocator


#include "SymbolMap.h"
TEST(steedUtilTest, testSymbolMap) {
    steed::SymbolMap<uint32_t>  sm(64);
    uint32_t i = 0;
    sm.insert("a", i = 1);
    sm.insert("b", i = 2);
    sm.insert("c", i = 3);

    EXPECT_EQ(sm.find("a")->second, 1);
    EXPECT_EQ(sm.find("b")->second, 2);
    EXPECT_EQ(sm.find("c")->second, 3);
    EXPECT_EQ(sm.find("d"), sm.cend());

    sm.erase("b");
    sm.erase("d");
    EXPECT_EQ(sm["a"]->second, 1);
    EXPECT_EQ(sm["c"]->second, 3);
    EXPECT_EQ(sm["b"], sm.cend());
    EXPECT_EQ(sm["d"], sm.cend());
//    sm.show();

    EXPECT_EQ(sm.count("a"), 1);
    EXPECT_EQ(sm.count("c"), 1);
    EXPECT_EQ(sm.count("b"), 0);
    EXPECT_EQ(sm.count("d"), 0);

    sm.clear();
    EXPECT_EQ(sm.empty(), true);
//    sm.show();
} // testSymbolMap

