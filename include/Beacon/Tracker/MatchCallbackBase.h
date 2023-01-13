#pragma once

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Frontend/CompilerInstance.h>

namespace beacon::callback {

class MatchCallbackBase : public clang::ast_matchers::MatchFinder::MatchCallback {
protected:
  clang::CompilerInstance &CI;
  template <class NodeType> std::string getFilenameOfNode(NodeType node) {
    return CI.getSourceManager().getFilename(node->getSourceRange().getBegin()).str();
  }
  clang::SourceManager& source_manager() {
    return CI.getSourceManager();
  }
  [[nodiscard]] virtual bool isUserSourceCode(std::string const& filename) const {
    return !filename.empty() && filename.find("/Library/Developer/CommandLineTools/") != 0;
  }

public:
  explicit MatchCallbackBase(clang::CompilerInstance &CI) : CI(CI) {}
  ~MatchCallbackBase() override = default;
  void run(clang::ast_matchers::MatchFinder::MatchResult const& Result) override {}
};

} // namespace CodeBeacon

