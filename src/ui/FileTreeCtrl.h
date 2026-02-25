#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/FileNode.h"
#include "core/ThemeEngine.h"

#include <wx/bitmap.h>
#include <wx/bmpbndl.h>
#include <wx/event.h>
#include <wx/panel.h>
#include <wx/timer.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{
class SidebarSkeletonPlaceholder;
class ThemedScrollbar;

class FileTreeCtrl : public ThemeAwareWindow
{
public:
    FileTreeCtrl(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    // Data
    void SetFileTree(const std::vector<core::FileNode>& roots);
    void SetActiveFileId(const std::string& file_id);
    void EnsureNodeVisible(const std::string& node_id);
    void CollapseAllNodes(); // R4 Fix 15

    // Filtering
    void ApplyFilter(const std::string& filter);
    void ClearFilter();

    // Callbacks
    using FileSelectCallback = std::function<void(const core::FileNode&)>;
    using FileOpenCallback = std::function<void(const core::FileNode&)>;
    void SetOnFileSelect(FileSelectCallback callback);
    void SetOnFileOpen(FileOpenCallback callback);

    // Workspace root for relative path calculation
    void SetWorkspaceRoot(const std::string& root_path);

    // Loading State
    void ShowLoadingState();
    void HideLoadingState();

    // Scroll Persistence
    [[nodiscard]] int GetScrollOffset() const
    {
        return scroll_offset_;
    }
    void SetScrollOffset(int offset);

    // Layout constants
    [[deprecated(
        "Use ComponentSizeResolver for tree row height")]] static constexpr int kRowHeight = 24;
    static constexpr int kIndentWidth = 16;
    [[deprecated(
        "Use SpacingGrid or ComponentSizeResolver padding")]] static constexpr int kLeftPadding = 6;
    static constexpr int kIconSize = 16;
    static constexpr int kTwistieSize = 16; // Width of the twistie/chevron area
    static constexpr int kIconTextGap = 6;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    // Rendering
    void OnSize(wxSizeEvent& event);
    void OnPaint(wxPaintEvent& event);
    void
    DrawNode(wxDC& dc, const core::FileNode& node, int depth, int& y_offset, int& current_index);

    // Icons
    void LoadIcons();

    // Interaction
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnDoubleClick(wxMouseEvent& event);
    void OnRightClick(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // Context menu
    void ShowFileContextMenu(core::FileNode& node);
    void ShowEmptyAreaContextMenu();

    // Keyboard navigation
    int focused_node_index_{-1};
    auto GetVisibleNodes() -> std::vector<core::FileNode*>;
    void CollectVisibleNodes(std::vector<core::FileNode*>& result,
                             std::vector<core::FileNode>& nodes);
    auto FindParentIndex(const std::vector<core::FileNode*>& visible, int child_index) -> int;
    [[nodiscard]] auto GetFocusedNodeId() const -> std::string;

    // R3 Fix 4: Type-ahead search
    std::string type_ahead_buffer_;
    wxTimer type_ahead_timer_;
    void OnTypeAheadTimerExpired(wxTimerEvent& event);

    // R3 Fix 2: Auto-reveal helpers
    auto ExpandAncestors(const std::string& node_id) -> bool;

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
    void OnScrollbarDrag(wxScrollWinEvent& event);
    void UpdateScrollbar();

    // Filtering
    auto MatchesFilter(const core::FileNode& node, const std::string& lower_filter) const -> bool;
    void ApplyFilterRecursive(std::vector<core::FileNode>& nodes, const std::string& lower_filter);

    // State
    std::vector<core::FileNode> roots_;
    std::string active_file_id_;
    std::string hovered_node_id_;
    std::string filter_text_;
    std::string workspace_root_;
    FileSelectCallback on_file_select_;
    FileOpenCallback on_file_open_;
    core::EventBus& event_bus_;
    core::Subscription keyboard_mode_sub_; // Phase 05 Task 3

    int scroll_offset_{0};
    int virtual_height_{0};

    SidebarSkeletonPlaceholder* loading_skeleton_{nullptr};
    bool is_loading_{false};

    ThemedScrollbar* scrollbar_{nullptr};
};

} // namespace markamp::ui
