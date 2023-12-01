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
 * @file BinaryValueArray.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for BinaryValueArray 
 */

#pragma once 

#include <stdint.h>
#include <vector>

#include "Buffer.h"
#include "DataType.h"


namespace steed {

using std::vector;

class BinaryValueArray {
protected:  
    // meta 
    const uint32_t m_align   {0};  /**< memory aligned base */
    DataType      *m_dt{nullptr};  /**< data type instance  */

    // content  
    char        *m_cont_bgn{nullptr};  /**< binary value begin  */
    uint64_t     m_val_cap {0};        /**< CAB value capacity  */
    uint64_t     m_val_num {0};        /**< value number in CAB */

public: 
    virtual ~BinaryValueArray(void) = default; 
    BinaryValueArray(Buffer *b, DataType *dt): m_align(b->getAlignSize()), m_dt(dt) {}

public: 
    DataType   *getDataType    (void) { return m_dt; }
    const char *getContentBegin(void) { return m_cont_bgn; }
    uint64_t    getValueNumber (void) { return m_val_num ; }

    /**
     * get fix part size, fix part is: 
     *     FixLengthValueArray: binary value array 
     *     VarLengthValueArray: offset value array  
     * 
     * @param cap    element capacity in array 
     * @return fixed part used size 
     */
    virtual uint64_t getFixSize (uint64_t cap) = 0;

    bool             inited     (void) { return m_cont_bgn != nullptr; }
    virtual void     uninit     (void) = 0; // uninit members
    virtual int      init2read  (uint64_t len, void* bgn, uint64_t num) = 0;
    virtual int      init2write (uint64_t len, void* bgn) = 0; 
    virtual int64_t  copyContent(BinaryValueArray*   src) = 0;
    virtual int64_t  resizeElemUsed(uint64_t num)         = 0;

    virtual const void *getOffsetBegin     (void) = 0;
    virtual void     setBeginOffset(uint32_t off) = 0;
    virtual uint64_t getOffsetSize         (void) = 0;
    virtual uint64_t getOffsetArrayUsed    (void) = 0;

    virtual uint64_t getValueSize          (void) = 0;
    virtual uint64_t getReadValueArrayUsed (void) = 0;
    virtual uint64_t getWriteValueArrayUsed(void) = 0;

public: // read 
    /**
     * check the bin value is null 
     * @param idx    value index to check is null 
     * @return bin value is null  
     */
    virtual bool isNull(uint64_t idx) = 0;

    /**
     * read bin value by index
     * @param idx    value index to get
     * @return bin value;  nullptr for failed 
     */
    virtual const void *read(uint64_t idx) = 0;

    /**
     * read bin value by index
     * @param idx    value index to get
     * @return >0 got bin value; 0 for failed 
     */
    virtual int read(uint64_t idx, const void* &bin, uint32_t &len) = 0;

public: // write 
    /**
     * write null as next value 
     * @return >0 success; 0 array is full; <0 failed  
     */
    int writeNull(void) 
    {   return (m_val_num == m_val_cap) ? 0 : (++m_val_num, 1);   }

    /**
     * write next text value 
     * @param txt input  text value content
     * @param bin output binary value result
     * @return >0 success; 0 array is full; <0 failed  
     */
    virtual int writeText(const char *txt, const void* &bin) = 0;

    /**
     * write next binary value 
     * @param len    next binary value length 
     * @param bin    next binary value content 
     * @return >0 success; 0 array is full; <0 failed  
     */
    virtual int writeBinVal(uint64_t len, const void *bin) = 0;

//    /**
//     * get the binary value just written
//     * @param len    written binary value length 
//     * @param bin    written binary value content 
//     * @return 1 success; 0 got nothing
//     */
//    virtual int getWrittenBin(const void* &bin, uint32_t &len) = 0;

public: // write related 
    /**
     * append variable length binary value 2 Buffer
     * @param buf    Buffer instance 
     * @return >=0 flushed size as success; <0 failed  
     */
    virtual int64_t appendOffsets(Buffer *buf) = 0;

    /**
     * append variable length binary value 2 Buffer
     * @param buf    Buffer instance 
     * @return >=0 flushed size as success; <0 failed  
     */
    virtual int64_t appendValues (Buffer *buf) = 0;

public:
    /**
     * create a new BinaryValueArray instance
     * @param dt    DataType instance  
     * @param cap   array capacity 
     * @return new instance pointer  
     */
    static BinaryValueArray *create(Buffer *buf, DataType *dt);

public:
    /** output 2 debug */
    virtual void output2debug(void); 
}; // BinaryValueArray





class FixLengthValueArray : public BinaryValueArray {
protected: 
    uint32_t    m_length{0};  /**< fixed binary value length */

public: 
    FixLengthValueArray (Buffer *buf, DataType *dt);
    ~FixLengthValueArray(void) = default;

public: 
    uint64_t    getFixSize (uint64_t cap) override { return cap * m_length; }

