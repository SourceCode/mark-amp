#pragma once

#include "ComponentSizeResolver.h"
#include "DesignSystemContext.h"
#include "DesignTokenRegistry.h"
#include "ElevationSystem.h"
#include "LayoutMetrics.h"
#include "SidebarPanelRegistry.h"
#include "SidebarToolbar.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/FileNode.h"
#include "core/ThemeEngine.h"
#include "layout/WorkbenchShell.h"

#include <nlohmann/json.hpp>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/srchctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/window.h>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class Config;
class FeatureRegistry;
class IMermaidRenderer;
class IMathRenderer;
} // namespace markamp::core

namespace markamp::core
{
class IExtensionManagementService;
class IExtensionGalleryService;
} // namespace markamp::core

namespace markamp::ui::animation
{
class TransitionManager;
}

namespace markamp::ui
{

class ActivityBar;
class BreadcrumbBar;
class ExplorerPanel;
class ExtensionsBrowserPanel;
class FileTreeCtrl;
class OutputPanel;
class PreviewPanel;
class ProblemsPanel;
class SecondarySidebarTabStrip;
class SplitterBar;
class SplitView;
class StatusBarPanel;
class TabBar;
class Toolbar;
class TreeViewHost;
class WalkthroughPanel;
class WebviewHostPanel;

// SidebarMode enum defined in SidebarMode.h, included via SidebarPanelRegistry.h.

class CanvasWorkspacePanel;

/// Orchestrates the three-zone layout below CustomChrome:
///   Sidebar (256px) | Content Area (flex)
///   StatusBar (24px, bottom)
class LayoutManager : public ThemeAwareWindow
{
public:
    LayoutManager(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  core::EventBus& event_bus,
                  core::Config* config,
                  core::FeatureRegistry* feature_registry = nullptr,
                  core::IMermaidRenderer* mermaid_renderer = nullptr,
                  core::IMathRenderer* math_renderer = nullptr);

    // Zone access (for later phases to populate)
    [[nodiscard]] auto sidebar_container() -> wxWindow*;
    [[nodiscard]] auto content_container() -> wxWindow*;
    [[nodiscard]] auto statusbar_container() -> StatusBarPanel*;

    // Data
    void setFileTree(const std::vector<core::FileNode>& roots);
    void SetWorkspaceRoot(const std::string& root_path);
    void SaveFile(const std::string& path);

    // Multi-file tab management (QoL features 1-5)
    void OpenFileInTab(const std::string& path);
    void CloseTab(const std::string& path);
    void SwitchToTab(const std::string& path);
    void SaveActiveFile();
    void SaveActiveFileAs();
    [[nodiscard]] auto GetActiveFilePath() const -> std::string;
    [[nodiscard]] auto GetActiveFileContent() const -> std::string;
    [[nodiscard]] auto GetTabBar() -> TabBar*;
    [[nodiscard]] auto HasUnsavedFiles() const -> bool;
    [[nodiscard]] auto GetOpenFileCount() const -> size_t;
    void FocusEditor();

    // R2 Fixes 15-17: Save All, Revert, Close All
    void SaveAllFiles();
    void RevertActiveFile();
    void CloseAllTabs();

    // Auto-save (feature 12)
    void StartAutoSave();
    void StopAutoSave();

    // File reload (feature 13)
    void CheckExternalFileChanges();

    // Sidebar control
    void toggle_sidebar();
    void set_sidebar_visible(bool visible);
    [[nodiscard]] auto is_sidebar_visible() const -> bool;

    // Sidebar width
    void set_sidebar_width(int width);
    [[nodiscard]] auto sidebar_width() const -> int;

    // Phase 6D/7A: Forward minimap toggle to editor
    void ToggleEditorMinimap();

    // Phase 06 Task 15/16: Zen and Presentation modes
    void ToggleZenMode();
    void SetZenMode(bool enable);
    [[nodiscard]] auto is_zen_mode() const -> bool;

    void TogglePresentationMode();
    void SetPresentationMode(bool enable);
    [[nodiscard]] auto is_presentation_mode() const -> bool;

    // Phase 8: Sidebar panel switching
    void SetSidebarMode(SidebarMode mode);
    [[nodiscard]] auto GetSidebarMode() const -> SidebarMode;
    void SetExtensionServices(core::IExtensionManagementService* mgmt_service,
                              core::IExtensionGalleryService* gallery_service);

