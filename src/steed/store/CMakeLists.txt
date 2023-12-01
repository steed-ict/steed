# steed core  
project(steed_store)


#file(GLOB STORE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(${PROJECT_NAME} STATIC ${STORE_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../conf
   ${CMAKE_CURRENT_SOURCE_DIR}/../util
   ${CMAKE_CURRENT_SOURCE_DIR}/../base
   ${CMAKE_CURRENT_SOURCE_DIR}/../schema
)

target_link_libraries(${PROJECT_NAME} PUBLIC
   steed_conf
   steed_util
   steed_base
   steed_schema
)
