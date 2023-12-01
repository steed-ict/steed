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
 * @file JSONTypeMapper.cpp
 * @author  Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   Defines funcs to class JSONTypeMapper related funcs 
 */

#include "JSONTypeMapper.h"


namespace steed {

namespace JSONTypeMapper {
/**
 * map JSONType id to DataType
 * @param jtp  JSONType id  
 * @return DataType id 
 */
int  mapType (uint8_t jtp)
{
    int dt_id = DataType::s_type_invalid;
    switch  (jtp)
    {
        case JSONType::s_string:
                dt_id = DataType::s_type_string ; break; 

        case JSONType::s_number:
                dt_id = DataType::s_type_double ; break; 

        case JSONType::s_true :
        case JSONType::s_false:
                dt_id = DataType::s_type_boolean; break; 

        default:  break;
    } // switch

    return dt_id;
} // mapType 
//int  mapType (SampleNode *n)


/**
 * map json text number to DataType id 
 * @param tval  text value content 
 * @return DataType id 
 */
int mapNumber (const char *tval)
{
    (void)tval;
    //TODO: map DataType to number type 
    return DataType::s_type_invalid;
} // mapNumber


} // namespace JSONTypeMapper

} // namespace steed

