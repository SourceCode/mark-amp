/// @file PersistenceCompletionAuditor.h
/// @brief V23 Phase 04 — Persistence, save, restore, recovery, and repository completion auditor.
///
/// Audits that save, autosave, recovery, rollback, import/export, and snapshot
/// subsystems perform real durable work rather than stopping at event publication
/// or stubbed storage helpers.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// PersistenceCapability — categories of persistence behavior
// ============================================================================

enum class PersistenceCapability : uint8_t
{
    kSave,
    kSaveAll,
    kAutosave,
    kSaveAs,
    kRecoveryJournal,
    kReplay,
    kRollback,
    kSnapshotDiff,
    kImportExport,
    kBuildLogPersistence,
};

/// Label for PersistenceCapability.
[[nodiscard]] constexpr auto persistence_capability_label(PersistenceCapability cap) -> const char*
{
    switch (cap)
    {
    case PersistenceCapability::kSave:              return "Save";
    case PersistenceCapability::kSaveAll:            return "SaveAll";
    case PersistenceCapability::kAutosave:           return "Autosave";
    case PersistenceCapability::kSaveAs:             return "SaveAs";
    case PersistenceCapability::kRecoveryJournal:    return "RecoveryJournal";
    case PersistenceCapability::kReplay:             return "Replay";
    case PersistenceCapability::kRollback:           return "Rollback";
    case PersistenceCapability::kSnapshotDiff:       return "SnapshotDiff";
    case PersistenceCapability::kImportExport:       return "ImportExport";
    case PersistenceCapability::kBuildLogPersistence: return "BuildLogPersistence";
    }
    return "Unknown";
}

// ============================================================================
// PersistenceArtifactKind — artifact type the capability targets
// ============================================================================

enum class PersistenceArtifactKind : uint8_t
{
    kTextFile,
    kWorkspace,
    kAll,
};

/// Label for PersistenceArtifactKind.
[[nodiscard]] constexpr auto persistence_artifact_label(PersistenceArtifactKind kind) -> const char*
{
    switch (kind)
    {
    case PersistenceArtifactKind::kTextFile:  return "TextFile";
    case PersistenceArtifactKind::kWorkspace: return "Workspace";
    case PersistenceArtifactKind::kAll:       return "All";
    }
    return "Unknown";
}

// ============================================================================
// PersistenceCoverageItem — evidence that a capability is implemented
// ============================================================================

struct PersistenceCoverageItem
{
    PersistenceCapability capability{PersistenceCapability::kSave};
    PersistenceArtifactKind artifact_kind{PersistenceArtifactKind::kAll};
    bool is_authoritative{false};     ///< Not just optimistic/event-based
    bool is_transactional{false};     ///< Uses atomic write semantics
    bool has_failure_handling{false};  ///< Handles write errors
    bool has_journal_support{false};  ///< Backed by recovery journal
    std::string evidence_file;
    int evidence_line{0};
    std::string notes;

    /// Whether this capability is fully covered.
    [[nodiscard]] auto is_complete() const noexcept -> bool
    {
        return is_authoritative && has_failure_handling;
    }
};

// ============================================================================
// PersistenceGapReport — summary of persistence coverage
// ============================================================================

struct PersistenceGapReport
{
    std::size_t total_items{0};
    std::size_t complete{0};
    std::size_t incomplete{0};
    std::size_t authoritative{0};
    std::size_t transactional{0};
    std::size_t with_journal{0};

    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }

    [[nodiscard]] auto coverage_pct() const noexcept -> double
    {
        return total_items > 0
            ? (static_cast<double>(complete) / static_cast<double>(total_items)) * 100.0
            : 100.0;
    }
};

// ============================================================================
// PersistenceCompletionAuditor — the auditing engine
// ============================================================================

class PersistenceCompletionAuditor
{
public:
    PersistenceCompletionAuditor() = default;

    // ── Registration ──

    void add_item(PersistenceCoverageItem item);
    void add_items(std::vector<PersistenceCoverageItem> items);

    // ── Queries ──

    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_capability(PersistenceCapability cap) const
        -> std::vector<const PersistenceCoverageItem*>;
    [[nodiscard]] auto items_by_artifact(PersistenceArtifactKind kind) const
        -> std::vector<const PersistenceCoverageItem*>;
    [[nodiscard]] auto complete_items() const
        -> std::vector<const PersistenceCoverageItem*>;
    [[nodiscard]] auto incomplete_items() const
        -> std::vector<const PersistenceCoverageItem*>;

    // ── Report ──

    [[nodiscard]] auto gap_report() const -> PersistenceGapReport;

    // ── Clear ──

    void clear();

    // ── Export ──

    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;

private:
    std::vector<PersistenceCoverageItem> items_;
};

} // namespace markamp::core
