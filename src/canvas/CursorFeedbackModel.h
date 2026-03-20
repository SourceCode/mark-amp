#pragma once

/**
 * @file CursorFeedbackModel.h
 * @brief V17 Phase 01 W08: Cursor & mode feedback model.
 *
 * Maps tool state to cursor style and status-bar label so the UI
 * can reflect the active tool, pending action, and drop-target.
 */

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// Canvas cursor styles matching platform cursors.
enum class CanvasCursorStyle : uint8_t
{
    kDefault,    ///< Arrow
    kCrosshair,  ///< Draw / create tool
    kGrab,       ///< Pan idle (open hand)
    kGrabbing,   ///< Pan active (closed hand)
    kMove,       ///< Object move
    kResize,     ///< Handle resize
    kText,       ///< Text I-beam
    kForbidden,  ///< Invalid drop target
    kPointer,    ///< Clickable element
};

/// Testable model for cursor and mode-feedback state.
class CursorFeedbackModel
{
public:
    // ── Cursor ──────────────────────────────────────────────────────

    /// Set the current cursor style.
    auto set_cursor(CanvasCursorStyle style) -> void { cursor_ = style; }

    /// Get the current cursor style.
    [[nodiscard]] auto cursor() const noexcept -> CanvasCursorStyle { return cursor_; }

    // ── Mode label ──────────────────────────────────────────────────

    /// Set the status-bar label (e.g. "Drawing", "Panning").
    auto set_mode_label(const std::string& label) -> void { mode_label_ = label; }

    /// Get the status-bar label.
    [[nodiscard]] auto mode_label() const noexcept -> const std::string& { return mode_label_; }

    // ── Visibility ──────────────────────────────────────────────────

    /// Toggle feedback visibility.
    auto set_show_feedback(bool show) -> void { show_feedback_ = show; }

    /// Whether feedback is visible.
    [[nodiscard]] auto show_feedback() const noexcept -> bool { return show_feedback_; }

    // ── Helpers ─────────────────────────────────────────────────────

    /// Human-readable cursor name.
    [[nodiscard]] static auto cursor_name(CanvasCursorStyle style) -> std::string
    {
        switch (style)
        {
        case CanvasCursorStyle::kDefault: return "default";
        case CanvasCursorStyle::kCrosshair: return "crosshair";
        case CanvasCursorStyle::kGrab: return "grab";
        case CanvasCursorStyle::kGrabbing: return "grabbing";
        case CanvasCursorStyle::kMove: return "move";
        case CanvasCursorStyle::kResize: return "resize";
        case CanvasCursorStyle::kText: return "text";
        case CanvasCursorStyle::kForbidden: return "forbidden";
        case CanvasCursorStyle::kPointer: return "pointer";
        }
        return "default";
    }

private:
    CanvasCursorStyle cursor_{CanvasCursorStyle::kDefault};
    std::string mode_label_;
    bool show_feedback_{true};
};

} // namespace markamp::canvas
