#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/FileNode.h"
#include "core/ThemeEngine.h"
#include "ui/GitStatusProvider.h"

#include <wx/bitmap.h>
#include <wx/bmpbndl.h>
#include <wx/dnd.h>
#include <wx/event.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/timer.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{
class SidebarSkeletonPlaceholder;
class ThemedScrollbar;
class FileTypeIconRegistry;
class IFileTreeDecorationProvider;

class FileTreeCtrl : public ThemeAwareWindow
{
public:
    FileTreeCtrl(wxWindow* parent,
                 core::ThemeEngine& theme_engine,
                 core::EventBus& event_bus,
                 FileTypeIconRegistry& icon_registry);

    // Data
    void SetFileTree(const std::vector<core::FileNode>& roots);
    void SetActiveFileId(const std::string& file_id);
    void EnsureNodeVisible(const std::string& node_id);
    void CollapseAllNodes(); // R4 Fix 15

    // File Operations (Copy, Paste, Delete)
    void DeleteSelectedNodes();
    void CopySelectedNodes();
    void CutSelectedNodes();
    void PasteNodes();

    // Drag and Drop (Task 7, Task 22)
    void OnBeginDrag(wxMouseEvent& event);
    bool HandleDropData(wxCoord x, wxCoord y, const wxString& text);
    wxDragResult HandleDragOver(wxCoord x, wxCoord y, wxDragResult defResult);
    void HandleDragLeave();

    // Creation & Renaming
    void CreateNewFile();
    void CreateNewFolder();
    void RenameActiveNode();

    // Filtering
    void ApplyFilter(const std::string& filter);
    void ClearFilter();

    // Decorations (Task 12)
    void AddDecorationProvider(IFileTreeDecorationProvider* provider);
    void RemoveDecorationProvider(IFileTreeDecorationProvider* provider);

    // Configuration Settings (Task 24 & 25)
    void SetAutoReveal(bool enabled);
    void SetCompactFolders(bool enabled);
    void SetFileNesting(bool enabled);
    void SetExclusions(const std::vector<std::string>& exclusions);
    void SetShowFileMetadata(bool enabled);

    [[nodiscard]] bool GetAutoReveal() const
    {
        return auto_reveal_;
    }
    [[nodiscard]] bool GetCompactFolders() const
    {
        return compact_folders_;
    }
    [[nodiscard]] bool GetFileNesting() const
    {
        return file_nesting_;
    }
    [[nodiscard]] const std::vector<std::string>& GetExclusions() const
    {
        return exclusions_;
    }
    [[nodiscard]] bool GetShowFileMetadata() const
    {
        return show_file_metadata_;
    }

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
    void DrawNode(wxDC& dc,
                  wxGraphicsContext* gc,
                  const core::FileNode& node,
                  int depth,
                  int& y_offset,
                  int& current_index);

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

    // Sorting (Task 18)
    enum class SortOrder
    {
        Name,
        TypeFirst, // Groups by extension
        ModifiedDate
    };
    void SetSortOrder(SortOrder order);
    void SortFileTree();

    // Inline Editing
    enum class InlineEditMode
    {
        None,
        NewFile,
        NewFolder,
        Rename
    };
    InlineEditMode inline_edit_mode_{InlineEditMode::None};
    std::string inline_edit_parent_id_;
    std::string inline_edit_original_name_;
    wxTextCtrl* inline_editor_{nullptr};

    void StartInlineEdit(InlineEditMode mode,
                         const std::string& parent_id,
                         const std::string& initial_text = "");
    void CommitInlineEdit();
    void CancelInlineEdit();
    void InsertDummyNodeForEdit();
    void RemoveDummyNodeForEdit();

    // State
    std::vector<core::FileNode> roots_;
    std::string active_file_id_;
    std::vector<std::string> selected_node_ids_; // For multi-selection
    std::string hovered_node_id_;
    std::string filter_text_;
    std::string workspace_root_;
    SortOrder sort_order_{SortOrder::Name};

    // Configuration State
    bool auto_reveal_{true};
    bool compact_folders_{false};
    bool file_nesting_{true}; // Default to true for Task 25
    bool show_file_metadata_{false};
    std::vector<std::string> exclusions_;

    std::string hover_drop_target_id_; // Node ID currently being hovered with a drag
    [[maybe_unused]] bool hover_drop_insert_after_{
        false}; // True if dropping *between* items rather than *into* a folder

    // Clipboard State
    bool is_cut_operation_{false};
    std::vector<std::string> clipboard_paths_;

    FileSelectCallback on_file_select_;
    FileOpenCallback on_file_open_;
    core::EventBus& event_bus_;
    FileTypeIconRegistry& icon_registry_;
    GitStatusProvider git_status_;
    std::vector<IFileTreeDecorationProvider*> decoration_providers_;
    core::Subscription keyboard_mode_sub_; // Phase 05 Task 3

    int scroll_offset_{0};
    int virtual_height_{0};

    SidebarSkeletonPlaceholder* loading_skeleton_{nullptr};
    bool is_loading_{false};

    ThemedScrollbar* scrollbar_{nullptr};
};

} // namespace markamp::ui
