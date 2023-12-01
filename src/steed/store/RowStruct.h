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
 * @file RowStruct.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   Binary row struct layout definition 
 */

#pragma once

#include <vector>

#include "Row.h"
#include "RowInfo.h"

namespace steed {
namespace Row {

using std::vector;


////////// Binary Layout //////////////////////////////////////////////
//                                                                   //
//    ------------------------------------------------------         //
//    | Size | Binary Content | Offset Array | Member Info |         //
//    ------------------------------------------------------         //
//                                                                   //
//      Size:           :  binary struct  length                     //
//      Binary Content  :  binary content                            //
//      Offset Array    :  element begin offset                      // 
//      Member Info     :  Member elements info                      //
//                                                                   //
///////////////////////////////////////////////////////////////////////

/**
 * @brief Binary row struct layout
 */
class RowStructBase {
protected:
    char    *m_rbgn{nullptr};  /**< row bin cont begin */
    Size    *m_size{nullptr};  /**< row bin cont size  */
    char    *m_vals{nullptr};  /**< bin value array begin */
    Info    *m_info{nullptr};  /**< row info begin */

public: // ctor & dtor
    RowStructBase         (void) = default;
    virtual ~RowStructBase(void) = default;


public: // init 
    virtual void uninit    (void)      = 0;
    virtual void init2read (char *bgn) = 0;
    void         init2write(char *bgn) 
    { m_rbgn = bgn; m_size = (Size*)bgn; m_vals = bgn + sizeof(Size); }


public: // read 
    uint32_t getTotalSize(void) { return *m_size     ; } 
    uint32_t getHeadSize (void) { return sizeof(Size); } 
    uint32_t getInfoSize (void) { return sizeof(Info); } 
    uint32_t getElemNum  (void) { return m_info->getNumber         (); }
    uint32_t getOffSize  (void) { return m_info->getOffsetArrayUsed(); }
    uint32_t getValSize  (void)
    { return getTotalSize() - getHeadSize() - getInfoSize() - getOffSize(); } 

    virtual uint32_t    getOffset (uint32_t i) = 0;
    virtual uint32_t    getBinSize(uint32_t i) = 0;
    virtual const char *getBinVal (uint32_t i) = 0;
    virtual const char *getOffsetBegin  (void) = 0;


public: // write
    /** set row binary struct size */
    void setTotalSize(uint32_t s) { *m_size =  s; } 

    /**
     * package struct with  offset array and Info 
     * @param flag    offset length flag, which is already got 
     * @param ov      elements' offset array in vector
     * @param bin     binary position begin to append (offset array begin)
     * @return append bytes number to package (offset array length + info length)
     */
    virtual uint32_t package(uint32_t flag, vector<uint32_t> &ov, char* bin) = 0;

    /**
     * get total used size
     * @param mnum    member number in array
     * @param vlen    total binary value length
     * @return total used size
     */
    virtual uint32_t getTotalSize(uint32_t mnum, uint32_t vlen) = 0;

public: // debug 
    void output2debug(void);
}; // RowStructBase





/**
 * @brief Binary row struct layout
 * @tparam OT  offset type
 */
template <class OT>
class RowStruct : public RowStructBase {
protected:
    OT    *m_offs{nullptr}; /**< var-length offset array */

public:
    RowStruct (void) = default ; 
    ~RowStruct(void) { uninit(); }

public: // init 
    void uninit    (void)      override;
    void init2read (char *bgn) override;

public: // read
    uint32_t    getOffset (uint32_t i) override  { return m_offs[i]; }
    uint32_t    getBinSize(uint32_t i) override;
    const char *getBinVal (uint32_t i) override  { return m_vals +  m_offs[i]; } 
    const char *getOffsetBegin  (void) override  { return (const char*)m_offs; }

public: // write  
    uint32_t package(uint32_t flag, vector<uint32_t> &ov, char* bin) override;

    uint32_t getTotalSize (uint32_t mnum, uint32_t vlen) override 
    { return sizeof(Size) + vlen + sizeof(OT) * mnum + sizeof(Info); } 
}; // RowStruct 



} // namespace Row
} // namespace steed

#include "RowStruct_inline.h"