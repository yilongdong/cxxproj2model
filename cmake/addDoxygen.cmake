find_package(Doxygen)
if (Doxygen_FOUND)
    MESSAGE(STATUS "proj2model: doxygen")
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else (Doxygen_FOUND)
    MESSAGE(STATUS "proj2model: Doxygen need to be installed to generate the doxygen documentation")
endif(Doxygen_FOUND)