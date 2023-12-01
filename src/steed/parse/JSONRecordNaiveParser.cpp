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
 * @file JSONRecordNaiveParser.cpp
 * @author  Zhiyi Wang  <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *   definitions and functions for DataType.
 */

#include <ctype.h>
#include <stdio.h>

#include "JSONRecordNaiveParser.h"


namespace steed {

int JSONRecordNaiveParser::
    parseObject(JSONBinTree* jbt, JSONBinField::Index idx, char* &c)
{
    int s = skipWhitespace(c); 
    if (s <= 0)  { return s; } 
    
    char delim  = *c++; // next delimiter
    if  (delim != '{') 
    {
        printf("JSONRecordNaiveParser: wrong OBJECT begin delim!\n");
        printf("%s\n", --c);
        fflush(nullptr);
        abort();
        return -1; 
    }

    s = skipWhitespace(c);  
    if (s <= 0)  { return s; } 

    delim = *c; // check "{}"

    while (true)
    {
        // end  parse done
        if (delim == '}') { ++c; return 1; }
    
        // parse next key value pair 
        char *key = c; 
        s = skipString(c);
        char *keyend = c; // key end 
        if (s <= 0) { return s; } 
        

        s = skipWhitespace(c);  
        if (s <= 0) { return s; } 

        delim = *c++;
        if (delim == ':') { *keyend = '\0'; }
        else
        {
            printf("JSONRecordNaiveParser: miss \':\'!\n");
            return -1;
        }
        
        
        s = skipWhitespace(c);  
        if (s <= 0)  { return s; } 
        
        JSONBinField::Index cidx = jbt->getNextChild(idx);
        s = parseValue(jbt, cidx, c, delim, key);
        if (s <= 0) { return s; }

        
        if (delim == ',')
        {
            ++c; 
            s = skipWhitespace(c);  
            if (s <= 0)  { return s; } 
        }
        else if (delim != '}')
        {
            printf("JSONRecordNaiveParser: wrong OBJECT end delim!\n");
            return -1; 
        }
    } // while 

    return -1; 
} // parseObject





//DEL int JSONRecordNaiveParser::parseArray (JSONElement* jelem, char* &c)
int JSONRecordNaiveParser::
    parseArray (JSONBinTree* jbt, JSONBinField::Index idx, char* &c)
{
    int s = skipWhitespace(c); 
    if (s <= 0)  { return s; } 
    
    char delim  = *c++; // next delimiter
    if  (delim != '[') 
    {
        printf("JSONRecordNaiveParser: wrong ARRAY begin delim!\n");
        return -1;
    }

    s = skipWhitespace(c);  
    if (s <= 0)  { return s; } 

    delim = *c; // check "[]"

    // element index 
    uint32_t ei = 0; 
    while (true)
    {
        assert(ei < s_elem_cap);

        // end  parse done
        if (delim == ']') { ++c; return 1; }
        
        JSONBinField::Index cidx = jbt->getNextChild(idx);
        char *key = (char*) s_idx_str[ei++];
        s = parseValue(jbt, cidx, c, delim, key);
        if (s <= 0) { return s; } 


        if (delim == ',')
        {
            ++c; 
            s = skipWhitespace(c);  
            if (s <= 0)  { return s; } 
        }
        else if (delim != ']')
        {
            printf("JSONRecordNaiveParser: wrong ARRAY end delim!\n");
            return -1; 
        }
    } // while 

    return -1; 
} // parseArray





int JSONRecordNaiveParser::
    parseValue(JSONBinTree* jbt, JSONBinField::Index idx,
            char* &c, char &delim, char *kbgn)
{
    JSONBinField *jbf = jbt->getNode(idx); // JSONBinField
    int      s = -1; 
    char    *value  = c; 
    char    *valend = nullptr; // value end 
    uint8_t  ctype = JSONType::type(value); // value's json type  
    switch  (ctype)
    {
        case JSONType::s_object:
             jbf->set(ctype, kbgn);
             s = parseObject(jbt, idx, c);
             break;
        
        case JSONType::s_array :
             jbf->set(ctype, kbgn);
             s = parseArray (jbt, idx, c);
             break; 
        
        case JSONType::s_string: s = skipString(c); valend = c; break;
        case JSONType::s_number: s = skipNumber(c); valend = c; break; 
        case JSONType::s_true  : s = skipTrue  (c); valend = c; break; 
        case JSONType::s_false : s = skipFalse (c); valend = c; break;
        case JSONType::s_null  : s = skipNull  (c); valend = c; break; 

        default: s = -1;  break; // illegal json type 
    } // switch

    // buffer next delimiter then modify the record content  
    s = skipWhitespace(c);  
    if (s <= 0) { return s; } 

    delim = *c; 

    if  ((JSONType::isPrimitive(ctype)) || (JSONType::isNull(ctype)))
    {
        *valend = '\0';
        jbf->set(ctype, kbgn, value);
    } 

    return 1;
} // parseValue 

} // namespace steed 
