# thirdparty project 
project(thirdparty)


## # Locate GTest
## find_package(GTest REQUIRED)
## include_directories(${GTEST_INCLUDE_DIRS})

# Add CLI11
set(CLI11_INCLUDE_DIR ${THIRD_PARTY_DIR}/cli11/include CACHE PATH "Path to include folder for CLI11")
message(STATUS "steed @ src/steed includes CLI11 lib @ " ${CLI11_INCLUDE_DIR})

add_subdirectory(cli11)
add_subdirectory(googletest)





#### lz4 
FetchContent_Declare(
  lz4
  GIT_REPOSITORY https://github.com/lz4/lz4.git
  GIT_TAG v1.9.0
  SOURCE_SUBDIR  ./contrib/cmake_unofficial/
)

find_package(LZ4 QUIET)

if(LZ4_FOUND)
  message(STATUS "Found liblz4: ${LZ4_INCLUDE_DIRS}")
else()
  message(STATUS "liblz4 not found. Fetching...")
  
  ## # avoid download again 
  FetchContent_GetProperties(lz4)
  if(NOT lz4_POPULATED)
    FetchContent_Populate(lz4)

    # Bring the populated content into the build
    message(STATUS "lz4_SOURCE_DIR @ " ${lz4_SOURCE_DIR})
    message(STATUS "lz4_BINARY_DIR @ " ${lz4_BINARY_DIR})
    add_subdirectory(${lz4_SOURCE_DIR}/contrib/cmake_unofficial/ ${lz4_BINARY_DIR})
  #  add_subdirectory(${lz4_SOURCE_DIR} ${lz4_BINARY_DIR})
  endif()

  FetchContent_MakeAvailable(lz4)
endif()





## #################### FetchContent ####################
## 
## ### GoogleTest
## # @see https://google.github.io/googletest/quickstart-cmake.html
## FetchContent_Declare(
##   googletest
##   GIT_REPOSITORY https://github.com/google/googletest.git
##   GIT_TAG        release-1.11.0
## ## OR 
## #  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
## )
## 
## # avoid download again 
## FetchContent_GetProperties(googletest)
## if(NOT googletest_POPULATED)
##   FetchContent_Populate(googletest)
## 
##   # Bring the populated content into the build
##   add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
## endif()
## #message(STATUS "googletest_SOURCE_DIR @ " ${googletest_SOURCE_DIR})
## #message(STATUS "googletest_BINARY_DIR @ " ${googletest_BINARY_DIR})
## 
## # find_package get googletest 
## FetchContent_MakeAvailable(googletest)





## #### cli11 
## FetchContent_Declare(
##   CLI11
##   GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
##   GIT_TAG v2.3.2 
## )
## 
## find_package(CLI11 QUIET)
## if (CLI11_FOUND)
##   message(STATUS "Found CLI11: ${CLI11_INCLUDE_DIRS}")
## else()
##   message(STATUS "CLI11 not found. Fetching...")
## 
##   # avoid download again
##   FetchContent_GetProperties(CLI11)
##   if(NOT CLI11_POPULATED)
##     FetchContent_Populate(CLI11)
## 
##     message(STATUS "cli11_SOURCE_DIR @ " ${cli11_SOURCE_DIR})
##     message(STATUS "cli11_BINARY_DIR @ " ${cli11_BINARY_DIR})
##     add_subdirectory(${cli11_SOURCE_DIR} ${cli11_BINARY_DIR})
##   endif()
## 
##   FetchContent_MakeAvailable(CLI11)
## endif()