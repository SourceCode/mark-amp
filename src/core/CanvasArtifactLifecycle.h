/// @file CanvasArtifactLifecycle.h
/// @brief V20 P04-T01/T03: Canvas artifact lifecycle orchestration.
///
/// Routes canvas/board creation through the unified artifact pipeline
/// instead of placeholder UI stubs. Provides full lifecycle: create, open,
/// save, save-as, rename, duplicate, delete, close, and restore.
/// Bridges to the existing CanvasWorkbench for actual board operations.
#pragma once

#include "ArtifactCreationService.h"
#include "ArtifactRegistry.h"
#include "EventBus.h"

#include <optional>
#include <string>

namespace markamp::core
{
class Config;

/// Result of a canvas lifecycle operation.
struct CanvasOperationResult
{
    bool success{false};
    std::string error_message;
    ArtifactId artifact_id;
    std::optional<std::string> board_id;      ///< CanvasWorkbench board identifier
    std::optional<std::string> resolved_path;

    [[nodiscard]] auto ok() const noexcept -> bool { return success; }
};

/// Context for canvas placement (where a board file lives).
struct CanvasPlacementContext
{
    std::optional<std::string> workspace_root;
    std::optional<std::string> target_directory;
    std::string entry_point;                      ///< palette, menu, toolbar, explorer, etc.
    std::optional<std::string> template_id;       ///< Optional template for creation
};

/// Orchestrates the full lifecycle for canvas board artifacts.
class CanvasArtifactLifecycle
{
public:
    CanvasArtifactLifecycle(EventBus& bus, ArtifactRegistry& registry,
                             ArtifactCreationService& creation_service, Config& config);

    // ── Creation ──

    /// Create a new canvas board.
    [[nodiscard]] auto create_board(const CanvasPlacementContext& context,
                                     const std::string& name = {}) -> CanvasOperationResult;

    /// Create a board from a template.
    [[nodiscard]] auto create_from_template(const CanvasPlacementContext& context,
                                              const std::string& template_id,
                                              const std::string& name = {})
        -> CanvasOperationResult;

    // ── Open ──

    /// Open an existing board from disk.
    [[nodiscard]] auto open_board(const std::string& file_path) -> CanvasOperationResult;

    /// Restore a board from session data.
    [[nodiscard]] auto restore_board(const std::string& file_path,
                                      const std::string& board_id_hint = {})
        -> CanvasOperationResult;

    // ── Save ──

    /// Save a canvas artifact.
    [[nodiscard]] auto save(const ArtifactId& artifact_id) -> CanvasOperationResult;

    /// Save a canvas to a new path.
    [[nodiscard]] auto save_as(const ArtifactId& artifact_id, const std::string& new_path)
        -> CanvasOperationResult;

    // ── Rename / Duplicate / Delete ──

    /// Rename a canvas board.
    [[nodiscard]] auto rename(const ArtifactId& artifact_id, const std::string& new_name)
        -> CanvasOperationResult;

    /// Duplicate a canvas board.
    [[nodiscard]] auto duplicate(const ArtifactId& artifact_id) -> CanvasOperationResult;

    /// Delete/remove a canvas board.
    [[nodiscard]] auto remove(const ArtifactId& artifact_id, bool delete_from_disk = false)
        -> CanvasOperationResult;

    // ── Close ──

    /// Close a canvas artifact.
    [[nodiscard]] auto close(const ArtifactId& artifact_id) -> CanvasOperationResult;

    // ── Queries ──

    [[nodiscard]] auto open_board_count() const -> int;
    [[nodiscard]] auto unsaved_board_count() const -> int;
    [[nodiscard]] auto has_unsaved_changes(const ArtifactId& artifact_id) const -> bool;
    [[nodiscard]] auto operation_count() const noexcept -> int { return operation_count_; }

private:
    EventBus& event_bus_;
    ArtifactRegistry& registry_;
    ArtifactCreationService& creation_service_;
    Config& config_;
    int operation_count_{0};
};

} // namespace markamp::core
