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
    [[nodiscard]] beacon::model::HashLocInfo to_model(clang::SourceLocation const& value) const;

};


BeaconPPCallbackTracker::BeaconPPCallbackTracker(beacon::model::TranslationUnitModel &TUModel,
                                                 std::vector<std::regex> const&filters,
                                                 clang::Preprocessor &PP) : TU(TUModel), filters(filters), PP(PP) {}

BeaconPPCallbackTracker::~BeaconPPCallbackTracker() = default;

void BeaconPPCallbackTracker::FileChanged(clang::SourceLocation Loc,
                                          clang::PPCallbacks::FileChangeReason Reason,
                                          clang::SrcMgr::CharacteristicKind FileType,
                                          clang::FileID PrevFID) {
//    LOG_INFO("FileChanged");
}

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
    auto absolutePath = !File ? "(null)" : convertor.to_string(*File);
    for (auto const& regex : filters) {
        if (std::regex_match(absolutePath, regex)) {
            return;
        }
    }

    TU.include_list.push_back({});
    auto& includeInfo = TU.include_list.back();
    includeInfo.hashLocInfo = convertor.to_model(HashLoc);
    includeInfo.filename = convertor.to_string(FileName);
    includeInfo.isAngleBracket = IsAngled;
    includeInfo.absolutePath = absolutePath;
    includeInfo.searchPath = convertor.to_string(SearchPath);
    includeInfo.module = !Imported ? "(null)" : convertor.to_string(*Imported);
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

beacon::model::HashLocInfo ClangTypeConvertor::to_model(const SourceLocation &value) const {
    beacon::model::HashLocInfo locInfo;
    locInfo.isInvalid = value.isInvalid();
    locInfo.isFileID = value.isFileID();
    PresumedLoc PLoc = PP.getSourceManager().getPresumedLoc(value);
    locInfo.isInvalid |= PLoc.isInvalid();
    if (locInfo.isInvalid || !locInfo.isFileID) {
        return locInfo;
    }
    locInfo.file = PLoc.getFilename();
    locInfo.line = PLoc.getLine();
    locInfo.column = PLoc.getColumn();
    return locInfo;
}
