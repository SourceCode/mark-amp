/// @file NotebookStorageResolver.cpp
/// @brief V20 P03-T02: Notebook storage resolver implementation.

#include "NotebookStorageResolver.h"

#include "Logger.h"

#include <filesystem>

namespace markamp::core
{

namespace fs = std::filesystem;

auto NotebookStorageResolver::resolve(const std::string& workspace_root,
                                        const std::string& explicit_path,
                                        const std::string& knowledgebase_dir)
    -> NotebookStorageLocation
{
    ++resolution_count_;
    NotebookStorageLocation location;

    // Priority: explicit path > workspace > knowledgebase > empty
    if (!explicit_path.empty())
    {
        location.resolved_path = explicit_path;
        location.display_label = "Explicit: " + explicit_path;
        location.is_workspace = !workspace_root.empty() &&
                                is_workspace_path(explicit_path, workspace_root);
    }
    else if (!workspace_root.empty())
    {
        location.resolved_path = workspace_notebook_dir(workspace_root);
        location.display_label = "Workspace: " + location.resolved_path;
        location.is_workspace = true;
    }
    else if (!knowledgebase_dir.empty())
    {
        location.resolved_path = knowledgebase_dir;
        location.display_label = "Knowledgebase: " + knowledgebase_dir;
        location.is_legacy = true;
    }

    MARKAMP_LOG_DEBUG("Notebook storage resolved: '{}'", location.resolved_path);
    return location;
}

auto NotebookStorageResolver::workspace_notebook_dir(const std::string& workspace_root) const
    -> std::string
{
    if (workspace_root.empty())
    {
        return {};
    }
    return (fs::path(workspace_root) / "notebooks").string();
}

auto NotebookStorageResolver::is_workspace_path(const std::string& path,
                                                  const std::string& workspace_root) const -> bool
{
    if (path.empty() || workspace_root.empty())
    {
        return false;
    }
    return path.find(workspace_root) == 0;
}

auto NotebookStorageResolver::is_legacy_path(const std::string& path,
                                               const std::string& knowledgebase_dir) const -> bool
{
    if (path.empty() || knowledgebase_dir.empty())
    {
        return false;
    }
    return path.find(knowledgebase_dir) == 0;
}

} // namespace markamp::core
