#pragma once

#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <wx/panel.h>
#include <wx/stattext.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// A breadcrumb navigation bar that shows the current file path and heading hierarchy.
/// Inspired by VSCode's breadcrumb bar above the editor.
class BreadcrumbBar : public wxPanel
{
public:
    BreadcrumbBar(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    /// Set the file path segments (e.g. ["src", "ui", "EditorPanel.cpp"])
    void SetFilePath(const std::vector<std::string>& segments);

    /// Set the heading path (e.g. ["Chapter 1", "Section 1.1"])
    void SetHeadingPath(const std::vector<std::string>& headings);

    /// Update theme colors
    void ApplyTheme();

    // R3 Fix 20: Callback when a segment is clicked (receives full path up to that segment)
    using SegmentClickCallback = std::function<void(const std::string& path)>;
    void SetSegmentClickCallback(SegmentClickCallback callback);

private:
    void Rebuild();
    void OnLabelClick(wxMouseEvent& event); // R3 Fix 20

    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    core::Subscription theme_sub_;

    std::vector<std::string> file_segments_;
    std::vector<std::string> heading_segments_;
    wxStaticText* label_{nullptr};
    SegmentClickCallback segment_click_callback_; // R3 Fix 20
};

} // namespace markamp::ui
