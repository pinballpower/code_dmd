cmake_minimum_required (VERSION 3.11)

###
### BEGIN Raylib
###

find_package(raylib 4.0.0 QUIET) # Requires at least version 4.0

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

###
### BEGIN JsonCPP
###

find_package(jsoncpp)

if (NOT jsoncpp_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)

  FetchContent_Declare(
    jsoncpp
    URL https://github.com/open-source-parsers/jsoncpp/archive/refs/tags/1.9.5.tar.gz
  )

  FetchContent_GetProperties(jsoncpp)
  if (NOT jsoncpp_POPULATED) # Have we downloaded jsoncpp yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(jsoncpp)

    # build raylib
    add_subdirectory(${jsoncpp_SOURCE_DIR} ${jsoncpp_BINARY_DIR})

    include_directories(${jsoncpp_SOURCE_DIR}/src)

  endif()

endif()


###
### END JSonCPP
###

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("DMDReader")

add_executable (DMDReader "dmdreader.cpp" "dmdreader.h" "util/crc32.cpp" "util/crc32.h" "render/framerenderer.h" "render/raylibrenderer.h" "render/raylibrenderer.cpp"  "dmd/dmdframe.cpp" "dmd/dmdframe.h" "render/framerenderer.cpp" "util/bmp.h" "util/bmp.cpp" "util/numutils.cpp" "util/numutils.h" "dmd/color.h" "dmd/color.cpp" "util/endian.h" "dmd/pubcapture.cpp" "dmd/frameprocessor.h" "dmd/frameprocessor.cpp" "util/image.h" "util/image.cpp" "dmd/dmdsource.h" "dmd/dmdsource.cpp" "util/objectfactory.h")

target_link_libraries(${PROJECT_NAME} raylib)