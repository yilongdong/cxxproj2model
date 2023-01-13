#pragma once
#include <clang/AST/ASTContext.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/DeclGroup.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Frontend/CompilerInstance.h>

#include "Beacon/Tracker/ClassMatchCallback.h"
#include "Beacon/Tracker/FuncCallFuncExprCallback.h"
#include "TU.pb.h"

namespace beacon {
    class BeaconASTConsumer : public clang::ASTConsumer {
    public:
        explicit BeaconASTConsumer(clang::CompilerInstance &CI, beacon::model::TU &TU);
        ~BeaconASTConsumer() override;
        bool HandleTopLevelDecl(clang::DeclGroupRef D) override;
        void HandleTranslationUnit(clang::ASTContext &Context) override;
    private:
        clang::ast_matchers::MatchFinder matchFinder;
        callback::ClassMatchCallback classCallback;
        callback::FuncCallFuncExprCallback funcCallback;
        beacon::model::TU &TU;
    };
}