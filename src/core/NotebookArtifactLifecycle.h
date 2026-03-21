/// @file NotebookArtifactLifecycle.h
/// @brief V20 P03-T01/T03: Notebook artifact lifecycle orchestration.
///
/// Routes notebook creation through the unified artifact pipeline (Phase 01)
/// instead of direct event-only paths. Provides full lifecycle operations:
/// create, open, save, save-as, rename, duplicate, delete, close, and restore.
/// Integrates with the ArtifactRegistry, NotebookDocumentLifecycle, and
/// NotebookShellHost to keep state synchronized.
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

/// Cell type for notebook cells.
enum class NotebookCellKind
{
    kCode,
    kMarkdown,
    kRaw,
};

/// A single cell within a notebook artifact.
struct NotebookCellRecord
{
    std::string cell_id;
    NotebookCellKind kind{NotebookCellKind::kCode};
    std::string source;                              ///< Cell source content
    std::string language_id;                         ///< Language for code cells
    int execution_order{0};                          ///< Last execution order number
    bool is_dirty{false};

    [[nodiscard]] auto is_code() const noexcept -> bool
    {
        return kind == NotebookCellKind::kCode;
    }
    [[nodiscard]] auto is_markdown() const noexcept -> bool
    {
        return kind == NotebookCellKind::kMarkdown;
    }
    [[nodiscard]] auto source_length() const noexcept -> size_t { return source.size(); }
};

/// Notebook execution state for kernel tracking.
enum class NotebookKernelState
{
    kIdle,
    kBusy,
    kStarting,
    kStopped,
    kError,
};

/// Storage location strategy for notebook placement.
enum class NotebookStorageStrategy
{
    kWorkspaceRelative,   ///< Save alongside project files
    kKnowledgeBase,       ///< Save in knowledgebase data dir (legacy)
    kExplicitPath,        ///< User-specified path
};

/// Result of a notebook lifecycle operation.
struct NotebookOperationResult
{
    bool success{false};
    std::string error_message;
    ArtifactId artifact_id;
    std::optional<std::string> resolved_path;

    [[nodiscard]] auto ok() const noexcept -> bool { return success; }
};

/// Context for notebook creation placement.
struct NotebookPlacementContext
{
    std::optional<std::string> workspace_root;
    std::optional<std::string> target_directory;
    NotebookStorageStrategy strategy{NotebookStorageStrategy::kWorkspaceRelative};
    std::string entry_point;                         ///< palette, menu, toolbar, etc.
};

/// Orchestrates the full lifecycle for notebook artifacts.
///
/// All notebook creation funnel through this service. Lifecycle operations
/// (save, rename, duplicate, delete) update the artifact registry, shell host,
/// and publish events for UI synchronization.
class NotebookArtifactLifecycle
{
public:
    NotebookArtifactLifecycle(EventBus& bus, ArtifactRegistry& registry,
                                ArtifactCreationService& creation_service,
                                UnsavedDocumentBufferManager& buffer_manager, Config& config);

    // ── Creation ──

    /// Create a new notebook from any entry point.
    [[nodiscard]] auto create_notebook(const NotebookPlacementContext& context,
                                        const std::string& title = {},
                                        const std::string& kernel_language = "python")
        -> NotebookOperationResult;

    // ── Open ──

    /// Open an existing notebook from disk.
    [[nodiscard]] auto open_notebook(const std::string& file_path) -> NotebookOperationResult;

    /// Restore a notebook from session data.
    [[nodiscard]] auto restore_notebook(const std::string& file_path,
                                         const std::string& artifact_id_hint = {})
        -> NotebookOperationResult;

    // ── Save ──

    /// Save a notebook artifact.
    [[nodiscard]] auto save(const ArtifactId& artifact_id) -> NotebookOperationResult;

    /// Save a notebook to a new path (Save As).
    [[nodiscard]] auto save_as(const ArtifactId& artifact_id, const std::string& new_path)
        -> NotebookOperationResult;

    // ── Rename / Duplicate / Delete ──

    /// Rename a notebook.
    [[nodiscard]] auto rename(const ArtifactId& artifact_id, const std::string& new_title)
        -> NotebookOperationResult;

    /// Duplicate a notebook.
    [[nodiscard]] auto duplicate(const ArtifactId& artifact_id) -> NotebookOperationResult;

    /// Delete a notebook.
    [[nodiscard]] auto remove(const ArtifactId& artifact_id, bool delete_from_disk = false)
        -> NotebookOperationResult;

    // ── Close ──

    /// Close a notebook.
    [[nodiscard]] auto close(const ArtifactId& artifact_id) -> NotebookOperationResult;

    // ── Cell management ──

    /// Add a cell to a notebook.
    [[nodiscard]] auto add_cell(const ArtifactId& artifact_id, NotebookCellKind kind,
                                 const std::string& source = {}) -> bool;

    /// Get cell count for a notebook.
    [[nodiscard]] auto cell_count(const ArtifactId& artifact_id) const -> int;

    /// Get all cells for a notebook.
    [[nodiscard]] auto get_cells(const ArtifactId& artifact_id) const
        -> const std::vector<NotebookCellRecord>&;

    // ── Queries ──

    /// Count of open notebook artifacts.
    [[nodiscard]] auto open_notebook_count() const -> int;

    /// Count of unsaved notebooks.
    [[nodiscard]] auto unsaved_notebook_count() const -> int;

    /// Count of dirty notebooks.
    [[nodiscard]] auto dirty_notebook_count() const -> int;

    /// Whether a notebook has unsaved changes.
    [[nodiscard]] auto has_unsaved_changes(const ArtifactId& artifact_id) const -> bool;

    /// Total lifecycle operations performed.
    [[nodiscard]] auto operation_count() const noexcept -> int { return operation_count_; }

private:
    EventBus& event_bus_;
    ArtifactRegistry& registry_;
    ArtifactCreationService& creation_service_;
    UnsavedDocumentBufferManager& buffer_manager_;
    Config& config_;

    int operation_count_{0};

    /// Per-notebook cell storage.
    std::unordered_map<std::string, std::vector<NotebookCellRecord>> notebook_cells_;
    static const std::vector<NotebookCellRecord> kEmptyCells;

    int next_cell_id_{1};
};

} // namespace markamp::core
