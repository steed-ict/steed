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
 * @file CABAppender.h
 * @brief CABAppender class
 * @version 1.0
 * @section LICENSE
 * TBD
 * @section DESCRIPTION
 *  CABAppender class
 */

#pragma once

#include "CABOperator.h"
#include "CABReader.h"
#include "CABWriter.h"

namespace steed {

class CABAppender : public CABWriter {
public:
    CABAppender(void) = default;
    ~CABAppender(void) = default;

public:
    /**
     * init ins to append
     * @param base   data storage base name string
     * @param tree   SchemaTree instance
     * @param path   path in SchemaTree
     * @param cap    capacity in ColumnAlignBlock
     * @return 0 success; <0 failed
     */
    int init2append(const string &base, SchemaTree* tree, SchemaPath &path, uint64_t cap);
}; // class CABAppender


} // namespace steed