# steed core  
project(steed_base)


#file(GLOB BASE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(${PROJECT_NAME} STATIC ${BASE_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../conf
   ${CMAKE_CURRENT_SOURCE_DIR}/../util
)


find_package(LZ4 REQUIRED)
message(STATUS "steed includes lz4 lib @ " ${LZ4_LIBRARY})

target_link_libraries(${PROJECT_NAME} PUBLIC
   ${LZ4_LIBRARY}
   steed_conf
   steed_util
)
