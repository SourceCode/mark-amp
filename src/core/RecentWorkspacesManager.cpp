/// @file RecentWorkspacesManager.cpp
/// @brief Phase 40 Task 5 — Recent workspaces implementation.

#include "core/RecentWorkspacesManager.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace markamp::core
{

void RecentWorkspacesManager::add(const std::string& path, const std::string& name)
{
    // Remove if exists (will re-add at front)
    remove(path);

    RecentWorkspace ws;
    ws.path = path;
    ws.name = name.empty() ? std::filesystem::path(path).filename().string() : name;
    ws.last_opened = "now"; // Simplified timestamp

    workspaces_.insert(workspaces_.begin(), ws);

    // Trim to max
    if (workspaces_.size() > kMaxRecent)
        workspaces_.resize(kMaxRecent);
}

auto RecentWorkspacesManager::remove(const std::string& path) -> bool
{
    auto it = std::remove_if(workspaces_.begin(),
                             workspaces_.end(),
                             [&](const RecentWorkspace& w) { return w.path == path; });
    if (it == workspaces_.end())
        return false;
    workspaces_.erase(it, workspaces_.end());
    return true;
}

auto RecentWorkspacesManager::all() const -> const std::vector<RecentWorkspace>&
{
    return workspaces_;
}

auto RecentWorkspacesManager::most_recent() const -> const RecentWorkspace*
{
    return workspaces_.empty() ? nullptr : &workspaces_.front();
}

void RecentWorkspacesManager::clear()
{
    workspaces_.clear();
}

void RecentWorkspacesManager::cleanup_missing()
{
    workspaces_.erase(std::remove_if(workspaces_.begin(),
                                     workspaces_.end(),
                                     [](const RecentWorkspace& w)
                                     { return !std::filesystem::exists(w.path); }),
                      workspaces_.end());
}

auto RecentWorkspacesManager::count() const -> std::size_t
{
    return workspaces_.size();
}

void RecentWorkspacesManager::save(const std::string& config_dir) const
{
    auto path = std::filesystem::path(config_dir) / "recent_workspaces.json";
    std::filesystem::create_directories(config_dir);

    nlohmann::json j = nlohmann::json::array();
    for (const auto& ws : workspaces_)
    {
        j.push_back({{"path", ws.path}, {"name", ws.name}, {"lastOpened", ws.last_opened}});
    }

    std::ofstream file(path);
    if (file.is_open())
        file << j.dump(2);
}

void RecentWorkspacesManager::load(const std::string& config_dir)
{
    auto path = std::filesystem::path(config_dir) / "recent_workspaces.json";
    if (!std::filesystem::exists(path))
        return;

    std::ifstream file(path);
    if (!file.is_open())
        return;

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    auto j = nlohmann::json::parse(content, nullptr, false);
    if (j.is_discarded() || !j.is_array())
        return;

    workspaces_.clear();
    for (const auto& entry : j)
    {
        RecentWorkspace ws;
        ws.path = entry.value("path", "");
        ws.name = entry.value("name", "");
        ws.last_opened = entry.value("lastOpened", "");
        if (!ws.path.empty())
            workspaces_.push_back(ws);
    }
}

} // namespace markamp::core
