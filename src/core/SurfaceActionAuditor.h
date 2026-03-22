/// @file SurfaceActionAuditor.h
/// @brief V21 Phase 07 — Workbench surface action auditing for Explorer, Search, Problems, etc.
///
/// Audits that each workbench surface panel (Explorer, Search, Problems, Output,
/// Terminal, Debug, SCM) has its controls mapped to canonical actions and
/// no dead/placeholder controls remain exposed.
#pragma once

#include "core/ControlActionManifest.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// SurfaceControlKind — classification of surface-local controls
// ============================================================================

enum class SurfaceControlKind : uint8_t
{
    kToolbarButton,
    kContextMenuItem,
    kInlineAction,
    kTreeAction,
    kHeaderAction,
    kSearchAction,
    kNavigationAction,
    kQuickFixAction,
};

/// Label for SurfaceControlKind.
[[nodiscard]] constexpr auto surface_control_label(SurfaceControlKind kind) -> const char*
{
    switch (kind)
    {
    case SurfaceControlKind::kToolbarButton: return "ToolbarButton";
    case SurfaceControlKind::kContextMenuItem: return "ContextMenuItem";
    case SurfaceControlKind::kInlineAction: return "InlineAction";
    case SurfaceControlKind::kTreeAction: return "TreeAction";
    case SurfaceControlKind::kHeaderAction: return "HeaderAction";
    case SurfaceControlKind::kSearchAction: return "SearchAction";
    case SurfaceControlKind::kNavigationAction: return "NavigationAction";
    case SurfaceControlKind::kQuickFixAction: return "QuickFixAction";
    }
    return "Unknown";
}

// ============================================================================
// SurfaceControlBinding — a control within a workbench surface
// ============================================================================

struct SurfaceControlBinding
{
    std::string action_id;
    std::string surface_id;      ///< e.g., "explorer", "search", "problems"
    std::string label;
    SurfaceControlKind kind{SurfaceControlKind::kToolbarButton};
    bool is_bound{false};
    bool is_enabled{true};
    bool is_gated{false};        ///< Gated until feature is complete
    std::string gate_reason;     ///< Why it's gated
};

// ============================================================================
// SurfaceAuditEntry — diagnostic for a surface
// ============================================================================

struct SurfaceAuditEntry
{
    std::string surface_id;
    std::string action_id;
    std::string issue;
    bool is_dead{false};
    bool is_placeholder{false};
    bool is_wrong_target{false};
};

// ============================================================================
// SurfaceActionAuditor — the auditing engine
// ============================================================================

class SurfaceActionAuditor
{
public:
    SurfaceActionAuditor() = default;

    // ── Registration ──

    void register_control(SurfaceControlBinding binding);
    void register_controls(std::vector<SurfaceControlBinding> bindings);

    [[nodiscard]] auto get_control(const std::string& action_id) const
        -> const SurfaceControlBinding*;

    [[nodiscard]] auto controls_for_surface(const std::string& surface_id) const
        -> std::vector<const SurfaceControlBinding*>;

    [[nodiscard]] auto controls_by_kind(SurfaceControlKind kind) const
        -> std::vector<const SurfaceControlBinding*>;

    [[nodiscard]] auto all_surfaces() const -> std::vector<std::string>;

    [[nodiscard]] auto control_count() const -> std::size_t;

    // ── Gating ──

    void gate_control(const std::string& action_id, const std::string& reason);
    void ungate_control(const std::string& action_id);
    [[nodiscard]] auto gated_controls() const -> std::vector<const SurfaceControlBinding*>;

    // ── Manifest Validation ──

    void refresh_from_manifest(const ControlActionManifest& manifest);

    // ── Diagnostics ──

    [[nodiscard]] auto audit(const ControlActionManifest& manifest) const
        -> std::vector<SurfaceAuditEntry>;

    [[nodiscard]] auto live_count() const -> std::size_t;
    [[nodiscard]] auto dead_count() const -> std::size_t;
    [[nodiscard]] auto gated_count() const -> std::size_t;

    // ── V24 P01-T04: Release Gate Enforcement ──

    /// Result of enforce_release_gate — hard pass/fail instead of advisory.
    struct ReleaseGateResult
    {
        bool passed{false};
        int total_controls{0};
        int dead_controls{0};
        int placeholder_controls{0};
        int unbound_controls{0};
        std::vector<std::string> blocking_action_ids;

        [[nodiscard]] auto has_blockers() const noexcept -> bool
        {
            return !blocking_action_ids.empty();
        }

        [[nodiscard]] auto blocker_count() const noexcept -> int
        {
            return static_cast<int>(blocking_action_ids.size());
        }
    };

    /// Enforce release gate: returns hard pass/fail.
    /// Unlike audit(), this returns a single pass/fail decision suitable for CI.
    [[nodiscard]] auto enforce_release_gate(const ControlActionManifest& manifest) const
        -> ReleaseGateResult;

private:
    std::unordered_map<std::string, SurfaceControlBinding> controls_;
    std::vector<std::string> insertion_order_;
};

} // namespace markamp::core
