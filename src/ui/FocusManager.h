#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Identifies a focusable region within the application chrome.
/// Used by the global focus manager to implement Tab/Shift+Tab traversal.
enum class FocusZoneId : uint8_t
{
    kMenuBar,
    kActivityBar,
    kSidebar,
    kEditorArea,
    kBottomPanel,
    kStatusBar,
    kBreadcrumb,

    kCount ///< Sentinel — must be last
};

/// Direction for focus traversal.
enum class FocusDirection : uint8_t
{
    kForward,  ///< Tab / Right / Down
    kBackward, ///< Shift+Tab / Left / Up
};

/// Arrow key behavior contract for a single control surface.
enum class ArrowKeyBehavior : uint8_t
{
    kVerticalList,    ///< Up/Down navigate items; Left/Right collapse/expand
    kHorizontalStrip, ///< Left/Right navigate items; Up/Down are no-ops
    kGrid,            ///< Full 2D arrow navigation
    kNone,            ///< Control does not handle arrow keys
};

/// Focus restoration context — remembers where focus was before
/// dialogs/overlays opened, so it can be restored on close.
struct FocusSnapshot
{
    FocusZoneId zone{FocusZoneId::kEditorArea};
    int item_index{-1}; ///< Index within zone, -1 = zone-level focus
};

/// Global focus manager for chrome-level keyboard navigation.
///
/// Provides:
/// - Tab/Shift+Tab traversal between focus zones
/// - Arrow key contract queries per zone
/// - Focus restoration after dialog/overlay close
///
/// Usage:
///   auto& fm = FocusManager::get();
///   fm.advance(FocusDirection::kForward); // Tab
///   fm.restore();                          // After dialog close
class FocusManager
{
public:
    static auto get() -> FocusManager&;

    /// Current active focus zone.
    [[nodiscard]] auto current_zone() const -> FocusZoneId
    {
        return current_zone_;
    }

    /// Explicitly set the active focus zone, resetting item focus.
    void set_zone(FocusZoneId zone);

    /// Explicitly set the active focus zone and item.
    void set_focus(FocusZoneId zone, int item_index = -1);

    /// Explicitly set only the active item focus within the current zone.
    void set_item(int item_index);

    /// Current active focus item within the zone (-1 means zone-level focus).
    [[nodiscard]] auto current_item() const -> int
    {
        return current_item_;
    }

    /// Move focus forward or backward through the zone ring.
    void advance(FocusDirection direction);

    /// Take a snapshot of current focus state (before opening overlay).
    void push_snapshot();

    /// Restore focus from the most recent snapshot.
    void restore();

    /// Query arrow key behavior for a given zone.
    [[nodiscard]] static auto arrow_behavior(FocusZoneId zone) -> ArrowKeyBehavior;

    /// Query whether a zone is currently visible/enabled.
    /// Invisible zones are skipped during Tab traversal.
    [[nodiscard]] auto is_zone_enabled(FocusZoneId zone) const -> bool;

    /// Enable or disable a focus zone (e.g., when sidebar is hidden).
    void set_zone_enabled(FocusZoneId zone, bool enabled);

    using FocusChangeCallback = std::function<void(FocusZoneId zone, int item_index)>;

    /// Register a callback to be notified when focus changes. Returns a listener ID.
    auto on_focus_changed(FocusChangeCallback callback) -> std::size_t;

    /// Remove a previously registered focus change listener.
    void remove_focus_listener(std::size_t listener_id);

    /// Get the focus zone traversal order (for testing/introspection).
    [[nodiscard]] auto zone_order() const -> std::vector<FocusZoneId>;

private:
    FocusManager();

    void publish_focus_change();

    FocusZoneId current_zone_{FocusZoneId::kEditorArea};
    int current_item_{-1};
    std::vector<FocusSnapshot> snapshot_stack_;

    std::vector<std::pair<std::size_t, FocusChangeCallback>> listeners_;
    std::size_t next_listener_id_{0};

    // Visibility flags per zone
    static constexpr auto kZoneCount = static_cast<std::size_t>(FocusZoneId::kCount);
    std::array<bool, kZoneCount> zone_enabled_{};

    /// Find next enabled zone in the given direction.
    [[nodiscard]] auto next_enabled_zone(FocusZoneId from, FocusDirection dir) const -> FocusZoneId;
};

} // namespace markamp::ui
