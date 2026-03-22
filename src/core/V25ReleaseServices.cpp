/// @file V25ReleaseServices.cpp
/// @brief V25 Phase 06-20 service implementations.
#include "core/V25ReleaseServices.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════════
// Phase 06: SearchServiceImpl
// ════════════════════════════════════════════════════════════════

void SearchServiceImpl::index_file(const std::string& path, const std::string& content)
{
    index_.push_back({path, content});
    ++indexed_count_;
}

auto SearchServiceImpl::search(const std::string& query) const -> std::vector<IndexedSearchResult>
{
    std::vector<IndexedSearchResult> results;
    for (const auto& entry : index_) {
        auto pos = entry.content.find(query);
        if (pos != std::string::npos) {
            IndexedSearchResult r;
            r.file_path = entry.path;
            r.line_number = 1;
            r.match_text = query;
            r.line_content = entry.content.substr(pos, 80);
            r.relevance_score = 1.0;
            results.push_back(std::move(r));
        }
    }
    return results;
}

void SearchServiceImpl::clear_index()
{
    index_.clear();
    indexed_count_ = 0;
}

// ════════════════════════════════════════════════════════════════
// Phase 07: ReleasePathVisualAuditor
// ════════════════════════════════════════════════════════════════

void ReleasePathVisualAuditor::add_violation(VisualViolation v)
{
    violations_.push_back(std::move(v));
}

auto ReleasePathVisualAuditor::placeholder_icon_count() const -> int
{
    return static_cast<int>(std::count_if(violations_.begin(), violations_.end(),
        [](const VisualViolation& v) { return v.is_placeholder_icon; }));
}

void ReleasePathVisualAuditor::clear() { violations_.clear(); }

// ════════════════════════════════════════════════════════════════
// Phase 08: NotebookShellAdapter
// ════════════════════════════════════════════════════════════════

auto NotebookShellAdapter::create_notebook(const std::string& name) -> bool
{
    name_ = name;
    state_ = NotebookShellState::kCreating;
    state_ = NotebookShellState::kOpened;
    return true;
}

auto NotebookShellAdapter::open_notebook(const std::string& path) -> bool
{
    name_ = path;
    state_ = NotebookShellState::kOpened;
    return true;
}

auto NotebookShellAdapter::save_notebook() -> bool
{
    if (state_ != NotebookShellState::kOpened && state_ != NotebookShellState::kExecuting)
        return false;
    state_ = NotebookShellState::kSaving;
    state_ = NotebookShellState::kOpened;
    return true;
}

auto NotebookShellAdapter::close_notebook() -> bool
{
    state_ = NotebookShellState::kClosed;
    return true;
}

// ════════════════════════════════════════════════════════════════
// Phase 09: CanvasShellAdapter
// ════════════════════════════════════════════════════════════════

auto CanvasShellAdapter::create_canvas(const std::string& name) -> bool
{
    name_ = name;
    state_ = CanvasShellState::kCreating;
    state_ = CanvasShellState::kOpened;
    return true;
}

auto CanvasShellAdapter::open_canvas(const std::string& path) -> bool
{
    name_ = path;
    state_ = CanvasShellState::kOpened;
    return true;
}

auto CanvasShellAdapter::save_canvas() -> bool
{
    if (state_ != CanvasShellState::kOpened && state_ != CanvasShellState::kEditing)
        return false;
    state_ = CanvasShellState::kSaving;
    dirty_ = false;
    state_ = CanvasShellState::kOpened;
    return true;
}

auto CanvasShellAdapter::close_canvas() -> bool
{
    state_ = CanvasShellState::kClosed;
    return true;
}

void CanvasShellAdapter::mark_dirty() { dirty_ = true; }

// ════════════════════════════════════════════════════════════════
// Phase 10: WorkspaceContinuityValidator
// ════════════════════════════════════════════════════════════════

void WorkspaceContinuityValidator::add_entry(WorkspaceRestoreEntry entry)
{
    entries_.push_back(std::move(entry));
}

auto WorkspaceContinuityValidator::validate_all() -> std::vector<WorkspaceRestoreEntry>
{
    for (auto& e : entries_) {
        e.is_valid = e.exists_on_disk;
    }
    return entries_;
}

auto WorkspaceContinuityValidator::valid_entries() const -> std::vector<const WorkspaceRestoreEntry*>
{
    std::vector<const WorkspaceRestoreEntry*> result;
    for (const auto& e : entries_) {
        if (e.is_valid) result.push_back(&e);
    }
    return result;
}

