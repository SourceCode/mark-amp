/// @file LifecycleCompletionTracker.h
/// @brief V23 Phase 03 — Artifact, workspace, and session lifecycle completion tracker.
///
/// Tracks lifecycle coverage for text files, notebooks, canvases, workspace
/// membership, and session identity. Verifies each artifact type has authoritative
/// ownership through creation → open → rename → save → restore → close.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// ArtifactKind — categories of shell-owned artifacts
// ============================================================================

enum class ArtifactKind : uint8_t
{
    kTextFile,
    kNotebook,
    kCanvas,
    kWorkspace,
    kSession,
};

/// Label for ArtifactKind.
[[nodiscard]] constexpr auto artifact_kind_label(ArtifactKind kind) -> const char*
{
    switch (kind)
    {
    case ArtifactKind::kTextFile:  return "TextFile";
    case ArtifactKind::kNotebook: return "Notebook";
    case ArtifactKind::kCanvas:   return "Canvas";
    case ArtifactKind::kWorkspace: return "Workspace";
    case ArtifactKind::kSession:  return "Session";
    }
    return "Unknown";
}

// ============================================================================
// LifecycleStage — stages in an artifact's lifecycle
// ============================================================================

enum class LifecycleStage : uint8_t
{
    kCreation,
    kOpen,
    kRename,
    kSave,
    kRestore,
    kClose,
    kDuplicate,
};

/// Label for LifecycleStage.
[[nodiscard]] constexpr auto lifecycle_stage_label(LifecycleStage stage) -> const char*
{
    switch (stage)
    {
    case LifecycleStage::kCreation:  return "Creation";
    case LifecycleStage::kOpen:      return "Open";
    case LifecycleStage::kRename:    return "Rename";
    case LifecycleStage::kSave:      return "Save";
    case LifecycleStage::kRestore:   return "Restore";
    case LifecycleStage::kClose:     return "Close";
    case LifecycleStage::kDuplicate: return "Duplicate";
    }
    return "Unknown";
}

// ============================================================================
// LifecycleCoverageItem — evidence that a stage is covered
// ============================================================================

struct LifecycleCoverageItem
{
    ArtifactKind artifact_kind{ArtifactKind::kTextFile};
    LifecycleStage stage{LifecycleStage::kCreation};
    bool is_authoritative{false};  ///< No fake paths or dummy IDs
    bool is_shell_owned{false};    ///< Shell owns the lifecycle
    bool has_stable_id{false};     ///< Uses stable artifact IDs
    std::string evidence_file;
    int evidence_line{0};
    std::string notes;

    /// Whether this stage is fully covered.
    [[nodiscard]] auto is_complete() const noexcept -> bool
    {
        return is_authoritative && is_shell_owned && has_stable_id;
    }
};

// ============================================================================
// LifecycleGapReport — summary of lifecycle coverage gaps
// ============================================================================

struct LifecycleGapReport
{
    std::size_t total_items{0};
    std::size_t complete{0};
    std::size_t incomplete{0};
    std::size_t text_file_gaps{0};
    std::size_t notebook_gaps{0};
    std::size_t canvas_gaps{0};
    std::size_t workspace_gaps{0};
    std::size_t session_gaps{0};

    [[nodiscard]] auto has_gaps() const noexcept -> bool
    {
        return incomplete > 0;
    }

    [[nodiscard]] auto coverage_pct() const noexcept -> double
    {
        return total_items > 0
            ? (static_cast<double>(complete) / static_cast<double>(total_items)) * 100.0
            : 100.0;
    }
};

// ============================================================================
// LifecycleCompletionTracker — the tracking engine
// ============================================================================

class LifecycleCompletionTracker
{
public:
    LifecycleCompletionTracker() = default;

    // ── Registration ──

    void add_item(LifecycleCoverageItem item);
    void add_items(std::vector<LifecycleCoverageItem> items);

    // ── Queries ──

    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_kind(ArtifactKind kind) const
        -> std::vector<const LifecycleCoverageItem*>;
    [[nodiscard]] auto items_by_stage(LifecycleStage stage) const
        -> std::vector<const LifecycleCoverageItem*>;
    [[nodiscard]] auto complete_items() const
        -> std::vector<const LifecycleCoverageItem*>;
    [[nodiscard]] auto incomplete_items() const
        -> std::vector<const LifecycleCoverageItem*>;
    [[nodiscard]] auto gaps_for_kind(ArtifactKind kind) const
        -> std::vector<const LifecycleCoverageItem*>;

    // ── Report ──

    [[nodiscard]] auto gap_report() const -> LifecycleGapReport;

    // ── Clear ──

    void clear();

    // ── Export ──

    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;

private:
    std::vector<LifecycleCoverageItem> items_;
};

} // namespace markamp::core
