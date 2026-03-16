#pragma once

/// @file LightbulbWidget.h
/// @brief V13 Phase 29 — Gutter lightbulb indicator for available code actions.
///
/// Renders a small lightbulb icon in the editor gutter to indicate that
/// code actions are available for the current line. The icon color encodes
/// the primary action kind: yellow (quick fix), blue (refactor), grey (source).

#include "core/CodeIntelligenceTypes.h"

#include <wx/dc.h>

namespace markamp::ui
{

/// Lightweight gutter icon that indicates available code actions.
///
/// Usage:
/// ```cpp
/// LightbulbWidget lightbulb;
/// lightbulb.SetVisible(true, 5, core::CodeActionKind::kQuickFix);
/// lightbulb.Paint(dc, gutter_x, line_y, 14);
/// ```
class LightbulbWidget
{
public:
    /// Set the lightbulb visibility and state.
    /// @param visible Whether to show the lightbulb
    /// @param line The line number to show it on (0-based)
    /// @param primary_kind The kind of the highest-priority action
    void SetVisible(bool visible,
                    int line = -1,
                    core::CodeActionKind primary_kind = core::CodeActionKind::kQuickFix);

    /// Hide the lightbulb.
    void Hide();

    /// Whether the lightbulb is currently visible.
    [[nodiscard]] auto IsVisible() const -> bool;

    /// The line the lightbulb is shown on.
    [[nodiscard]] auto GetLine() const -> int;

    /// The primary action kind.
    [[nodiscard]] auto GetKind() const -> core::CodeActionKind;

    /// Paint the lightbulb icon at the given position.
    /// @param device_context The drawing context
    /// @param pos_x Left edge of the icon
    /// @param pos_y Top edge of the icon
    /// @param size Icon size in pixels (default 14)
    void Paint(wxDC& device_context, int pos_x, int pos_y, int size = 14) const;

    /// Check if a point is within the lightbulb icon bounds.
    [[nodiscard]] auto HitTest(int click_x, int click_y) const -> bool;

    /// Whether the primary action kind is a quick fix.
    [[nodiscard]] auto is_quick_fix() const -> bool
    {
        return kind_ == core::CodeActionKind::kQuickFix;
    }

private:
    bool visible_{false};
    int line_{-1};
    core::CodeActionKind kind_{core::CodeActionKind::kQuickFix};

    // Cached paint position for hit testing
    mutable int last_x_{0};
    mutable int last_y_{0};
    mutable int last_size_{14};
};

} // namespace markamp::ui
