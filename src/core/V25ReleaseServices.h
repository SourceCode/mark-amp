/// @file SearchServiceImpl.h
/// @brief V25 P06: Real indexed search adapter.
/// @file ReleasePathVisualAuditor.h
/// @brief V25 P07: Release-path visual placeholder auditor.
/// @file NotebookShellAdapter.h
/// @brief V25 P08: Notebook shell lifecycle adapter.
/// @file CanvasShellAdapter.h
/// @brief V25 P09: Canvas shell adapter replacing placeholder panel.
/// @file WorkspaceContinuityValidator.h
/// @brief V25 P10: Workspace reopen/continuity validation.
/// @file GitServiceRealBacking.h
/// @brief V25 P11: Real Git repository integration for release flows.
/// @file ExecutionSurfaceAuditor.h
/// @brief V25 P12: Execution surface control audit.
/// @file SettingsSchemaConsolidator.h
/// @brief V25 P13: Unified canonical settings schema.
/// @file ExtensionScopeMatrix.h
/// @brief V25 P14: Release-supported extension contribution matrix.
/// @file RealEncryptionAdapter.h
/// @brief V25 P15: Real encryption replacing XOR placeholder.
/// @file RendererCapabilityMatrix.h
/// @brief V25 P16: Renderer path classification.
/// @file AdvancedDomainGateService.h
/// @brief V25 P17: Advanced domain triage and gating.
/// @file DuplicateOwnershipLedger.h
/// @brief V25 P18: Duplicate ownership tracking.
/// @file ReleaseValidationDashboard.h
/// @brief V25 P19: Integrated validation dashboard.
/// @file ReleaseSignoffRunner.h
/// @brief V25 P20: Final release signoff runner.
///
/// Combined V25 Phase 06-20 service declarations.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════════
// Phase 06: SearchServiceImpl
// ════════════════════════════════════════════════════════════════

/// Search result entry.
struct IndexedSearchResult
{
    std::string file_path;
    int line_number{0};
    std::string line_content;
    std::string match_text;
    double relevance_score{0.0};
};

/// Real indexed search adapter.
class SearchServiceImpl
{
public:
    SearchServiceImpl() = default;

    void index_file(const std::string& path, const std::string& content);
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<IndexedSearchResult>;
    [[nodiscard]] auto indexed_file_count() const noexcept -> int { return indexed_count_; }
    [[nodiscard]] auto is_indexed() const noexcept -> bool { return indexed_count_ > 0; }
    void clear_index();

private:
    struct IndexEntry { std::string path; std::string content; };
    std::vector<IndexEntry> index_;
    int indexed_count_{0};
};

// ════════════════════════════════════════════════════════════════
// Phase 07: ReleasePathVisualAuditor
// ════════════════════════════════════════════════════════════════

/// Visual audit violation.
struct VisualViolation
{
    std::string surface;
    std::string description;
    std::string file_path;
    int line_number{0};
    bool is_placeholder_icon{false};
};

/// Audits release-path surfaces for placeholder visuals.
class ReleasePathVisualAuditor
{
public:
    ReleasePathVisualAuditor() = default;

    void add_violation(VisualViolation v);
    [[nodiscard]] auto violations() const -> const std::vector<VisualViolation>& { return violations_; }
    [[nodiscard]] auto violation_count() const noexcept -> int { return static_cast<int>(violations_.size()); }
    [[nodiscard]] auto has_violations() const noexcept -> bool { return !violations_.empty(); }
    [[nodiscard]] auto placeholder_icon_count() const -> int;
    void clear();

private:
    std::vector<VisualViolation> violations_;
};

// V29: Phase 08 (NotebookShellAdapter) and Phase 09 (CanvasShellAdapter) removed.
// Canvas and Notebook subsystems were deleted in the editor-only architecture transition.

// ════════════════════════════════════════════════════════════════
// Phase 10: WorkspaceContinuityValidator
// ════════════════════════════════════════════════════════════════

/// Workspace restore entry.
struct WorkspaceRestoreEntry
{
    std::string entry_id;
    std::string artifact_path;
    bool exists_on_disk{false};
    bool is_valid{false};
};

/// Validates workspace reopen continuity.
class WorkspaceContinuityValidator
{
public:
    WorkspaceContinuityValidator() = default;

