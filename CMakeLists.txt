cmake_minimum_required (VERSION 3.13)

project (steed VERSION 1.0 DESCRIPTION "Steed is a C++ library for JSON processing." LANGUAGES CXX)


# set executable and library output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)   


# set cmake module path:
# 1. The package should be installed by `apt-get install` and contains .a or .so
# 2. Config file FindXXX.cmake created in CMAKE_MODULE_PATH directory 
# 3. Then call find_package() to search for cmake modules for the include and libraries 
include(FindPkgConfig)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")
message(STATUS "CMAKE_MODULE_PATH @ " ${CMAKE_MODULE_PATH})


# ## make install setting
# ## The installed include, lib and bin are defined in the ./src/steed/CMakelists.txt
# set(CMAKE_INSTALL_PREFIX /mydir)


# -std=c++11 required 
# make sure to add CMAKE_CXX_STANDARD before add_executable() 
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_compile_options(-Wall -Wextra -Werror)
add_compile_options(-fPIC)
##add_compile_definitions(BUILD_TESTING=1)


# build type:
# cmake -DCMAKE_BUILD_TYPE=Debug .. 
#   Debug, Release, RelWithDebInfo and MinSizeRe 
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "release")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "release")
    add_compile_options(-O3)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "debug")
    add_compile_options(-O0 -g -pg)
    add_link_options(-rdynamic)
endif()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")


enable_testing()


# add source directory
add_subdirectory(src)
add_subdirectory(tools)