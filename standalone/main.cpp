#include <algorithm>
#include <regex>
#include <vector>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <gflags/gflags.h>
#include "version.h"
#include "Beacon/BeaconFrontendAction.h"

using namespace clang;
using namespace clang::tooling;
using Config = proj2model::Config;

namespace beacon {
    class BeaconFrontendActionFactory : public clang::tooling::FrontendActionFactory {
        std::unique_ptr<clang::FrontendAction> create() override {
            static std::vector<std::regex> filters {
                std::regex("^/Library/Developer/CommandLineTools/SDKs/.*"),
                std::regex(R"(^/usr/local/bin/\.\./include/c\+\+/.*)"),
                std::regex ("^/usr/local/include/gflags/.*"),
                std::regex ("^/usr/local/include/llvm/.*"),
                std::regex ("^/usr/local/include/llvm-c/.*"),
                std::regex ("^/usr/local/include/clang/.*"),
                std::regex ("^/usr/local/include/clang-c/.*"),
                std::regex ("^/Users/dongyilong/Documents/毕业设计/repo/project2model/build/lib/.*"),
                std::regex ("^/Users/dongyilong/Documents/毕业设计/repo/project2model/include/Comm/.*"),
                std::regex ("^/Users/dongyilong/Documents/毕业设计/repo/project2model/src/Comm/.*"),
                std::regex ("^/opt/homebrew/include/google/.*"),
            };

            return std::make_unique<BeaconFrontendAction>(filters);
        }
    };
}



DEFINE_string(conf, "", "配置文件路径");

int main(int argc, char **argv) {
    google::SetUsageMessage("--conf=xx");
    google::SetVersionString(VERSION);
    google::ParseCommandLineFlags(&argc, &argv, true);
    proj2model::Config::init(FLAGS_conf);

    std::string errorMsg;
    auto db = CompilationDatabase::loadFromDirectory(Config::get().project.compdb, errorMsg);
    if (db == nullptr) {
//        LOG_ERROR(errorMsg);
        exit(0);
    }

    // 为每个cpp文件分析一遍，过程中会有大量头文件重复词法分析。但是暂时不知道如何去除。
    std::vector<std::string> all_files{ db->getAllFiles() }, source_files;
    std::copy_if(all_files.begin(), all_files.end(), std::back_inserter(source_files),
            [](auto& file) { return Config::get().project.shouldInclude(file); });
    for (auto const& path : source_files) {
        LOG_INFO("source file = {%s}", path.c_str());
    }
    ClangTool tool{ *db.get(), source_files };
    beacon::BeaconFrontendActionFactory factory;
    return tool.run(&factory);
}