# @see https://cmake.org/cmake/help/v3.22/module/GoogleTest.html


# steed component 
project(steed_tools)



# steed schema tool 
add_executable(schema Schema.cpp)

target_include_directories(schema PUBLIC 
   ${STEED_ALL_SUB_DIR}
)

target_link_libraries(schema
    steed_conf
    steed_util
    steed_base
    steed_schema
)
