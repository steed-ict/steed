# FindLibLZ4.cmake

# find liblz4-dev 
find_path(LZ4_INCLUDE_DIR lz4.h PATH_SUFFIXES lz4)
find_library(LZ4_LIBRARY NAMES lz4)

# output variables to parent scope
set(LZ4_FOUND FALSE)
if (LZ4_INCLUDE_DIR AND LZ4_LIBRARY)
    set(LZ4_FOUND TRUE)
endif ()

if (LZ4_FOUND)
    if (NOT LZ4_FIND_QUIETLY)
        message(STATUS "Found liblz4-dev: ${LZ4_LIBRARY}")
    endif ()
else ()
    if (LZ4_FIND_REQUIRED)
        message(FATAL_ERROR "liblz4-dev not found")
    endif ()
endif ()

mark_as_advanced(LZ4_INCLUDE_DIR LZ4_LIBRARY)