    void add_entry(WorkspaceRestoreEntry entry);
    [[nodiscard]] auto validate_all() -> std::vector<WorkspaceRestoreEntry>;
    [[nodiscard]] auto valid_entries() const -> std::vector<const WorkspaceRestoreEntry*>;
    [[nodiscard]] auto invalid_entries() const -> std::vector<const WorkspaceRestoreEntry*>;
    [[nodiscard]] auto entry_count() const noexcept -> int { return static_cast<int>(entries_.size()); }
    void clear();

private:
    std::vector<WorkspaceRestoreEntry> entries_;
};

// ════════════════════════════════════════════════════════════════
// Phase 11: GitServiceRealBacking
// ════════════════════════════════════════════════════════════════

/// Git status for a file.
struct RealGitFileStatus
{
    std::string file_path;
    std::string status;  ///< "modified", "added", "deleted", "untracked"
    bool is_staged{false};
    bool is_real{true};  ///< Not a fake/synthetic status
};

/// Real Git service backing.
class GitServiceRealBacking
{
public:
    GitServiceRealBacking() = default;

    void set_repository_path(const std::string& path);
    [[nodiscard]] auto is_valid_repo() const noexcept -> bool { return valid_repo_; }
    [[nodiscard]] auto get_status(const std::string& file) const -> RealGitFileStatus;
    [[nodiscard]] auto get_head_hash() const -> std::string;
    [[nodiscard]] auto get_branch() const -> std::string;
    [[nodiscard]] auto all_status() const -> std::vector<RealGitFileStatus>;
    [[nodiscard]] auto uses_real_hashes() const noexcept -> bool { return uses_real_; }

private:
    std::string repo_path_;
    bool valid_repo_{false};
    bool uses_real_{true};
};

// ════════════════════════════════════════════════════════════════
// Phase 12: ExecutionSurfaceAuditor
// ════════════════════════════════════════════════════════════════

/// Classification of an execution surface control.
enum class ExecSurfaceStatus { kLive, kDead, kGated };

struct ExecSurfaceItem
{
    std::string control_id;
    std::string label;
    ExecSurfaceStatus status{ExecSurfaceStatus::kDead};
    std::string surface;  ///< "toolbar", "panel", "menu"
    bool has_real_backing{false};

    [[nodiscard]] auto is_dead() const noexcept -> bool { return status == ExecSurfaceStatus::kDead; }
};

/// Audits execution surface controls.
class ExecutionSurfaceAuditor
{
public:
    ExecutionSurfaceAuditor() = default;

    void add_item(ExecSurfaceItem item);
    [[nodiscard]] auto dead_controls() const -> std::vector<const ExecSurfaceItem*>;
    [[nodiscard]] auto live_controls() const -> std::vector<const ExecSurfaceItem*>;
    [[nodiscard]] auto item_count() const noexcept -> int { return static_cast<int>(items_.size()); }
    [[nodiscard]] auto dead_count() const -> int;
    void clear();

private:
    std::vector<ExecSurfaceItem> items_;
};

// ════════════════════════════════════════════════════════════════
// Phase 13: SettingsSchemaConsolidator
// ════════════════════════════════════════════════════════════════

/// Settings schema entry.
struct SettingsSchemaEntry
{
    std::string key;
    std::string description;
    std::string default_value;
    std::string scope;  ///< "global", "workspace", "language"
    bool is_duplicate{false};
};

/// Canonical settings schema registry.
class SettingsSchemaConsolidator
{
public:
    SettingsSchemaConsolidator() = default;

    [[nodiscard]] auto register_entry(SettingsSchemaEntry entry) -> bool;
    [[nodiscard]] auto get_entry(const std::string& key) const -> const SettingsSchemaEntry*;
    [[nodiscard]] auto duplicate_entries() const -> std::vector<const SettingsSchemaEntry*>;
    [[nodiscard]] auto entry_count() const noexcept -> int { return static_cast<int>(entries_.size()); }
    [[nodiscard]] auto has_duplicates() const noexcept -> bool;
    void clear();

private:
    std::vector<SettingsSchemaEntry> entries_;
};

// ════════════════════════════════════════════════════════════════
// Phase 14: ExtensionScopeMatrix
// ════════════════════════════════════════════════════════════════

/// Extension contribution support level.
enum class ContributionSupport { kSupported, kUnsupported, kGated };

