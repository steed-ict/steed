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
 * @file steed.cpp
 * @brief steed library
 * @version 1.0
 */

#include "steed.h"

// Python C extension module (Python can import it as a module)
// It's a wrapper function that translates Python objects to C types,
//   calls the C/C++ function, and then converts the result back to a Python object
// $ python setup.py build_ext --inplace
#define PY_SSIZE_T_CLEAN
#include <Python.h>

extern "C" {

// Python wrapper functions for make_clean 
PyObject *py_make_clean(PyObject *self, PyObject *args)
{
    make_clean();
    Py_RETURN_NONE;
} // py_make_clean


// Python wrapper functions for free_string
PyObject *py_free_string(PyObject *self, PyObject *args)
{
    char *str = nullptr;
    if (!PyArg_ParseTuple(args, "s", &str))
    {   return nullptr;   }

    free_string(str);
    Py_RETURN_NONE;
} // py_free_string


// Python wrapper functions for init
PyObject *py_init(PyObject *self, PyObject *args)
{
    char *cfile = nullptr;
    if (!PyArg_ParseTuple(args, "s", &cfile))
    {   return nullptr;   }

    init(cfile);
    Py_RETURN_NONE;
} // py_init


// Python wrapper functions for uninit
PyObject *py_uninit(PyObject *self, PyObject *args)
{
    uninit();
    Py_RETURN_NONE;
} // py_uninit


// Python wrapper functions for create_database
PyObject *py_create_database(PyObject *self, PyObject *args)
{
    char *db = nullptr;
    if (!PyArg_ParseTuple(args, "s", &db))
    {   return nullptr;   }

    int status = create_database(db);
    return Py_BuildValue("i", status);
} // py_create_database


// Python wrapper functions for drop_database
PyObject *py_drop_database(PyObject *self, PyObject *args)
{
    char *db = nullptr;
    if (!PyArg_ParseTuple(args, "s", &db))
    {   return nullptr;   }

    int status = drop_database(db);
    return Py_BuildValue("i", status);
} // py_drop_database


// Python wrapper functions for create_table
PyObject *py_create_table(PyObject *self, PyObject *args)
{
    char *db = nullptr, *table = nullptr;
    if (!PyArg_ParseTuple(args, "ss", &db, &table))
    {   return nullptr;   }

    int status = create_table(db, table);
    return Py_BuildValue("i", status);
} // py_create_table


// Python wrapper functions for drop_table
PyObject *py_drop_table(PyObject *self, PyObject *args)
{
    char *db = nullptr, *table = nullptr;
    if (!PyArg_ParseTuple(args, "ss", &db, &table))
    {   return nullptr;   }

    int status = drop_table(db, table);
    return Py_BuildValue("i", status);
} // py_drop_table


// Python wrapper functions for parse_file
PyObject *py_parse_file(PyObject *self, PyObject *args)
{
    char *db = nullptr, *table = nullptr, *jpath = nullptr;
    if (!PyArg_ParseTuple(args, "sss", &db, &table, &jpath))
    {   return nullptr;   }

    int status = parse_file(db, table, jpath);
    return Py_BuildValue("i", status);
} // py_parse_file


// Python wrapper functions for assemble_to_file  
PyObject *py_assemble_to_file(PyObject *self, PyObject *args)
{
    char *db = nullptr, *table = nullptr, *jpath = nullptr;
    PyObject *py_cols = nullptr;
    if (!PyArg_ParseTuple(args, "ssO!s", &db, &table, &PyList_Type, &py_cols, &jpath))
    {   return nullptr;   }

    int ncol = PyList_Size(py_cols);
    const char **cols = new const char*[ncol];
    for (int ci = 0; ci < ncol; ++ci)
    {
        PyObject *py_col = PyList_GetItem(py_cols, ci);
        cols[ci] = (char *)PyUnicode_AsUTF8(py_col);
    } // for ci

    int status = assemble_to_file(db, table, cols, ncol, jpath);
    delete [] cols; cols = nullptr;
    return Py_BuildValue("i", status);
} // py_assemble_to_file


// Python wrapper functions for assemble_to_string
PyObject *py_assemble_to_string(PyObject *self, PyObject *args)
{
    char *db = nullptr, *table = nullptr;
    PyObject *py_cols = nullptr;
    if (!PyArg_ParseTuple(args, "ssO!", &db, &table, &PyList_Type, &py_cols))
    {   return nullptr;   }

    int ncol = PyList_Size(py_cols);
    const char **cols = new const char*[ncol];
    for (int ci = 0; ci < ncol; ++ci)
    {
        PyObject *py_col = PyList_GetItem(py_cols, ci);
        cols[ci] = PyUnicode_AsUTF8(py_col);
    } // for ci

    const char *str = assemble_to_string(db, table, cols, ncol);
    delete [] cols; cols = nullptr;

    PyObject *ret = Py_BuildValue("s", str);
    free_string((char *)str);
    return ret;
} // py_assemble_to_string



//// Column Parser API /////////////////////////////////////////////////

// template struct to hold a pointer:
// 1. avoid double free the steed instance
struct SteedColumnParserIns
{
    steed::ColumnParser *ptr{nullptr}; 
    bool valid{false};  

    ~SteedColumnParserIns() = default;
    SteedColumnParserIns () = default;
    SteedColumnParserIns (steed::ColumnParser *p): ptr(p), valid(true) {}
};


// delete pointer instance 
void delete_steed_instance(PyObject* capsule)
{
    SteedColumnParserIns *si =
        (SteedColumnParserIns *)PyCapsule_GetPointer(capsule, "Steed ColumnParser");
    if (si && si->valid)
    {
        delete si->ptr;
        si->ptr = nullptr;
        si->valid = false;
    } // if
} // delete_steed_instance


// Python wrapper functions for open_parser
PyObject *py_open_parser(PyObject *self, PyObject *args)
{
    char *db = nullptr, *table = nullptr;

    // parse Python arguments
    // "ss" means two strings: one char for each string
    if (!PyArg_ParseTuple(args, "ss", &db, &table))
    {   return nullptr;   }

    // create a ColumnParser object
    // create a Python capsule object to hold the ColumnParser pointer
    steed::ColumnParser *cp = open_parser(db, table);
    printf("STEED: open parser [%s.%s] at %p\n", db, table, cp);

    // hold the ColumnParser pointer
    SteedColumnParserIns *si = new SteedColumnParserIns(cp);
    return PyCapsule_New(si, "Steed ColumnParser", delete_steed_instance);
} // py_open_parser


// Python wrapper functions for insert_parser
PyObject *py_insert_parser(PyObject *self, PyObject *args)
{
    // parse Python arguments
    PyObject *pyins = nullptr;
    Py_buffer pybuf = {nullptr, 0};  // Py_buffer for Python bytes object
    if (!PyArg_ParseTuple(args, "Oy*", &pyins, &pybuf))
    {
        PyErr_SetString(PyExc_TypeError, "Parameter parsing failed.");
        return nullptr;
    } // if 

    // get ColumnParser pointer from Python capsule object
    SteedColumnParserIns *si =
        (SteedColumnParserIns*)PyCapsule_GetPointer(pyins, "Steed ColumnParser");
//    printf("STEED: SteedInstance:%p ColumnParser:%p\n", si, si->ptr);
//    printf("Py_buffer: len = %ld, buf = %p\n", pybuf.len, pybuf.buf);
    if (!si || !si->valid)
    {
        PyErr_SetString(PyExc_TypeError, "Invalid ColumnParser object.");
        PyBuffer_Release(&pybuf);
        return nullptr;
    } // if

    // insert record to ColumnParser
    steed::ColumnParser *cp = si->ptr;
    int status = insert_parser(cp, (char *)pybuf.buf, pybuf.len);
    PyBuffer_Release(&pybuf);

    return Py_BuildValue("i", status);
} // py_insert_parser


// Python wrapper functions for close_parser
PyObject *py_close_parser(PyObject *self, PyObject *args)
{
    PyObject *pyins = nullptr;
    if (!PyArg_ParseTuple(args, "O", &pyins))
    {
        PyErr_SetString(PyExc_TypeError, "Parameter parsing failed.");
        return nullptr;
    } // if

    delete_steed_instance(pyins);

    Py_RETURN_NONE;
} // py_close_parser



//// Python Module Definition //////////////////////////////////////////

// Python method definition
PyMethodDef steed_methods[] = {
    // python function name, C function name, argument type, doc string
    {"make_clean"        , py_make_clean      , METH_VARARGS, "make clean"},
    {"free_string"       , py_free_string     , METH_VARARGS, "free string"},
    {"init"              , py_init            , METH_VARARGS, "init steed"},
    {"uninit"            , py_uninit          , METH_VARARGS, "uninit steed"},
    {"create_database"   , py_create_database , METH_VARARGS, "create database"},
    {"drop_database"     , py_drop_database   , METH_VARARGS, "drop database"},
    {"create_table"      , py_create_table    , METH_VARARGS, "create table"},
    {"drop_table"        , py_drop_table      , METH_VARARGS, "drop table"},
    {"parse_file"        , py_parse_file      , METH_VARARGS, "parse json file"},
    {"assemble_to_file"  , py_assemble_to_file, METH_VARARGS, "assemble json to file"},
    {"assemble_to_string", py_assemble_to_string, METH_VARARGS, "assemble json to string"},
    {"open_parser"       , py_open_parser     , METH_VARARGS, "open column parser"},
    {"insert_json_object", py_insert_parser   , METH_VARARGS, "insert record to parser"},
    {"close_parser"      , py_close_parser    , METH_VARARGS, "close column parser"},
    {nullptr, nullptr, 0, nullptr}
}; // steed_methods


// Python module definition
struct PyModuleDef steed_module = {
    PyModuleDef_HEAD_INIT,
    "pysteed",
    "steed module",
    -1,
    steed_methods
}; // steed_module


// Python module initialization
PyMODINIT_FUNC PyInit_pysteed(void)
{
    return PyModule_Create(&steed_module);
} // PyInit_pysteed


} // extern "C"