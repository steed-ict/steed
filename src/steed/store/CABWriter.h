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
 * @file CABWriter.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    definitions and functions for CABWriter 
 */

#pragma once 

#include <string>

#include "Utility.h"
#include "CABOperator.h"

namespace steed {
using std::string;

class CABWriter : virtual public CABOperator {
public:
    uint64_t    m_file_off{0};  /**< write file offset */

public:
    CABWriter (void) = default;
    ~CABWriter(void);

public: 
    /**
     * init ins to write
     * @param fbase  file base name string  
     * @param tree   SchemaTree instance  
     * @param path   path in SchemaTree 
     * @param cap    capacity in ColumnAlignBlock 
     * @param rbgn   begin record id (index) to write  
     * @return 0 success; <0 failed
     */
    int init2write(const string &fbase, SchemaTree* tree, SchemaPath &path,
        uint64_t cap, uint64_t rbgn);

public:
    /**
     * write null item
     * @param rep    max repetition value
     * @param def    max definition value 
     * @param nnum   null number 
     * @return <0 failed; 0 CAB is full; 1 (>0) success
     */
    int writeNull(uint32_t rep, uint32_t def, uint64_t nnum);

    /**
     * write null item
     * @param rep    max repetition value
     * @param def    max definition value 
     * @return <0 failed; 0 CAB is full; 1 (>0) success
     */
    int writeNull(uint32_t rep, uint32_t def);

    /**
     * write column item 
     * @param rep  repetition value  
     * @param def  definition value 
     * @param txt  text data value content 
     * @return 0 successed; <0 failed 
     */
    int writeText(uint32_t rep, uint32_t def, const char* txt);

    /**
     * write column item 
     * @param rep  repetition value  
     * @param def  definition value 
     * @param len  binary value length
     * @param bin  bin data value content 
     * @return 0 successed; <0 failed 
     */
    int writeBinVal(uint32_t rep, uint32_t def, const void *bin, uint32_t len);

protected:
    /**
     * prepare next CAB to write 
     * @return <0 failed; =0 EOF; >0 success 
     */ 
    int prepareCAB2write(void); 

    /**
     * get new cab info to write  
     * @return >0 success; < 0 failed 
     */
    int getInfo2Write(void);

    /**
     * flush CAB binary content
     * @param tail    flush the tail CAB
     * @return 0 succes; <0 failed
     */
    int flush(bool tail);

protected:
    /**
     * init value info to null content 
     * @param info   ColumnValueInfo in CAB or Column 
     * @return 0 succes; <0 failed
     */
    int initValueInfo  (ColumnValueInfo *info);

    /**
     * update value info by binary value  
     * @param bin    bin value used to update
     * @param info   ColumnValueInfo in CAB or Column 
     * @return 0 succes; <0 failed
     */
    int updateValueInfo(const void *bin, ColumnValueInfo *info); 

    /**
     * merge value info by binary value  
     * @param cab_info    CAB  ColumnValueInfo 
     * @param file_info   file ColumnValueInfo in footer 
     * @return 0 succes; <0 failed
     */
    int mergeValueInfo(ColumnValueInfo *cab_info, ColumnValueInfo *file_info); 


#if DEFINE_BLM
    /**
     * update BloomFilter  
     * @param bin    bin value used to update
     * @param info   ColumnValueInfo in CAB or Column 
     */
    void updateBloom(const void *bin, uint32_t len)
    {   m_bloom->addBytes((const char*)bin, int64_t(len));   }

    /**
     * reset BloomFilter  
     */
    void resetBloom(void)
    {   m_bloom->reset();   }
#endif 
}; // CABWriter

} // namespace 



#include "CABWriter_inline.h"
