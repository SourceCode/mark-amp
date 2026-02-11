#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/FileNode.h"
#include "core/ThemeEngine.h"

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

    // Callbacks
    using FileSelectCallback = std::function<void(const core::FileNode&)>;
    void SetOnFileSelect(FileSelectCallback callback);

    // Layout constants
    static constexpr int kRowHeight = 28;
    static constexpr int kIndentWidth = 16;
    static constexpr int kLeftPadding = 8;
    static constexpr int kIconSize = 14;
    static constexpr int kChevronSize = 12;
    static constexpr int kIconTextGap = 8;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    // Rendering
    void OnPaint(wxPaintEvent& event);
    void DrawNode(wxDC& dc, const core::FileNode& node, int depth, int& y_offset);
    void DrawFolderIcon(wxDC& dc, bool is_open, int x, int y);
    void DrawFileIcon(wxDC& dc, int x, int y);
    void DrawChevron(wxDC& dc, bool is_open, int x, int y);

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

    // State
    std::vector<core::FileNode> roots_;
    std::string active_file_id_;
    std::string hovered_node_id_;
    FileSelectCallback on_file_select_;
    core::EventBus& event_bus_;

    int scroll_offset_{0};
    int virtual_height_{0};
};

} // namespace markamp::ui
