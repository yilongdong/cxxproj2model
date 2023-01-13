#pragma once
#include "Beacon/Tracker/MatchCallbackBase.h"
#include "TU.pb.h"

namespace beacon::callback {

class FuncCallFuncExprCallback : public MatchCallbackBase {
public:
  explicit FuncCallFuncExprCallback(clang::CompilerInstance &CI, beacon::model::TU &TU) : MatchCallbackBase(CI), TU(TU) {}
  ~FuncCallFuncExprCallback() override = default;
  void run(clang::ast_matchers::MatchFinder::MatchResult const& Result) override;

  beacon::model::TU &TU;
};
} // namespace CodeBeacon