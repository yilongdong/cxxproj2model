#include <memory>
#include <fstream>
#include <unordered_set>
#include <regex>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>

#include "Comm/log.h"
#include "Beacon/Tracker/BeaconPPCallbackTracker.h"
#include "proj2model/Config.h"

namespace beacon {
    using Filter = std::regex;

    class BeaconFrontendAction : public clang::ASTFrontendAction {
    public:
        explicit BeaconFrontendAction(std::vector<Filter> const& filters) : filters(filters) {}
    protected:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
                clang::CompilerInstance &CI, clang::StringRef InFile) override {
            TUModel.source_file = InFile.str();
            clang::Preprocessor &PP = CI.getPreprocessor();
            PP.addPPCallbacks(std::make_unique<tracker::BeaconPPCallbackTracker>(TUModel, filters, PP));
            return std::make_unique<clang::ASTConsumer>();
        }

        void EndSourceFileAction() override {
            auto const& config = proj2model::Config::get();

            std::hash<std::string> hash;
            std::string filename  = config.output.result_file + "/" + std::to_string(hash(TUModel.source_file)) + ".json";
            LOG_INFO("TU include list size = {%lu}", TUModel.include_list.size());
            std::unordered_set<beacon::model::IncludeInfo> unique_include_list{TUModel.include_list.begin(),TUModel.include_list.end()};
            TUModel.include_list = std::vector<beacon::model::IncludeInfo>{unique_include_list.begin(), unique_include_list.end()};
            LOG_INFO("TU unique include list size = {%lu}", TUModel.include_list.size());
            std::string content = nlohmann::json{TUModel}.dump(4);
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
        model::TranslationUnitModel TUModel;
        std::vector<Filter> const& filters;
    };
}