/// @file NotebookStorageResolver.h
/// @brief V20 P03-T02: Notebook storage location resolution.
///
/// Resolves where notebooks should be placed — workspace-relative, knowledge
/// base, or explicit path — replacing the legacy knowledgebase-only default.
#pragma once

#include <optional>
#include <string>

namespace markamp::core
{

/// Storage location for a notebook.
struct NotebookStorageLocation
{
    std::string resolved_path;
    std::string display_label;    ///< Human-readable description (e.g., "Workspace: /project/notebooks")
    bool is_workspace{false};     ///< Whether the path is within a workspace
    bool is_legacy{false};        ///< Whether this is a legacy knowledgebase location
};

/// Resolves notebook storage locations based on context.
class NotebookStorageResolver
{
public:
    NotebookStorageResolver() = default;

    /// Resolve storage for a new notebook.
    [[nodiscard]] auto resolve(const std::string& workspace_root = {},
                                const std::string& explicit_path = {},
                                const std::string& knowledgebase_dir = {}) -> NotebookStorageLocation;

    /// Resolve workspace-relative notebook directory.
    [[nodiscard]] auto workspace_notebook_dir(const std::string& workspace_root) const
        -> std::string;

    /// Check if a path is within a workspace.
    [[nodiscard]] auto is_workspace_path(const std::string& path,
                                          const std::string& workspace_root) const -> bool;

    /// Detect if a notebook path is a legacy knowledgebase location.
    [[nodiscard]] auto is_legacy_path(const std::string& path,
                                       const std::string& knowledgebase_dir) const -> bool;

    /// Total resolutions performed.
    [[nodiscard]] auto resolution_count() const noexcept -> int { return resolution_count_; }

private:
    int resolution_count_{0};
};

} // namespace markamp::core
