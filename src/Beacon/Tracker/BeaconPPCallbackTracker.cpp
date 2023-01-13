#include <string>
#include <sstream>
#include "Comm/log.h"
#include "Beacon/Tracker/BeaconPPCallbackTracker.h"

using namespace beacon::tracker;

class ClangTypeConvertor {
    Preprocessor &PP;

    // FileChangeReason strings.
    constexpr static const char *const FileChangeReasonStrings[] = {
            "EnterFile", "ExitFile", "SystemHeaderPragma", "RenameFile"
    };

    // CharacteristicKind strings.
    constexpr static const char *const CharacteristicKindStrings[] = { "C_User", "C_System",
                                                             "C_ExternCSystem" };

    // MacroDirective::Kind strings.
    constexpr static const char *const MacroDirectiveKindStrings[] = {
            "MD_Define","MD_Undefine", "MD_Visibility"
    };
public:
    explicit ClangTypeConvertor(Preprocessor &PP):PP(PP) {}
    // Get a "file:line:column" source location string.
    [[nodiscard]] std::string to_string(clang::SourceLocation const& value) const;
    [[nodiscard]] std::string to_string(clang::Token const& value) const;
    [[nodiscard]] std::string to_string(clang::CharSourceRange const& value) const;
    [[nodiscard]] std::string to_string(clang::FileEntryRef const& value) const;
    [[nodiscard]] std::string to_string(clang::Module const& value) const;
    [[nodiscard]] std::string to_string(llvm::StringRef const& value) const;

};


BeaconPPCallbackTracker::BeaconPPCallbackTracker(beacon::model::TU &TU,
                                                 std::vector<std::regex> const&filters,
                                                 clang::Preprocessor &PP) : TU(TU), filters(filters), PP(PP) {}

BeaconPPCallbackTracker::~BeaconPPCallbackTracker() = default;

bool BeaconPPCallbackTracker::isExcludeFile(std::string const& filename) {
    return std::any_of(filters.begin(), filters.end(), [&filename](std::regex const& regex) {
        return std::regex_match(filename, regex);
    });
}

void BeaconPPCallbackTracker::FileChanged(clang::SourceLocation Loc,
                                          clang::PPCallbacks::FileChangeReason Reason,
                                          clang::SrcMgr::CharacteristicKind FileType,
                                          clang::FileID PrevFID) {}

void BeaconPPCallbackTracker::InclusionDirective(clang::SourceLocation HashLoc,
                                                 const clang::Token &IncludeTok,
                                                 llvm::StringRef FileName, bool IsAngled,
                                                 clang::CharSourceRange FilenameRange,
                                                 llvm::Optional<clang::FileEntryRef> File,
                                                 llvm::StringRef SearchPath,
                                                 llvm::StringRef RelativePath,
                                                 const clang::Module *Imported,
                                                 clang::SrcMgr::CharacteristicKind FileType) {
    ClangTypeConvertor convertor(PP);
    auto filename = !File ? "(null)" : convertor.to_string(*File);
    if (isExcludeFile(filename)) {
        return;
    }
    auto& includeInfo = *TU.add_include_list();
    PresumedLoc PLoc = PP.getSourceManager().getPresumedLoc(HashLoc);
    includeInfo.mutable_source_location()->set_is_invalid(PLoc.isInvalid());
    includeInfo.mutable_source_location()->set_is_file_id(HashLoc.isFileID());
    if (HashLoc.isFileID() && !PLoc.isInvalid()) {
        includeInfo.mutable_source_location()->set_file(PLoc.getFilename());
        includeInfo.mutable_source_location()->set_line((int)PLoc.getLine());
        includeInfo.mutable_source_location()->set_column((int)PLoc.getColumn());
    }
    includeInfo.mutable_file()->set_name(filename);
    includeInfo.set_is_angle_bracket(IsAngled);
}

void BeaconPPCallbackTracker::MacroExpands(const clang::Token &MacroNameTok,
                                           const clang::MacroDefinition &MD,
                                           clang::SourceRange Range,
                                           const clang::MacroArgs *Args) {
    PPCallbacks::MacroExpands(MacroNameTok, MD, Range, Args);
}

void BeaconPPCallbackTracker::MacroDefined(const clang::Token &MacroNameTok,
                                           const clang::MacroDirective *MD) {
    const clang::MacroInfo* mi = MD->getMacroInfo();
}

void BeaconPPCallbackTracker::MacroUndefined(const clang::Token &MacroNameTok,
                                             const clang::MacroDefinition &MD,
                                             const clang::MacroDirective *Undef) {
    PPCallbacks::MacroUndefined(MacroNameTok, MD, Undef);
}


std::string ClangTypeConvertor::to_string(clang::SourceLocation const& value) const {
    if (value.isInvalid()) return {"(invalid)"};
    if (!value.isFileID()) return {"(nonfile)"};
    PresumedLoc PLoc = PP.getSourceManager().getPresumedLoc(value);
    if (PLoc.isInvalid())  return {"(invalid)"};

    std::stringstream ss;
    ss << "\"" << PLoc.getFilename() << ':' << PLoc.getLine() << ':'
       << PLoc.getColumn() << "\"";
    return ss.str();
}

std::string ClangTypeConvertor::to_string(const Token &value) const {
    return PP.getSpelling(value);
}

std::string ClangTypeConvertor::to_string(const CharSourceRange &value) const {
    const char *begin = PP.getSourceManager().getCharacterData(value.getBegin());
    const char *end = PP.getSourceManager().getCharacterData(value.getEnd());
    size_t length = static_cast<size_t>(end - begin);
    return {begin,  length};
}

std::string ClangTypeConvertor::to_string(const FileEntryRef &value) const {
    return value.getName().str();
}

std::string ClangTypeConvertor::to_string(const Module &value) const {
    return value.Name;
}

std::string ClangTypeConvertor::to_string(const StringRef &value) const {
    return value.str();
}
