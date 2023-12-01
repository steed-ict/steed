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

#pragma once 

#include <stdint.h>
#include <string>
#include <vector>

#include "CLI/CLI.hpp"


namespace steed {

using std::vector;
using std::string; 

class Config {
public:
    typedef enum Mode {
        invalid = 0,
        createDatabase,
        createTable,
        dropDatabase,
        dropTable,
        parse,
        append,
        assemble
    } Mode;

protected: // CLI11 related params
    CLI::App m_app{"Steed Client App"};
    Mode m_run_mode{invalid};

public: // call related 
    string          m_db{""};
    string          m_tb{""};
    vector<string>  m_cols{};
    string          m_jpath{""};

public: // store related
    /**< base dir to store binary data */
    string m_store_base{"/data"}; 
    string m_data_dir  {"cols"};   /**< binary data storage directory */
    string m_schema_dir{"schema"}; /**< dataset schema  directory */
//    string m_result_dir{"result"}; /**< assemble result directory */
//    string m_index_info{"index"};  /**< index directory */

public: // memory related
    uint32_t m_mem_align_size{4096};    /**< memory aligned size */

public: // schema related
    /** SampleNode has many sibling threshold  */
    uint32_t m_many_sib_thrd = 10;

    /** template related name strings */
    string m_schema_temp_name{"TMPLT"}; 
    string m_schema_temp_key {"=KEY="};
    string m_schema_temp_val {"=VAL="};

public: // cab
    uint32_t m_cab_recd_num{8};  /** CAB contains recd num */
//    uint32_t m_cab_recd_num{8 * 1024};  /** CAB contains recd num */
//    uint32_t m_cab_recd_num{128 * 1024};  /** CAB contains recd num */

    /** max binary value length */
    uint32_t m_recd_max_len{64 * 1024}; // 64KB 
    uint32_t m_max_bin_val_len {1024 * 1024}; // 1MB
    uint32_t m_max_nested_level{32};  // max nested level

    double m_reserve_factor {1.618};

public: // parse related 
    /** record number in text record buffer */
    uint32_t m_text_recd_num = 16;
    uint32_t m_text_recd_avg_len  = 1024*1024;
    uint32_t m_text_buffer_number = m_text_recd_num;

    /**
     * field delimiter in path expression
     * each field in path expression is a SchemaNode 
     * */
    static const string s_field_delim;
    
    /**
     * SchemaNode attribute delimiter in postfixed name
     * the attributes are separated by this delimiter
     * We use '_' as the delimiter, which is legal in name in Windows, Mac and Linux
     */
    static const char s_schema_attr_delim;

    /**
     * SchemaTreeMap related:
     *   use the sign delimiter to separate database name and table name
     *   [database name][sign delimiter][table name]
     */
    static const char s_schema_map_sign_delim;

public: // assemble
    uint32_t m_recd_cap  = 2048;
    uint32_t m_assemble_buf_cap = {64 * 1024 * 1024}; // 64MB 


public:
    Config (const Config&) = default;
    Config (void) = default;
    ~Config(void) = default;

public:
    /**
     *  init config using the config file
     */
    int init(const string &cfile);

    /**
     * init config using the command line arguments and config file
     * @param argc argument count
     * @param argv argument vector
     * @param cfile config file path
     */
    int init(int argc, char** argv, const string &cfile);

public:
    /**
     * load config file, default path is ./conf/steed.conf 
     * @param cfile config file path
     */
    void loadConfigFile(const string &cfile);

    /**
     * add config options
     */
    void addConfOptions(void);

private:
    /**
     * get CLI11 app
     * @return CLI11 app
     */
    CLI::App& getApp(void) { return m_app; }
    Mode getRunMode(void) const { return m_run_mode; }

    void addDB2Option(CLI::App* app, string &db)
    {   app->add_option("-d,--database", db, "Database name")->required();   } 
    
    void addTB2Option(CLI::App* app, string &tb, bool required = true)
    {
        auto op = app->add_option("-t,--table", tb, "Table name");
        if (required) { op->required(); }
    } // addTB2Option
    
    void addJSON2Option(CLI::App* app, string &jpath)
    {   app->add_option("-j,--json", jpath, "JSON file path")->required();   }
    
