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
 * @file Config.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *     Config related param 
 */

#include "Config.h"
#include <unistd.h> 

namespace steed {

const string Config::s_field_delim{"."}; 
const char Config::s_schema_attr_delim{'_'};
const char Config::s_schema_map_sign_delim{':'};



int Config::init (const string &cfile)
{
    loadConfigFile(cfile);
    addConfOptions();

    int argc = 1;
    const char *argname = "steedlib";
    CLI11_PARSE(m_app, argc, &argname);
    return 0;
} // init



int Config::init (int argc, char** argv, const string &cfile)
{
    // load config file
    loadConfigFile(cfile);
    addConfOptions();


    // subcommands
    m_app.require_subcommand(1);    // one of the following subcommands is required

    auto op_create = m_app.add_subcommand("create", "Create a database or table");
    addDB2Option(op_create, m_db);
    addTB2Option(op_create, m_tb, false);

    auto op_drop = m_app.add_subcommand("drop", "Drop a database or table");
    addDB2Option(op_drop,   m_db);
    addTB2Option(op_drop, m_tb, false);

    auto op_parse = m_app.add_subcommand("parse", "Parse data");
    addDB2Option  (op_parse,  m_db);
    addTB2Option  (op_parse, m_tb);
    addJSON2Option(op_parse, m_jpath);

    auto op_append= m_app.add_subcommand("append", "Append data");
    addDB2Option  (op_append, m_db);
    addTB2Option  (op_append, m_tb);
    addJSON2Option(op_append, m_jpath);

    auto op_assemble = m_app.add_subcommand("assemble", "Assemble data");
    addDB2Option  (op_assemble, m_db);
    addTB2Option  (op_assemble, m_tb);
    addCols2Option(op_assemble, m_cols);


    // parse the command line and config params
    CLI11_PARSE(m_app, argc, argv);


    if (op_create->parsed()) {
        m_run_mode = m_tb.empty() ? createDatabase : createTable;
    }
    else if (op_drop->parsed()) {
        m_run_mode = m_tb.empty() ? dropDatabase : dropTable;
    }
    else if (op_parse->parsed()) {
        m_run_mode = parse;
    }
    else if (op_append->parsed()) {
        m_run_mode = append;
    }
    else if (op_assemble->parsed()) {
        m_run_mode = assemble;
    }

    return 0;
} // init



void Config::loadConfigFile(const string &cfile)
{
//    m_app.allow_config_extras(true); // allow extra options

//    char buffer[4096];
//    if (getcwd(buffer, sizeof(buffer)) != nullptr) {
//        printf("%s\n", buffer);
//    }

    const char *path = cfile.empty() ? "steed.conf" : cfile.c_str();
    m_app.set_config("--config", path, "Steed config file", true)
        ->transform(CLI::FileOnDefaultPath("../conf/"))
        ->transform(CLI::FileOnDefaultPath("./conf/", false))
        ->transform(CLI::FileOnDefaultPath("./", false));
} // loadConfigFile



void Config::addConfOptions(void)
{
    // database params
    m_app.add_option("-d,--database", m_db, "database name");
    m_app.add_option("-t,--table"   , m_tb, "table name");

    // store related
    m_app.add_option("--store_base", m_store_base, "base dir to store binary data");
    m_app.add_option("--data_dir"  , m_data_dir,   "binary data storage directory");
    m_app.add_option("--schema_dir", m_schema_dir, "dataset schema  directory");
//    m_app.add_option("--result_dir", m_result_dir, "assemble result directory");
    if (m_store_base.back() == '/') { m_store_base.pop_back(); }
    if (m_data_dir  .back() == '/') { m_data_dir  .pop_back(); }
    if (m_schema_dir.back() == '/') { m_schema_dir.pop_back(); }
//    if (m_result_dir.back() == '/') { m_result_dir.pop_back(); }

    // runtime related
    m_app.add_option("--mem_align_size", m_mem_align_size, "memory aligned size");
    m_app.add_option("--cab_recd_num"  , m_cab_recd_num, "number of records in a cab file");
    m_app.add_option("--text_recd_num" , m_text_recd_num, "number of records in text record buffer");
} // addConfOptions



void Config::output(void) const
{
    printf("Config:\n");
    printf("  m_mem_align_size: %u\n", m_mem_align_size);
    printf("  m_store_base: %s\n", m_store_base.c_str());
    printf("  m_data_dir: %s\n", m_data_dir.c_str());
    printf("  m_schema_dir: %s\n", m_schema_dir.c_str());
//    printf("  m_index_info: %s\n", m_index_info.c_str());

    puts("Steed Config: got the following options:"); 
    printf("Steed Config: m_run_mode: %d\n", m_run_mode);
    bool default_also = true; 
    bool write_description = true;
    std::cout<< m_app.config_to_str(default_also, write_description);
} // output

} // namespace steed