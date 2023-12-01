# steed core  
project(steed_conf)

#file(GLOB CONFIG_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(${PROJECT_NAME} STATIC ${CONFIG_SOURCE_FILES})


message(STATUS "steed Config includes CLI11 lib @ " ${CLI11_INCLUDE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
   ${CLI11_INCLUDE_DIR} # CLI11
)

target_link_libraries(${PROJECT_NAME} PUBLIC
   CLI11::CLI11
)