# steed core  
project(steed_core)


# top level project name 
set(BINARY ${CMAKE_PROJECT_NAME}) 
set(STEED_ALL_SUB_DIR
   ${CMAKE_CURRENT_SOURCE_DIR}/conf
   ${CMAKE_CURRENT_SOURCE_DIR}/util
   ${CMAKE_CURRENT_SOURCE_DIR}/base
   ${CMAKE_CURRENT_SOURCE_DIR}/store
   ${CMAKE_CURRENT_SOURCE_DIR}/schema
   ${CMAKE_CURRENT_SOURCE_DIR}/parse
   ${CMAKE_CURRENT_SOURCE_DIR}/assemble
)


file(GLOB CONFIG_SOURCE_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/conf/*.cpp)
file(GLOB UTIL_SOURCE_FILES      ${CMAKE_CURRENT_SOURCE_DIR}/util/*.cpp)
file(GLOB BASE_SOURCE_FILES      ${CMAKE_CURRENT_SOURCE_DIR}/base/*.cpp)
file(GLOB STORE_SOURCE_FILES     ${CMAKE_CURRENT_SOURCE_DIR}/store/*.cpp)
file(GLOB SCHEMA_SOURCE_FILES    ${CMAKE_CURRENT_SOURCE_DIR}/schema/*.cpp)
file(GLOB PARSE_SOURCE_FILES     ${CMAKE_CURRENT_SOURCE_DIR}/parse/*.cpp)
file(GLOB ASSEMBLE_SOURCE_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/assemble/*.cpp)

### generate steed as a SHARED library
message(INFO "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
add_library(${BINARY} SHARED steed.cpp
   ${CONFIG_SOURCE_FILES}
   ${UTIL_SOURCE_FILES}
   ${BASE_SOURCE_FILES}
   ${STORE_SOURCE_FILES}
   ${SCHEMA_SOURCE_FILES}
   ${PARSE_SOURCE_FILES}
   ${ASSEMBLE_SOURCE_FILES}
)

target_include_directories(${BINARY} PUBLIC 
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${STEED_ALL_SUB_DIR}
)

target_link_libraries(${BINARY} PUBLIC
   steed_conf
   steed_util
   steed_base
   steed_store
   steed_schema
   steed_parse
   steed_assemble
)


add_subdirectory(conf)
add_subdirectory(util)
add_subdirectory(base)
add_subdirectory(store)
add_subdirectory(schema)
add_subdirectory(parse)
add_subdirectory(assemble)
add_subdirectory(unittest)


# make install path
install(TARGETS ${BINARY}
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
   RUNTIME DESTINATION bin
   INCLUDES DESTINATION include/steed
)

# install header files
install(FILES ../../include/steed.h DESTINATION include/steed)

# get all header files
file(GLOB_RECURSE HEADER_FILES 
     ${CMAKE_SOURCE_DIR}/include/*.h
     ${CMAKE_SOURCE_DIR}/src/steed/assemble/*.h
     ${CMAKE_SOURCE_DIR}/src/steed/base/*.h
     ${CMAKE_SOURCE_DIR}/src/steed/conf/*.h
     ${CMAKE_SOURCE_DIR}/src/steed/parse/*.h
     ${CMAKE_SOURCE_DIR}/src/steed/schema/*.h
     ${CMAKE_SOURCE_DIR}/src/steed/store/*.h
     ${CMAKE_SOURCE_DIR}/src/steed/unittest/*.h
     ${CMAKE_SOURCE_DIR}/src/steed/util/*.h
)


## # print to check
## message(STATUS "HEADER_FILES: ${HEADER_FILES}")

# install header files
install(FILES ${HEADER_FILES} DESTINATION include/steed)