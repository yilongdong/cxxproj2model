#include <memory>
#include <fstream>
#include <unordered_set>
#include <regex>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>

#include "Comm/log.h"
#include "Beacon/Tracker/BeaconPPCallbackTracker.h"
#include "BeaconASTConsumer.h"
#include "google/protobuf/util/json_util.h"
#include "proj2model/Config.h"

namespace beacon {
    using Filter = std::regex;

    class BeaconFrontendAction : public clang::ASTFrontendAction {
    public:
        explicit BeaconFrontendAction(std::vector<Filter> const& filters) : filters(filters) {}
    protected:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
                clang::CompilerInstance &CI, clang::StringRef InFile) override {
//            TU.set_source_file(InFile.str());

            TU.mutable_file()->set_name(InFile.str());
            clang::Preprocessor &PP = CI.getPreprocessor();
            PP.addPPCallbacks(std::make_unique<tracker::BeaconPPCallbackTracker>(TU, filters, PP));
            return std::make_unique<BeaconASTConsumer>(CI, TU);
        }

        void EndSourceFileAction() override {
            auto const& config = proj2model::Config::get();

            std::hash<std::string> hash;
            std::string filename  = config.output.result_file + "/" + std::to_string(hash(TU.file().name())) + ".json";
            LOG_INFO("TU include list size = %d", TU.include_list_size());
            LOG_INFO("TU class list size = %d", TU.class_list_size());
            std::string content;
            google::protobuf::util::MessageToJsonString(TU, &content);
            {
                std::ofstream fout(filename);
                if(!fout.is_open()) {
                    LOG_ERROR("TU 写入文件 {%s} 失败", filename.c_str());
                    return;
                }
                std::hash<std::string> hash;
                fout << content;
            }
            LOG_INFO("TU 写入文件 {%s}，大小 {%lu} bytes", filename.c_str(), content.size());
        }

    private:
        beacon::model::TU TU;
        std::vector<Filter> const& filters;
    };
}