    /// Phase 06 Task 2/4: Lazy panel registry for sidebar modes
    [[nodiscard]] auto sidebar_panel_registry() -> SidebarPanelRegistry&;

    /// Phase 06 Task 11: Secondary sidebar on the right side of the content area
    void ToggleSecondarySidebar();
    void SetSecondarySidebarMode(SidebarMode mode);
    void set_secondary_sidebar_visible(bool visible);
    [[nodiscard]] auto is_secondary_sidebar_visible() const -> bool;

    // Phase 09 Task 21: Sidebar Swap Command
    void SwapSidebars();

    static constexpr int kDefaultSidebarWidth = 256;
    static constexpr int kMinSidebarWidth = 180;
    static constexpr int kMaxSidebarWidth = 400;
    static constexpr int kStatusBarHeight = 24;
    static constexpr int kBottomPanelHeight = 200;

    // Bottom panel control
    void ShowBottomPanel(bool show);
    [[nodiscard]] auto is_bottom_panel_visible() const -> bool;

    // V8 Phase 6: Canvas mode switching
    void ShowCanvasWorkspace();
    void ShowEditorWorkspace();
    [[nodiscard]] auto is_canvas_mode() const -> bool;
    [[nodiscard]] auto canvas_workspace() -> CanvasWorkspacePanel*;

    // V8 Phase 11: Unified workbench mode
    void SetWorkbenchMode(core::events::WorkbenchMode mode);
    [[nodiscard]] auto GetWorkbenchMode() const -> core::events::WorkbenchMode;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    core::Config* config_;
    core::FeatureRegistry* feature_registry_{nullptr};
    core::IMermaidRenderer* mermaid_renderer_{nullptr};
    core::IMathRenderer* math_renderer_{nullptr};

    std::unique_ptr<DesignTokenRegistry> design_registry_;
    std::unique_ptr<TypographyScale> typography_scale_;
    std::unique_ptr<SpacingGrid> spacing_grid_;
    std::unique_ptr<ElevationSystem> elevation_system_;
    std::unique_ptr<DesignSystemContext> ds_context_;

    // Core layout container
    layout::WorkbenchShell* shell_{nullptr};

    // Child components
    StatusBarPanel* statusbar_panel_{nullptr};
    FileTreeCtrl* file_tree_{nullptr};
    TabBar* tab_bar_{nullptr};
    wxSearchCtrl* search_field_{nullptr}; // R5 Fix 7: wxSearchCtrl has built-in cancel button
    SplitView* split_view_{nullptr};
    Toolbar* toolbar_{nullptr};
    BreadcrumbBar* breadcrumb_bar_{nullptr}; // R3 Fix 14

    // Bottom panel host (Output, Problems, Walkthrough tabs)
    wxNotebook* bottom_panel_notebook_{nullptr};
    OutputPanel* output_panel_{nullptr};
    ProblemsPanel* problems_panel_{nullptr};
    WalkthroughPanel* walkthrough_panel_{nullptr};
    TreeViewHost* tree_view_host_{nullptr};
    WebviewHostPanel* webview_host_panel_{nullptr};
    bool bottom_panel_visible_{false};

    // Phase 8 / Phase 06: Sidebar mode switching
    SidebarMode sidebar_mode_{kSidebarModeExplorer};
    SidebarPanelRegistry panel_registry_;
    ExplorerPanel* explorer_panel_{nullptr}; // Container for file tree + search + footer
    ExtensionsBrowserPanel* extensions_panel_{nullptr};
    core::IExtensionManagementService* ext_mgmt_service_{nullptr};
    core::IExtensionGalleryService* ext_gallery_service_{nullptr};
    core::Subscription show_extensions_sub_;
    core::Subscription show_explorer_sub_;
    core::Subscription feature_toggled_sub_;
    core::Subscription activity_bar_selection_sub_;
    core::Subscription sidebar_focus_sub_;
    core::Subscription file_opened_sub_; // Phase 09 Task 22: Side-by-side auto outline

    // V8 Phase 6: Canvas workspace
    CanvasWorkspacePanel* canvas_workspace_{nullptr};
    bool canvas_mode_{false};
    core::Subscription board_open_sub_;

    // Phase 08 Task 4: Sidebar Transition Animation
    std::unique_ptr<animation::TransitionManager> sidebar_transition_mgr_;

