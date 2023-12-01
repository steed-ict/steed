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
 * @file DataType_inline.h
 * @author  Zhiyi Wang <wangzhiyi@ict.ac.cn>
 * @version 0.4 
 * @section DESCRIPTION
 *   DataType inline functions
 */


#pragma once 

#include <stdlib.h>


namespace steed {


struct DataType::TypeDescStruct {
    const char *name{nullptr}; /**< data type name string */
    const char *fmt {nullptr}; /**< default text format   */
    int         id  {0};       /**< data type id          */
    int         size{0};       /**< fixed > 0; var = 0; invalid < 0 */

    TypeDescStruct (const char *n, const char *f, int i, int s)
        : name(n), fmt(f), id(i), size(s)
    {}
}; // TypeDescStruct


struct DataType::CompareDescStruct {
    const char *name{nullptr}; /**< comparison name string */
    int         id  {0};       /**< comparison type ID     */

    CompareDescStruct (const char *n, int i)
        : name(n), id(i)
    {}
}; // CompareDescStruct





inline
DataType *DataType::getDataType(int dt_id)
{ return ((dt_id < s_type_max) ? s_type_ins[dt_id] : nullptr); }

inline
int         DataType::getDefSize(void)
{ return s_type_desc[m_type_id].size; }

inline
const char *DataType::getDefName(void)
{ return s_type_desc[m_type_id].name; }

inline
const char *DataType::getFormat (void)
{ return s_type_desc[m_type_id].fmt;  }


inline
const void* DataType::trans2BinConst(const char *txt)
{
    int bs = this->getBinSizeByTxt(txt);
    if (bs < 0)   { return nullptr; } // invalid

    void *ret = steedMalloc(bs); 
    int  used = transTxt2Bin(txt, ret, bs);
    if  (used < 0)
    {
        printf("DataType: trans const text to bin failed!\n");
        free(ret);  ret = nullptr;
    } // if 

    return ret;
} // trans2BinConst





template<class NT>  
inline void TypeNumeric<NT>::fillNull(void *bgn, uint64_t num)
{
    NT* dst = (NT*)bgn;

    bool alinged_access = ((uint64_t(bgn) % sizeof(NT)) == 0); 
    if  (alinged_access)
    {
        for (uint64_t ei = 0; ei < num; ++ei)
        {   dst[ei] = s_null;   }
    }
    else
    {
        // BUG: directly set NT content got segmentation fault @ gcc O3 
        // Unaligned Memory Accesses: 
        //   m_offs may be not evenly divisible by sizeof(OT)
        // @ see https://www.kernel.org/doc/html/latest/process/unaligned-memory-access.html
        // instead
        uint64_t cplen = num * sizeof(NT);
        vector<NT>  trans(num, s_null); 
        memcpy(dst, trans.data(), cplen);
    } // if 
} // fillNull


template<class NT>  
inline int TypeNumeric <NT>::
    transTxt2Bin(const char *txt, void *bin, uint64_t size)
{
    if (txt == nullptr)    { return  0; } // null value 
    if (size < sizeof(NT)) { return -1; } // space not enough

    int used = -1;
    const char *fmt = s_type_desc[m_type_id].fmt;
    if (sscanf(txt, fmt, (NT*)bin) == 1) 
    {   used = this->getBinSize(bin);   }
    return used;
} // transTxt2Bin


template<class NT>  
inline int TypeNumeric <NT>::
    transBin2Txt(const void *bin, char *txt, uint64_t size)
{
    int len =  0;
    if (bin == nullptr)
    {   len = snprintf(txt, size, "%s", "null")   + 1;   }
    else
    {
        const char *fmt = s_type_desc[m_type_id].fmt;
        len = snprintf(txt, size, fmt, *(NT*)bin) + 1; 
    }
    return (len < (int)size) ? len : -1;
} // transBin2Txt 



inline
int TypeBoolean::transTxt2Bin(const char *txt, void *bin, uint64_t size) 
{
    if (txt == nullptr)      { return  0; } // null value 
    if (size < sizeof(char)) { return -1; } // space not enough

    int retval = -1;
    if (strcmp(txt, "true" ) == 0)
    {   *(uint8_t*)bin = 1; retval = 1;   }
    else if(strcmp(txt, "false") == 0)
    {   *(uint8_t*)bin = 0; retval = 1;   }
    return retval;
}  // transTxt2Bin



inline
int TypeBoolean::transBin2Txt(const void *bin, char *txt, uint64_t size)
{
    std::ignore = size;

    int retval = -1;
    if (*(uint8_t*)bin == 1)
    {   retval = 5; memcpy(txt, "true" , retval);  }
    else if (*(uint8_t*)bin == 0)
    {   retval = 6; memcpy(txt, "false", retval);  }
    return retval;
}  // transBin2Txt



inline
int TypeBoolean::outputText2Stream(const void *bin, std::ostream &ostrm)
{
    ostrm << ((*(uint8_t*)bin == 1) ? "true" : "false");
    return 0; 
} // outputText2Stream





inline
int TypeString::transTxt2Bin(const char *txt, void *bin, uint64_t size)
{
    int   ret_val = -1;
    uint64_t blen = getBinSizeByTxt(txt);
    bool enough = (blen <= size);
    if  (enough)
    {
        strncpy((char*)bin, txt + 1, blen); // +1: skip begin '"'
        *((char*)bin + blen - 1) = '\0';    // blen - 1: char number to index
        ret_val = (int)blen;
    }
    return ret_val;
} // transTxt2Bin



inline
int TypeString::transBin2Txt(const void *bin, char *txt, uint64_t size)
{
    int      ret_val = -1; 
    const char *val  = (bin == nullptr) ? "null" : (const char *)bin;
    uint64_t    blen = getBinSize(val);  
    bool enough = (blen + 2 <= size); // text len == bin len + '"' delims
    if  (enough) 
    {
        strncpy(txt + 1, val, blen - 1); // blen includes '\0' 
        txt[0] = '"', txt[blen] = '"', txt [blen+1] = '\0';
        ret_val = (int)blen + 2;
    }
    return ret_val;
} // transBin2Txt



inline
int TypeString::compareIsNull(const void *bin)
{
    int retval = 0; // false 
    if ((bin == nullptr)            ||            // no    cont 
        (*(const char*)bin == '\0') ||            // empty cont 
        (strcmp((const char *)bin, "null") == 0)) // null  cont 
    { retval = 1; }
    return retval;
} // compareIsNull



inline
const void *TypeString::trans2LikeConst(const char *txt)
{
    regex_t *reg= new regex_t();// convert txt pattern to regex_t ins 
    int  cflags = REG_EXTENDED | REG_NEWLINE | REG_NOSUB;
    char ebuf[4096]; // error buffer 
    int  state = regcomp(reg, txt, cflags);
    if (state != 0)
    {
        regerror(state, reg, ebuf, sizeof(ebuf));
        printf("%s: txt '%s' \n", ebuf, txt);
        printf("%s: txt '%s' \n", ebuf, txt);
        regfree(reg);
        delete reg; reg = NULL; 
    } 
    return reg;
} // trans2LikeConst 



inline
int TypeString::compareLike (const void *l, const void *pattern)
{
    const char *cmp_str = (const char *)l;
    char ebuf[4096]; // error buffer 
    regex_t *reg = (regex_t*)pattern;
    int  ret_val = regexec(reg, cmp_str, 0, NULL, 0);
    switch (ret_val)
    {
        case 0          : ret_val = 1; break; // pattern matched 
        case REG_NOMATCH: ret_val = 0; break; // pattern not matched  
        default : 
          regerror(ret_val, reg, ebuf, sizeof(ebuf));
          printf("string regular failed %s!\n", ebuf);
          ret_val = -1;
    } // state
  
    return ret_val;
} // compareLike





inline
int TypeBytes::transTxt2Bin(const char *txt, void *bin, uint64_t size)
{
    if ((int)size < getDefSize()) { return -getDefSize(); }

    char* bc = (char *)bin; // binary content 
    for (int i = 0; i < getDefSize(); i++)
    {   bc[i] = fromHex(txt); txt += 2;   }

    return getDefSize();
} // transTxt2Bin



inline
int TypeBytes::transBin2Txt(const void *bin, char *txt, uint64_t size)
{
    int txt_len = getDefSize() * 2; // 2 chars for each byte
    if (txt_len > (int)size)  { return -txt_len; }

    const char *bc = (const char*)bin;
    const char  hexchars[] = "0123456789abcdef";
    for (int i = 0; i < getDefSize(); ++i) // size() = 12
    {
        char c = bc[i];
        char hi = hexchars[(c & 0xF0) >> 4];
        char lo = hexchars[(c & 0x0F)];
        *txt++= hi; *txt++= lo; 
    }

    return txt_len; 
} // transBin2Txt



inline
int TypeBytes::outputText2Stream(const void *bin, std::ostream &ostrm)
{
    uint32_t bin_size = getDefSize();
    uint32_t txt_size = bin_size * 2;
    char txt[txt_size]= {'\0'};
    transBin2Txt(bin, txt, txt_size);
    ostrm << txt;
    return txt_size;
} // outputText2Stream



inline
int TypeBytes::compareEqual (const void *val, const void *tgt)
{ 
    bool is_eq = true;
    const char* rc = (const char*)val;
    const char* cc = (const char*)tgt;
    for (int i = 0; i < getDefSize(); ++i)
    {   is_eq &= (rc[i] == cc[i]);   }
    return is_eq;
} // compareEqual



inline
int TypeBytes::fromHex(char c)
{
    if ('0' <= c && c <= '9') return c - '0'     ;
    if ('a' <= c && c <= 'f') return c - 'a' + 10;
    if ('A' <= c && c <= 'F') return c - 'A' + 10;
    return 0xff;
} // fromHex



inline
char TypeBytes::fromHex(const char* c)
{   return ((fromHex(c[0]) << 4) | fromHex(c[1]));   }

} // namespace steed 