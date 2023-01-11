#pragma once
#include <regex>
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/SourceManager.h"
#include "Beacon/Model/TranslationUnitModel.h"

namespace beacon::tracker {
        using namespace clang;
        class BeaconPPCallbackTracker : public clang::PPCallbacks {
            model::TranslationUnitModel &TU;
            std::vector<std::regex> const&filters;
            clang::Preprocessor &PP;


        public:
            BeaconPPCallbackTracker(model::TranslationUnitModel &TUModel, std::vector<std::regex> const& filters,clang::Preprocessor &PP);
            ~BeaconPPCallbackTracker() override;

            void FileChanged(SourceLocation Loc, PPCallbacks::FileChangeReason Reason,
                             SrcMgr::CharacteristicKind FileType,
                             FileID PrevFID) override;
            void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok,
                                    llvm::StringRef FileName, bool IsAngled,
                                    CharSourceRange FilenameRange,
                                    Optional<FileEntryRef> File,
                                    llvm::StringRef SearchPath,
                                    llvm::StringRef RelativePath, const Module *Imported,
                                    SrcMgr::CharacteristicKind FileType) override;
            void MacroExpands(const Token &MacroNameTok, const MacroDefinition &MD,
                              SourceRange Range, const MacroArgs *Args) override;
            void MacroDefined(const Token &MacroNameTok,
                              const MacroDirective *MD) override;
            void MacroUndefined(const Token &MacroNameTok, const MacroDefinition &MD,
                                const MacroDirective *Undef) override;
        };

    } // tracker

