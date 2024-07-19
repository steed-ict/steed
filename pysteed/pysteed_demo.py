import json
import pysteed

conf=""
db="testdb"
table="testtable"
jinput="./json/try.json"
joutput="./output.json"


pysteed.init(conf) 
pysteed.create_database(db)
pysteed.create_table(db, table)

########## parse and insert json data ##############################
##- ## 1. parse json file
##- pysteed.parse_file(db, table, jinput)

## 2. parse JSON object
pysteed_parser = pysteed.open_parser(db, table)

json_array = [
    {"item": "apple", "size": {"h": 10, "w": 20}},
    {"item": "banana", "size": {"h": 5, "w": 10}},
    {"item": "orange", "size": {"h": 8, "w": 12}},
    {"item": "pear", "size": {"h": 6, "w": 8}},
    {"item": "grape", "size": {"h": 4, "w": 6}},
    {"item": "pineapple", "size": {"h": 15, "w": 30}}
]

for json_object in json_array:
    json_str = json.dumps(json_object)  # Serialize dict to JSON string
    json_bytes = json_str.encode('utf-8')  # Encode string to bytes
##    print(json_bytes) # print json_bytes to debug
    pysteed.insert_json_object(pysteed_parser, json_bytes) # insert json bytes

pysteed.close_parser(pysteed_parser)
pysteed_parser = None
#### 

########## query and assemble json data ##############################
cols = ["item", "size.h"]
## 1. assemble to file
pysteed.assemble_to_file(db, table, cols, joutput)

## 2. assemble to python json object
jdata = pysteed.assemble_to_string(db, table, cols)
print(jdata)

## clean up 
pysteed.drop_table(db, table)
pysteed.drop_database(db)
pysteed.uninit()

pysteed.make_clean()