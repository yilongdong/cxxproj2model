#pragma once
#include <memory>
#include <queue>
#include <mutex>
#include <string>
#include <filesystem>
#include "Beacon/Model/TranslationUnitModel.h"

namespace beacon {
    namespace backend {

//        class TUQueue final {
//        public:
//            void enqueue(std::unique_ptr<model::TranslationUnitModel> element);
//            std::unique_ptr<model::TranslationUnitModel> dequeue();
//            size_t size() const;
//        private:
//            std::queue<std::unique_ptr<model::TranslationUnitModel>> queue;
//            mutable std::mutex mutex;
//        };

        // 之后可以写一个git for data
        // key = hash(文件名+文件内容+数据库版本号)
        // value = snappy(文件名+文件内容)
        class StorageExecutor {
        public:
            void write(std::filesystem::path const& path, std::string const& data);
        };

//        class TUModelDatabase final {
//        public:
//            TUModelDatabase();
//            ~TUModelDatabase();
//            // 计算文件内容的hash，存储到对象数据库，返回hash
//            void submit(std::filesystem::path const& filename, std::unique_ptr<model::TranslationUnitModel> TU);
//            void init(std::string const& directory);
//        private:
//            TUQueue queue;
//            StorageExecutor executor;
//            hash hasher;
//        };
    }
}