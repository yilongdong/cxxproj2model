#include "Beacon/BeaconASTConsumer.h"


beacon::BeaconASTConsumer::BeaconASTConsumer(clang::CompilerInstance &CI, beacon::model::TU &TU)
    : classCallback(CI, TU), funcCallback(CI, TU), TU(TU) {
    static clang::ast_matchers::DeclarationMatcher const recordMatcher =
            clang::ast_matchers::cxxRecordDecl(clang::ast_matchers::isDefinition()).bind("id");
    // 简单的函数调用
    static clang::ast_matchers::StatementMatcher callExprMatcher =
            clang::ast_matchers::callExpr(
                    clang::ast_matchers::hasAncestor(clang::ast_matchers::functionDecl().bind("caller")),
                    clang::ast_matchers::callee(clang::ast_matchers::functionDecl().bind("callee")));

    matchFinder.addMatcher(recordMatcher, &classCallback);
    matchFinder.addMatcher(callExprMatcher, &funcCallback);
}

beacon::BeaconASTConsumer::~BeaconASTConsumer() = default;

bool beacon::BeaconASTConsumer::HandleTopLevelDecl(clang::DeclGroupRef D) {
    return ASTConsumer::HandleTopLevelDecl(D);
}

void beacon::BeaconASTConsumer::HandleTranslationUnit(clang::ASTContext &Context) {
    matchFinder.matchAST(Context);
}


