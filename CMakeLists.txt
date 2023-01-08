cmake_minimum_required(VERSION 3.23)
project(project2model LANGUAGES CXX VERSION 0.1)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "\"${CMAKE_COMMAND}\" -E time")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "\"${CMAKE_COMMAND}\" -E time")
list(APPEND CMAKE_CXX_FLAGS "-ftime-trace")
list(APPEND CMAKE_C_FLAGS "-ftime-trace")

if (NOT CMAKE_BUILD_TYPE)
    #   RelWithDebInfo
    set(CMAKE_BUILD_TYPE  Release CACHE STRING "Build Type (default Release):" FORCE)
endif()

include(addClangLLVM)
include(addDoxygen)
include(addSpdlog)
include(addCatch2)

string(TIMESTAMP COMPILE_TIMESTAMP %Y-%m-%d_%H:%M:%S)
configure_file(include/version.h.in ${CMAKE_SOURCE_DIR}/include/version.h @ONLY)
configure_file(conf/proj2model-conf.json.in conf/proj2model-conf.json)

include_directories(include)
add_subdirectory(src)
add_subdirectory(standalone)
add_subdirectory(tests)