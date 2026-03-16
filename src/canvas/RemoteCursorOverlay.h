// ============================================================================
// File: src/canvas/RemoteCursorOverlay.h
// Phase 13: Canvas Collaboration — remote cursor rendering overlay
// ============================================================================
#pragma once

#include "canvas/CanvasTypes.h"

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// The tool a remote participant is currently using.
enum class RemoteToolType : uint8_t
{
    kPointer,
    kPan,
    kDraw,
    kText,
    kShape,
    kEraser,
    kLaser
};

/// A single remote cursor entry tracked by the overlay.
struct CursorEntry
{
    std::string participant_id;
    std::string display_name;
    CanvasColor color{0, 120, 215, 255}; ///< Participant color
    Point2D position{0.0, 0.0};          ///< World-space cursor position
    Point2D target_position{0.0, 0.0};   ///< Interpolation target
    RemoteToolType tool{RemoteToolType::kPointer};
    bool show_label{true};
    double opacity{1.0};

    /// Timestamp of last update (for idle detection)
    std::chrono::steady_clock::time_point last_update{std::chrono::steady_clock::now()};

    /// Whether the cursor is using the pointer tool.
    [[nodiscard]] auto is_pointer() const noexcept -> bool
    {
        return tool == RemoteToolType::kPointer;
    }

    /// Whether the cursor is drawing.
    [[nodiscard]] auto is_drawing() const noexcept -> bool
    {
        return tool == RemoteToolType::kDraw;
    }

    // ── Round 3 Batch 5-6 (#48-52) ──────────────────────────────

    /// (#48) Whether the cursor is using the text tool.
    [[nodiscard]] auto is_text_tool() const noexcept -> bool
    {
        return tool == RemoteToolType::kText;
    }

    /// (#49) Whether the cursor is an eraser.
    [[nodiscard]] auto is_eraser() const noexcept -> bool
    {
        return tool == RemoteToolType::kEraser;
    }

    /// (#50) Whether the cursor is a laser pointer.
    [[nodiscard]] auto is_laser() const noexcept -> bool
    {
        return tool == RemoteToolType::kLaser;
    }

    /// (#51) Whether the cursor label is shown.
    [[nodiscard]] auto has_label() const noexcept -> bool
    {
        return show_label;
    }

    /// (#52) Whether the cursor is visible (opacity > 0).
    [[nodiscard]] auto is_visible() const noexcept -> bool
    {
        return opacity > 0.0;
    }
};

/// Configuration for the cursor overlay renderer.
struct CursorOverlayConfig
{
    double interpolation_speed{0.15};  ///< Lerp factor per frame (0–1)
    double idle_timeout_seconds{30.0}; ///< Seconds before cursor fades
    double fade_duration_seconds{2.0}; ///< Duration of fade-out animation
    double label_offset_x{12.0};       ///< Name label offset from cursor
    double label_offset_y{18.0};
    bool show_labels_by_default{true};
    double cursor_size{16.0}; ///< Base cursor icon size

    /// Whether labels are shown by default.
    [[nodiscard]] auto has_labels() const noexcept -> bool
    {
        return show_labels_by_default;
    }

    // ── Round 3 Batch 6 (#53) ───────────────────────────────────

    /// (#53) Whether an idle timeout is configured.
    [[nodiscard]] auto has_idle_timeout() const noexcept -> bool
    {
        return idle_timeout_seconds > 0.0;
    }
};

/// Overlay that renders remote participant cursors on the canvas.
///
/// Features:
///   - Color-coded cursors with participant name labels
///   - Smooth position interpolation for remote updates
///   - Idle cursor fade-out after configurable timeout
///   - Tool-specific cursor shapes (pointer, draw, text, etc.)
class RemoteCursorOverlay
{
public:
    RemoteCursorOverlay();
    explicit RemoteCursorOverlay(CursorOverlayConfig config);

    // ── Cursor Updates ────────────────────────────────────────────

    /// Add or update a remote cursor position.
    auto update_cursor(const std::string& participant_id,
                       const std::string& display_name,
                       const Point2D& position,
                       const CanvasColor& color) -> void;

    /// Update the tool type for a remote cursor.
    auto update_tool(const std::string& participant_id, RemoteToolType tool) -> void;

    /// Remove a remote cursor (participant left).
    auto remove_cursor(const std::string& participant_id) -> void;

    /// Remove all cursors.
    auto clear() -> void;

    // ── Frame Update ──────────────────────────────────────────────

    /// Advance interpolation and idle timers. Call once per frame.
    auto tick(double delta_seconds) -> void;

    // ── Query ─────────────────────────────────────────────────────

    /// Get all active cursor entries for rendering.
    [[nodiscard]] auto cursors() const -> const std::unordered_map<std::string, CursorEntry>&;

    /// Get visible cursors only (not fully faded).
    [[nodiscard]] auto visible_cursors() const -> std::vector<const CursorEntry*>;

    /// Get a specific cursor by participant ID.
    [[nodiscard]] auto find_cursor(const std::string& participant_id) const -> const CursorEntry*;

    /// Number of tracked cursors.
    [[nodiscard]] auto cursor_count() const -> size_t;

    // ── Configuration ─────────────────────────────────────────────

    [[nodiscard]] auto config() const -> const CursorOverlayConfig&;
    auto set_config(const CursorOverlayConfig& config) -> void;

    /// Toggle label visibility globally.
    auto set_labels_visible(bool visible) -> void;
    [[nodiscard]] auto labels_visible() const -> bool;

    // ── Utility ───────────────────────────────────────────────────

    /// Get a human-readable name for a tool type.
    [[nodiscard]] static auto tool_name(RemoteToolType tool) -> std::string;

    /// Get a default color palette for participants (up to 8 distinct colors).
    [[nodiscard]] static auto participant_color(size_t participant_index) -> CanvasColor;

    /// Whether any cursors are being tracked.
    [[nodiscard]] auto has_cursors() const noexcept -> bool
    {
        return !cursors_.empty();
    }

    /// Whether the overlay has no cursors.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return cursors_.empty();
    }

private:
    CursorOverlayConfig config_;
    std::unordered_map<std::string, CursorEntry> cursors_;
    bool labels_visible_{true};

    /// Interpolate cursor position toward target.
    static auto lerp_position(Point2D& current, const Point2D& target, double factor) -> void;

    /// Compute opacity based on idle time.
    [[nodiscard]] auto compute_idle_opacity(const CursorEntry& entry) const -> double;
};

} // namespace markamp::canvas