    void addCols2Option(CLI::App* app, vector<string> &cols)
    {   app->add_option("-c,--column", cols, "Columns")->required();   }
    
public:
    void output(void) const;
}; // class Config

} // namespace steed





// //////////////////// parse related ////////////////////
// 
// /**
//  * load the existed SchemaTree instead of sampling to infer
//  * default value is false 
//  */
// const bool s_schema_is_existed       = true;
// 
// /** sample the records to generate SchemaTree */
// const bool s_sample_schema           = true ;
// 
// /** only generate the SampleTree */
// const bool s_only_sample_recd        = false; 
// 
// 
// /** only generate the SchemaTree from SampleTree */
// const bool s_only_gen_sampled_schema = false ; 
// 
// /** generate the SchemaTree: sample + full parse */
// const bool s_only_gen_full_schema    = false; 
// 
// /** parse records to CABs in file */
// const bool s_parse_2_cabs            = true;
// 
// 
// 
// /** update SchemaTree in ColumnItemGenerator */
// const uint8_t s_forbid_update = 0;
// const uint8_t s_update_2_temp = 1;
// const uint8_t s_update_2_node = 2;
// 
// /** always add new field as a SchemaNode */
// const uint8_t s_update_schema = s_update_2_node;
// 
// 
// 
// 
// 
// 
// /**
//  * the number of sampled record to infer SchemaTree
//  * wikidata small dataset recd number: 200000 
//  * wikidata large dataset recd number: 2000000
//  */
// const uint32_t  s_sample_number = 1000;
// 
// 
// /** the number of sampled record to infer SchemaTree */
// //constexpr double    s_frequent_threshold = 0;
// constexpr double    s_frequent_threshold = 0.90;
// //constexpr double    s_frequent_threshold = 1; // all SchemaNode trans to KAV
// 
// /**
//  * if the children number of a SampleNode n is large than s_bloated_sib_thrd, 
//  * it is a bloated node and merge as a sample node: 
//  *   0.0, merge all candidate SampleNode to SchemaTemplate 
//  *   1.0, each parent object has one unique KAV child 
//  * Note: s_bloated_sib_thrd * s_sample_number is large then s_many_sib_thrd 
//  */
// constexpr double s_bloated_sib_ratio = 0.01;
// const uint32_t   s_bloated_sib_thrd  = s_sample_number * s_bloated_sib_ratio;
 
 
 

// //////////////////// CAB related ////////////////////
// 
// //static const int s_cab_recd_num = 8;  /** CAB contains recd num */
// //static const int s_cab_recd_num = 4;  /** CAB contains recd num */
// //static const int s_cab_recd_num = (4 * 1024);  /** CAB contains recd num */
// static const int s_cab_recd_num = (128 * 1024);  /** CAB contains recd num */
// //static const int s_cab_recd_num = (256 * 1024);  /** CAB contains recd num */
// //static const int s_cab_recd_num = (1024 * 1024);  /** CAB contains recd num */
// 
// 
// 
// static const bool     s_kav_key_blm = false; /** blooming filter on KAV key */ 
// 
// //TODO
// //static const bool     s_kav_val_blm = false; /** blooming filter on KAV val */ 
// //static const bool     s_cab_str_blm = false; /** blooming filter on string cab */ 
// 
// 
// 
// 
// 
// //////////////////// query related ////////////////////
// 
// static const uint32_t s_recd_cap  = 2048;
// static const uint32_t s_def_rlen  = 1024;  /**< default bin size record */ 
// 
// 
// static const string s_space_delim {" \t"};
// static const string s_stmt_delim  {";"};
// static const string s_clause_delim{"\n"};
// static const string s_field_delim  {" \t(),;"};
// static const string s_field_delim {"."};
// 
// static const string s_key_word_as {"as"};
// 
// /** query output to row format */
// const uint8_t s_query_output_nothing = 0;
// const uint8_t s_query_out_kav_pairs  = 1;
// const uint8_t s_query_out_kav_array  = 2;
// 
// /** always add new field as a SchemaNode */
// const uint8_t s_query_output = s_query_out_kav_array;
