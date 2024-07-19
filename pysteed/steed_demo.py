import steed

conf=""
db="testdb"
table="testtable"
jinput="./json/try.json"
joutput="./output.json"


steed.init(conf) 
steed.create_database(db)
steed.create_table(db, table)

# ## 1. parse json file
# steed.parse_file(db, table, jinput)

## 2. parse JSON object
steed_parser = steed.open_parser(db, table)

json_array = [
    {"item": "apple", "size": {"h": 10, "w": 20}},
    {"item": "banana", "size": {"h": 5, "w": 10}},
    {"item": "orange", "size": {"h": 8, "w": 12}},
    {"item": "pear", "size": {"h": 6, "w": 8}},
    {"item": "grape", "size": {"h": 4, "w": 6}},
    {"item": "pineapple", "size": {"h": 15, "w": 30}}
]

for json_object in json_array:
    steed.insert_json_object(steed_parser, json_object)

steed.close_parser(steed_parser)
steed_parser = None
#### 

cols = ["item", "size.h"]
## 1. assemble to file
steed.assemble_to_file(db, table, cols, joutput)

## 2. assemble to python json object
steed.malloc_json_bytes
jdata = steed.assemble_to_string(db, table, cols)
print(jdata)
steed.free_json_bytes


## clean up 
steed.remove_file(joutput)
steed.drop_table(db, table)
steed.drop_database(db)
steed.uninit()

steed.make_clean()
