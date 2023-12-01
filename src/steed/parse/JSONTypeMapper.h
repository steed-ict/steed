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
 * @file JSONTypeMapper.h
 * @author Zhiyi  Wang <zhiyiwang@ict.ac.cn>
 * @version 0.3
 * @section DESCRIPTION
 *     definitions and functions for JSONTypeMapper: 
 *     map JSONType to steed::DataType
 */
#pragma once 

#include <stdint.h>

#include "DataType.h"
#include "JSONType.h"
//#include "SampleNode.h"



namespace steed {

namespace JSONTypeMapper {

/**
 * map JSONType id to DataType
 * @param jtp  JSONType id  
 * @return DataType id 
 */
int  mapType (uint8_t jtp);

/**
 * map json text number to DataType id 
 * @param tval  text value content 
 * @return DataType id 
 */
int mapNumber (const char *tval);

} // namespace JSONTypeMapper

} // namespace steed
