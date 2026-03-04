#pragma once

/// @file RecentWorkspacesManager.h
/// @brief Phase 40 Task 5 — Track recently opened workspaces.

#include <string>
#include <vector>

namespace markamp::core
{

/// A recently opened workspace entry.
struct RecentWorkspace
{
    std::string path;
    std::string name;
    std::string last_opened; ///< ISO 8601 timestamp
};

/// Tracks recently opened workspaces with persistence.
class RecentWorkspacesManager
{
public:
    RecentWorkspacesManager() = default;

    /// Add or promote a workspace.
    void add(const std::string& path, const std::string& name = "");

    /// Remove a workspace from recents.
    auto remove(const std::string& path) -> bool;

    /// Get all recent workspaces (most recent first).
    [[nodiscard]] auto all() const -> const std::vector<RecentWorkspace>&;

    /// Get the most recently opened workspace.
    [[nodiscard]] auto most_recent() const -> const RecentWorkspace*;

    /// Clear all entries.
    void clear();

    /// Remove entries whose paths no longer exist.
    void cleanup_missing();

    /// Count of recent workspaces.
    [[nodiscard]] auto count() const -> std::size_t;

    /// Persistence.
    void save(const std::string& config_dir) const;
    void load(const std::string& config_dir);

    static constexpr std::size_t kMaxRecent = 20;

private:
    std::vector<RecentWorkspace> workspaces_;
};

} // namespace markamp::core
