/// @file WorkspaceConfig.cpp
/// @brief Phase 40 Task 1 — WorkspaceConfig implementation.

#include "core/WorkspaceConfig.h"

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <yaml-cpp/yaml.h>

namespace markamp::core
{

auto WorkspaceConfig::to_yaml() const -> std::string
{
    YAML::Emitter out;
    out << YAML::BeginMap;

    // folders
    out << YAML::Key << "folders" << YAML::Value << YAML::BeginSeq;
    for (const auto& f : folders)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "path" << YAML::Value << f.path;
        if (!f.name.empty())
            out << YAML::Key << "name" << YAML::Value << f.name;
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;

    // extensions
    if (!recommended_extensions.empty())
    {
        out << YAML::Key << "extensions" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "recommendations" << YAML::Value << YAML::BeginSeq;
        for (const auto& ext : recommended_extensions)
            out << ext.extension_id;
        out << YAML::EndSeq;
        if (!unwanted_extensions.empty())
        {
            out << YAML::Key << "unwantedRecommendations" << YAML::Value << YAML::BeginSeq;
            for (const auto& ext : unwanted_extensions)
                out << ext;
            out << YAML::EndSeq;
        }
        out << YAML::EndMap;
    }

    // exclude patterns
    if (!exclude_patterns.empty())
    {
        out << YAML::Key << "excludePatterns" << YAML::Value << YAML::BeginSeq;
        for (const auto& p : exclude_patterns)
            out << p;
        out << YAML::EndSeq;
    }

    out << YAML::EndMap;
    return out.c_str();
}

auto WorkspaceConfig::from_yaml(const std::string& yaml_str) -> WorkspaceConfig
{
    WorkspaceConfig config;
    if (yaml_str.empty())
        return config;

    YAML::Node node = YAML::Load(yaml_str);
    if (!node.IsMap())
        return config;

    if (node["folders"])
    {
        for (const auto& f : node["folders"])
        {
            WorkspaceRootFolder folder;
            folder.path = f["path"].as<std::string>("");
            folder.name = f["name"].as<std::string>("");
            config.folders.push_back(folder);
        }
    }

    if (node["extensions"])
    {
        auto ext = node["extensions"];
        if (ext["recommendations"])
        {
            for (const auto& rec : ext["recommendations"])
            {
                ExtensionRecommendation r;
                r.extension_id = rec.as<std::string>();
                config.recommended_extensions.push_back(r);
            }
        }
        if (ext["unwantedRecommendations"])
        {
            for (const auto& u : ext["unwantedRecommendations"])
                config.unwanted_extensions.push_back(u.as<std::string>());
        }
    }

    if (node["excludePatterns"])
    {
        for (const auto& p : node["excludePatterns"])
            config.exclude_patterns.push_back(p.as<std::string>());
    }

    return config;
}

auto WorkspaceConfig::is_multi_root() const -> bool
{
    return folders.size() > 1;
}

void WorkspaceConfig::add_folder(const std::string& path, const std::string& name)
{
    WorkspaceRootFolder folder;
    folder.path = path;
    folder.name = name.empty() ? std::filesystem::path(path).filename().string() : name;
    folders.push_back(folder);
}

auto WorkspaceConfig::remove_folder(const std::string& path) -> bool
{
    auto it = std::remove_if(folders.begin(),
                             folders.end(),
                             [&](const WorkspaceRootFolder& f) { return f.path == path; });
    if (it == folders.end())
        return false;
    folders.erase(it, folders.end());
    return true;
}

void WorkspaceConfig::reorder_folder(std::size_t from_index, std::size_t to_index)
{
    if (from_index >= folders.size() || to_index >= folders.size())
        return;
    auto folder = std::move(folders[from_index]);
    folders.erase(folders.begin() + static_cast<long>(from_index));
    folders.insert(folders.begin() + static_cast<long>(to_index), std::move(folder));
}

auto WorkspaceConfig::single_folder(const std::string& path) -> WorkspaceConfig
{
    WorkspaceConfig config;
    config.add_folder(path);
    return config;
}

} // namespace markamp::core
