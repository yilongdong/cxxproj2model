add_library(Beacon "")
add_subdirectory(Model)
add_subdirectory(Tracker)
target_sources(Beacon
        PRIVATE
        BeaconFrontendAction.cpp)
target_link_libraries(Beacon
        PRIVATE
        comm::log)