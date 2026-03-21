/// @file TextArtifactLifecycle.h
/// @brief V20 P02-T03: Complete text-document lifecycle orchestration.
///
/// Provides a unified service for all text artifact lifecycle operations:
/// create, open, save, save-as, rename, duplicate, move, delete, and reopen.
/// This replaces scattered logic in LayoutManager, MainFrame, TabBar, and
/// FileTreeCtrl with a single orchestration layer that keeps the artifact
/// registry, unsaved buffers, and shell state in sync.
#pragma once

#include "ArtifactCreationService.h"
#include "ArtifactRegistry.h"
#include "EventBus.h"
#include "UnsavedDocumentBuffer.h"

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{
class Config;

/// Result of a lifecycle operation.
struct LifecycleOperationResult
{
    bool success{false};
    std::string error_message;
    ArtifactId artifact_id;
    std::optional<std::string> resolved_path;

    [[nodiscard]] auto ok() const noexcept -> bool { return success; }
};

/// Encapsulates placement context for file creation (target folder, workspace root).
struct FilePlacementContext
{
    std::optional<std::string> target_directory;       ///< Where to place on first save
    std::optional<std::string> workspace_root;         ///< Current workspace root
    std::optional<std::string> selected_tree_path;     ///< Currently selected item in file tree
    std::string entry_point;                           ///< What triggered creation
};

/// Orchestrates the full lifecycle for text artifacts.
///
/// All creation entry points (menu, toolbar, palette, explorer, tab bar,
/// welcome state) funnel through this service. Lifecycle operations
/// (save, rename, duplicate, move, delete) update the artifact registry,
/// unsaved buffers, and publish events for UI synchronization.
class TextArtifactLifecycle
{
public:
    TextArtifactLifecycle(EventBus& bus, ArtifactRegistry& registry,
                           ArtifactCreationService& creation_service,
                           UnsavedDocumentBufferManager& buffer_manager, Config& config);

    // ── Creation ──

    /// Create a new text file from any entry point.
    [[nodiscard]] auto create_new_file(const FilePlacementContext& context,
                                        const std::string& display_name = {},
                                        const std::string& language_id = "markdown")
        -> LifecycleOperationResult;

    // ── Open ──

    /// Open an existing file from disk. Registers it if not already registered.
    [[nodiscard]] auto open_file(const std::string& file_path) -> LifecycleOperationResult;

    /// Reopen a previously closed file (via history/recents).
    [[nodiscard]] auto reopen_file(const std::string& file_path) -> LifecycleOperationResult;

    // ── Save ──

    /// Save an artifact (first save or subsequent).
    [[nodiscard]] auto save(const ArtifactId& artifact_id) -> LifecycleOperationResult;

    /// Save an artifact to a new path (Save As).
    [[nodiscard]] auto save_as(const ArtifactId& artifact_id, const std::string& new_path)
        -> LifecycleOperationResult;

    // ── Rename / Move ──

    /// Rename an artifact (changes display name and optionally filesystem name).
    [[nodiscard]] auto rename(const ArtifactId& artifact_id, const std::string& new_name)
        -> LifecycleOperationResult;

    /// Move an artifact to a new directory.
    [[nodiscard]] auto move_to(const ArtifactId& artifact_id, const std::string& new_directory)
        -> LifecycleOperationResult;

    // ── Duplicate ──

    /// Duplicate an artifact (creates a new artifact with copied content).
    [[nodiscard]] auto duplicate(const ArtifactId& artifact_id) -> LifecycleOperationResult;

    // ── Delete ──

    /// Delete an artifact from registry and optionally from disk.
    [[nodiscard]] auto remove(const ArtifactId& artifact_id, bool delete_from_disk = false)
        -> LifecycleOperationResult;

    // ── Close ──

    /// Close an artifact (unmount from editor, optionally prompt for save).
    [[nodiscard]] auto close(const ArtifactId& artifact_id) -> LifecycleOperationResult;

    // ── Queries ──

    /// Check if an artifact has unsaved changes.
    [[nodiscard]] auto has_unsaved_changes(const ArtifactId& artifact_id) const -> bool;

    /// Get the unsaved buffer for an artifact (nullptr if none).
    [[nodiscard]] auto get_buffer(const ArtifactId& artifact_id) const -> const UnsavedBuffer*;

    /// Count of open text artifacts.
    [[nodiscard]] auto open_text_file_count() const -> int;

    /// Count of unsaved (never-persisted) text artifacts.
    [[nodiscard]] auto unsaved_count() const -> int;

    /// Count of dirty (modified since last save) text artifacts.
    [[nodiscard]] auto dirty_count() const -> int;

    /// Total lifecycle operations performed.
    [[nodiscard]] auto operation_count() const noexcept -> int { return operation_count_; }

private:
    EventBus& event_bus_;
    ArtifactRegistry& registry_;
    ArtifactCreationService& creation_service_;
    UnsavedDocumentBufferManager& buffer_manager_;
    Config& config_;

    int operation_count_{0};
};

} // namespace markamp::core
