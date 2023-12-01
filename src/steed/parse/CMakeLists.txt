# steed core  
project(steed_parse)


#file(GLOB PARSE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(${PROJECT_NAME} STATIC ${PARSE_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../conf
   ${CMAKE_CURRENT_SOURCE_DIR}/../util
   ${CMAKE_CURRENT_SOURCE_DIR}/../base
   ${CMAKE_CURRENT_SOURCE_DIR}/../schema
   ${CMAKE_CURRENT_SOURCE_DIR}/../store
)

target_link_libraries(${PROJECT_NAME} PUBLIC
   steed_conf
   steed_util
   steed_base
   steed_schema
   steed_store
)
