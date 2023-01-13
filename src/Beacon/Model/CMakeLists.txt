INCLUDE(FindProtobuf)
find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRC PROTO_HEADER TU.proto)
add_library(TUProto ${PROTO_HEADER} ${PROTO_SRC})
target_include_directories(TUProto PUBLIC ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(TUProto PUBLIC ${PROTOBUF_LIBRARY})

target_link_libraries(Beacon PUBLIC TUProto)