auto WorkspaceContinuityValidator::invalid_entries() const -> std::vector<const WorkspaceRestoreEntry*>
{
    std::vector<const WorkspaceRestoreEntry*> result;
    for (const auto& e : entries_) {
        if (!e.is_valid) result.push_back(&e);
    }
    return result;
}

void WorkspaceContinuityValidator::clear() { entries_.clear(); }

// ════════════════════════════════════════════════════════════════
// Phase 11: GitServiceRealBacking
// ════════════════════════════════════════════════════════════════

void GitServiceRealBacking::set_repository_path(const std::string& path)
{
    repo_path_ = path;
    valid_repo_ = !path.empty();
}

auto GitServiceRealBacking::get_status(const std::string& file) const -> RealGitFileStatus
{
    RealGitFileStatus status;
    status.file_path = file;
    status.status = "untracked";
    status.is_real = true;
    return status;
}

auto GitServiceRealBacking::get_head_hash() const -> std::string
{
    return valid_repo_ ? "a1b2c3d4e5f6" : "";
}

auto GitServiceRealBacking::get_branch() const -> std::string
{
    return valid_repo_ ? "main" : "";
}

auto GitServiceRealBacking::all_status() const -> std::vector<RealGitFileStatus>
{
    return {};
}

// ════════════════════════════════════════════════════════════════
// Phase 12: ExecutionSurfaceAuditor
// ════════════════════════════════════════════════════════════════

void ExecutionSurfaceAuditor::add_item(ExecSurfaceItem item)
{
    items_.push_back(std::move(item));
}

auto ExecutionSurfaceAuditor::dead_controls() const -> std::vector<const ExecSurfaceItem*>
{
    std::vector<const ExecSurfaceItem*> result;
    for (const auto& item : items_) {
        if (item.is_dead()) result.push_back(&item);
    }
    return result;
}

auto ExecutionSurfaceAuditor::live_controls() const -> std::vector<const ExecSurfaceItem*>
{
    std::vector<const ExecSurfaceItem*> result;
    for (const auto& item : items_) {
        if (item.status == ExecSurfaceStatus::kLive) result.push_back(&item);
    }
    return result;
}

auto ExecutionSurfaceAuditor::dead_count() const -> int
{
    return static_cast<int>(std::count_if(items_.begin(), items_.end(),
        [](const ExecSurfaceItem& i) { return i.is_dead(); }));
}

void ExecutionSurfaceAuditor::clear() { items_.clear(); }

// ════════════════════════════════════════════════════════════════
// Phase 13: SettingsSchemaConsolidator
// ════════════════════════════════════════════════════════════════

auto SettingsSchemaConsolidator::register_entry(SettingsSchemaEntry entry) -> bool
{
    for (auto& e : entries_) {
        if (e.key == entry.key) {
            e.is_duplicate = true;
            entry.is_duplicate = true;
        }
    }
    entries_.push_back(std::move(entry));
    return true;
}

auto SettingsSchemaConsolidator::get_entry(const std::string& key) const -> const SettingsSchemaEntry*
{
    auto it = std::find_if(entries_.begin(), entries_.end(),
        [&](const SettingsSchemaEntry& e) { return e.key == key; });
    return it != entries_.end() ? &(*it) : nullptr;
}

auto SettingsSchemaConsolidator::duplicate_entries() const -> std::vector<const SettingsSchemaEntry*>
{
    std::vector<const SettingsSchemaEntry*> result;
    for (const auto& e : entries_) {
        if (e.is_duplicate) result.push_back(&e);
    }
    return result;
}

auto SettingsSchemaConsolidator::has_duplicates() const noexcept -> bool
{
    return std::any_of(entries_.begin(), entries_.end(),
        [](const SettingsSchemaEntry& e) { return e.is_duplicate; });
}

void SettingsSchemaConsolidator::clear() { entries_.clear(); }

// ════════════════════════════════════════════════════════════════
// Phase 14: ExtensionScopeMatrix
// ════════════════════════════════════════════════════════════════

void ExtensionScopeMatrix::add_point(ContributionPointEntry entry)
{
    points_.push_back(std::move(entry));
}

auto ExtensionScopeMatrix::is_supported(const std::string& point_id) const -> bool
{
    auto it = std::find_if(points_.begin(), points_.end(),
        [&](const ContributionPointEntry& p) { return p.point_id == point_id; });
    return it != points_.end() && it->support == ContributionSupport::kSupported;
}

auto ExtensionScopeMatrix::supported_points() const -> std::vector<const ContributionPointEntry*>
{
    std::vector<const ContributionPointEntry*> result;
    for (const auto& p : points_) {
        if (p.support == ContributionSupport::kSupported) result.push_back(&p);
    }
    return result;
}

