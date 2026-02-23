#pragma once

#include "DesignSystemContext.h"
#include "LayoutMetrics.h"
#include "SpacingGrid.h"
#include "ThemeAwareWindow.h"
#include "core/EventBus.h"

#include <string>
#include <vector>

namespace markamp::ui
{

/// A breadcrumb navigation bar that shows the current file path and heading hierarchy.
/// Inspired by VSCode's breadcrumb bar above the editor.
class BreadcrumbBar : public ThemeAwareWindow
{
public:
    BreadcrumbBar(wxWindow* parent, DesignSystemContext& design_system);

    /// Set the file path segments (e.g. ["src", "ui", "EditorPanel.cpp"])
    void SetFilePath(const std::vector<std::string>& segments);

    /// Set the heading path (e.g. ["Chapter 1", "Section 1.1"])
    void SetHeadingPath(const std::vector<std::string>& headings);

    using SegmentClickCallback = std::function<void(const std::string& path)>;
    void SetSegmentClickCallback(SegmentClickCallback callback);

    struct TraversalSegment
    {
        std::string surface_label;
        std::string anchor_label;
        int nav_entry_index{-1};
    };

    void SetTraversalSegments(const std::vector<TraversalSegment>& segments);
    [[nodiscard]] auto GetTraversalSegments() const -> const std::vector<TraversalSegment>&;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    void Rebuild();

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnLeaveWindow(wxMouseEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    DesignSystemContext& ds_;

    std::vector<std::string> file_segments_;
    std::vector<std::string> heading_segments_;
    SegmentClickCallback segment_click_callback_;

    std::vector<TraversalSegment> traversal_segments_;

    struct DrawnSegment
    {
        wxRect bounds;
        std::string full_path; // For tooltip and callback
    };
    std::vector<DrawnSegment> drawn_segments_;
    int hovered_segment_{-1};
    int focused_segment_index_{-1};
    bool is_focused_{false};
};

} // namespace markamp::ui
