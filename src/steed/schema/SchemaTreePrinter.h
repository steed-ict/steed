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
 * @file   SchemaTreePrinter.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn> 
 * @version 1.0
 * @section DESCRIPTION
 * definition and functions of Schema Tree Printer
 */

#pragma once 

#include <algorithm>
#include <vector>
#include <unordered_map>

#include "SchemaTree.h"

namespace steed {
namespace SchemaTreePrinter {

// Debug 
/** output to debug SchemaTree */
void output2debug(SchemaTree *tree);

/** output in tree format */
void output2tree(SchemaTree * tree);



// statistic SchemaTree
/**
 * compare function for pair<uint32_t, uint32_t>
 */
bool distItemCmp(pair<uint32_t, uint32_t> i, pair<uint32_t, uint32_t> j);

/** output the child number distribution */
void outputChildNumberDist(SchemaTree *tree);




// output dot file to generate graph using graphviz
/** 
 * output dot file to generate graph using graphviz
 * Usage:
 *   $ dot -Tjpg file.dot -o graph.jpg
 * @param tree    SchemaTree instance 
 * @param file    output name string
 */
void output2dot(SchemaTree *tree);

} // namespace SchemaTreePrinter 
} // namespace steed
