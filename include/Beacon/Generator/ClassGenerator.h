#pragma once
#include <unordered_set>
#include <clang/AST/Type.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/ASTContext.h>
#include "TU.pb.h"

namespace beacon::generator {

// 实现参考
// https://github.com/bkryza/clang-uml
class ClassGenerator {
public:
  static void Create(
      clang::CXXRecordDecl const& clsDecl,
      beacon::model::Class *clsModel,
      clang::SourceManager const& sourceManager);
  static beacon::model::Class::Access toAccessModel(clang::AccessSpecifier const& accessSpecifier);
};

}