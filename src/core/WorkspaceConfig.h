#pragma once

/// @file WorkspaceConfig.h
/// @brief Phase 40 Task 1 — Multi-root workspace data model (.markamp-workspace).

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// A root folder in a multi-root workspace.
struct WorkspaceRootFolder
{
    std::string path; ///< Absolute path
    std::string name; ///< Display name (defaults to folder basename)
};

/// Extension recommendation for a workspace.
struct ExtensionRecommendation
{
    std::string extension_id;
    std::string reason;
};

/// Workspace trust level.
enum class WorkspaceTrustLevel : uint8_t
{
    kTrusted,
    kRestricted,
    kUntrusted,
};

/// Multi-root workspace configuration stored in .markamp-workspace file.
struct WorkspaceConfig
{
    std::vector<WorkspaceRootFolder> folders;
    std::string settings_path; ///< .markamp/settings.yaml
    std::string tasks_path;    ///< .markamp/tasks.json
    std::string launch_path;   ///< .markamp/launch.json
    std::vector<ExtensionRecommendation> recommended_extensions;
    std::vector<std::string> unwanted_extensions;
    std::vector<std::string> file_associations; ///< e.g., "*.md:markdown"
    std::vector<std::string> exclude_patterns;  ///< e.g., "**/node_modules"

    // ── Serialization ──
    [[nodiscard]] auto to_yaml() const -> std::string;
    [[nodiscard]] static auto from_yaml(const std::string& yaml_str) -> WorkspaceConfig;

    /// Check if workspace has multiple root folders.
    [[nodiscard]] auto is_multi_root() const -> bool;

    /// Add a root folder.
    void add_folder(const std::string& path, const std::string& name = "");

    /// Remove a root folder by path.
    auto remove_folder(const std::string& path) -> bool;

    /// Reorder folders (move index from -> to).
    void reorder_folder(std::size_t from_index, std::size_t to_index);

    /// Default workspace config for a single folder.
    [[nodiscard]] static auto single_folder(const std::string& path) -> WorkspaceConfig;
};

} // namespace markamp::core