    // Phase 06 Task 11 / Phase 09: Secondary sidebar
    bool secondary_sidebar_visible_{false};
    SidebarMode secondary_sidebar_mode_{kSidebarModeSearch};
    // Phase 09 Task 2 & 3: Secondary Sidebar
    SidebarPanelRegistry secondary_panel_registry_;
    ActivityBar* activity_bar_{nullptr};
    SecondarySidebarTabStrip* secondary_tab_strip_{nullptr};
    wxPanel* secondary_sidebar_container_{nullptr};
    wxStaticText* secondary_empty_state_{nullptr}; // Phase 09 Task 15: Empty State Label
    core::Subscription secondary_sidebar_selection_sub_;
    core::Subscription sidebar_panel_moved_sub_;

    // Phase 06 Task 15/16: Zen and Presentation Mode state
    bool zen_mode_{false};
    nlohmann::json pre_zen_state_;
    bool presentation_mode_{false};
    nlohmann::json pre_presentation_state_;

    // V8 Phase 11: Unified workbench mode
    core::events::WorkbenchMode workbench_mode_{core::events::WorkbenchMode::kEditor};
    core::Subscription workbench_mode_switch_sub_;

    bool sidebar_visible_{true};
    int sidebar_width_{kDefaultSidebarWidth};
    int sidebar_current_width_{kDefaultSidebarWidth};

    // Event subscriptions
    core::Subscription sidebar_toggle_sub_;

    void CreateLayout();
    void CreateBottomPanelHost();
    void RegisterSidebarPanels();
    void RegisterSecondarySidebarPanels();
    void SaveLayoutState();
    void RestoreLayoutState();

    // Multi-file state
    struct FileBuffer
    {
        std::string content;
        bool is_modified{false};
        int cursor_position{0};
        int first_visible_line{0};
        std::filesystem::file_time_type last_write_time{};
    };
    std::unordered_map<std::string, FileBuffer> file_buffers_;
    std::string active_file_path_;
    std::string workspace_name_; // V8 Phase 1: Project-first sidebar header

    // Event subscriptions for tabs
    core::Subscription tab_switched_sub_;
    core::Subscription tab_close_sub_;
    core::Subscription tab_save_sub_;
    core::Subscription tab_save_as_sub_;
    core::Subscription content_changed_sub_;
    core::Subscription file_reload_sub_;
    core::Subscription goto_line_sub_;

    // R6 subscriptions
    core::Subscription find_sub_;
    core::Subscription replace_sub_;
    core::Subscription dup_line_sub_;
    core::Subscription toggle_comment_sub_;
    core::Subscription delete_line_sub_;
    core::Subscription wrap_toggle_sub_;

    core::Subscription panel_context_menu_sub_;

    // R7 subscriptions
    core::Subscription move_line_up_sub_;
    core::Subscription move_line_down_sub_;
    core::Subscription join_lines_sub_;
    core::Subscription sort_asc_sub_;
    core::Subscription sort_desc_sub_;
    core::Subscription transform_upper_sub_;
    core::Subscription transform_lower_sub_;
    core::Subscription transform_title_sub_;
    core::Subscription select_all_occ_sub_;
    core::Subscription expand_line_sub_;
    core::Subscription insert_line_above_sub_;
    core::Subscription insert_line_below_sub_;
    core::Subscription fold_all_sub_;
    core::Subscription unfold_all_sub_;
    core::Subscription toggle_line_numbers_sub_;
    core::Subscription toggle_whitespace_sub_;

