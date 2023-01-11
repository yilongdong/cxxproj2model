add_library(log log.cpp)
add_library(comm::log ALIAS log)
#target_link_libraries(log PUBLIC spdlog)

add_library(json INTERFACE ${PROJECT_SOURCE_DIR}/include/Comm/json.hpp)
add_library(comm::json ALIAS json)

add_subdirectory(Util)