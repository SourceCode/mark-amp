#pragma once

#include "DesignSystemContext.h"
#include "LayoutMetrics.h"
#include "SpacingGrid.h"
#include "ThemeAwareWindow.h"
#include "core/EventBus.h"

#include <wx/stattext.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// A breadcrumb navigation bar that shows the current file path and heading hierarchy.
/// Inspired by VSCode's breadcrumb bar above the editor.
class BreadcrumbBar : public ThemeAwareWindow
{
public:
    BreadcrumbBar(wxWindow* parent, DesignSystemContext& ds, core::EventBus& event_bus);

    /// Set the file path segments (e.g. ["src", "ui", "EditorPanel.cpp"])
    void SetFilePath(const std::vector<std::string>& segments);

    /// Set the heading path (e.g. ["Chapter 1", "Section 1.1"])
    void SetHeadingPath(const std::vector<std::string>& headings);

    // R3 Fix 20: Callback when a segment is clicked (receives full path up to that segment)
    using SegmentClickCallback = std::function<void(const std::string& path)>;
    void SetSegmentClickCallback(SegmentClickCallback callback);

    // V8 Phase 12 (Phase 38): Cross-surface traversal segments

    /// A breadcrumb segment representing a cross-surface traversal point.
    struct TraversalSegment
    {
        std::string surface_label; ///< e.g. "Editor", "Canvas"
        std::string anchor_label;  ///< e.g. "main.md:42", "board:node-7"
        int nav_entry_index{-1};   ///< Index into global navigation timeline
    };

    /// Set traversal breadcrumb segments for cross-surface navigation display.
    void SetTraversalSegments(const std::vector<TraversalSegment>& segments);

    /// Get the current traversal segments.
    [[nodiscard]] auto GetTraversalSegments() const -> const std::vector<TraversalSegment>&;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    void Rebuild();
    void OnLabelClick(wxMouseEvent& event); // R3 Fix 20

    DesignSystemContext& ds_;
    core::EventBus& event_bus_;

    std::vector<std::string> file_segments_;
    std::vector<std::string> heading_segments_;
    wxStaticText* label_{nullptr};
    SegmentClickCallback segment_click_callback_; // R3 Fix 20

    // V8 Phase 12 (Phase 38): Cross-surface traversal
    std::vector<TraversalSegment> traversal_segments_;
};

} // namespace markamp::ui
