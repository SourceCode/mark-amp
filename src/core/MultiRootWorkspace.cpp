/// @file MultiRootWorkspace.cpp
/// @brief Phase 20 – Multi-root workspace implementation.

#include "core/MultiRootWorkspace.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>

namespace markamp::core
{

namespace fs = std::filesystem;

// ============================================================================
// Construction
// ============================================================================

MultiRootWorkspace::MultiRootWorkspace(EventBus& event_bus)
    : event_bus_(event_bus)
{
    config_.created_at = current_timestamp();
}

// ============================================================================
// Root Management
// ============================================================================

auto MultiRootWorkspace::add_root(const std::string& path, const std::string& display_name)
    -> std::expected<std::string, std::string>
{
    if (path.empty())
    {
        return std::unexpected("Empty path");
    }

    // Check for duplicate paths.
    for (const auto& root : config_.roots)
    {
        if (root.path == path)
        {
            return std::unexpected("Root already exists: " + path);
        }
    }

    auto root_id = generate_root_id();

    WorkspaceRoot root;
    root.id = root_id;
    root.path = path;
    root.display_name = display_name.empty() ? fs::path(path).filename().string() : display_name;
    root.sort_order = static_cast<int>(config_.roots.size());
    root.is_active = config_.roots.empty(); // First root is active by default.

    config_.roots.push_back(std::move(root));

    // Publish event.
    events::WorkspaceRootAddedEvent evt;
    evt.root_path = path;
    evt.display_name = display_name;
    event_bus_.publish(evt);

    return root_id;
}

auto MultiRootWorkspace::remove_root(const std::string& root_id) -> bool
{
    auto iter = std::find_if(config_.roots.begin(),
                             config_.roots.end(),
                             [&root_id](const WorkspaceRoot& root) { return root.id == root_id; });

    if (iter == config_.roots.end())
    {
        return false;
    }

    config_.roots.erase(iter);

    // Re-number sort order.
    for (int idx = 0; idx < static_cast<int>(config_.roots.size()); ++idx)
    {
        config_.roots[static_cast<std::size_t>(idx)].sort_order = idx;
    }

    return true;
}

void MultiRootWorkspace::reorder_roots(const std::vector<std::string>& root_ids)
{
    std::vector<WorkspaceRoot> reordered;
    reordered.reserve(root_ids.size());

    for (const auto& root_id : root_ids)
    {
        auto iter =
            std::find_if(config_.roots.begin(),
                         config_.roots.end(),
                         [&root_id](const WorkspaceRoot& root) { return root.id == root_id; });
        if (iter != config_.roots.end())
        {
            reordered.push_back(*iter);
        }
    }

    // Update sort order.
    for (int idx = 0; idx < static_cast<int>(reordered.size()); ++idx)
    {
        reordered[static_cast<std::size_t>(idx)].sort_order = idx;
    }

    config_.roots = std::move(reordered);
}

auto MultiRootWorkspace::set_active_root(const std::string& root_id) -> bool
{
    bool found = false;
    for (auto& root : config_.roots)
    {
        root.is_active = (root.id == root_id);
        if (root.is_active)
        {
            found = true;
        }
    }
    return found;
}

auto MultiRootWorkspace::active_root() const -> std::optional<WorkspaceRoot>
{
    for (const auto& root : config_.roots)
    {
        if (root.is_active)
        {
            return root;
        }
    }
    return std::nullopt;
}

auto MultiRootWorkspace::all_roots() const -> std::vector<WorkspaceRoot>
{
    auto roots = config_.roots;
    std::sort(roots.begin(),
              roots.end(),
              [](const WorkspaceRoot& root_a, const WorkspaceRoot& root_b)
              { return root_a.sort_order < root_b.sort_order; });
    return roots;
}

auto MultiRootWorkspace::root_count() const -> std::size_t
{
    return config_.roots.size();
}

auto MultiRootWorkspace::find_root_for_path(const std::string& file_path) const
    -> std::optional<WorkspaceRoot>
{
    for (const auto& root : config_.roots)
    {
        if (file_path.starts_with(root.path))
        {
            return root;
        }
    }
    return std::nullopt;
}

// ============================================================================
// Workspace File I/O
// ============================================================================

auto MultiRootWorkspace::save_workspace_file(const std::string& path) const
    -> std::expected<void, std::string>
{
    if (path.empty())
    {
        return std::unexpected("Empty path");
    }

    // Simplified: in production, would serialize config_ to JSON.
    return {};
}

auto MultiRootWorkspace::load_workspace_file(const std::string& path)
    -> std::expected<void, std::string>
{
    if (path.empty())
    {
        return std::unexpected("Empty path");
    }

    // Simplified: in production, would parse JSON from file.
    return {};
}

// ============================================================================
// File Tree
// ============================================================================

auto MultiRootWorkspace::merge_file_trees() const -> FileNode
{
    FileNode merged;
    merged.id = "workspace-root";
    merged.name = config_.name.empty() ? "Workspace" : config_.name;
    merged.type = FileNodeType::Folder;
    merged.is_open = true;

    for (const auto& root : config_.roots)
    {
        FileNode root_node;
        root_node.id = root.path;
        root_node.name = root.display_name;
        root_node.type = FileNodeType::Folder;
        root_node.is_open = true;
        merged.children.push_back(std::move(root_node));
    }

    return merged;
}

// ============================================================================
// Configuration
// ============================================================================

auto MultiRootWorkspace::name() const -> const std::string&
{
    return config_.name;
}

void MultiRootWorkspace::set_name(const std::string& workspace_name)
{
    config_.name = workspace_name;
}

auto MultiRootWorkspace::get_setting(const std::string& key) const -> std::optional<std::string>
{
    auto iter = config_.settings_overrides.find(key);
    if (iter != config_.settings_overrides.end())
    {
        return iter->second;
    }
    return std::nullopt;
}

void MultiRootWorkspace::set_setting(const std::string& key, const std::string& value)
{
    config_.settings_overrides[key] = value;
}

void MultiRootWorkspace::clear()
{
    config_.roots.clear();
    config_.settings_overrides.clear();
    config_.name.clear();
}

// ============================================================================
// Private Helpers
// ============================================================================

auto MultiRootWorkspace::generate_root_id() -> std::string
{
    return "root-" + std::to_string(next_root_id_++);
}

auto MultiRootWorkspace::current_timestamp() -> std::string
{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::array<char, 32> buffer{};
    struct tm time_info
    {
    };
    gmtime_r(&time_t_now, &time_info);
    std::strftime(buffer.data(), buffer.size(), "%Y-%m-%dT%H:%M:%SZ", &time_info);
    return {buffer.data()};
}

} // namespace markamp::core
