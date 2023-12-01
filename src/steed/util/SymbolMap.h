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
 * @file SymbolMap.h
 * @version 1.0
 * @section DESCRIPTION
 *   implement the functionality of mapping a set of symbols to integer ID
 *   ID 0 is always reserved for invalid.
 *
 * Usage:
 *   SymbolMap  mymap;
 *   mymap.insert("int8", 1);
 *   mymap.erase ("fixed32");
 *   // lookup
 *   mymap["int8"]
 */

#pragma once 

#include <stdint.h>
#include <string>
#include <iostream>
#include <unordered_map>


namespace steed {

using std::cout;
using std::string;
using std::unordered_map;

template <class T>
class SymbolMap {
public: 
    typedef typename unordered_map<string,T>::const_iterator const_iterator;
    typedef typename unordered_map<string,T>::iterator       iterator      ;

private:
    unordered_map <string, T> m_map; 
    static const uint32_t     s_cap = 32; /**< default capacity */

public:
    SymbolMap (uint32_t num = s_cap) : m_map(num) {}
    ~SymbolMap(void) = default; 

public:
    void     clear (void) { m_map.clear(); }
    bool     empty (void) { return m_map.empty(); }

    void     insert(const string &k, T &v) { m_map[k] = v;   }
    void     erase (const string &k)       { m_map.erase(k); }
    iterator erase (const_iterator pos)    { return m_map.erase(pos); }

    uint32_t       count (const string& k)       { return m_map.count(k); }
    const_iterator find  (const string& k) const { return m_map.find (k); }
    const_iterator operator[] (const string& k) const  { return find (k); }
  
    const_iterator cbegin  () { return m_map.cbegin(); }
    const_iterator cend    () { return m_map.cend  (); }
    iterator       begin   () { return m_map.begin (); }
    iterator       end     () { return m_map.end   (); }

public: 
    void show(void);
}; // SymbolMap



template <class T> inline
void SymbolMap <T>::show(void)
{
    for (auto& elem : m_map) 
    {
        cout << "K" << " -> " << "V : "; 
        cout << "[" << elem.first << "]->[" << elem.second << "]" << std::endl; 
    } // for 
    cout << "--------------------------------------" << std::endl << std::endl;
} // show 

} //  namespace steed 
