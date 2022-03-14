# CMakeList.txt: CMake-Projekt für "DMDReader". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.11)



###
### BEGIN Raylib
###
#include(FetchContent)
#set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
#set(BUILD_GAMES OFF CACHE BOOL "" FORCE) # don't build the supplied example games
#fetchcontent_declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG 4.0.0 GIT_PROGRESS TRUE)
#fetchcontent_makeavailable(raylib)
###
### END Raylib
###


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("DMDReader")

add_executable (DMDReader "dmdreader.cpp" "dmdreader.h" "util/crc32.cpp" "util/crc32.h" "render/framerenderer.h" "render/raylibrenderer.h" "render/raylibrenderer.cpp"  "dmdframe.cpp" "dmdframe.h")