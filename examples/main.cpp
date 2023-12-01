/**
 * @file main.cpp
 * @brief An example to show how to use steed shared library 
 * @version 1.0
 * @section LICENSE TBD
 */

#include <string>
#include <vector>
#include "steed.h"

using namespace steed;

int main(int argc, char *argv[])
{
    (void)argc, (void)argv; 

    // use default config file
    string conf_file(""); 
    steed::init(conf_file);

    std::string db("demo"), table("mytry"); 
    createDatabase(db);
    createTable(db, table);

    { // create a column parser to parse JSON records  
        std::string jpath("./json/try.json");
        ifstream ifs(jpath);
        if (!ifs.is_open())
        {
            printf("Main: cannot open [%s]!\n", jpath.c_str());
            return -1;
        } // ifs
    
        steed::ColumnParser *cp = new steed::ColumnParser();
        istream *is = &ifs;
        if (cp->init(db, table, is) < 0)
        {
            printf("Main: ColumnParser init failed!\n");
            return -1;
        } // if
    
        int status = 0;
        while ((status = cp->parseOne()) > 0)
        {   /* do nothing */   }
    
        delete cp; cp = nullptr;
        ifs.close();

        if (status < 0)
        {
            printf("Main: insert failed!\n");
            return -1;
        } // if
    }

    { // create a column assembler to assemble binary records and output
        std::vector< std::string > cols;
        cols.emplace_back("item");
        cols.emplace_back("size.h");

        ColumnAssembler *ca = new ColumnAssembler();
        if (ca->init(db, table, cols) < 0)
        {
            printf("STEED::init ColumnAssembler failed!\n");
            return -1;
        } // if

//        uint64_t cnt = 0;
        char *rbgn = nullptr;
        ostream *ostrm = &std::cout;
        RecordOutput ro( ca->getSchemaTree() );
        while (ca->getNext(rbgn) > 0)
        {
//            *ostrm << ++cnt << "\t";
            ro.outJSON2Strm(ostrm, rbgn);
        } // while

        delete ca; ca = nullptr;
    }

    dropTable(db, table);
    dropDatabase(db);

    steed::uninit();

    return 0; 
} // main
