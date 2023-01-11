find_package(gflags REQUIRED)
set(ClangLLVMLibs clangAST clangBasic clangFrontend clangSerialization clangTooling)
add_executable(project2model main.cpp)
target_link_libraries(project2model
        PRIVATE
        ${ClangLLVMLibs}
        gflags
        comm::log
        proj2model::config
        Beacon)