#include <algorithm>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <llvm/Support/CommandLine.h>
#include <gflags/gflags.h>
#include "version.h"
#include "comm/log.h"
#include "proj2model/Config.h"
using namespace clang;
using namespace clang::tooling;

using Config = proj2model::Config;
//namespace beacon {
//
//    class BeaconFrontendAction : public clang::ASTFrontendAction {
//    protected:
//        std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef InFile) override {
//            Preprocessor &PP = CI.getPreprocessor();
//            PP.addPPCallbacks(
//                    std::make_unique<PPCallbacksTracker>(Filters, CallbackCalls, PP));
//            return std::make_unique<ASTConsumer>();
//        }
//    };
//
//    class BeaconFrontendActionFactory : public tooling::FrontendActionFactory {
//        std::unique_ptr<FrontendAction> create() override {
//            return std::make_unique<BeaconFrontendAction>()
//        }
//    };
//}
//class CodeBeaconGenFrontendAction : public clang::ASTFrontendAction {
//public:
//    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) override {
//        return nullptr;
//    }
//};


DEFINE_string(conf, "", "配置文件路径");

int main(int argc, char **argv) {
    google::SetUsageMessage("--conf=xx");
    google::SetVersionString(VERSION);
    google::ParseCommandLineFlags(&argc, &argv, true);
    proj2model::Config::init(FLAGS_conf);

    std::string errorMsg;
    auto db = CompilationDatabase::loadFromDirectory(Config::get().project.compdb, errorMsg);
    if (db == nullptr) {
        LOG_ERROR(errorMsg);
        exit(0);
    }

    // 为每个cpp文件分析一遍，过程中会有大量头文件重复词法分析。但是暂时不知道如何去除。
    std::vector<std::string> all_files{ db->getAllFiles() }, source_files;
    std::copy_if(all_files.begin(), all_files.end(), std::back_inserter(source_files),
            [](auto& file) { return Config::get().project.shouldInclude(file); });
    for (auto const& path : source_files) {
        LOG_INFO("source file = {}", path);
    }
//    ClangTool tool{ *db.get(), db->getAllFiles() };
//    return tool.run(newFrontendActionFactory<CodeBeaconGenFrontendAction>().get());
}