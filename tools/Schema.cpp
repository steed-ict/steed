/**
 * @file SchemaTreeUtil.cpp
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section LICENSE TBD
 * @section DESCRIPTION
 *   SchemaTree Util tools
 */

#include "Config.h"
#include "DataType.h"
#include "SchemaTree.h"
#include "SchemaTreePrinter.h"


namespace steed {
steed::Config g_config;
} // namespace steed

int main(int argc, const char *argv[])
{
    using namespace steed;

    if (argc != 3)
    {
        printf("Usage: %s <database> <collection>\n", argv[0]);
        return -1;
    } // if 


    string db(argv[1]), col(argv[2]); 
    SchemaTree *t  = new SchemaTree(db, col);
    if (t->load()< 0)
    {
        printf("SchemaTree::flush failed!\n");
        return -1;
    }
    
//    SchemaTreePrinter::output2dot(t);
//    SchemaTreePrinter::output2tree(t);
    SchemaTreePrinter::output2debug(t);
//    SchemaTreePrinter::outputChildNumberDist(t);

    delete t; t = nullptr;

    return 0;
} // main
