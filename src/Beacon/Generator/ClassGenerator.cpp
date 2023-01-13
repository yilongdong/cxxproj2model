#include <clang/Lex/Lexer.h>
#include "Beacon/Generator/ClassGenerator.h"

void beacon::generator::ClassGenerator::Create( const clang::CXXRecordDecl &clsDecl,
                                                beacon::model::Class *clsModel,
                                                const clang::SourceManager &sourceManager) {
    clsModel->set_name(clsDecl.getNameAsString());
    clsModel->set_is_struct(clsDecl.isStruct());

    for (clang::CXXMethodDecl const* method : clsDecl.methods()) {
        if (method && (!method->isDefaulted() || method->isExplicitlyDefaulted())) {
            auto methodModel = clsModel->add_method_list();
            methodModel->set_name(method->getNameAsString());
            methodModel->set_access(toAccessModel(method->getAccess()));
            methodModel->set_return_type(method->getReturnType().getAsString());
            methodModel->set_is_const(method->isConst());
            methodModel->set_is_default(method->isDefaulted());
            methodModel->set_is_delete(method->isDeleted());
            methodModel->set_is_deprecated(method->isDeprecated());
            methodModel->set_is_implicit(method->isImplicit());
            methodModel->set_is_pure_virtual(method->isPure());
            methodModel->set_is_virtual(method->isVirtual());
            methodModel->set_is_static(method->isStatic());
            for (auto const* param : method->parameters()) {
                if (param) {
                    auto paramModel = methodModel->add_param_list();
                    paramModel->mutable_var()->set_name(param->getNameAsString());
                    paramModel->mutable_var()->set_type(param->getType().getAsString());
                    if (param->hasDefaultArg()) {
                        auto SR = param->getDefaultArg()->getSourceRange();
                        auto CR = clang::CharSourceRange::getCharRange(SR);
                        paramModel->set_default_value(clang::Lexer::getSourceText(CR, sourceManager, clang::LangOptions()).str());
                    }
                }
            }
        }
    }

    for (clang::CXXBaseSpecifier const& base : clsDecl.bases()) {
        auto baseModel = clsModel->add_base_list();
        baseModel->set_name(base.getType().getAsString());
        baseModel->set_access(toAccessModel(base.getAccessSpecifier()));
        baseModel->set_is_virtual(base.isVirtual());
    }

    for (clang::FieldDecl const* field : clsDecl.fields()) {
        if (field) {
            auto fieldModel = clsModel->add_field_list();
            fieldModel->set_access(toAccessModel(field->getAccess()));
            auto var = fieldModel->mutable_var();
            var->set_name(field->getNameAsString());
            var->set_type(field->getType().getAsString());
        }
    }
}

beacon::model::Class::Access
beacon::generator::ClassGenerator::toAccessModel(const clang::AccessSpecifier &accessSpecifier) {
    switch (accessSpecifier) {
        case clang::AccessSpecifier::AS_public:
            return beacon::model::Class::AC_PUBLIC;
        case clang::AccessSpecifier::AS_protected:
            return beacon::model::Class::AC_PROTECTED;
        case clang::AccessSpecifier::AS_private:
            return beacon::model::Class::AC_PRIVATE;
        default:
            return beacon::model::Class::AC_UNKNOWN;
    }
}
