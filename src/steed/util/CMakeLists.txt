# steed core  
project(steed_util)


#file(GLOB UTIL_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(${PROJECT_NAME} STATIC ${UTIL_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../conf
)

target_link_libraries(${PROJECT_NAME} PUBLIC
   steed_conf
)
