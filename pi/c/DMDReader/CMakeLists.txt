cmake_minimum_required (VERSION 3.11)

###
### BEGIN Raylib
###

find_package(raylib 4.0.0 QUIET) # Requires at least version 3.0

if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)

  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/4.0.0.tar.gz
  )

  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)

    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples

    # build raylib
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})

    include_directories(${raylib_SOURCE_DIR}/src)

  endif()

endif()

set(CMAKE_C_STANDARD 11)



###
### END Raylib
###

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("DMDReader")

add_executable (DMDReader "dmdreader.cpp" "dmdreader.h" "util/crc32.cpp" "util/crc32.h" "render/framerenderer.h" "render/raylibrenderer.h" "render/raylibrenderer.cpp"  "dmdframe.cpp" "dmdframe.h" "render/framerenderer.cpp" "util/bmp.h" "util/bmp.cpp" "util/numutils.cpp" "util/numutils.h")

target_link_libraries(${PROJECT_NAME} raylib)