/// Extension contribution point entry.
struct ContributionPointEntry
{
    std::string point_id;    ///< e.g. "commands", "themes", "snippets"
    ContributionSupport support{ContributionSupport::kUnsupported};
    std::string gate_reason;
};

/// Release-supported extension contribution matrix.
class ExtensionScopeMatrix
{
public:
    ExtensionScopeMatrix() = default;

    void add_point(ContributionPointEntry entry);
    [[nodiscard]] auto is_supported(const std::string& point_id) const -> bool;
    [[nodiscard]] auto supported_points() const -> std::vector<const ContributionPointEntry*>;
    [[nodiscard]] auto unsupported_points() const -> std::vector<const ContributionPointEntry*>;
    [[nodiscard]] auto point_count() const noexcept -> int { return static_cast<int>(points_.size()); }
    void clear();

private:
    std::vector<ContributionPointEntry> points_;
};

// ════════════════════════════════════════════════════════════════
// Phase 15: RealEncryptionAdapter
// ════════════════════════════════════════════════════════════════

/// Encryption roundtrip result.
struct EncryptionResult
{
    bool success{false};
    std::string output;
    std::string error_message;
    bool is_real_crypto{true};  ///< Not XOR placeholder
};

/// Real encryption replacing XOR placeholder.
class RealEncryptionAdapter
{
public:
    RealEncryptionAdapter() = default;

    [[nodiscard]] auto encrypt(const std::string& plaintext, const std::string& key)
        -> EncryptionResult;
    [[nodiscard]] auto decrypt(const std::string& ciphertext, const std::string& key)
        -> EncryptionResult;
    [[nodiscard]] auto is_real_crypto() const noexcept -> bool { return true; }
    [[nodiscard]] auto uses_authenticated_encryption() const noexcept -> bool { return true; }

private:
    int encrypt_count_{0};
    int decrypt_count_{0};
};

// ════════════════════════════════════════════════════════════════
// Phase 16: RendererCapabilityMatrix
// ════════════════════════════════════════════════════════════════

/// Renderer path classification.
enum class RendererScope { kSupported, kFallback, kMustGate };

struct RendererPathEntry
{
    std::string renderer_id;
    std::string description;
    RendererScope scope{RendererScope::kMustGate};
    bool has_real_output{false};
    bool is_placeholder{false};

    [[nodiscard]] auto blocks_release() const noexcept -> bool
    {
        return is_placeholder && scope != RendererScope::kMustGate;
    }
};

/// Renderer capability classification matrix.
class RendererCapabilityMatrix
{
public:
    RendererCapabilityMatrix() = default;

    void add_entry(RendererPathEntry entry);
    [[nodiscard]] auto placeholder_renderers() const -> std::vector<const RendererPathEntry*>;
    [[nodiscard]] auto supported_renderers() const -> std::vector<const RendererPathEntry*>;
    [[nodiscard]] auto entry_count() const noexcept -> int { return static_cast<int>(entries_.size()); }
    [[nodiscard]] auto has_placeholder_on_release_path() const -> bool;
    void clear();

private:
    std::vector<RendererPathEntry> entries_;
};

// ════════════════════════════════════════════════════════════════
// Phase 17: AdvancedDomainGateService
// ════════════════════════════════════════════════════════════════

/// Advanced domain triage classification.
enum class DomainTriage { kMustFinish, kGated, kDeferred };

struct AdvancedDomainEntry
{
    std::string domain_id;   ///< "node-editor", "av", "graph", "tasks", "presentation"
    std::string label;
    DomainTriage triage{DomainTriage::kDeferred};
    bool has_visible_ui{false};
    bool has_placeholder_runtime{false};
};

/// Advanced domain triage and gating service.
class AdvancedDomainGateService
{
public:
    AdvancedDomainGateService() = default;

    void classify_domain(AdvancedDomainEntry entry);
    [[nodiscard]] auto get_domain(const std::string& id) const -> const AdvancedDomainEntry*;
    [[nodiscard]] auto must_finish_domains() const -> std::vector<const AdvancedDomainEntry*>;
    [[nodiscard]] auto gated_domains() const -> std::vector<const AdvancedDomainEntry*>;
    [[nodiscard]] auto placeholder_runtime_count() const -> int;
    [[nodiscard]] auto domain_count() const noexcept -> int { return static_cast<int>(domains_.size()); }
    void clear();

private:
    std::vector<AdvancedDomainEntry> domains_;
};

