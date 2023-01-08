#pragma once

#include <string>
#include <vector>

namespace proj2model {
    struct FilterConfig {
        std::string path;
        enum class Type : int {
            UNKNOWN, INCLUDE, EXCLUDE
        } type{Type::UNKNOWN};
    };

    struct ProjectConfig {
        std::string name;
        std::string compdb;
        std::string root_dir;
        std::vector<struct FilterConfig> path_filters;

        bool shouldInclude(const std::string &path) const;
    };

    struct OutputConfig {
        std::string temp_dir;
        std::string result_file;
        std::string index_file;
        enum class Type {
            UNKNOWN, JSON, PROTO
        } type{Type::UNKNOWN};
    };

    class Config {
    public:
        struct ProjectConfig project;
        struct OutputConfig output;

        static Config& init(const std::string& filename) {
            static Config config(filename);
            return config;
        }
        static const Config& get() {
            return init("");
        }
    private:
        void loadFromFile(const std::string& filename);

        Config() = default;
        explicit Config(const std::string& filename) {
            loadFromFile(filename);
        }
        ~Config() = default;
        Config(const Config&) = default;
        Config& operator=(const Config&) = default;
    };
}



