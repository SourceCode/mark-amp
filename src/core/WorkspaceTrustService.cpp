/// @file WorkspaceTrustService.cpp
/// @brief Phase 40 Task 4 — Workspace trust implementation.

#include "core/WorkspaceTrustService.h"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

namespace markamp::core
{

auto WorkspaceTrustService::is_trusted(const std::string& workspace_path) const -> bool
{
    return trusted_paths_.count(workspace_path) > 0;
}

auto WorkspaceTrustService::trust_level(const std::string& workspace_path) const
    -> WorkspaceTrustLevel
{
    if (is_trusted(workspace_path))
        return WorkspaceTrustLevel::kTrusted;
    return WorkspaceTrustLevel::kUntrusted;
}

void WorkspaceTrustService::grant_trust(const std::string& workspace_path)
{
    trusted_paths_.insert(workspace_path);
}

void WorkspaceTrustService::revoke_trust(const std::string& workspace_path)
{
    trusted_paths_.erase(workspace_path);
}

auto WorkspaceTrustService::trusted_workspaces() const -> std::vector<std::string>
{
    return {trusted_paths_.begin(), trusted_paths_.end()};
}

auto WorkspaceTrustService::is_feature_allowed(const std::string& feature,
                                               WorkspaceTrustLevel level) const -> bool
{
    if (level == WorkspaceTrustLevel::kTrusted)
        return true;

    // In restricted/untrusted mode, disallow dangerous features
    auto restricted = restricted_features();
    for (const auto& f : restricted)
    {
        if (f == feature)
            return false;
    }
    return true;
}

auto WorkspaceTrustService::restricted_features() const -> std::vector<std::string>
{
    return {"task.execution",
            "terminal.commands",
            "extension.activation",
            "build.commands",
            "debug.launch"};
}

void WorkspaceTrustService::save(const std::string& config_dir) const
{
    auto path = std::filesystem::path(config_dir) / "trusted_workspaces.json";
    std::filesystem::create_directories(config_dir);

    nlohmann::json j;
    j["trusted"] = nlohmann::json::array();
    for (const auto& p : trusted_paths_)
        j["trusted"].push_back(p);

    std::ofstream file(path);
    if (file.is_open())
        file << j.dump(2);
}

void WorkspaceTrustService::load(const std::string& config_dir)
{
    auto path = std::filesystem::path(config_dir) / "trusted_workspaces.json";
    if (!std::filesystem::exists(path))
        return;

    std::ifstream file(path);
    if (!file.is_open())
        return;

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    auto j = nlohmann::json::parse(content, nullptr, false);
    if (j.is_discarded())
        return;

    if (j.contains("trusted"))
    {
        for (const auto& p : j["trusted"])
            trusted_paths_.insert(p.get<std::string>());
    }
}

} // namespace markamp::core
