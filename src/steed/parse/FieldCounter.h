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
 * @file FieldCounter.h
 * @author Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *    definitions and functions for FieldCounter:
 *    calc field's appeared count in SchemaTree 
 */
#pragma once 

#include <stdint.h>
#include <string>
#include <functional>  // hash
#include <unordered_map>

#include "SchemaNode.h"


namespace steed {

using std::hash;   // hash template func 
using std::string; 
using std::unordered_map; 



class FieldCounter {
public:
    class HashKey {
    public:
        string             m_name {""}; /**< appeared name string  */
        SchemaSignature    m_parent{0}; /**< parent SchemaSignature*/
    
    public:
        ~HashKey(void) = default; 
        HashKey (void) = default; 
        HashKey (const string  &k,  SchemaSignature p): m_name(k), m_parent(p) {}
        HashKey (const HashKey &h): m_name(h.m_name), m_parent(h.m_parent)     {}
        void output2debug(void) const
        { printf("FieldCounter [%s] parent @ [%u]", m_name.c_str(), m_parent); }
    }; // HashKey

    
    class Hasher {
    protected:
        hash<string>           m_hash_str ;
        hash<SchemaSignature>  m_hash_sign;
    
    public:
        Hasher (void) = default; 
        ~Hasher(void) = default; 
        size_t operator() (HashKey const& h) const
        { return m_hash_str(h.m_name) + m_hash_sign(h.m_parent); }
    }; // Hasher
    
    
    class KeyEqual {
    public:
        bool operator() (const HashKey & l, const HashKey & r) const
        { return ((l.m_parent == r.m_parent) && (l.m_name.compare(r.m_name) == 0)); }
    }; // KeyEqual

    
    /**
     * use hash table to count field's appearance  
     * KEY:   parent SchemaSignature 
     * VALUE: field appeared counter  
     */
    typedef unordered_map<HashKey, uint64_t, Hasher, KeyEqual> HashTable;


public:
    HashTable    m_counter; /**< field appeared counter */

public:
    FieldCounter (void) = default;
    ~FieldCounter(void) = default;

public:
    /**
     * update counter 
     * @param psign   parent SchemaNode SchemaSignature in SchemaTree 
     * @param name    field key name string  
     */
    void update(SchemaSignature psign, const string &name);

    void clear (void) { m_counter.clear(); } 

public: 
    void output2debug(void);
}; // FieldCounter





inline
void FieldCounter::update(SchemaSignature psign, const string &name)
{
    HashKey  hkey(name, psign);
    auto got =  m_counter.find(hkey);
    if  (got == m_counter.end())
    {   m_counter[hkey] = 1;   }
    else 
    {   got->second    += 1;   }
} // update 



inline
void FieldCounter::output2debug(void)
{
    puts("\t--------------------------------");
    printf("\t| [Parent SchemaSignature]+[Name String]:[Appeared Number] |\n");
    for (auto & s : m_counter)
    {
        printf("\t|  [%X,%u]+[%s]:[%lu]  |\n", s.first.m_parent, s.first.m_parent,
            s.first.m_name.c_str(), s.second);
    }
    puts("\t--------------------------------");
} // output2debug

} // namespace steed 
