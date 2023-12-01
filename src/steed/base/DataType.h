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
 * @file DataType.h
 * @author  Zhiyi Wang <zhiyiwang@ict.ac.cn>
 * @version 1.0
 * @section DESCRIPTION
 *     definitions and functions for steed data types.
 */

#pragma once 

#include <assert.h>
#include <float.h>
#include <stdint.h>
#include <regex.h>
#include <string.h>

#include <vector>
#include <iostream>

#include "Allocator.h"
#include "SymbolMap.h"


namespace steed {
using std::vector; 


class DataType;

/** DataType copy function pointer */
typedef
void (DataType::*DTCopyFP) (const void *bin_in, void *bin_out); 

/**
 * macro 4 copy, T is data type 
 * @param T         type name, such as int8_t...
 * @param bin_in    input  binary value  
 * @param bin_out   output binary value 
 */
#define MACRO_COPY_FUNC(T) \
void copy(const void *bin_in, void *bin_out) override \
{ *(T*)bin_out = *(T*)bin_in; }


/** DataType single value compare with single value function pointer */
typedef
int (DataType::*DTCompareFP)(const void *left, const void *right);

/*
 * macro defined compare functions between single value
 * @param FUNC_NAME  func name 
 * @param T          type name, such as int8_t...
 * @param OP         compare operation, such as <, > ... 
 * @param l_bin      left  binary value  
 * @param r_bin      right binary value 
 * @return <0 error; 0 false; >0 true
 */
#define MACRO_COMPARE_FUNC(FUNC_NAME, T, OP)  \
int FUNC_NAME(const void *l_bin, const void *r_bin) override \
{ return ((*(T*)l_bin OP *(T*)r_bin)); }





/**
 * DataType base class definition
 *   STEED has only one global instance for each child class
 */
class DataType {
private:
    struct TypeDescStruct; // data type description
    struct CompareDescStruct; // compare operation
    struct OperateDescStruct; // algebraic operation

public:
    typedef struct TypeDescStruct    TypeDesc   ;
    typedef struct CompareDescStruct CompareDesc;

    static int       s_init; // = 1 after initialization
    static int       initStatic  (void);
    static int       uninitStatic(void);
    static DataType *getDataType (int dt_id);

    static const int s_type_invalid; // 'A'
    static const int s_type_boolean; // 'B'
    static const int s_type_int_8  ; // 'C'
    static const int s_type_int_16 ; // 'D'
    static const int s_type_int_32 ; // 'E'
    static const int s_type_int_64 ; // 'F'
    static const int s_type_tstamp ; // 'F'
    static const int s_type_float  ; // 'G'
    static const int s_type_double ; // 'H'
    static const int s_type_string ; // 'I'
    static const int s_type_bytes  ; // 'J'
    static const int s_type_max    ; // 'K'

    /** static type instances array */ 
    static DataType      *s_type_ins[]; 
    static SymbolMap<int> s_data_type_tab;    /**< map type name string to id */
    static const TypeDesc s_type_desc[];      /**< system type description    */


protected: 
    int    m_type_id {s_type_invalid}; /**< data type id */


public: 
    DataType(void)  : DataType (s_type_invalid) {}
    DataType(int tp): m_type_id(tp)             {}
    virtual ~DataType(void) = default;

public: 
    int         getTypeID  (void) { return m_type_id; }
    int         getDefSize (void);
    const char *getDefName (void);
    const char *getFormat  (void);
  
    bool        isInvalid  (void) { return (getDefSize() <  0);  }
    bool        isFixedType(void) { return (getDefSize() >  0);  } 
    bool        isVarType  (void) { return (getDefSize() == 0);  }

    bool        isPrimitive(void)
    {   return ((m_type_id > s_type_invalid) && (m_type_id < s_type_max));   }

public: // NULL value functions
    /**
     * get bin null binary
     * @return bin null content 
     */
    virtual const void* getBinNull(void)
    { return nullptr; }

    /**
     * fill null bin values to array  
     * @param bgn    bin value array begin 
     * @param num    elem number in array  
     */
    virtual void fillNull(void *bgn, uint64_t num)
    { (void)bgn; (void)num; }

public: // copy and size functions, trans between text and binary
    /**
     * binary content copy func 
     * @param in   copy in bin content from  
     * @param out  copy in bin to 
     */
    virtual void copy(const void *in, void *out) 
    { (void)in; (void)out; }
  
    /** 
     * get binary value size 
     * @param bin    binary value content
     * @result binary value used size
     */
    virtual int getBinSize(const void *bin) 
    { (void)bin; return -1;}
  
    /**
     * get binary value size using text value content 
     * @param txt    text value content as input
     * @return <0 failed; >=0 num of bytes used 
     */ 
    virtual int getBinSizeByTxt(const char *txt) 
    { (void)txt; return -1;}

    /**
     * trans text to binary value 
     * @param txt    text value content 
     * @param bin    binary value content after trans 
     * @param size   size available to trans
     * @return <0 failed, neg needed size; 0 null; >0 valued used 
     */
    virtual
    int transTxt2Bin(const char *txt, void *bin, uint64_t size) 
    { (void)txt; (void)bin; (void)size; return -1; }
  
