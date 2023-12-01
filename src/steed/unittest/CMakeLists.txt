# @see https://cmake.org/cmake/help/v3.22/module/GoogleTest.html


# steed component 
project(steed_unittest)



# steed config unittest 
add_executable(test_config Config.cpp)
target_link_libraries(test_config
    gtest_main
    steed_conf
)
target_include_directories(test_config PUBLIC 
   ${STEED_ALL_SUB_DIR}
)
add_test(NAME Test_Config COMMAND test_config)



# steed util unittest 
add_executable(test_util Util.cpp)
target_link_libraries(test_util
    gtest_main
    steed_util
)
target_include_directories(test_util PUBLIC 
   ${STEED_ALL_SUB_DIR}
)
add_test(NAME Test_Util COMMAND test_util)



# steed base unittest 
add_executable(test_base Base.cpp)
target_link_libraries(test_base
    gtest_main
    steed_util
    steed_base
)
target_include_directories(test_base PUBLIC 
   ${STEED_ALL_SUB_DIR}
)
add_test(NAME Test_Base COMMAND test_base)



# steed store unittest 
add_executable(test_store Store.cpp)
target_link_libraries(test_store
    gtest_main
    steed_conf
    steed_util
    steed_base
    steed_store
)
target_include_directories(test_store PUBLIC 
   ${STEED_ALL_SUB_DIR}
)
add_test(NAME Test_Store COMMAND test_store)



# steed schema unittest 
add_executable(test_schema Schema.cpp)
target_link_libraries(test_schema
    gtest_main
    steed_conf
    steed_util
    steed_base
    steed_schema
)
target_include_directories(test_schema PUBLIC 
   ${STEED_ALL_SUB_DIR}
)
add_test(NAME Test_Schema COMMAND test_schema)



# steed schema unittest 
add_executable(test_parse Parse.cpp)
target_link_libraries(test_parse
    gtest_main
    steed_conf
    steed_util
    steed_base
    steed_schema
    steed_store
    steed_parse
)
target_include_directories(test_parse PUBLIC 
   ${STEED_ALL_SUB_DIR}
)
add_test(NAME Test_Parse COMMAND test_parse)



# steed schema unittest 
add_executable(test_assemble Assemble.cpp)
target_link_libraries(test_assemble
    gtest_main
    steed_conf
    steed_util
    steed_base
    steed_schema
    steed_store
    steed_assemble
)
target_include_directories(test_assemble PUBLIC 
   ${STEED_ALL_SUB_DIR}
)
add_test(NAME Test_Assemble COMMAND test_assemble)



# ## demo 1 
# add_executable(
#   hello_test
#   hello_test.cc
# )
# target_link_libraries(
#   hello_test
#   GTest::gtest_main
# )
# 
# include(GoogleTest)
# gtest_discover_tests(hello_test)

# ## demo 2 
# add_executable(FormulaTest FormulaTest.cpp)
# target_link_libraries(FormulaTest
#     gtest_main
#     steed_util
# )
# 
# target_include_directories(FormulaTest PUBLIC 
#    ${STEED_ALL_SUB_DIR}
# )
# 
# add_test(NAME FormulaTest COMMAND FormulaTest)
