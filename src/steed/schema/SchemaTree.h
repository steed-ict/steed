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
 * @file SchemaTree.h
 * @author Zhiyi Wang <wangzhiyi@ict.ac.cn> 
 * @version 1.0
 * @section DESCRIPTION
 * definition and functions of Schema Tree
 */

#pragma once 

#include <map>
#include <string>
#include <vector>

#include "Row.h"
#include "Config.h"
#include "Container.h"
#include "SchemaNode.h"
#include "Utility.h"


namespace steed {

using std::map;
using std::string;
using std::vector;

class SchemaTree {
protected:
    string                   m_db_name   {""}; /**< database   name str */
    string                   m_clt_name  {""}; /**< collection name str */

    Container<SchemaNode>   *m_nodes{nullptr}; /**< field node content  */
    vector   <string>        m_names       {}; /**< field name string   */
    vector   <uint8_t>       m_node_valid  {}; /**< SchemaNode is valid */

    SchemaNode::HashTable    m_name_map {};    /**< name to index map   */
    Row::ID                  m_next_fid{1};    /**< next field id */ 

public:
    static const SchemaSignature s_invalid_sign{uint32_t(-1)}; /**< invalid sign */ 
    static const Row::ID         s_max_field_id{0xFFC0};       /**< max field id */ 


public:
    SchemaTree (const string &db, const string &col);
    ~SchemaTree(void);

public:
    Row::ID         getNextFieldID(void) { return m_next_fid; }
    const string&   getDBName     (void) { return m_db_name ; }
    const string&   getCltName    (void) { return m_clt_name; }
    uint64_t        getLeafNum    (void);
    uint64_t        getNodeNum    (void) { return m_nodes->size(); } 
    SchemaNode     *getRoot       (void) { return getNode     (0); }
    SchemaNode     *getNode(SchemaSignature i) { return m_nodes->get(i); }

    /** get next SchemaNode index (signature) */ 
    SchemaSignature getNextIndex(void) { return m_nodes->getNextIndex(); } 

    /**
     * get SchemaSignature ID 
     * SchemaNode's sign and id share the same value
     */ 
    SchemaSignature getSignByID (Row::ID id) { return SchemaSignature(id); }
    bool            isDefined   (Row::ID id) { return id < s_max_field_id; }

//    const string&   getName     (SchemaSignature s) { return m_names.at(s); }
    const string&   getName     (SchemaSignature s) { return m_names[s]; }
    const string    getNameWPost(SchemaSignature s);


    bool      isLeaf     (SchemaSignature s) { return getNode(s)->isLeaf      (); }
    bool      isTemplate (SchemaSignature s) { return getNode(s)->isTemplate  (); }
    bool      isIndexed  (SchemaSignature s) { return getNode(s)->isIndexArray(); }
    bool      isRepeated (SchemaSignature s) // treat template node as repeated array 
    { return (getNode(s)->isMultiArray()) || (getNode(s)->isTemplate()); }

    Row::ID   getFieldID (SchemaSignature s) { return getNode(s)->getFieldID  (); }
    uint32_t  getLevel   (SchemaSignature s) { return getNode(s)->getLevel    (); }
    uint8_t   getCategory(SchemaSignature s) { return getNode(s)->getCategory (); }
    DataType* getDataType(SchemaSignature s) { return getNode(s)->getDataType (); }

    /** get repeated SchemaNodes number from path*/
    uint32_t  getRepeatedNumber(SchemaPath &path);

    /** get max repeated node's level from path */
    uint32_t  getMaxRepeatLevel(SchemaPath &path);

    /** get path by leaf SchemaNode's SchemaSignature */
    void      getPath(SchemaSignature l, SchemaPath &p); 

    /**
     * TODO
     * get the align base path for the new SchemaPath
     * the align base path has the same lowest repeated SchemaNode to sp  
     * 
     * @param sp        new append SchemaPath 
     * @param base      alignment base SchemaPath
     * @param same_rep  the same rep value 
     * @param same_def  the same def value 
     * @return 0 success; <0 failed
     */
    int getAlignBasePath(SchemaPath &sp, SchemaPath &base,
            uint32_t &same_rep, uint32_t &same_def);

public:
    bool isEnabled  (SchemaSignature i) { return m_node_valid[i]; }
    void enableNode (SchemaSignature i) { m_node_valid[i] = 1; }
    void disableNode(SchemaSignature i) { m_node_valid[i] = 0; }

