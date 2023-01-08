add_library(Config Config.cpp)
add_library(proj2model::config ALIAS Config)
target_link_libraries(Config INTERFACE comm::json)
target_link_libraries(Config PUBLIC comm::log comm::util)