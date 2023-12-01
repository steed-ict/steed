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
 * @file RecordOutput.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   output binary record to text JSON content:
 *   output all fields in the record 
 */

#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "Config.h"
#include "Buffer.h"
#include "SchemaTree.h"
#include "RowArrayOperator.h"
#include "RowObjectOperator.h"


namespace steed {

using std::string;
using std::vector;
using std::ostream;

using namespace Row;

extern Config g_config;


class RecordOutput {
protected:
    class LevelReader {
    public:
        RowObjectOperator  m_obj{}; /**< binary row object */
        RowArrayOperator   m_arr{}; /**< binary row array  */
    }; // LevelReader


protected:
    vector<LevelReader>  m_lvl_exp    {}; /**< each reader level  */
    Buffer              *m_tbuf{nullptr}; /**< buffer value text  */
    SchemaTree          *m_tree{nullptr}; /**< relate SchemaTree  */

public:
    ~RecordOutput(void);
    RecordOutput (SchemaTree *tree);

public:
    /**
     * output JSON records to ostream 
     * @param ostrm  output stream isn 
     * @param recd   binary record begin position 
     * @return 0 success; <0 failed 
     */
    int outJSON2Strm(ostream *ostrm, char* recd);

protected:
    /**
     * output binary arrays for debug
     * @param ostrm  output stream ins  
     * @param bgn    binary array begin  
     * @param lvl    nested level in record  
     * @param ss     SchemaNode's sign in SchemaTree 
     * @return 0 success; <0 failed 
     */
    int outJSONArr2Strm(ostream *ostrm, char *bgn, uint32_t lvl, SchemaSignature ss); 

    /**
     * output binary object for debug
     * @param bgn    binary array begin  
     * @param lvl    nested level in record  
     * @param ss     SchemaNode's sign in SchemaTree 
     * @param num    nested number for output format
     * @return 0 success; <0 failed 
     */
    int outJSONObj2Strm(ostream *ostrm, char *bgn, uint32_t lvl, SchemaSignature ss); 

    /**
     * output text value to ostrm
     * @param ostrm  output stream ins  
     * @param dt     bin DataType ins 
     * @param bin    binary value begin 
     * @return 0 success; <0 failed 
     */
    int outJSONValue2Strm(ostream *ostrm, DataType *dt, char *bin);


public:
    /**
     * output binary record for debug to stdout by printf
     * @param recd    binary record begin position 
     * @return 0 success; <0 failed 
     */
    int out4debug(char* recd)
    { return outObj4Debug(recd, 0, SchemaSignature(0), 0); }

protected:
    /**
     * output binary arrays for debug
     * @param bgn    binary array begin  
     * @param lvl    nested level in record  
     * @param ss     SchemaNode's sign in SchemaTree 
     * @param num    nested number for output format
     * @return 0 success; <0 failed 
     */
    int outArr4Debug (char *bgn, uint32_t lvl, SchemaSignature ss, uint32_t num); 

    /**
     * output binary object for debug
     * @param bgn    binary array begin  
     * @param lvl    nested level in record  
     * @param ss     SchemaNode's sign in SchemaTree 
     * @param num    nested number for output format
     * @return 0 success; <0 failed 
     */
    int outObj4Debug(char *bgn, uint32_t lvl, SchemaSignature ss, uint32_t num); 

    /**
     * output tabs for alignment format
     * @param tnum    tab number to output 
     */
    void output4alignment(uint32_t tnum)
    { for (uint32_t ti = 0; ti < tnum; ++ti) printf("    "); } 
}; // RecordOutput





inline
RecordOutput::~RecordOutput(void)
{
    delete m_tbuf;
    m_tbuf = nullptr;
    m_tree = nullptr;
} // dtor 



inline
RecordOutput::RecordOutput(SchemaTree *tree): m_tree(tree)
{
    m_lvl_exp.resize (g_config.m_max_nested_level);  
    m_tbuf = new Buffer();
} // ctor 



inline
int RecordOutput::outJSON2Strm(ostream *ostrm, char* recd)
{
    int got = outJSONObj2Strm(ostrm, recd, 0, SchemaSignature(0)); 
    (*ostrm) << '\n';
    return got;
} // outJSON2Strm 



inline
int RecordOutput::outJSONValue2Strm(ostream *ostrm, DataType *dt, char *bin)
{
    uint64_t len = m_tbuf->available();
    char    *txt = (char*)m_tbuf->getNextPosition();
    if (dt->transBin2Txt(bin, txt, len) < 0)
    {
        puts("RecordOutput:: outArr4Debug to transBin2Txt failed!\n");
        return -1;
    } // if 

    (*ostrm) << txt; 

    return 0;
} // outJSONValue2Strm


} // namespace