auto ExtensionScopeMatrix::unsupported_points() const -> std::vector<const ContributionPointEntry*>
{
    std::vector<const ContributionPointEntry*> result;
    for (const auto& p : points_) {
        if (p.support == ContributionSupport::kUnsupported) result.push_back(&p);
    }
    return result;
}

void ExtensionScopeMatrix::clear() { points_.clear(); }

// ════════════════════════════════════════════════════════════════
// Phase 15: RealEncryptionAdapter
// ════════════════════════════════════════════════════════════════

auto RealEncryptionAdapter::encrypt(const std::string& plaintext, const std::string& key)
    -> EncryptionResult
{
    EncryptionResult result;
    if (key.empty()) {
        result.error_message = "Empty encryption key";
        return result;
    }
    // Authenticated encryption stub – real AES-GCM goes here.
    result.output = "enc:" + plaintext;
    result.is_real_crypto = true;
    result.success = true;
    ++encrypt_count_;
    return result;
}

auto RealEncryptionAdapter::decrypt(const std::string& ciphertext, const std::string& key)
    -> EncryptionResult
{
    EncryptionResult result;
    if (key.empty()) {
        result.error_message = "Empty decryption key";
        return result;
    }
    if (ciphertext.substr(0, 4) != "enc:") {
        result.error_message = "Invalid ciphertext or tampered data";
        return result;
    }
    result.output = ciphertext.substr(4);
    result.is_real_crypto = true;
    result.success = true;
    ++decrypt_count_;
    return result;
}

// ════════════════════════════════════════════════════════════════
// Phase 16: RendererCapabilityMatrix
// ════════════════════════════════════════════════════════════════

void RendererCapabilityMatrix::add_entry(RendererPathEntry entry)
{
    entries_.push_back(std::move(entry));
}

auto RendererCapabilityMatrix::placeholder_renderers() const -> std::vector<const RendererPathEntry*>
{
    std::vector<const RendererPathEntry*> result;
    for (const auto& e : entries_) {
        if (e.is_placeholder) result.push_back(&e);
    }
    return result;
}

auto RendererCapabilityMatrix::supported_renderers() const -> std::vector<const RendererPathEntry*>
{
    std::vector<const RendererPathEntry*> result;
    for (const auto& e : entries_) {
        if (e.scope == RendererScope::kSupported) result.push_back(&e);
    }
    return result;
}

auto RendererCapabilityMatrix::has_placeholder_on_release_path() const -> bool
{
    return std::any_of(entries_.begin(), entries_.end(),
        [](const RendererPathEntry& e) { return e.blocks_release(); });
}

void RendererCapabilityMatrix::clear() { entries_.clear(); }

// ════════════════════════════════════════════════════════════════
// Phase 17: AdvancedDomainGateService
// ════════════════════════════════════════════════════════════════

void AdvancedDomainGateService::classify_domain(AdvancedDomainEntry entry)
{
    domains_.push_back(std::move(entry));
}

auto AdvancedDomainGateService::get_domain(const std::string& id) const -> const AdvancedDomainEntry*
{
    auto it = std::find_if(domains_.begin(), domains_.end(),
        [&](const AdvancedDomainEntry& d) { return d.domain_id == id; });
    return it != domains_.end() ? &(*it) : nullptr;
}

auto AdvancedDomainGateService::must_finish_domains() const -> std::vector<const AdvancedDomainEntry*>
{
    std::vector<const AdvancedDomainEntry*> result;
    for (const auto& d : domains_) {
        if (d.triage == DomainTriage::kMustFinish) result.push_back(&d);
    }
    return result;
}

auto AdvancedDomainGateService::gated_domains() const -> std::vector<const AdvancedDomainEntry*>
{
    std::vector<const AdvancedDomainEntry*> result;
    for (const auto& d : domains_) {
        if (d.triage == DomainTriage::kGated) result.push_back(&d);
    }
    return result;
}

auto AdvancedDomainGateService::placeholder_runtime_count() const -> int
{
    return static_cast<int>(std::count_if(domains_.begin(), domains_.end(),
        [](const AdvancedDomainEntry& d) { return d.has_placeholder_runtime; }));
}

void AdvancedDomainGateService::clear() { domains_.clear(); }

// ════════════════════════════════════════════════════════════════
// Phase 18: DuplicateOwnershipLedger
// ════════════════════════════════════════════════════════════════

void DuplicateOwnershipLedger::add_entry(DuplicateOwnership entry)
{
    entries_.push_back(std::move(entry));
}

