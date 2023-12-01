project(steed_source)


# thirdparty stored in fc_base_dir 
include (FetchContent)
set(FETCHCONTENT_QUIET off)
get_filename_component(fc_base_dir "./thirdparty/"
                       REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
set(FETCHCONTENT_BASE_DIR ${fc_base_dir})
message(STATUS "STEED download thirdparty @ " ${fc_base_dir})


set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)
add_subdirectory(thirdparty)
add_subdirectory(steed)