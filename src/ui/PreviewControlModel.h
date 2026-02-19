#pragma once

#include <cstdint>
#include <string>

namespace markamp::ui
{

/// Sync mode between editor and preview (Phase 24 Task 2).
enum class PreviewSyncMode : uint8_t
{
    kFollowCursor,  ///< Preview follows editor cursor
    kFollowHeading, ///< Preview follows nearest heading
    kLocked,        ///< Preview scroll is independent
};

/// Scroll anchor mode (Phase 24 Task 3).
enum class ScrollAnchor : uint8_t
{
    kTop,
    kCursor,
    kHeading,
};

/// Testable model for Preview Surface Controls (Phase 24).
///
/// Encapsulates:
/// - Sync mode state (follow cursor / follow heading / locked)
/// - Scroll anchor management
/// - Pin and refresh controls
/// - Block action state
class PreviewControlModel
{
public:
    // ── Sync mode ───────────────────────────────────────────────────

    void set_sync_mode(PreviewSyncMode mode);
    [[nodiscard]] auto sync_mode() const -> PreviewSyncMode;

    /// Cycle to next sync mode.
    void cycle_sync_mode();

    /// Human-readable sync mode label.
    [[nodiscard]] auto sync_mode_label() const -> std::string;

    // ── Scroll anchor ───────────────────────────────────────────────

    void set_scroll_anchor(ScrollAnchor anchor);
    [[nodiscard]] auto scroll_anchor() const -> ScrollAnchor;

    // ── Pin ─────────────────────────────────────────────────────────

    void set_pinned(bool pinned);
    [[nodiscard]] auto is_pinned() const -> bool;

    // ── Refresh ─────────────────────────────────────────────────────

    /// Whether live refresh is enabled.
    void set_live_refresh(bool enabled);
    [[nodiscard]] auto live_refresh() const -> bool;

    // ── Block actions ───────────────────────────────────────────────

    /// Whether preview needs manual refresh (content changed while paused).
    void set_stale(bool stale);
    [[nodiscard]] auto is_stale() const -> bool;

private:
    PreviewSyncMode sync_mode_{PreviewSyncMode::kFollowCursor};
    ScrollAnchor scroll_anchor_{ScrollAnchor::kCursor};
    bool pinned_{false};
    bool live_refresh_{true};
    bool stale_{false};
};

} // namespace markamp::ui