    int  addNode (const char *k, SchemaSignature pidx, int dt_id, uint8_t vcate);
    int  addTemplate            (SchemaSignature pidx, int dt_id, uint8_t vcate);

public:
    /**
     * find the SchemaNode using given param 
     * @param k      key name string 
     * @param psign  parent SchemaSignature
     * @param dt_id  this SchemaNode DataType id 
     * @param cate   this SchemaNode value category
     * @return 0 success; <0 failed since path mis-match 
     */
    SchemaSignature findNode(const char *k, SchemaSignature psign, int dt_id, uint8_t cate);

    SchemaNode::hash_crange findNode(const char *k, SchemaSignature pidx)
    { return  m_name_map.equal_range(SchemaNode::HashKey(k, pidx)); }

public:
    /**
     * find SchemaTemplate using given param 
     * @param psign  parent SchemaSignature
     * @param dt_id  this SchemaNode DataType id 
     * @return 0 success; <0 failed since path mis-match 
     */
    SchemaSignature findTemplate(SchemaSignature psign, int dt_id, uint8_t cate);

    /**
     * get SchemaTemplate's key child SchemaNode 
     * @see addTemplate func 
     * @param tsign  SchemaTemplate SchemaSignature
     */
    SchemaSignature getTempKeySign  (SchemaSignature tsign) { return tsign + 1; }

    /**
     * get SchemaTemplate's value child SchemaNode 
     * @see addTemplate func 
     * @param tsign  SchemaTemplate SchemaSignature
     */
    SchemaSignature getTempValueSign(SchemaSignature tsign) { return tsign + 2; }

    /**
     * get SchemaTemplate's SchemaSignature by SchemaTemplate Key 
     * @param ksign  SchemaTemplate SchemaSignature
     */
    SchemaSignature getTempSignByKey(SchemaSignature ksign) { return ksign - 1; }

    /**
     * get SchemaTemplate's SchemaSignature by SchemaTemplate Value 
     * @param vsign  SchemaTemplate SchemaSignature
     */
    SchemaSignature getTempSignByVal(SchemaSignature vsign) { return vsign - 2; }

public:
    /**
     * get the lowest repeated SchemaNode index in SchemaPath
     * @param path  SchemaPath to check  
     * @return node index in SchemaPath
     */
    uint32_t getLowestRepeatedNodeIndex(SchemaPath &path);

public:
    /**
     * get node name from const text string 
     * @param name    SchemaNode name string 
     * @param text    const char text string 
     */
    static void getNameFromText(string &name, const char *text);

    /**
     * append the node's name to str 
     * @param str       path string name result 
     * @param path      SchemaPath on the path 
     * @return 0 success; <0 failed since path mis-match 
     */
    int appendPathName(string &str, SchemaPath &path);

    /**
     * append path with postfix 
     * @param str       path string name result 
     * @param path      SchemaPath on the path 
     * @return 0 success; <0 failed since path mis-match 
     */
    int appendPathWPost(string &str, SchemaPath &path);


public:
    /**
     * flush SchemaTree binary content to file 
     * @return 0 success; <0 failed
     */
    int flush(void);
    
    /**
     * load SchemaTree instance from binary content 
     * @return 1 success; 0 not defined; <0 failed
     */
    int load(void);
    
protected:
    /**
     * get dir path from SchemaTree info 
     * @return 0 success; <0 failed
     */
    void getDirPath(string &path);


public:
    /** output SchemaTree info to debug */
    void output2debug(void);

    /** output all SchemaPath to string */
    void outputPathes(void)
    {   SchemaPath sp; outputPathes4Node(sp, SchemaSignature(0));   }

protected:
    /** output all pathes to SchemaNode */
    void outputPathes4Node(SchemaPath &sp, SchemaSignature ss);


public:
    /** output in tree format */
    void output2tree(const string &name, SchemaSignature ss = 0);
}; // SchemaTree


} // namespace steed


#include "SchemaTree_inline.h"
