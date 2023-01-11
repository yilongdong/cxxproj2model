#include <fstream>
#include "Comm/log.h"
#include "Comm/json.hpp"
#include "proj2model/Config.h"
#include "Comm/Util/util.h"

using json = nlohmann::json;

template <typename ConfigItemType>
void loadFromJson(ConfigItemType& , const json&) {
    LOG_ERROR("not implemented");
}

template<>
void loadFromJson(proj2model::FilterConfig& filter_config, const json& filter_json) {
    filter_config.path = filter_json["path"];
    std::string type = filter_json["type"];
    if (type == "include") {
        filter_config.type = proj2model::FilterConfig::Type::INCLUDE;
    }
    else if (type == "exclude"){
        filter_config.type = proj2model::FilterConfig::Type::EXCLUDE;
    }
    else {
        filter_config.type = proj2model::FilterConfig::Type::UNKNOWN;
    }
}

template<>
void loadFromJson(proj2model::OutputConfig& output_config, const json& output_json) {
    output_config.temp_dir = output_json["temp-dir"];
    output_config.result_file = output_json["result-file"];
    output_config.index_file = output_json["index-file"];
    std::string type = output_json["type"];
    if (type == "json") {
        output_config.type = proj2model::OutputConfig::Type::JSON;
    }
    else if (type == "proto"){
        output_config.type = proj2model::OutputConfig::Type::PROTO;
    }
    else {
        output_config.type = proj2model::OutputConfig::Type::UNKNOWN;
    }
}

template<>
void loadFromJson(proj2model::ProjectConfig& project_config, const json& project_json) {
    project_config.name = project_json["name"];
    project_config.compdb = project_json["compile-commands-json"];
    project_config.root_dir = project_json["root-dir"];
    for (const auto& path_filter_json : project_json["path-filters"]) {
        project_config.path_filters.push_back({});
        loadFromJson(project_config.path_filters.back(), path_filter_json);
    }
}

template<>
void loadFromJson(proj2model::Config& config, const json& config_json) {
    const auto& project_json =  config_json["project"];
    const auto& output_json =  config_json["output"];
    loadFromJson(config.project, project_json);
    loadFromJson(config.output, output_json);
}

void proj2model::Config::loadFromFile(const std::string &filename) {
    // TODO: 支持TOML配置文件
    if (filename.substr(filename.find_last_of('.') + 1) == "json") {
        std::ifstream fin(filename);
        json config_json = json::parse(fin);
        loadFromJson(*this, config_json);
    } else {
        LOG_ERROR("暂时只支持json配置文件");
    }
}


bool proj2model::ProjectConfig::shouldInclude(const std::string &path) const {
    bool is_included = false, is_excluded = false;
    for (const auto& path_filter : path_filters) {
        std::string path_prefix = root_dir + "/" + path_filter.path;
        path_prefix.pop_back();
        if(comm::util::start_with(path, path_prefix)) {
            is_included = (path_filter.type == proj2model::FilterConfig::Type::INCLUDE);
            is_excluded = (path_filter.type == proj2model::FilterConfig::Type::EXCLUDE);
        }
    }
    return is_included && !is_excluded;
};