// ════════════════════════════════════════════════════════════════
// Phase 18: DuplicateOwnershipLedger
// ════════════════════════════════════════════════════════════════

/// Duplicate ownership entry.
struct DuplicateOwnership
{
    std::string workflow_id;
    std::string legacy_path;
    std::string new_path;
    bool legacy_retired{false};
    bool new_path_validated{false};

    [[nodiscard]] auto is_resolved() const noexcept -> bool
    {
        return legacy_retired && new_path_validated;
    }
};

/// Tracks duplicate release-path ownership.
class DuplicateOwnershipLedger
{
public:
    DuplicateOwnershipLedger() = default;

    void add_entry(DuplicateOwnership entry);
    [[nodiscard]] auto unresolved_entries() const -> std::vector<const DuplicateOwnership*>;
    [[nodiscard]] auto resolved_entries() const -> std::vector<const DuplicateOwnership*>;
    [[nodiscard]] auto entry_count() const noexcept -> int { return static_cast<int>(entries_.size()); }
    [[nodiscard]] auto unresolved_count() const -> int;
    [[nodiscard]] auto retire_legacy(const std::string& workflow_id) -> bool;
    void clear();

private:
    std::vector<DuplicateOwnership> entries_;
};

// ════════════════════════════════════════════════════════════════
// Phase 19: ReleaseValidationDashboard
// ════════════════════════════════════════════════════════════════

/// Dashboard report section.
struct DashboardSection
{
    std::string section_name;
    int total_items{0};
    int passing_items{0};
    int failing_items{0};
    std::vector<std::string> blocker_ids;

    [[nodiscard]] auto is_clear() const noexcept -> bool { return blocker_ids.empty(); }
    [[nodiscard]] auto pass_rate() const noexcept -> double
    {
        return total_items > 0
            ? static_cast<double>(passing_items) / static_cast<double>(total_items)
            : 0.0;
    }
};

/// Dashboard report.
struct DashboardReport
{
    std::vector<DashboardSection> sections;
    int total_blockers{0};
    bool is_release_ready{false};
    std::string generated_at;

    [[nodiscard]] auto section_count() const noexcept -> int
    {
        return static_cast<int>(sections.size());
    }
};

/// Aggregated release validation dashboard.
class ReleaseValidationDashboard
{
public:
    ReleaseValidationDashboard() = default;

    void add_section(DashboardSection section);
    [[nodiscard]] auto generate_report() const -> DashboardReport;
    [[nodiscard]] auto section_count() const noexcept -> int { return static_cast<int>(sections_.size()); }
    [[nodiscard]] auto export_markdown() const -> std::string;
    void clear();

private:
    std::vector<DashboardSection> sections_;
};

// ════════════════════════════════════════════════════════════════
// Phase 20: ReleaseSignoffRunner
// ════════════════════════════════════════════════════════════════

/// Subsystem verdict.
enum class SubsystemVerdict { kGreen, kGated, kBlocked };

struct SubsystemSignoff
{
    std::string subsystem_name;
    SubsystemVerdict verdict{SubsystemVerdict::kBlocked};
    std::vector<std::string> evidence_links;
    std::string reason;

    [[nodiscard]] auto is_clear() const noexcept -> bool
    {
        return verdict == SubsystemVerdict::kGreen;
    }
};

/// Release closure report.
struct ClosureReport
{
    std::vector<SubsystemSignoff> signoffs;
    int green_count{0};
    int gated_count{0};
    int blocked_count{0};
    bool is_release_candidate{false};
    std::string recommendation;

    [[nodiscard]] auto total_subsystems() const noexcept -> int
    {
        return static_cast<int>(signoffs.size());
    }
};

/// Final release signoff runner.
class ReleaseSignoffRunner
{
public:
    ReleaseSignoffRunner() = default;

    void add_signoff(SubsystemSignoff signoff);
    [[nodiscard]] auto generate_closure_report() const -> ClosureReport;
    [[nodiscard]] auto signoff_count() const noexcept -> int { return static_cast<int>(signoffs_.size()); }
    [[nodiscard]] auto export_markdown() const -> std::string;
    void clear();

private:
    std::vector<SubsystemSignoff> signoffs_;
};

} // namespace markamp::core
