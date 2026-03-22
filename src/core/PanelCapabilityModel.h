/// @file PanelCapabilityModel.h
/// @brief V21 Phase 06 — Panel capability metadata, content-host contracts, toggle routing.
///
/// Extends the panel lifecycle system with:
///   - Capability metadata for each panel (searchable, closable, resizable, etc.)
///   - Content-host contract enforcement (bottom panels, secondary sidebar)
///   - Panel toggle command routing through canonical commands
///   - Snapshot persistence for layout state
///   - Stub panel classification and gating
#pragma once

#include "core/ControlActionManifest.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// PanelCapability — what a panel can do
// ============================================================================

/// Capability flags for a panel.
enum class PanelCapability : uint16_t
{
    kNone         = 0,
    kSearchable   = 1 << 0,  ///< Panel content is searchable
    kClosable     = 1 << 1,  ///< Panel can be closed by user
    kResizable    = 1 << 2,  ///< Panel can be resized
    kDraggable    = 1 << 3,  ///< Panel can be moved/reordered
    kCollapsible  = 1 << 4,  ///< Panel can be collapsed
    kPersistable  = 1 << 5,  ///< Panel state is persisted
    kRefreshable  = 1 << 6,  ///< Panel supports manual refresh
    kHasToolbar   = 1 << 7,  ///< Panel has a toolbar
    kHasStatusBar = 1 << 8,  ///< Panel has a status bar
    kShowOnEvent  = 1 << 9,  ///< Panel auto-shows on events (build error, etc.)
};

/// Combine capabilities.
[[nodiscard]] constexpr auto operator|(PanelCapability a, PanelCapability b) -> PanelCapability
{
    return static_cast<PanelCapability>(
        static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

/// Check if capability set contains a flag.
[[nodiscard]] constexpr auto has_capability(PanelCapability set, PanelCapability flag) -> bool
{
    return (static_cast<uint16_t>(set) & static_cast<uint16_t>(flag)) != 0;
}

// ============================================================================
// PanelHostArea — where the panel lives
// ============================================================================

/// The host area for a panel.
enum class PanelHostArea : uint8_t
{
    kPrimarySidebar,
    kSecondarySidebar,
    kBottomPanel,
    kFloating,
};

/// Label for PanelHostArea.
[[nodiscard]] constexpr auto panel_host_label(PanelHostArea area) -> const char*
{
    switch (area)
    {
    case PanelHostArea::kPrimarySidebar: return "PrimarySidebar";
    case PanelHostArea::kSecondarySidebar: return "SecondarySidebar";
    case PanelHostArea::kBottomPanel: return "BottomPanel";
    case PanelHostArea::kFloating: return "Floating";
    }
    return "Unknown";
}

// ============================================================================
// PanelCapabilityEntry — metadata for a registered panel
// ============================================================================

/// Extended capability metadata for a panel.
struct PanelCapabilityEntry
{
    std::string panel_id;
    std::string label;
    PanelHostArea host_area{PanelHostArea::kBottomPanel};
    PanelCapability capabilities{PanelCapability::kNone};
    std::string toggle_action_id;  ///< Canonical action for show/hide toggle
    std::string activation_event;  ///< Event that auto-activates the panel (empty=manual only)
    bool is_stub{false};           ///< Whether this panel is a stub/placeholder
    bool is_gated{false};          ///< Whether gated behind a feature flag
    int activation_priority{0};    ///< Higher = activated earlier on event

    [[nodiscard]] auto has(PanelCapability cap) const noexcept -> bool
    {
        return has_capability(capabilities, cap);
    }
};

// ============================================================================
// PanelToggleCommand — a toggle command for a panel
// ============================================================================

/// Represents a toggle command binding for a panel.
struct PanelToggleCommand
{
    std::string panel_id;
    std::string action_id;       ///< Canonical action ID
    std::string shortcut;        ///< Keyboard shortcut display
    bool is_bound{false};        ///< Whether the toggle has a handler
};

// ============================================================================
// PanelLayoutSnapshot — complete layout state at a point in time
// ============================================================================

/// Snapshot of the entire panel layout for persistence.
struct PanelLayoutSnapshot
{
    struct PanelState
    {
        std::string panel_id;
        bool is_visible{true};
        int width{-1};
        int height{-1};
    };

    std::string active_bottom_panel;
    std::string active_secondary_panel;
    int bottom_panel_height{200};
    int secondary_sidebar_width{250};
    bool bottom_visible{false};
    bool secondary_visible{false};
    std::vector<PanelState> panel_states;
};

// ============================================================================
// PanelCapabilityDiagnostic — audit entry
// ============================================================================

/// Diagnostic for panel capability issues.
struct PanelCapabilityDiagnostic
{
    std::string panel_id;
    std::string issue;
    bool is_stub{false};
    bool is_missing_toggle{false};  ///< Panel has no toggle command
    bool is_missing_handler{false}; ///< Toggle command has no handler
    bool is_orphaned{false};        ///< Panel not in any host area
};

// ============================================================================
// PanelCapabilityModel — the capability engine
// ============================================================================

/// Manages panel capability metadata, toggle commands, and layout snapshots.
class PanelCapabilityModel
{
public:
    PanelCapabilityModel() = default;

    // ── Registration ──

    void register_panel(PanelCapabilityEntry entry);

    [[nodiscard]] auto get_panel(const std::string& panel_id) const
        -> const PanelCapabilityEntry*;

    [[nodiscard]] auto all_panels() const -> std::vector<const PanelCapabilityEntry*>;

    [[nodiscard]] auto panels_for_area(PanelHostArea area) const
        -> std::vector<const PanelCapabilityEntry*>;

    [[nodiscard]] auto stub_panels() const -> std::vector<const PanelCapabilityEntry*>;

    [[nodiscard]] auto panel_count() const -> std::size_t;

    // ── Toggle Commands ──

    void register_toggle(PanelToggleCommand toggle);

    [[nodiscard]] auto get_toggle(const std::string& panel_id) const
        -> const PanelToggleCommand*;

    auto dispatch_toggle(const std::string& panel_id,
                         ControlActionManifest& manifest) -> bool;

    void refresh_toggles(const ControlActionManifest& manifest);

    // ── Layout Snapshots ──

    void set_layout_state(const std::string& panel_id, bool visible,
                          int width = -1, int height = -1);

    void set_active_bottom(const std::string& panel_id);
    void set_active_secondary(const std::string& panel_id);
    void set_bottom_height(int height);
    void set_secondary_width(int width);
    void set_bottom_visible(bool visible);
    void set_secondary_visible(bool visible);

    [[nodiscard]] auto take_layout_snapshot() const -> PanelLayoutSnapshot;
    void restore_layout_snapshot(const PanelLayoutSnapshot& snapshot);

    // ── Diagnostics ──

    [[nodiscard]] auto diagnose(const ControlActionManifest& manifest) const
        -> std::vector<PanelCapabilityDiagnostic>;

    [[nodiscard]] auto live_panel_count() const -> std::size_t;
    [[nodiscard]] auto stub_count() const -> std::size_t;

private:
    std::unordered_map<std::string, PanelCapabilityEntry> panels_;
    std::vector<std::string> panel_order_;
    std::unordered_map<std::string, PanelToggleCommand> toggles_;

    // Layout state
    std::unordered_map<std::string, PanelLayoutSnapshot::PanelState> layout_states_;
    std::string active_bottom_;
    std::string active_secondary_;
    int bottom_height_{200};
    int secondary_width_{250};
    bool bottom_visible_{false};
    bool secondary_visible_{false};
};

} // namespace markamp::core
