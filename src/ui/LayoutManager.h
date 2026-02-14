#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/FileNode.h"
#include "core/ThemeEngine.h"

#include <wx/sizer.h>
#include <wx/srchctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/timer.h>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class Config;
class FeatureRegistry;
} // namespace markamp::core

namespace markamp::core
{
class IExtensionManagementService;
class IExtensionGalleryService;
} // namespace markamp::core

namespace markamp::ui
{

class BreadcrumbBar;
class ExtensionsBrowserPanel;
class FileTreeCtrl;
class PreviewPanel;
class SplitterBar;
class SplitView;
class StatusBarPanel;
class TabBar;
class Toolbar;

/// Sidebar display mode.
enum class SidebarMode
{
    kExplorer,
    kExtensions
};

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
                  core::FeatureRegistry* feature_registry = nullptr);

    // Zone access (for later phases to populate)
    [[nodiscard]] auto sidebar_container() -> wxPanel*;
    [[nodiscard]] auto content_container() -> wxPanel*;
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

    // Phase 8: Sidebar panel switching
    void SetSidebarMode(SidebarMode mode);
    [[nodiscard]] auto GetSidebarMode() const -> SidebarMode;
    void SetExtensionServices(core::IExtensionManagementService* mgmt_service,
                              core::IExtensionGalleryService* gallery_service);

    static constexpr int kDefaultSidebarWidth = 256;
    static constexpr int kMinSidebarWidth = 180;
    static constexpr int kMaxSidebarWidth = 400;
    static constexpr int kStatusBarHeight = 24;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    core::Config* config_;
    core::FeatureRegistry* feature_registry_{nullptr};

    // Child panels
    wxPanel* sidebar_panel_{nullptr};
    wxPanel* content_panel_{nullptr};
    StatusBarPanel* statusbar_panel_{nullptr};
    SplitterBar* splitter_{nullptr};
    FileTreeCtrl* file_tree_{nullptr};
    TabBar* tab_bar_{nullptr};
    wxSearchCtrl* search_field_{nullptr}; // R5 Fix 7: wxSearchCtrl has built-in cancel button
    SplitView* split_view_{nullptr};
    Toolbar* toolbar_{nullptr};
    wxStaticText* file_count_label_{nullptr};
    wxStaticText* header_label_{nullptr};    // R3 Fix 19
    BreadcrumbBar* breadcrumb_bar_{nullptr}; // R3 Fix 14

    // Phase 8: Sidebar mode switching
    SidebarMode sidebar_mode_{SidebarMode::kExplorer};
    wxPanel* explorer_panel_{nullptr}; // Container for file tree + search + footer
    ExtensionsBrowserPanel* extensions_panel_{nullptr};
    core::IExtensionManagementService* ext_mgmt_service_{nullptr};
    core::IExtensionGalleryService* ext_gallery_service_{nullptr};
    core::Subscription show_extensions_sub_;
    core::Subscription show_explorer_sub_;
    core::Subscription feature_toggled_sub_;

    // Sizer management
    wxBoxSizer* main_sizer_{nullptr};
    wxBoxSizer* body_sizer_{nullptr};

    bool sidebar_visible_{true};
    int sidebar_width_{kDefaultSidebarWidth};
    int sidebar_current_width_{kDefaultSidebarWidth};

    // Animation
    wxTimer sidebar_anim_timer_;
    int sidebar_anim_start_width_{0};
    int sidebar_anim_target_width_{0};
    double sidebar_anim_progress_{0.0};
    bool sidebar_anim_showing_{false};
    static constexpr int kAnimFrameMs = 16; // ~60fps
    static constexpr double kShowDurationMs = 300.0;
    static constexpr double kHideDurationMs = 200.0;

    void OnSidebarAnimTimer(wxTimerEvent& event);

    // Event subscriptions
    core::Subscription sidebar_toggle_sub_;

    void CreateLayout();
    void UpdateSidebarSize(int width);
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

    // Sidebar custom painting
    void OnSidebarPaint(wxPaintEvent& event);
};

} // namespace markamp::ui
