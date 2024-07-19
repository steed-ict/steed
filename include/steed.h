/**
 * @file steed.h
 * @brief steed library header 
 * @version 1.0
 * @section LICENSE TBD
 */


#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdint.h>

#include "Config.h"
#include "Utility.h"
#include "SchemaTreeMap.h"
#include "ColumnParser.h"
#include "ColumnAssembler.h"
#include "RecordOutput.h"

namespace steed {
using std::string;
using std::ifstream;

//// steed static  
/**
 * init steed static data
 * @param cfile config file path
 */
void init(const string &cfile);

/**
 * uninit steed static data
 */
void uninit(void);


// database and table operations
/**
 * create a database
 * @param db database name
 * @return 1 success created, 0 already existed, -1 if failed
 */
int createDatabase(const string &db);

/**
 * drop a database
 * @param db database name
 * @return 1 success dropped, 0 not existed, -1 if failed
 */
int dropDatabase  (const string &db);

/**
 * create a table
 * @param db database name
 * @param table table name
 * @return 1 success created, 0 already existed, -1 if failed
 */
int createTable(const string &db, const string &table);

/**
 * drop a table
 * @param db database name
 * @param table table name
 * @return 1 success dropped, 0 not existed, -1 if failed
 */
int dropTable  (const string &db, const string &table);

} // namespace steed



extern "C" {
    /**
     * make clean
     *   remove python generated files, the steed store base dir and all data
     */
    void make_clean(void);

    /**
     * free string memory
     * @param str string to be freed
     */
    void free_string(const char *str);

    /**
     * init steed static data
     * @param cfile config file path
     */
    void init(const char *cfile);

    /**
     * uninit steed static data
     */
    void uninit(void);

    /**
     * create a database
     * @param db database name
     * @return 1 success created, 0 already existed, -1 if failed
     */
    int create_database(const char *db);

    /**
     * drop a database
     * @param db database name
     * @return 1 success dropped, 0 not existed, -1 if failed
     */
    int drop_database  (const char *db);

    /**
     * create a table
     * @param db database name
     * @param table table name
     * @return 1 success created, 0 already existed, -1 if failed
     */
    int create_table(const char *db, const char *table);

    /**
     * drop a table
     * @param db database name
     * @param table table name
     * @return 1 success dropped, 0 not existed, -1 if failed
     */
    int drop_table  (const char *db, const char *table);

    /**
     * parse JSON records in a file and insert into table
     * @param db database name
     * @param table table name
     * @param jpath JSON file path
     * @return 1 success created, 0 already existed, -1 if failed
     */
    int parse_file(const char *db, const char *table, const char *jpath);

    /**
     * assemble binary records and output to file
     * @param db database name
     * @param table table name
     * @param cols column names
     * @param jpath output file path
     * @return 1 success created, 0 already existed, -1 if failed
     */
    int assemble_to_file(const char *db, const char *table, const char **cols, int ncol, const char *jpath);

    /**
     * assemble binary records and output to string
     * @param db database name
     * @param table table name
     * @param cols column names
     * @param ncol number of columns
     * @return a list of json records 
     */
    const char *assemble_to_string(const char *db, const char *table, const char **cols, int ncol);


    /*
     * parse JSON records in a string and insert into table
     * Python need to create and use a ColumnParser object
     */

    /**
     * open steed and create a column parser
     * @param db database name
     * @param table table name
     * @return a column parser object
     */
    steed::ColumnParser *open_parser(const char *db, const char *table);

    /**
     * insert a JSON record into table
     * @param cp column parser object
     * @param recd JSON record in a string
     * @param len length of the string
     * @return 1 success created, 0 already existed, -1 if failed
     */
    int insert_parser(steed::ColumnParser *cp, const char *recd, uint32_t len);

    /**
     * close steed and destroy a column parser
     * @param cp column parser object
     */
    void close_parser(steed::ColumnParser *cp);

} // extern "C"