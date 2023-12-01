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
 * @file SchemaTreePrinter.cpp
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 1.0 
 * @section DESCRIPTION
 *   SchemaTree functions
 */

#include "SchemaTreePrinter.h"

namespace steed {
namespace SchemaTreePrinter {


void output2debug(SchemaTree *tree)
{ return tree->output2debug(); }

void output2tree(SchemaTree * tree)  
{ string name; return tree->output2tree(name); }





bool distItemCmp(pair<uint32_t, uint32_t> i, pair<uint32_t, uint32_t> j)
{ return (i.first > j.first); }

void outputChildNumberDist(SchemaTree *tree)
{
    printf("db:[%s] table:[%s] Node #: %lu next id:%u\n",
        tree->getDBName().c_str(), tree->getCltName().c_str(),
        tree->getNodeNum(), tree->getNextFieldID());

    // distribution child node number -> count
    unordered_map <uint32_t,  uint32_t> dist; 

    uint32_t nnum = tree->getNodeNum();
    for (uint32_t i = 0; i < nnum; ++i)
    {
        SchemaNode *nd = tree->getNode(i);
        uint32_t cnum = nd->getChildNum();
        if (dist.find(cnum) == dist.end())
        { dist[cnum]  = 1; }
        else
        { dist[cnum] += 1; }
    } // for 

//    for (auto & it : dist)
//    { printf("%u, %u\n", it.first, it.second); }


    // output the sorted result: [child #, node #]
    vector< pair<uint32_t, uint32_t> > out(dist.begin(), dist.end()); 
    sort(out.begin(), out.end(), distItemCmp);  

    for (auto & it : out)
    { printf("%u, %u\n", it.first, it.second); }

} // outputChildNumberDist





void output2dot(SchemaTree *tree)
{
    uint32_t depth = 0; // max depth
    uint32_t nnum  = tree->getNodeNum();
    for (uint32_t ni = 0; ni < nnum; ++ni)
    {
        if (tree->isLeaf(ni))
        {
            uint32_t lvl = tree->getNode(ni)->getLevel();
            depth = (depth > lvl) ? depth : lvl;
        } // if 
    } // ni


    // dot file header: using di-graph
// test
    std::cout << "digraph graphname {"       << std::endl;
//    std::cout << "graph graphname {"         << std::endl;
    std::cout << "  graph [dpi=300]"         << std::endl;
    std::cout << "  // level tag definition" << std::endl;
    std::cout << "  {"                       << std::endl; // level tag 
    std::cout << "    node [shape=plaintext, frontsize=16];" << std::endl;
    std::cout << "   ";

    uint32_t buf_len = 4096;
    char     buf_bin[4096] = {0};

    vector<string> lvl_name;
    for (uint32_t li = 0; li <= depth; ++li)
    {
        if (snprintf(buf_bin, buf_len, "%u", li) < 0)
        {
            puts("SchemaTreePrinter: output2dot snprintf failed!");
            abort();
        } // if 

      string str("level_");
      str.append(buf_bin);
      lvl_name.emplace_back(str);


      std::cout << str;
      if (li < depth)
// test
      { std::cout << " -> "; }
//      { cout << " -- "; }
    } // for 
    std::cout << ";" << std::endl << "  }" << std::endl << std::endl; // output level tag  


    vector<string> color_name;
    color_name.emplace_back("green");  // single
    color_name.emplace_back("yellow"); // multi array 
    color_name.emplace_back("red");    // index array
    color_name.emplace_back("blue");   // template root  



    // use SchemaNode to define dots 
    std::cout << "  // SchemaNode define " << std::endl;
    for (uint32_t ni = 0; ni < nnum; ++ni)
    {
//if (ni>10000) { break; }

        // get color by SchemaNode type 
        uint32_t ci  = 0; // color index
        uint32_t cat = tree->getNode(ni)->getCategory();
        switch  (cat)
        {
            case SchemaNode::s_vcat_single:  ci = 0;  break;
            case SchemaNode::s_vcat_multi :  ci = 1;  break;
            case SchemaNode::s_vcat_index :  ci = 2;  break;
        } // switch
  
        if (tree->isTemplate(ni)) { ci = 3; }
  
        snprintf(buf_bin, buf_len, "%u", ni);
        std::cout << "  " << string(buf_bin) << " "; // node id  
        std::cout << " [label=\"" << tree->getName(ni) << "\",";
        std::cout << " style=filled,";
        std::cout << " fillcolor=" << color_name[ci] << "]" << std::endl;
    } // for ni
    std::cout << std::endl << std::endl;


    // align SchemaNodes in dots output
    std::cout << "  // rank alignment" << std::endl;
    for (uint32_t ni = 0; ni < nnum; ++ni)
    {
//if (ni>10000) { break; }

        snprintf(buf_bin, buf_len, "%u", ni);
        uint32_t node_lvl = tree->getNode(ni)->getLevel();
        string  &lvl_str  = lvl_name[node_lvl];
        std::cout << "  { rank = same; " << lvl_str << "; ";
        std::cout << buf_bin << " }" << std::endl;
    } // for ni
    std::cout << std::endl << std::endl;


    // direct lines in dots output 
    std::cout << "  // parent child relationship" << std::endl;
    for (uint32_t ni = 1; ni < nnum; ++ni) // without root
    {
//if (ni>10000) { break; }

        SchemaNode *nd = tree->getNode(ni);
        snprintf(buf_bin, buf_len, "%u", nd->getParent());
// test
        std::cout << "  " << buf_bin << "->";
//        std::cout << "  " << buf_bin << "--";
  
        snprintf(buf_bin, buf_len, "%u", ni);
        std::cout << buf_bin << std::endl;
    } // ni

    std::cout << "}" << std::endl;
} // output2dot

} // namespace SchemaTreePrinter

} // namespace steed