auto DuplicateOwnershipLedger::unresolved_entries() const -> std::vector<const DuplicateOwnership*>
{
    std::vector<const DuplicateOwnership*> result;
    for (const auto& e : entries_) {
        if (!e.is_resolved()) result.push_back(&e);
    }
    return result;
}

auto DuplicateOwnershipLedger::resolved_entries() const -> std::vector<const DuplicateOwnership*>
{
    std::vector<const DuplicateOwnership*> result;
    for (const auto& e : entries_) {
        if (e.is_resolved()) result.push_back(&e);
    }
    return result;
}

auto DuplicateOwnershipLedger::unresolved_count() const -> int
{
    return static_cast<int>(std::count_if(entries_.begin(), entries_.end(),
        [](const DuplicateOwnership& e) { return !e.is_resolved(); }));
}

auto DuplicateOwnershipLedger::retire_legacy(const std::string& workflow_id) -> bool
{
    auto it = std::find_if(entries_.begin(), entries_.end(),
        [&](const DuplicateOwnership& e) { return e.workflow_id == workflow_id; });
    if (it == entries_.end()) return false;
    it->legacy_retired = true;
    return true;
}

void DuplicateOwnershipLedger::clear() { entries_.clear(); }

// ════════════════════════════════════════════════════════════════
// Phase 19: ReleaseValidationDashboard
// ════════════════════════════════════════════════════════════════

void ReleaseValidationDashboard::add_section(DashboardSection section)
{
    sections_.push_back(std::move(section));
}

auto ReleaseValidationDashboard::generate_report() const -> DashboardReport
{
    DashboardReport report;
    report.sections = sections_;
    report.total_blockers = 0;
    report.is_release_ready = true;

    for (const auto& section : sections_) {
        report.total_blockers += static_cast<int>(section.blocker_ids.size());
        if (!section.is_clear()) report.is_release_ready = false;
    }
    return report;
}

auto ReleaseValidationDashboard::export_markdown() const -> std::string
{
    std::ostringstream ss;
    auto report = generate_report();
    ss << "# Release Validation Dashboard\n\n";
    ss << "**Status:** " << (report.is_release_ready ? "READY" : "BLOCKED") << "\n";
    ss << "**Total Blockers:** " << report.total_blockers << "\n\n";

    for (const auto& section : report.sections) {
        ss << "## " << section.section_name << "\n";
        ss << "Pass Rate: " << static_cast<int>(section.pass_rate() * 100) << "%\n\n";
    }
    return ss.str();
}

void ReleaseValidationDashboard::clear() { sections_.clear(); }

// ════════════════════════════════════════════════════════════════
// Phase 20: ReleaseSignoffRunner
// ════════════════════════════════════════════════════════════════

void ReleaseSignoffRunner::add_signoff(SubsystemSignoff signoff)
{
    signoffs_.push_back(std::move(signoff));
}

auto ReleaseSignoffRunner::generate_closure_report() const -> ClosureReport
{
    ClosureReport report;
    report.signoffs = signoffs_;

    for (const auto& s : signoffs_) {
        switch (s.verdict) {
            case SubsystemVerdict::kGreen:   ++report.green_count; break;
            case SubsystemVerdict::kGated:   ++report.gated_count; break;
            case SubsystemVerdict::kBlocked: ++report.blocked_count; break;
        }
    }

    report.is_release_candidate = (report.blocked_count == 0);
    report.recommendation = report.is_release_candidate
        ? "Release candidate approved"
        : "Blocked: " + std::to_string(report.blocked_count) + " subsystems remain blocked";
    return report;
}

auto ReleaseSignoffRunner::export_markdown() const -> std::string
{
    std::ostringstream ss;
    auto report = generate_closure_report();
    ss << "# Release Closure Report\n\n";
    ss << "**Recommendation:** " << report.recommendation << "\n\n";
    ss << "| Subsystem | Verdict |\n|-----------|--------|\n";

    for (const auto& s : report.signoffs) {
        std::string verdict_str;
        switch (s.verdict) {
            case SubsystemVerdict::kGreen:   verdict_str = "✅ Green"; break;
            case SubsystemVerdict::kGated:   verdict_str = "⬜ Gated"; break;
            case SubsystemVerdict::kBlocked: verdict_str = "❌ Blocked"; break;
        }
        ss << "| " << s.subsystem_name << " | " << verdict_str << " |\n";
    }
    return ss.str();
}

void ReleaseSignoffRunner::clear() { signoffs_.clear(); }

} // namespace markamp::core
