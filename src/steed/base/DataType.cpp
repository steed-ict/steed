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
 * @file DataType.cpp
 * @author  Zhiyi Wang  <zhiyiwang@ict.ac.cn>
 * @version 0.4
 * @section DESCRIPTION
 *   definitions and functions for DataType.
 */

#include "DataType.h"

namespace steed {

const int DataType::s_type_invalid =  0; // 'A'
const int DataType::s_type_boolean =  1; // 'B'
const int DataType::s_type_int_8   =  2; // 'C'
const int DataType::s_type_int_16  =  3; // 'D'
const int DataType::s_type_int_32  =  4; // 'E'
const int DataType::s_type_int_64  =  5; // 'F'
const int DataType::s_type_tstamp  =  5; // 'F'
const int DataType::s_type_float   =  6; // 'G'
const int DataType::s_type_double  =  7; // 'H'
const int DataType::s_type_string  =  8; // 'I'
const int DataType::s_type_bytes   =  9; // 'J'
const int DataType::s_type_max     = 10; // 'K'
                                              
DataType *DataType::s_type_ins[DataType::s_type_max]; 


SymbolMap<int> DataType::s_data_type_tab;

const DataType::TypeDesc  DataType::s_type_desc[] = {
    // main definitions (id === index) 
    // name      fmt       id              size
    {"invalid",  nullptr,  s_type_invalid,  -1},
    {"boolean",     "%s",  s_type_boolean,   1},
    {"int8"   ,     "%d",  s_type_int_8  ,   1},
    {"int16"  ,     "%d",  s_type_int_16 ,   2},
    {"int32"  ,     "%d",  s_type_int_32 ,   4},
    {"int64"  ,    "%ld",  s_type_int_64 ,   8},
    {"float"  ,     "%f",  s_type_float  ,   4},
    {"double" ,    "%lf",  s_type_double ,   8},
    {"string" , "\"%s\"",  s_type_string ,   0},
    {"bytes"  ,     "%s",  s_type_bytes  ,  12},
    {nullptr  ,  nullptr,  s_type_invalid,  -1}
}; // s_type_desc



SymbolMap<int> DataType::s_data_cmp_tab;

const DataType::CompareDesc DataType::s_cmp_operation[] = {
    // name       id
    {"invalid",  s_cmp_invalid  },
    {"<"      ,  s_cmp_less     },
    {"<="     ,  s_cmp_not_grt  },
    {"=="     ,  s_cmp_equal    },
    {"!="     ,  s_cmp_not_equal},
    {">"      ,  s_cmp_greater  },
    {">="     ,  s_cmp_not_less },
    {"like"   ,  s_cmp_like     },
    {"substr" ,  s_cmp_substr   },
    {nullptr  ,  s_cmp_invalid  }
}; // s_cmp_operation


  
const int DataType::s_cmp_invalid  = 0; 
const int DataType::s_cmp_less     = 1;   // less
const int DataType::s_cmp_not_grt  = 2;   // not greater 
const int DataType::s_cmp_equal    = 3;
const int DataType::s_cmp_not_equal= 4;
const int DataType::s_cmp_greater  = 5;
const int DataType::s_cmp_not_less = 6;
const int DataType::s_cmp_like     = 7;   // string only: pattern matched 
const int DataType::s_cmp_substr   = 8;   // stirng only: substring 
const int DataType::s_cmp_max      = 9;

DTCompareFP DataType::getCompareFunc(int cmp_op_id)
{
    DTCompareFP cfp = nullptr;  // compare function pointer 
    switch (cmp_op_id)
    {
        case s_cmp_less     :  cfp = &DataType::compareLess      ;  break; 
        case s_cmp_not_grt  :  cfp = &DataType::compareNotGreater;  break; 
        case s_cmp_equal    :  cfp = &DataType::compareEqual     ;  break; 
        case s_cmp_not_equal:  cfp = &DataType::compareNotEqual  ;  break; 
        case s_cmp_greater  :  cfp = &DataType::compareGreater   ;  break; 
        case s_cmp_not_less :  cfp = &DataType::compareNotLess   ;  break; 
        case s_cmp_like     :  cfp = &DataType::compareLike      ;  break; 
        case s_cmp_substr   :  cfp = &DataType::compareSubString ;  break; 
        default:    break;
    } // switch
    return cfp;
} // getCompareFunc



/** bin null value */ 
template<> const     int8_t  TypeNumeric<int8_t> ::s_null( INT8_MIN);
template<> const     int16_t TypeNumeric<int16_t>::s_null(INT16_MIN); 
template<> const     int32_t TypeNumeric<int32_t>::s_null(INT32_MIN); 
template<> const     int64_t TypeNumeric<int64_t>::s_null(INT64_MIN); 
template<> constexpr float   TypeNumeric<float>  ::s_null(  FLT_MIN); 
template<> constexpr double  TypeNumeric<double> ::s_null(  DBL_MIN); 




int DataType::s_init = DataType::initStatic();

int DataType::initStatic (void)
{
    if (s_init > 0)
    {   return 1;   }

    // 0. check static const values 
    if (strncmp(s_type_desc[0].name, "invalid", 7) != 0) 
    {   printf("DataType init failed!\n"); return -1;   }
  
    // 1. add DataType name string to SymbolMap
    for (int i = 0; s_type_desc[i].name != nullptr; i++) 
    {
        int id = s_type_desc[i].id;
        s_data_type_tab.insert(s_type_desc[i].name, id);
    }
   
    // 2. init global static DataType child instance 
    s_type_ins[s_type_invalid] = new TypeInvalid();
    s_type_ins[s_type_boolean] = new TypeBoolean();
    s_type_ins[s_type_int_8  ] = new TypeInt8   ();
    s_type_ins[s_type_int_16 ] = new TypeInt16  ();
    s_type_ins[s_type_int_32 ] = new TypeInt32  ();
    s_type_ins[s_type_int_64 ] = new TypeInt64  ();
    s_type_ins[s_type_float  ] = new TypeFloat  ();
    s_type_ins[s_type_double ] = new TypeDouble ();
    s_type_ins[s_type_string ] = new TypeString ();
    s_type_ins[s_type_bytes  ] = new TypeBytes  ();
  
    // 3. add DataType operation name string to SymbolMap 
    for (int i = 0; s_cmp_operation[i].name != nullptr; ++i)
    {
        int id = s_cmp_operation[i].id;
        s_data_cmp_tab.insert(s_cmp_operation[i].name, id);
    }
  
    return 1; 
} // initStatic



int DataType::uninitStatic(void)
{
    for (int id = s_type_invalid; id < s_type_max; ++id)
    {
        delete s_type_ins[id];
        s_type_ins[id] = nullptr;
    } // for id 

    return 0;
} // uninitStatic 

} // namespace steed 