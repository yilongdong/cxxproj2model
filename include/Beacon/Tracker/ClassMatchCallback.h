#pragma once
#include <unordered_set>
#include <clang/AST/Decl.h>
#include "Beacon/Tracker//MatchCallbackBase.h"
#include "TU.pb.h"

namespace beacon::callback {

class ClassMatchCallback : public MatchCallbackBase {
public:
  explicit ClassMatchCallback(clang::CompilerInstance &CI, beacon::model::TU &TU) : MatchCallbackBase(CI), TU(TU) {}
  ~ClassMatchCallback() override = default;
  void run(clang::ast_matchers::MatchFinder::MatchResult const& Result) override;
  bool shouldSkip(clang::CXXRecordDecl const* pClsDecl);

  static std::unordered_set<size_t> classIDSet;
  static std::unordered_set<size_t> fileIDSet;
  beacon::model::TU &TU;
};

} // namespace CodeBeacon
