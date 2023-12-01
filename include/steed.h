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