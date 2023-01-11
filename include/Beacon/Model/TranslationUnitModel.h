#pragma once
#include <vector>
#include <string>
#include "Comm/json.hpp"

namespace beacon::model {
        class HashLocInfo {
        public:
            std::string file;
            int line{0};
            int column{0};
            bool isInvalid{false};
            bool isFileID{false};
        public:
            NLOHMANN_DEFINE_TYPE_INTRUSIVE(HashLocInfo, file, line, column, isInvalid, isFileID);
        };

        class MacroInfo {

        };

        class IncludeInfo {
        public:
            std::string filename;
            std::string absolutePath;
            std::string searchPath;
            std::string module;
            bool isAngleBracket{false};
            HashLocInfo hashLocInfo;
        public:
            NLOHMANN_DEFINE_TYPE_INTRUSIVE(IncludeInfo, filename, absolutePath, searchPath, module, isAngleBracket, hashLocInfo);
        };

        class ClassInfo {

        };

        class VariableInfo {

        };

        class FunctionInfo {

        };

        class TranslationUnitModel {
        public:
            std::string source_file;
            std::vector<MacroInfo> macro_list;
            std::vector<IncludeInfo> include_list;
            std::vector<ClassInfo> class_list;
            std::vector<VariableInfo> var_list;
            std::vector<FunctionInfo> func_list;
        public:
            NLOHMANN_DEFINE_TYPE_INTRUSIVE(TranslationUnitModel, source_file, include_list);
        };

    } // model

// //大小不是8字节整数倍的不能用
//template <typename PODType>
//std::size_t PODTypeHash(PODType const& pod) {
//    std::size_t hash = 0;
//    auto* data = reinterpret_cast<uint8_t*>(&pod);
//    for(int i = 0; i < sizeof(PODType); ++i) {
//        hash = hash * 131 + data[i];
//    }
//}

template <>
struct ::std::hash<beacon::model::HashLocInfo> {
    std::size_t operator()(beacon::model::HashLocInfo const& locInfo) const noexcept {
        std::hash<std::string> strHasher;
        std::size_t hash = strHasher(locInfo.file);
        hash = hash ^ (locInfo.line << 1);
        hash = hash ^ (locInfo.column << 1);
        return hash;
    }
};


template <>
struct std::equal_to<beacon::model::HashLocInfo> {
    constexpr bool operator()(beacon::model::HashLocInfo const& lhs, beacon::model::HashLocInfo const& rhs) const {
        return (lhs.file == rhs.file) && (lhs.line == rhs.line) && (lhs.column == rhs.column);
    }
};

template <>
struct ::std::hash<beacon::model::IncludeInfo>{
    std::size_t operator()(beacon::model::IncludeInfo const& includeInfo) const noexcept {
        std::hash<beacon::model::HashLocInfo> hasher;
        return hasher(includeInfo.hashLocInfo);
    }
};

template <>
struct std::equal_to<beacon::model::IncludeInfo> {
    constexpr bool operator()(
            beacon::model::IncludeInfo const& lhs,
            beacon::model::IncludeInfo const& rhs) const {
        std::equal_to<beacon::model::HashLocInfo> equal_func;
        return equal_func(lhs.hashLocInfo, rhs.hashLocInfo);
    }
};

template <>
struct ::std::hash<beacon::model::TranslationUnitModel>{
    std::size_t operator()(beacon::model::TranslationUnitModel const& TU) const noexcept {
        std::hash<std::string> strHasher;
        return strHasher(TU.source_file);
    }
};

template <>
struct std::equal_to<beacon::model::TranslationUnitModel> {
    constexpr bool operator()(
            beacon::model::TranslationUnitModel const& lhs,
            beacon::model::TranslationUnitModel const& rhs) const {
        return lhs.source_file == rhs.source_file;
    }
};