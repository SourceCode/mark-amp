/// @file StatusBarActionRouter.h
/// @brief V21 Phase 03 — Status bar action routing and classification.
///
/// Makes status bar items either real, actionable controls dispatched
/// through the canonical manifest, or explicitly informational (non-clickable).
/// Provides:
///   - Action vs. informational classification
///   - Manifest-driven click dispatch
///   - Context-aware enablement
///   - Status bar health diagnostics
#pragma once

#include "core/ControlActionManifest.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// StatusItemKind — classification
// ============================================================================

/// Classification of a status bar item.
enum class StatusItemKind : uint8_t
{
    kActionable,    ///< Clickable, dispatches a canonical action
    kInformational, ///< Display-only, not clickable
    kToggle,        ///< Toggles a state on click
};

/// Label conversion for StatusItemKind.
[[nodiscard]] constexpr auto status_item_kind_label(StatusItemKind kind) -> const char*
{
    switch (kind)
    {
    case StatusItemKind::kActionable: return "Actionable";
    case StatusItemKind::kInformational: return "Informational";
    case StatusItemKind::kToggle: return "Toggle";
    }
    return "Unknown";
}

// ============================================================================
// StatusBarBinding — a status item bound to a canonical action
// ============================================================================

/// Represents a status bar item bound to a canonical action (or marked informational).
struct StatusBarBinding
{
    std::string item_id;         ///< Status bar item identifier
    std::string action_id;       ///< Canonical action ID (empty for informational)
    std::string label;           ///< Display text
    std::string tooltip;         ///< Tooltip text
    StatusItemKind kind{StatusItemKind::kInformational};
    bool is_enabled{true};       ///< Whether the action is currently enabled
    bool is_visible{true};       ///< Whether the item is visible
    bool is_bound{false};        ///< Whether a real handler exists

    /// Whether this item should render as clickable.
    [[nodiscard]] auto is_clickable() const noexcept -> bool
    {
        return kind != StatusItemKind::kInformational && is_bound && is_enabled;
    }
};

// ============================================================================
// StatusBarDiagnostic — audit entry
// ============================================================================

/// Diagnostic entry for status bar routing issues.
struct StatusBarDiagnostic
{
    std::string item_id;
    std::string action_id;
    std::string issue;
    bool is_dead_action{false};     ///< Marked actionable but no handler
    bool is_misleading{false};      ///< Looks clickable but does nothing
};

// ============================================================================
// StatusBarActionRouter — the routing engine
// ============================================================================

/// Routes status bar item clicks through the canonical ControlActionManifest.
class StatusBarActionRouter
{
public:
    StatusBarActionRouter() = default;

    // ── Registration ──

    /// Register a status bar item binding.
    void register_item(StatusBarBinding binding);

    /// Register multiple items at once.
    void register_items(std::vector<StatusBarBinding> bindings);

    /// Remove an item by its ID.
    auto remove_item(const std::string& item_id) -> bool;

    // ── Lookup ──

    /// Get a binding by item ID.
    [[nodiscard]] auto get_item(const std::string& item_id) const
        -> const StatusBarBinding*;

    /// Get all bindings.
    [[nodiscard]] auto all_items() const -> std::vector<const StatusBarBinding*>;

    /// Get only actionable items.
    [[nodiscard]] auto actionable_items() const -> std::vector<const StatusBarBinding*>;

    /// Get only informational items.
    [[nodiscard]] auto informational_items() const -> std::vector<const StatusBarBinding*>;

    /// Total item count.
    [[nodiscard]] auto item_count() const -> std::size_t;

    // ── Click Dispatch ──

    /// Handle a click on a status bar item. Dispatches to the manifest.
    /// Returns true if the action was found and executed.
    auto dispatch_click(const std::string& item_id,
                        ControlActionManifest& manifest) -> bool;

    // ── Enablement Sync ──

    /// Refresh enablement for all bindings using current context.
    void refresh_enablement(const ControlActionManifest& manifest,
                            const ContextKeyService& context);

    // ── Manifest Sync ──

    /// Populate status bindings from manifest actions targeting kStatusBar surface.
    auto sync_from_manifest(const ControlActionManifest& manifest) -> int;

    // ── Diagnostics ──

    /// Detect issues: dead actions, misleading clickable-but-no-op items.
    [[nodiscard]] auto diagnose(const ControlActionManifest& manifest) const
        -> std::vector<StatusBarDiagnostic>;

    /// Count of actionable items with live handlers.
    [[nodiscard]] auto live_action_count() const -> std::size_t;

    /// Count of actionable items without handlers.
    [[nodiscard]] auto dead_action_count() const -> std::size_t;

private:
    std::unordered_map<std::string, StatusBarBinding> items_;
    std::vector<std::string> insertion_order_;
};

} // namespace markamp::core