    void        uninit     (void) override;
    int         init2read  (uint64_t len, void *bgn, uint64_t num) override;
    int         init2write (uint64_t len, void *bgn) override; 
    int64_t     copyContent(BinaryValueArray   *src) override;
    int64_t     resizeElemUsed        (uint64_t num) override
    { m_val_num = num; return int64_t(m_val_num * m_length); }

    void        setBeginOffset(uint32_t off) override
    { (void)off; assert(m_val_num == 0); }
    const void *getOffsetBegin        (void) override { return nullptr; }
    uint64_t    getOffsetSize         (void) override { return 0; }
    uint64_t    getOffsetArrayUsed    (void) override { return 0; }

    uint64_t    getValueSize          (void) override { return m_length; }
    uint64_t    getReadValueArrayUsed (void) override { return m_length * m_val_num; }
    uint64_t    getWriteValueArrayUsed(void) override { return m_length * m_val_num; }

public:
    bool        isNull(uint64_t idx)    override
    { return m_dt->compareEqual( m_dt->getBinNull(), this->read(idx) ); }

    const void *read  (uint64_t idx)    override
    { return (idx < m_val_num) ? (m_cont_bgn + idx*m_length) : nullptr; }

    int         read  (uint64_t idx, const void* &bin, uint32_t &len) override;

public:
    int     writeText  (const char *txt, const void* &bin) override;
    int     writeBinVal(uint64_t     len, const void *bin) override;
//    int     getWrittenBin(const void* &bin, uint32_t   &len) override;

public:
    /**
     * fixed length binary value array has no offset array
     */
    int64_t appendOffsets(Buffer *buf) override { (void)buf; return 0; }
    int64_t appendValues (Buffer *buf) override;

public:
    void    output2debug(void) override; 
}; // FixLengthValueArray 





class VarLengthValueArray : public BinaryValueArray {
protected: 
    // offset for each variable length binary value 
    uint32_t             *m_offsets{nullptr};  /**< LAYOUT: offset array  */

    // read 
    const char           *m_rd_vbgn{nullptr};  /**< RD: bin value begin   */
    uint64_t              m_rd_vlen      {0};  /**< RD: total values used */

    // write
    uint32_t              m_nxt_buf_idx  {0};  /**< WT: next buffer idx   */
    uint32_t              m_cur_off      {0};  /**< WT: current offset    */
    Buffer               *m_cur_buf{nullptr};  /**< WT: current buffer ins*/
    vector<Buffer*>       m_buf_vec       {};  /**< WT: bin value buffers */

    static const uint32_t s_offset_size = sizeof(uint32_t); 
    static const uint32_t s_buffer_size = 4 * 1024 * 1024; // 4MB
    static const uint32_t s_invalid_off = uint32_t(-1); 

public: 
    VarLengthValueArray (Buffer *buf, DataType *dt) : BinaryValueArray(buf, dt) {}
    ~VarLengthValueArray(void);

public: 
    uint64_t    getFixSize (uint64_t cap) { return cap * s_offset_size; }

    void        uninit     (void) override;
    int         init2read  (uint64_t len, void *bgn, uint64_t num) override;
    int         init2write (uint64_t len, void *bgn) override; 
    int64_t     copyContent(BinaryValueArray   *src) override;
    int64_t     resizeElemUsed        (uint64_t num) override
    { m_val_num = num; return int64_t(m_val_num * s_offset_size); }


    void        setBeginOffset(uint32_t off) override
    {   assert(m_val_num ==0); m_cur_off = off;   }

    const void *getOffsetBegin        (void) override { return m_offsets    ; }
    uint64_t    getOffsetSize         (void) override { return s_offset_size; }
    uint64_t    getOffsetArrayUsed    (void) override { return s_offset_size * m_val_num; }

    uint64_t    getValueSize          (void) override { return 0; } // var-length
    uint64_t    getReadValueArrayUsed (void) override { return m_rd_vlen; }
    uint64_t    getWriteValueArrayUsed(void) override;

public:
    bool        isNull(uint64_t idx) override { return nullptr == read(idx); }
    const void *read  (uint64_t idx) override;
    int         read  (uint64_t idx, const void* &bin, uint32_t &len) override;

public:
    int  writeText  (const char *txt, const void* &bin) override;
    int  writeBinVal(uint64_t    len, const void*  bin) override;
//    int  getWrittenBin(const void* &bin, uint32_t &len) override;

protected:
    /**
     * trans text value string to binary value content 
     * @param txt text value string 
     * @param bin output binary value result
     * @return >0 bytes used number; <0 failed  
     */
    int  trans2Bin(const char *txt, const void* &bin);

public:
    int64_t appendOffsets(Buffer *buf) override;
    int64_t appendValues (Buffer *buf) override;

public:
    void output2debug(void) override; 
}; // VarLengthValueArray 


} // namespace steed


#include "BinaryValueArray_inline.h"