    /**
     * trans binary to text value, including '\0' 
     * @param bin    binary value content to trans 
     * @param txt    text value content 
     * @param size   size available to trans
     * @return <0 failed, neg needed size; >=0 num of bytes used 
     */
    virtual
    int transBin2Txt(const void *bin, char *txt, uint64_t size) 
    { (void)bin; (void)txt; (void)size; return -1; }

    /**
     * trans binary to text value and output to ostream 
     * @param bin    binary value content to trans 
     * @param ostrm  output stream instance 
     * @return <0 failed; ==0 success
     */
    virtual
    int outputText2Stream(const void *bin, std::ostream &ostrm) 
    { (void)bin; (void)ostrm; return -1; }


public: // compare related funcs  
    /**
     * trans txt const to bin const, malloc memory for it 
     * @param fd    text string format 
     * @param txt    const text value string  
     * @return memory for const bin; nullptr as failed 
     */
    virtual const void *trans2BinConst   (const char *txt);
    virtual const void *trans2LikeConst  (const char *txt)
    { (void)txt; return nullptr; }

    virtual const void *trans2SubStrConst(const char *txt)
    { (void)txt; return nullptr; }

    /**
     * binary content compare func 
     * @param bin  bin value to single operation 
     * @param l    left compared binary value 
     * @param r    left compared binary value 
     * @return <0 failed; ==0 false; >0 true 
     */
    int         compareInvalid   (const void *l, const void *r)
    { (void)l, (void)r;  return -1; }

    virtual int compareIsNull    (const void *bin)
    { (void)bin; return -1; }
//    { return (bin == nullptr); }

    virtual int compareLess      (const void *l, const void *r)
    { (void)l, (void)r; return -1; }

    virtual int compareGreater   (const void *l, const void *r)
    { (void)l, (void)r; return -1; }

    virtual int compareEqual     (const void *l, const void *r)
    { (void)l, (void)r; return -1; }

    virtual int compareNotLess   (const void *l, const void *r)
    { (void)l, (void)r; return -1; }

    virtual int compareNotGreater(const void *l, const void *r)
    { (void)l, (void)r; return -1; }

    virtual int compareNotEqual  (const void *l, const void *r)
    { (void)l, (void)r; return -1; }

    // only string valid  
    virtual int compareLike      (const void *l, const void *ptn)
    { (void)l, (void)ptn; return -1; }

    virtual int compareSubString (const void *l, const void *sub)
    { (void)l, (void)sub; return -1; }


    static SymbolMap<int>    s_data_cmp_tab;        /**< cmp name symbol map */
    static const CompareDesc s_cmp_operation[]; /**< cmp op string array */

    static const int s_cmp_invalid  ; 
    static const int s_cmp_less     ;   // less
    static const int s_cmp_not_grt  ;   // not greater 
    static const int s_cmp_equal    ;
    static const int s_cmp_not_equal;
    static const int s_cmp_greater  ;
    static const int s_cmp_not_less ;
    static const int s_cmp_like     ;   // string only: pattern matched 
    static const int s_cmp_substr   ;   // stirng only: substring 
    static const int s_cmp_max      ;
  
    /** 
     * compared func for single with single 
     *     the func is decided the compare type and data type 
     * @param  cmp_op_id    compared operation id 
     * @return single compared single func for type ins
     */
    static DTCompareFP getCompareFunc(int cmp_op_id);
}; // DataType











// invalid type: nothing to implement
class TypeInvalid : public DataType {
public: 
    TypeInvalid (void) : DataType(s_type_invalid) {}
    ~TypeInvalid(void) {}
}; // TypeInvalid



// NT == numeric types: intXX, float, double 
template <class NT> 
class TypeNumeric : public DataType {
protected:
    static const NT s_null; /**< bin null value */ 

public: 
    TypeNumeric (int id) : DataType(id) {}
    ~TypeNumeric(void)   = default;

public: 
    const void* getBinNull(void) override { return &s_null; }
    void        fillNull  (void *bgn, uint64_t num) override;

public: 
    MACRO_COPY_FUNC(NT) 
  
    // all fixed size
    virtual int getBinSize     (const void *bin) override
    { (void)bin; return sizeof(NT); } 
    virtual int getBinSizeByTxt(const char *txt) override
    { (void)txt; return sizeof(NT); } 

    virtual int transTxt2Bin   (const char *txt, void *bin, uint64_t size) override;
    virtual int transBin2Txt   (const void *bin, char *txt, uint64_t size) override;
    virtual int outputText2Stream(const void *bin, std::ostream &ostrm)
    {   ostrm << *(const NT*)bin; return 0;   }

public: // single compare single funcs 
    int compareIsNull (const void *bin) override
    {   return ((bin == nullptr) || (*(NT*)bin == s_null)); }

