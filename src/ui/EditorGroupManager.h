#pragma once

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"
#include "ui/BreadcrumbBar.h"
#include "ui/DesignSystemContext.h"
#include "ui/DiffPanel.h"
#include "ui/EditorPanel.h"
#include "ui/TabBar.h"
#include "ui/ThemeAwareWindow.h"

#include <nlohmann/json.hpp>
#include <wx/panel.h>
#include <wx/window.h>

#include <memory>
#include <string>
#include <vector>

namespace markamp::core
{
class IMermaidRenderer;
class IMathRenderer;
} // namespace markamp::core

namespace markamp::ui
{

enum class SplitOrientation
{
    kHorizontal,
    kVertical
};

enum class EditorGridLayout
{
    kSingle,
    kTwoColumns,
    kTwoRows,
    kGrid2x2,
    kLeftPlusTwoRight
};

struct EditorGroupLeaf
{
    int group_id{0};
    TabBar* tab_bar{nullptr};
    BreadcrumbBar* breadcrumb{nullptr};
    EditorPanel* editor{nullptr};
    DiffPanel* diff_panel{nullptr};
    wxPanel* container{nullptr};
    std::vector<std::string> open_files;
    std::string active_file;
    bool is_focused{false};
};

struct EditorGroupNode
{
    int id{0};
    SplitOrientation split{SplitOrientation::kHorizontal};
    double split_ratio{0.5};
    std::unique_ptr<EditorGroupNode> first_child;
    std::unique_ptr<EditorGroupNode> second_child;

    // Leaf data (null for internal nodes)
    std::unique_ptr<EditorGroupLeaf> leaf;
};

class EditorGroupManager : public ThemeAwareWindow
{
public:
    EditorGroupManager(wxWindow* parent,
                       core::ThemeEngine& theme_engine,
                       core::EventBus& event_bus,
                       core::Config* config,
                       core::IMermaidRenderer* mermaid_renderer,
                       core::IMathRenderer* math_renderer,
                       DesignSystemContext& ds_context);
    ~EditorGroupManager() override = default;

    EditorGroupManager(const EditorGroupManager&) = delete;
    auto operator=(const EditorGroupManager&) -> EditorGroupManager& = delete;
    EditorGroupManager(EditorGroupManager&&) = delete;
    auto operator=(EditorGroupManager&&) -> EditorGroupManager& = delete;

    // Split operations
    void SplitActiveGroupRight();
    void SplitActiveGroupDown();
    void SplitGroup(int group_id, SplitOrientation orientation);
    void CloseGroup(int group_id);
    void JoinGroupWithNext(int group_id);

    // Focus
    void SetFocusedGroup(int group_id);
    [[nodiscard]] auto GetFocusedGroupId() const -> int;
    auto GetGroupCount() const -> int;

    // Grid System
    void ApplyGridLayout(EditorGridLayout layout);
    void ResetGroupSizes();

    // File routing
    void OpenFileInGroup(int group_id, const std::string& path);
    void OpenFileInFocusedGroup(const std::string& path);

    // Diff routing
    void OpenDiffInGroup(int group_id, const std::string& left_path, const std::string& right_path);

    // Navigation
    void FocusNextGroup();
    void FocusPreviousGroup();
    void FocusGroupByIndex(int index);
    void GoBackInGroupHistory();
    void GoForwardInGroupHistory();

    // Queries
    [[nodiscard]] auto GetEditorForGroup(int group_id) -> EditorPanel*;
    [[nodiscard]] auto GetFocusedEditor() -> EditorPanel*;
    [[nodiscard]] auto GetFocusedTabBar() -> TabBar*;
    [[nodiscard]] auto GetFocusedBreadcrumbBar() -> BreadcrumbBar*;
    [[nodiscard]] auto GetAllGroupIds() const -> std::vector<int>;

    // State Persistence
    [[nodiscard]] auto SerializeState() const -> nlohmann::json;
    void RestoreState(const nlohmann::json& state);

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;
    void OnSize(wxSizeEvent& event);

private:
    void UpdateLayout();
    void LayoutNode(EditorGroupNode* node, const wxRect& rect);
    auto FindNode(EditorGroupNode* node, int group_id) -> EditorGroupNode*;
    auto CreateGroupLeaf(int group_id) -> std::unique_ptr<EditorGroupLeaf>;

    core::EventBus& event_bus_;
    [[maybe_unused]] core::Config* config_{nullptr};
    [[maybe_unused]] core::IMermaidRenderer* mermaid_renderer_{nullptr};
    [[maybe_unused]] core::IMathRenderer* math_renderer_{nullptr};
    DesignSystemContext& ds_context_;

    wxTimer symbol_update_timer_;

    std::unique_ptr<EditorGroupNode> root_{nullptr};
    int next_group_id_{1};
    int focused_group_id_{1};
    int maximized_group_id_{-1};

    // Task 23: History
    std::vector<int> focus_history_;
    int history_index_{-1};

    core::Subscription split_req_sub_;
    core::Subscription more_actions_sub_;
    core::Subscription focus_req_sub_;
    core::Subscription toggle_max_sub_;
    core::Subscription open_diff_sub_;
    core::Subscription breadcrumb_nav_sub_;
    core::Subscription cursor_pos_sub_;

    void OnSplitRequest(const core::events::EditorGroupSplitRequestEvent& evt);
    void OnMoreActions(const core::events::EditorGroupMoreActionsEvent& evt);
    void OnFocusGroupRequest(const core::events::EditorGroupFocusRequestEvent& evt);
    void OnToggleMaximizeEvent(const core::events::EditorGroupToggleMaximizeEvent& evt);
    void OnBreadcrumbNavigate(const core::events::BreadcrumbNavigateEvent& evt);
    void OnCursorPositionChanged(const core::events::CursorPositionChangedEvent& evt);

    void OnPaint(wxPaintEvent& evt);
    void OnMouseEvent(wxMouseEvent& evt);
    void OnSymbolUpdateTimer(wxTimerEvent& evt);

    auto HitTestDivider(EditorGroupNode* node, const wxPoint& pos, const wxRect& rect)
        -> EditorGroupNode*;
    void DrawDividers(wxDC& dc, EditorGroupNode* node, const wxRect& rect);

    EditorGroupNode* dragging_node_{nullptr};
    bool is_dragging_{false};
};

class EditorGroupDropTarget : public wxTextDropTarget
{
public:
    explicit EditorGroupDropTarget(EditorGroupManager* manager);
    bool OnDropText(wxCoord coord_x, wxCoord coord_y, const wxString& text) override;

private:
    EditorGroupManager* manager_{nullptr};
};

} // namespace markamp::ui
