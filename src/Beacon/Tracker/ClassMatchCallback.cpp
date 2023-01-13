#include <iostream>
#include <regex>
#include "TU.pb.h"
#include "Beacon/Tracker/ClassMatchCallback.h"
#include "Beacon/Generator/ClassGenerator.h"

namespace beacon::callback {
    std::unordered_set<size_t> ClassMatchCallback::classIDSet;
    std::unordered_set<size_t> ClassMatchCallback::fileIDSet;

    void ClassMatchCallback::run(clang::ast_matchers::MatchFinder::MatchResult const &Result) {
        auto const *pClsDecl = Result.Nodes.getNodeAs<clang::CXXRecordDecl>("id");
        if (shouldSkip(pClsDecl)) {
            return;
        }

        if (pClsDecl->isCompleteDefinition()) {
            auto clsModel = TU.add_class_list();
            generator::ClassGenerator::Create(*pClsDecl, clsModel, CI.getSourceManager());
        }
    }

    bool ClassMatchCallback::shouldSkip(clang::CXXRecordDecl const *pClsDecl) {
        if (!pClsDecl) return true;
        if (source_manager().isInSystemHeader(pClsDecl->getSourceRange().getBegin())) { return true; }
        size_t classID = std::hash<std::string>{}(pClsDecl->getQualifiedNameAsString());

        if (classIDSet.find(classID) != classIDSet.end()) {
            return true;
        }
        classIDSet.insert(classID);

        std::string filename = CI.getSourceManager().getFilename(pClsDecl->getSourceRange().getBegin()).str();
        static std::vector<std::regex> filters{
                std::regex("^/Library/Developer/CommandLineTools/SDKs/.*"),
                std::regex(R"(^/usr/local/bin/\.\./include/c\+\+/.*)"),
                std::regex("^/usr/local/include/gflags/.*"),
                std::regex("^/usr/local/include/llvm/.*"),
                std::regex("^/usr/local/include/llvm-c/.*"),
                std::regex("^/usr/local/include/clang/.*"),
                std::regex("^/usr/local/include/clang-c/.*"),
                std::regex("^/Users/dongyilong/Documents/毕业设计/repo/project2model/build/lib/.*"),
                std::regex("^/Users/dongyilong/Documents/毕业设计/repo/project2model/include/Comm/.*"),
                std::regex("^/Users/dongyilong/Documents/毕业设计/repo/project2model/src/Comm/.*"),
                std::regex("^/opt/homebrew/include/google/.*"),
        };
        for (auto& regex : filters) {
            if (std::regex_match(filename, regex)) {
                return true;
            }
        }
        return false;
    }

} // namespace CodeBeacon