    MACRO_COMPARE_FUNC(compareLess,       NT, < )
    MACRO_COMPARE_FUNC(compareGreater,    NT, > )
    MACRO_COMPARE_FUNC(compareEqual,      NT, ==)
    MACRO_COMPARE_FUNC(compareNotLess,    NT, >=)
    MACRO_COMPARE_FUNC(compareNotGreater, NT, <=)
    MACRO_COMPARE_FUNC(compareNotEqual,   NT, !=)
}; // TypeNumeric 



class TypeBoolean : public TypeNumeric<int8_t> {
public: 
    TypeBoolean (void) : TypeNumeric(s_type_boolean) {}
    ~TypeBoolean(void) = default;

public:
    int transTxt2Bin(const char *txt, void *bin, uint64_t size) override;
    int transBin2Txt(const void *bin, char *txt, uint64_t size) override;
    int outputText2Stream(const void *bin, std::ostream &ostrm) override;
}; // TypeBoolean



class TypeInt8 : public TypeNumeric<int8_t> {
public: 
    TypeInt8 (void) : TypeNumeric(s_type_int_8) {}
    ~TypeInt8(void) = default;
}; // TypeInt8 


class TypeInt16 : public TypeNumeric<int16_t> {
public: 
    TypeInt16 (void) : TypeNumeric(s_type_int_16) {}
    ~TypeInt16(void) = default;
};  // TypeInt16


class TypeInt32 : public TypeNumeric<int32_t> {
public: 
    TypeInt32 (void) : TypeNumeric(s_type_int_32) {}
    ~TypeInt32(void) = default;
}; // TypeInt32


class TypeInt64 : public TypeNumeric<int64_t> {
public: 
    TypeInt64 (void) : TypeNumeric(s_type_int_64) {}
    ~TypeInt64(void) = default;
}; // TypeInt64


class TypeFloat : public TypeNumeric<float> {
public: 
    TypeFloat (void) : TypeNumeric(s_type_float) {}
    ~TypeFloat(void) = default;
}; // TypeFloat


class TypeDouble : public TypeNumeric<double> {
public: 
    TypeDouble (void) : TypeNumeric(s_type_double) {}
    ~TypeDouble(void) = default;
}; // TypeDouble





// string binary == string content without format 
class TypeString : public DataType {
public: 
    TypeString (void) : DataType(s_type_string) {}
    ~TypeString(void) = default;

public:
    void copy(const void *in, void *out) override
    { strcpy((char *)out, (const char *)in); }
  
    int getBinSize(const void *bin) override
    { return (strlen((const char*)bin) + 1); } // additional '\0'

    int getBinSizeByTxt(const char *txt) override
    { return (strlen(txt) + 1 - 2); } // without begin and end '\"' delim 

    /** txt must be ended by '\0' */
    int transTxt2Bin(const char *txt, void *bin, uint64_t size) override;

    int transBin2Txt(const void *bin, char *txt, uint64_t size) override;

    int outputText2Stream(const void *bin, std::ostream &ostrm)
    {   ostrm << '"' << (const char*)bin << '"'; return 0;   }

public: 
    int compareIsNull    (const void *bin) override;
  
    int compareLess      (const void *value, const void *target_value) override
    { return (strcmp((const char*)value, (const char*)target_value) <  0); }
  
    int compareEqual     (const void *value, const void *target_value) override
    { return (strcmp((const char*)value, (const char*)target_value) == 0); }
  
    int compareGreater   (const void *value, const void *target_value) override
    { return (strcmp((const char*)value, (const char*)target_value) >  0); }
  
    int compareNotLess   (const void *value, const void *target_value) override
    { return (strcmp((const char*)value, (const char*)target_value) >= 0); }
  
    int compareNotEqual  (const void *value, const void *target_value) override
    { return (strcmp((const char*)value, (const char*)target_value) != 0); }
  
    int compareNotGreater(const void *value, const void *target_value) override
    { return (strcmp((const char*)value, (const char*)target_value) <= 0); }
  
    const void *trans2LikeConst  (const char *txt);
    const void *trans2SubStrConst(const char *txt) { return strdup(txt); }
  
    int         compareLike      (const void *l, const void *ptn);
    int         compareSubString (const void *l, const void *sub)
    { return (strstr((const char*)l, (const char*)sub) != nullptr); }
}; // TypeString



class TypeBytes : public DataType {
public: 
    TypeBytes (void) : DataType(s_type_bytes) {}
    ~TypeBytes(void) {}

private:
    void copy (const void *in, void *out) override { memcpy(out, in, getDefSize()); }
    int  getBinSize     (const void *bin) override
    { (void)bin; return getDefSize(); }

    int  getBinSizeByTxt(const char *txt) override
    { (void)txt; return getDefSize(); }

    int  transTxt2Bin (const char *txt, void *bin, uint64_t size);
    int  transBin2Txt (const void *bin, char *txt, uint64_t size);
    int  outputText2Stream (const void *bin, std::ostream &ostrm);

    int  compareIsNull(const void *bin) override 
    { (void)bin; return 0; }

    int  compareEqual (const void *val, const void *tgt) override;

private:
    int  fromHex(char c);
    char fromHex(const char* c);
}; // TypeBytes


} // namespace steed

#include "DataType_inline.h"