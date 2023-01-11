target_sources(Beacon
        PRIVATE
        TranslationUnitModel.cpp)
target_link_libraries(Beacon
        PUBLIC
        comm::json)