    // R8 subscriptions
    core::Subscription copy_line_up_sub_;
    core::Subscription copy_line_down_sub_;
    core::Subscription delete_all_left_sub_;
    core::Subscription delete_all_right_sub_;
    core::Subscription reverse_lines_sub_;
    core::Subscription delete_dup_lines_sub_;
    core::Subscription transpose_chars_sub_;
    core::Subscription indent_selection_sub_;
    core::Subscription outdent_selection_sub_;
    core::Subscription select_word_sub_;
    core::Subscription select_paragraph_sub_;
    core::Subscription toggle_read_only_sub_;
    core::Subscription convert_indent_spaces_sub_;
    core::Subscription convert_indent_tabs_sub_;
    core::Subscription jump_to_bracket_sub_;
    core::Subscription toggle_minimap_sub_;
    core::Subscription fold_current_sub_;
    core::Subscription unfold_current_sub_;
    core::Subscription add_line_comment_sub_;
    core::Subscription remove_line_comment_sub_;
    // R9 subscriptions
    core::Subscription trim_trailing_ws_sub_;
    core::Subscription expand_selection_sub_;
    core::Subscription shrink_selection_sub_;
    core::Subscription cursor_undo_sub_;
    core::Subscription cursor_redo_sub_;
    core::Subscription move_text_left_sub_;
    core::Subscription move_text_right_sub_;
    core::Subscription toggle_auto_indent_sub_;
    core::Subscription toggle_bracket_matching_sub_;
    core::Subscription toggle_code_folding_sub_;
    core::Subscription toggle_indent_guides_sub_;
    core::Subscription select_to_bracket_sub_;
    core::Subscription toggle_block_comment_sub_;
    core::Subscription insert_datetime_sub_;
    core::Subscription bold_sub_;
    core::Subscription italic_sub_;
    core::Subscription inline_code_sub_;
    core::Subscription blockquote_sub_;
    core::Subscription cycle_heading_sub_;
    core::Subscription insert_table_sub_;
    // R10 subscriptions
    core::Subscription toggle_smart_list_sub_;
    core::Subscription close_other_tabs_sub_;
    core::Subscription close_saved_tabs_sub_;
    core::Subscription insert_link_sub_;
    core::Subscription add_cursor_below_sub_;
    core::Subscription add_cursor_above_sub_;
    core::Subscription add_cursor_next_occurrence_sub_;
    core::Subscription remove_surrounding_brackets_sub_;
    core::Subscription duplicate_selection_or_line_sub_;
    core::Subscription show_table_editor_sub_;
    core::Subscription toggle_scroll_beyond_sub_;
    core::Subscription toggle_highlight_line_sub_;
    core::Subscription toggle_auto_close_brackets_sub_;
    core::Subscription toggle_sticky_scroll_sub_;
    core::Subscription toggle_font_ligatures_sub_;
    core::Subscription toggle_smooth_caret_sub_;
    core::Subscription toggle_color_preview_sub_;
    core::Subscription toggle_edge_ruler_sub_;
    core::Subscription ensure_final_newline_sub_;
    core::Subscription insert_snippet_sub_;
    // R11 subscriptions
    core::Subscription toggle_smooth_scrolling_sub_;
    core::Subscription toggle_trailing_ws_sub_;
    core::Subscription toggle_auto_trim_ws_sub_;
    core::Subscription toggle_gutter_separator_sub_;
    core::Subscription toggle_insert_final_newline_sub_;
    core::Subscription toggle_whitespace_boundary_sub_;
    core::Subscription toggle_link_auto_complete_sub_;
    core::Subscription toggle_drag_drop_sub_;
    core::Subscription toggle_auto_save_sub_;
    core::Subscription toggle_empty_sel_clipboard_sub_;
    core::Subscription cycle_render_whitespace_sub_;
    core::Subscription delete_current_line_sub_;
    core::Subscription copy_line_no_sel_sub_;
    core::Subscription add_sel_next_match_sub_;
    core::Subscription smart_backspace_sub_;
    core::Subscription hide_table_editor_sub_;
    core::Subscription auto_pair_bold_sub_;
    core::Subscription auto_pair_italic_sub_;
    core::Subscription auto_pair_code_sub_;
    core::Subscription toggle_minimap_r11_sub_;
    // R12 subscriptions
    core::Subscription close_tabs_to_left_sub_;
    core::Subscription close_tabs_to_right_sub_;
    core::Subscription pin_tab_sub_;
    core::Subscription unpin_tab_sub_;
    // R13 subscriptions
    core::Subscription copy_file_path_sub_;
    core::Subscription reveal_in_finder_sub_;
    core::Subscription zoom_in_sub_;
    core::Subscription zoom_out_sub_;
    core::Subscription zoom_reset_sub_;
    core::Subscription convert_eol_lf_sub_;
    core::Subscription convert_eol_crlf_sub_;
    // R14: workspace_refresh and open_folder handled in MainFrame

    // Auto-save
    wxTimer auto_save_timer_;
    static constexpr int kAutoSaveIntervalMs = 30000; // 30 seconds
    void OnAutoSaveTimer(wxTimerEvent& event);
};

} // namespace markamp::ui
