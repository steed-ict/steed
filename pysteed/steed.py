from __future__ import print_function # support print() in python2

import ctypes
import json
import os

libsteed = None

## load steed library
def load_library():
    global libsteed
    try:
        libsteed = ctypes.cdll.LoadLibrary("/usr/local/lib/libsteed.so")
    except OSError:
        print("Error: cannot load library libsteed.so")
        exit(1)

## remove steed storage base dir
def make_clean():
    libsteed.make_clean.argtypes = []
    libsteed.make_clean.restype = None
    libsteed.make_clean()    

## import steed functions 
# void init  (const char *cfile)
def init(conf):
    libsteed.init.argtypes = [ctypes.c_char_p]
    libsteed.init.restype = None
    libsteed.init(conf.encode("utf-8"))

# void uninit(void)
def uninit():
    libsteed.uninit.argtypes = []
    libsteed.uninit.restype = None
    libsteed.uninit

# int create_database(const char *db);
def create_database(db):
    libsteed.create_database.argtypes = [ctypes.c_char_p]
    libsteed.create_database.restype = ctypes.c_int
    return libsteed.create_database(db.encode("utf-8"))
    
# int drop_database  (const char *db);
def drop_database(db):
    libsteed.drop_database.argtypes = [ctypes.c_char_p]
    libsteed.drop_database.restype = ctypes.c_int
    return libsteed.drop_database(db.encode("utf-8"))

# int create_table   (const char *db, const char *table);
def create_table(db, table):
    libsteed.create_table.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    libsteed.create_table.restype = ctypes.c_int
    return libsteed.create_table(db.encode("utf-8"), table.encode("utf-8"))

# int drop_table     (const char *db, const char *table);
def drop_table(db, table):
    libsteed.drop_table.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    libsteed.drop_table.restype = ctypes.c_int
    return libsteed.drop_table(db.encode("utf-8"), table.encode("utf-8"))

# int parse_file(const char *db, const char *table, const char *jpath)
def parse_file(db, table, jpath):
    libsteed.parse_file.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    libsteed.parse_file.restype = ctypes.c_int
    return libsteed.parse_file(db.encode("utf-8"), table.encode("utf-8"), jpath.encode("utf-8"))

# int assemble_to_file(const char *db, const char *table, const char **cols, int ncol, const char *jpath);
def assemble_to_file(db, table, cols, jpath):
    cols_num = len(cols)
    cols_bytes = (ctypes.c_char_p * cols_num)(*[s.encode('utf-8') for s in cols])
    libsteed.assemble_to_file.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p), ctypes.c_int, ctypes.c_char_p]
    libsteed.assemble_to_file.restype = ctypes.c_int
    return libsteed.assemble_to_file(db.encode("utf-8"), table.encode("utf-8"), cols_bytes, cols_num, jpath.encode("utf-8"))


# const char *assemble_to_string(const char *db, const char *table, const char **cols, int ncol, int *len);
json_bytes = None
def assemble_to_string(db, table, cols):
    cols_num = len(cols)
    cols_bytes = (ctypes.c_char_p * cols_num)(*[s.encode('utf-8') for s in cols])
    libsteed.assemble_to_string.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p), ctypes.c_int]
    libsteed.assemble_to_string.restype = ctypes.c_char_p
    global json_bytes
    json_bytes = libsteed.assemble_to_string(db.encode("utf-8"), table.encode("utf-8"), cols_bytes, cols_num)
    json_string = json_bytes.decode("utf-8")
    jdata = json.loads(json_string)
    return jdata

def malloc_json_bytes():
    global json_bytes
    json_bytes = None

def free_json_bytes():
    libsteed.free_string.argtypes = [ctypes.c_char_p]
    libsteed.free_string.restype = None
    global json_bytes
    libsteed.free_string(json_bytes)


# steed::ColumnParser *open_parser(const char *db, const char *table);
def open_parser(db, table):
    libsteed.open_parser.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    libsteed.open_parser.restype = ctypes.c_void_p
    return libsteed.open_parser(db.encode("utf-8"), table.encode("utf-8"))

# insert json object
def insert_json_object(cp, json_obj):
    jstr = json.dumps(json_obj)
    jbytes = jstr.encode("utf-8")
    jlen = len(jbytes)
    libsteed.insert_parser.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_uint32]
    libsteed.insert_parser.restype = ctypes.c_int
    return libsteed.insert_parser(cp, jbytes, jlen)

# int insert_parser(steed::ColumnParser *cp, const char *recd, uint32_t len);
def insert_json_string(cp, recd, len):
    libsteed.insert_parser.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_uint32]
    libsteed.insert_parser.restype = ctypes.c_int
    return libsteed.insert_parser(cp, recd.encode("utf-8"), len)

# void close_parser(steed::ColumnParser *cp);
def close_parser(cp):
    libsteed.close_parser.argtypes = [ctypes.c_void_p]
    libsteed.close_parser.restype = None
    libsteed.close_parser(cp)


def remove_file(file_path):
    try:
        os.remove(file_path)
        print("STEED: File %s removed successfully." % file_path)
    except FileNotFoundError:
        print("STEED: File %s not found." % file_path)
    except PermissionError:
        print("STEED: Do not have permission to delete %s." % file_path)
    except Exception as e:
        print("STEED: Error while deleting file %s." % file_path)


# setup steed library
load_library()