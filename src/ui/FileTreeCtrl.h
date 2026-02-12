#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/FileNode.h"
#include "core/ThemeEngine.h"

#include <wx/bitmap.h>
#include <wx/bmpbndl.h>
#include <wx/event.h>
#include <wx/panel.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

class FileTreeCtrl : public ThemeAwareWindow
{
public:
    FileTreeCtrl(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    // Data
    void SetFileTree(const std::vector<core::FileNode>& roots);
    void SetActiveFileId(const std::string& file_id);

    // Filtering
    void ApplyFilter(const std::string& filter);
    void ClearFilter();

    // Callbacks
    using FileSelectCallback = std::function<void(const core::FileNode&)>;
    void SetOnFileSelect(FileSelectCallback callback);

    // Layout constants
    static constexpr int kRowHeight = 22;
    static constexpr int kIndentWidth = 16;
    static constexpr int kLeftPadding = 6;
    static constexpr int kIconSize = 16;
    static constexpr int kTwistieSize = 16; // Width of the twistie/chevron area
    static constexpr int kIconTextGap = 6;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    // Rendering
    void OnPaint(wxPaintEvent& event);
    void DrawNode(wxDC& dc, const core::FileNode& node, int depth, int& y_offset);

    // Icons
    void LoadIcons();
    wxBitmapBundle icon_folder_;
    wxBitmapBundle icon_folder_open_;
    wxBitmapBundle icon_file_;
    wxBitmapBundle icon_file_text_;
    wxBitmapBundle icon_chevron_right_;
    wxBitmapBundle icon_chevron_down_;

    // Interaction
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);

    // Hit testing
    struct HitResult
    {
        core::FileNode* node{nullptr};
        bool on_chevron{false};
    };
    auto HitTest(const wxPoint& point) -> HitResult;
    auto HitTestRecursive(const wxPoint& point,
                          std::vector<core::FileNode>& nodes,
                          int depth,
                          int& y_offset) -> HitResult;

    // Scrolling
    void UpdateVirtualHeight();
    void OnScroll(wxMouseEvent& event);

    // Filtering
    auto MatchesFilter(const core::FileNode& node, const std::string& lower_filter) const -> bool;
    void ApplyFilterRecursive(std::vector<core::FileNode>& nodes, const std::string& lower_filter);

    // State
    std::vector<core::FileNode> roots_;
    std::string active_file_id_;
    std::string hovered_node_id_;
    std::string filter_text_;
    FileSelectCallback on_file_select_;
    core::EventBus& event_bus_;

    int scroll_offset_{0};
    int virtual_height_{0};
};

} // namespace markamp::ui
