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
 * @file JSONRecordNaiveParser.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 * JSON naive text record parser: parse by checking chars
 */

#pragma once 

#include "JSONRecordParser.h"


namespace steed {

class JSONRecordNaiveParser : public JSONRecordParser {
public:
    JSONRecordNaiveParser (void) = default;
    ~JSONRecordNaiveParser(void) = default; 

public:
    /**
     * read one json text record from buffer and parse to JSONBinTree
     * @param bt    json binary tree for one json text record 
     * @param c     json text content begin address
     * @return >0 success; 0 end of content; <0 failed 
     */
    int parse(JSONBinTree* &bt, char* &c) override
    { return parseObject(bt, JSONBinField::Index(0), c); }

protected:
    /**
     * parse json struct 
     * @param jbt  JSONBinTree  instance 
     * @param idx  JSONBinField index in jbt
     * @param c    json content begin, must be valid (not space) 
     * @param kbgn key name c-string
     * @return skip number, 1 success; 0 end of content; <0 failed
     */
    int parseObject(JSONBinTree* jbt, JSONBinField::Index idx, char* &c);
    int parseArray (JSONBinTree* jbt, JSONBinField::Index idx, char* &c);
    int parseValue (JSONBinTree* jbt, JSONBinField::Index idx, char* &c,
            char &delim, char *kbgn);

    /**
     * skip json primitive value
     * @param c  value content begin 
     * @return >0 success; 0 end of content; <0 failed
     */
    int skipWhitespace(char* &c);
    int skipString    (char* &c);
    int skipNumber    (char* &c);
    int skipFalse     (char* &c);
    int skipTrue      (char* &c);
    int skipNull      (char* &c);
    int skipChars     (char* &c, uint64_t num);
}; // JSONRecordNaiveParser





inline
int JSONRecordNaiveParser::skipWhitespace(char* &c)
{
    if (*c == '\0') { return 0; }

    while (isspace(*c++));
    
    int skip = 1; 
    if (*(--c) == '\0') { skip = 0; }
    return skip; 
} // skipWhitespace



inline
int JSONRecordNaiveParser::skipString (char* &c)
{
    if ((c == nullptr) || (*c++ != '\"')) { return -1; }

    bool   cont = true; // is string content 
    while (cont)
    {
        switch (*c++)
        {
            case '\0':  
            case '"' :  cont = false;  break; // delim 
            case '\\':  ++c;           break;
            default  :  break;  
        } // switch
    } // while 

    int skip = 1;
    if (*(c-1) == '\0') { skip = 0; --c; }
    return skip; 
} // skipString 



inline
int JSONRecordNaiveParser::skipNumber (char* &c)
{
    do 
    {
        if (isdigit(*c)) { ++c; continue; }

        switch (*c++)
        {
            case '+':  case '-':  case '.':
            case 'e':  case 'E':  continue;
            default :  break;  
        } // switch

        break;
    } while (true);

    int skip = 1;
    if (*(--c) == '\0') { skip = 0; }
    return skip; 
} // skipString 



inline
int JSONRecordNaiveParser::skipFalse(char* &c)
{   return skipChars(c, 5); } // false



inline
int JSONRecordNaiveParser::skipTrue(char* &c)
{   return skipChars(c, 4); } // true



inline
int JSONRecordNaiveParser::skipNull (char* &c)
{   return skipChars(c, 4); } // null



inline
int JSONRecordNaiveParser::skipChars (char* &c, uint64_t num)
{
    uint64_t skipped = 0;
    while   (skipped < num)
    {
        if (*c++ == '\0') { --c;  break; }
    
        ++skipped;
    } // while 
    return (skipped == num) ? 1 : 0;
} // skipChars
 
} // namespace steed
