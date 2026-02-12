#include "LayoutManager.h"

#include "EditorPanel.h"
#include "FileTreeCtrl.h"
#include "SplitView.h"
#include "SplitterBar.h"
#include "StatusBarPanel.h"
#include "TabBar.h"
#include "ThemeGallery.h"
#include "Toolbar.h"
#include "core/Config.h"
#include "core/Events.h"
#include "core/Logger.h"
#include "core/SampleFiles.h"

#include <wx/dcbuffer.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>

namespace markamp::ui
{

LayoutManager::LayoutManager(wxWindow* parent,
                             core::ThemeEngine& theme_engine,
                             core::EventBus& event_bus,
                             core::Config* config)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
    , event_bus_(event_bus)
    , config_(config)
    , sidebar_anim_timer_(this)
{
    RestoreLayoutState();
    CreateLayout();

    // Subscribe to sidebar toggle events from CustomChrome
    sidebar_toggle_sub_ = event_bus_.subscribe<core::events::SidebarToggleEvent>(
        [this](const core::events::SidebarToggleEvent& /*evt*/) { toggle_sidebar(); });

    // Tab event subscriptions
    tab_switched_sub_ = event_bus_.subscribe<core::events::TabSwitchedEvent>(
        [this](const core::events::TabSwitchedEvent& evt) { SwitchToTab(evt.file_path); });

    tab_close_sub_ = event_bus_.subscribe<core::events::TabCloseRequestEvent>(
        [this](const core::events::TabCloseRequestEvent& evt) { CloseTab(evt.file_path); });

    tab_save_sub_ = event_bus_.subscribe<core::events::TabSaveRequestEvent>(
        [this](const core::events::TabSaveRequestEvent& /*evt*/) { SaveActiveFile(); });

    tab_save_as_sub_ = event_bus_.subscribe<core::events::TabSaveAsRequestEvent>(
        [this](const core::events::TabSaveAsRequestEvent& /*evt*/) { SaveActiveFileAs(); });

    content_changed_sub_ = event_bus_.subscribe<core::events::EditorContentChangedEvent>(
        [this](const core::events::EditorContentChangedEvent& evt)
        {
            if (!active_file_path_.empty())
            {
                auto buf_it = file_buffers_.find(active_file_path_);
                if (buf_it != file_buffers_.end())
                {
                    buf_it->second.content = evt.content;
                    buf_it->second.is_modified = true;
                    if (tab_bar_ != nullptr)
                    {
                        tab_bar_->SetTabModified(active_file_path_, true);
                    }
                }
            }
        });

    // Auto-save timer
    Bind(wxEVT_TIMER, &LayoutManager::OnAutoSaveTimer, this, auto_save_timer_.GetId());

    // Bind animation timer
    Bind(wxEVT_TIMER, &LayoutManager::OnSidebarAnimTimer, this, sidebar_anim_timer_.GetId());

    // Start auto-save
    StartAutoSave();

    MARKAMP_LOG_INFO(
        "LayoutManager created (sidebar={}px, visible={})", sidebar_width_, sidebar_visible_);
}

void LayoutManager::SaveFile(const std::string& path)
{
    if (split_view_)
    {
        split_view_->SaveFile(path);
    }
}

void LayoutManager::CreateLayout()
{
    // --- Sidebar panel ---
    sidebar_panel_ = new wxPanel(this, wxID_ANY);
    sidebar_panel_->SetBackgroundStyle(wxBG_STYLE_PAINT);
    sidebar_panel_->Bind(wxEVT_PAINT, &LayoutManager::OnSidebarPaint, this);

    // Sidebar internal layout: header + content + footer
    auto* sidebar_sizer = new wxBoxSizer(wxVERTICAL);

    // Header: "EXPLORER"
    auto* header_panel = new wxPanel(sidebar_panel_, wxID_ANY, wxDefaultPosition, wxSize(-1, 40));
    header_panel->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgHeader));

    // Fix 11: Render "EXPLORER" label in header
    auto* header_sizer = new wxBoxSizer(wxHORIZONTAL);
    auto* header_label = new wxStaticText(header_panel, wxID_ANY, "EXPLORER");
    header_label->SetFont(
        theme_engine().font(core::ThemeFontToken::MonoRegular).Bold().Scaled(0.85f));
    header_label->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMuted));
    header_sizer->AddSpacer(12);
    header_sizer->Add(header_label, 0, wxALIGN_CENTER_VERTICAL);
    header_panel->SetSizer(header_sizer);

    sidebar_sizer->Add(header_panel, 0, wxEXPAND);

    // Search field: filter bar between header and file tree
    auto* search_sizer = new wxBoxSizer(wxHORIZONTAL);
    search_field_ = new wxTextCtrl(sidebar_panel_,
                                   wxID_ANY,
                                   wxEmptyString,
                                   wxDefaultPosition,
                                   wxSize(-1, 28),
                                   wxTE_PROCESS_ENTER | wxNO_BORDER);
    search_field_->SetHint("Filter files\u2026");
    search_field_->SetBackgroundColour(
        theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(110));
    search_field_->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMain));
    search_field_->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));

    search_sizer->AddSpacer(8);
    search_sizer->Add(search_field_, 1, wxALIGN_CENTER_VERTICAL);
    search_sizer->AddSpacer(8);
    sidebar_sizer->Add(search_sizer, 0, wxEXPAND | wxTOP | wxBOTTOM, 8); // 8E: was 4

    // Spacer content area -> FileTreeCtrl
    file_tree_ = new FileTreeCtrl(sidebar_panel_, theme_engine(), event_bus_);
    sidebar_sizer->Add(file_tree_, 1, wxEXPAND);

    // Bind search field text changes to filter
    search_field_->Bind(wxEVT_TEXT,
                        [this](wxCommandEvent& /*evt*/)
                        {
                            if (file_tree_ != nullptr)
                            {
                                file_tree_->ApplyFilter(search_field_->GetValue().ToStdString());
                            }
                        });

    // Wire file open callback — double-click or context menu "Open" (Fix 1)
    file_tree_->SetOnFileOpen(
        [this](const core::FileNode& node)
        {
            if (node.is_file())
            {
                OpenFileInTab(node.id);
            }
        });

    // Wire file select callback — single-click opens file (Fix 2)
    file_tree_->SetOnFileSelect(
        [this](const core::FileNode& node)
        {
            if (node.is_file())
            {
                OpenFileInTab(node.id);
            }
        });

    // Load sample file tree only when not restoring a workspace (Fix 14)
    if (config_ == nullptr || config_->get_string("workspace.last_path", "").empty())
    {
        auto sample_root = core::get_sample_file_tree();
        file_tree_->SetFileTree(sample_root.children);

        // Auto-select first file
        if (!sample_root.children.empty())
        {
            for (const auto& child : sample_root.children)
            {
                if (child.is_file())
                {
                    file_tree_->SetActiveFileId(child.id);
                    break;
                }
            }
        }
    } // end: Fix 14 conditional sample tree

    // Footer — Fix 13: show file count
    auto* footer_panel = new wxPanel(sidebar_panel_, wxID_ANY, wxDefaultPosition, wxSize(-1, 28));
    footer_panel->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgApp));
    auto* footer_sizer = new wxBoxSizer(wxHORIZONTAL);
    file_count_label_ = new wxStaticText(footer_panel, wxID_ANY, "");
    file_count_label_->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.8f));
    file_count_label_->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMuted));
    footer_sizer->AddSpacer(12);
    footer_sizer->Add(file_count_label_, 1, wxALIGN_CENTER_VERTICAL);
    footer_panel->SetSizer(footer_sizer);
    sidebar_sizer->Add(footer_panel, 0, wxEXPAND);

    sidebar_panel_->SetSizer(sidebar_sizer);

    // --- Splitter ---
    splitter_ = new SplitterBar(this, theme_engine(), this);

    // --- Content panel ---
    content_panel_ = new wxPanel(this, wxID_ANY);
    content_panel_->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgApp));

    // Content internal: toolbar + split view
    auto* content_sizer = new wxBoxSizer(wxVERTICAL);

    toolbar_ = new Toolbar(content_panel_, theme_engine(), event_bus_);
    toolbar_->SetOnThemeGalleryClick(
        [this]()
        {
            ThemeGallery gallery(this, theme_engine(), theme_engine().registry());
            gallery.ShowGallery();
        });
    content_sizer->Add(toolbar_, 0, wxEXPAND);

    // Tab bar (QoL feature 1)
    tab_bar_ = new TabBar(content_panel_, theme_engine(), event_bus_);
    content_sizer->Add(tab_bar_, 0, wxEXPAND);

    split_view_ = new SplitView(content_panel_, theme_engine(), event_bus_, config_);
    content_sizer->Add(split_view_, 1, wxEXPAND);

    content_panel_->SetSizer(content_sizer);

    // --- Status bar ---
    statusbar_panel_ = new StatusBarPanel(this, theme_engine(), event_bus_);

    // --- Main layout ---
    main_sizer_ = new wxBoxSizer(wxVERTICAL);

    // Body: sidebar + splitter + content (horizontal)
    body_sizer_ = new wxBoxSizer(wxHORIZONTAL);

    sidebar_current_width_ = sidebar_visible_ ? sidebar_width_ : 0;

    body_sizer_->Add(sidebar_panel_, 0, wxEXPAND);
    body_sizer_->Add(splitter_, 0, wxEXPAND);
    body_sizer_->Add(content_panel_, 1, wxEXPAND);

    main_sizer_->Add(body_sizer_, 1, wxEXPAND);
    main_sizer_->Add(statusbar_panel_, 0, wxEXPAND);

    SetSizer(main_sizer_);

    // Apply initial sidebar width
    UpdateSidebarSize(sidebar_current_width_);
}

// --- Zone access ---

auto LayoutManager::sidebar_container() -> wxPanel*
{
    return sidebar_panel_;
}

auto LayoutManager::content_container() -> wxPanel*
{
    return content_panel_;
}

auto LayoutManager::statusbar_container() -> StatusBarPanel*
{
    return statusbar_panel_;
}

void LayoutManager::setFileTree(const std::vector<core::FileNode>& roots)
{
    if (file_tree_ != nullptr)
    {
        file_tree_->SetFileTree(roots);
    }

    // Fix 13: Update sidebar footer file count
    if (file_count_label_ != nullptr)
    {
        size_t total_files = 0;
        size_t total_folders = 0;
        for (const auto& root : roots)
        {
            total_files += root.file_count();
            total_folders += root.folder_count();
            if (root.is_folder())
            {
                ++total_folders;
            }
            else
            {
                ++total_files;
            }
        }
        file_count_label_->SetLabel(
            wxString::Format("%zu files, %zu folders", total_files, total_folders));
    }
}

// Fix 15: Forward workspace root to file tree for relative path computation
void LayoutManager::SetWorkspaceRoot(const std::string& root_path)
{
    if (file_tree_ != nullptr)
    {
        file_tree_->SetWorkspaceRoot(root_path);
    }
}

// --- Sidebar control ---

void LayoutManager::toggle_sidebar()
{
    set_sidebar_visible(!sidebar_visible_);
}

void LayoutManager::set_sidebar_visible(bool visible)
{
    if (visible == sidebar_visible_ && !sidebar_anim_timer_.IsRunning())
    {
        return;
    }

    sidebar_visible_ = visible;
    sidebar_anim_showing_ = visible;
    sidebar_anim_start_width_ = sidebar_current_width_;
    sidebar_anim_target_width_ = visible ? sidebar_width_ : 0;
    sidebar_anim_progress_ = 0.0;

    sidebar_anim_timer_.Start(kAnimFrameMs);
    MARKAMP_LOG_DEBUG("Sidebar animation started: {} -> {}",
                      sidebar_anim_start_width_,
                      sidebar_anim_target_width_);
}

auto LayoutManager::is_sidebar_visible() const -> bool
{
    return sidebar_visible_;
}

void LayoutManager::set_sidebar_width(int width)
{
    sidebar_width_ = std::clamp(width, kMinSidebarWidth, kMaxSidebarWidth);
    if (sidebar_visible_ && !sidebar_anim_timer_.IsRunning())
    {
        UpdateSidebarSize(sidebar_width_);
    }
    SaveLayoutState();
}

auto LayoutManager::sidebar_width() const -> int
{
    return sidebar_width_;
}

// --- Animation ---

void LayoutManager::OnSidebarAnimTimer(wxTimerEvent& /*event*/)
{
    double duration = sidebar_anim_showing_ ? kShowDurationMs : kHideDurationMs;
    sidebar_anim_progress_ += static_cast<double>(kAnimFrameMs) / duration;

    if (sidebar_anim_progress_ >= 1.0)
    {
        sidebar_anim_progress_ = 1.0;
        sidebar_anim_timer_.Stop();
        SaveLayoutState();
    }

    // Easing: ease-out for show (1 - (1-t)^3), ease-in for hide (t^3)
    double t = sidebar_anim_progress_;
    double eased = sidebar_anim_showing_ ? 1.0 - std::pow(1.0 - t, 3.0) // ease-out
                                         : std::pow(t, 3.0);            // ease-in

    int new_width = sidebar_anim_start_width_ +
                    static_cast<int>(eased * static_cast<double>(sidebar_anim_target_width_ -
                                                                 sidebar_anim_start_width_));

    UpdateSidebarSize(new_width);
}

void LayoutManager::UpdateSidebarSize(int width)
{
    sidebar_current_width_ = width;

    // Show/hide sidebar and splitter based on width
    bool show = (width > 0);
    sidebar_panel_->Show(show);
    splitter_->Show(show);

    if (show)
    {
        sidebar_panel_->SetMinSize(wxSize(width, -1));
        sidebar_panel_->SetMaxSize(wxSize(width, -1));
    }

    body_sizer_->Layout();
    main_sizer_->Layout();
}

// --- Theme ---

void LayoutManager::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);

    content_panel_->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgApp));
    content_panel_->Refresh();

    if (search_field_ != nullptr)
    {
        search_field_->SetBackgroundColour(
            theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(110));
        search_field_->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMain));
        search_field_->Refresh();
    }

    sidebar_panel_->Refresh();
}

void LayoutManager::OnSidebarPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(sidebar_panel_);
    auto client_sz = sidebar_panel_->GetClientSize();
    const int panel_width = client_sz.GetWidth();
    const int panel_height = client_sz.GetHeight();

    // 8D: Subtle top-to-bottom gradient (BgPanel → 3% darker)
    {
        auto base_col = theme_engine().color(core::ThemeColorToken::BgPanel);
        auto darker = base_col.ChangeLightness(97);
        for (int row = 0; row < panel_height; ++row)
        {
            const double frac =
                static_cast<double>(row) / static_cast<double>(std::max(panel_height - 1, 1));
            auto lerp = [](int from, int to, double ratio) -> unsigned char
            {
                return static_cast<unsigned char>(
                    std::clamp(static_cast<int>(from + ratio * (to - from)), 0, 255));
            };
            paint_dc.SetPen(wxPen(wxColour(lerp(base_col.Red(), darker.Red(), frac),
                                           lerp(base_col.Green(), darker.Green(), frac),
                                           lerp(base_col.Blue(), darker.Blue(), frac)),
                                  1));
            paint_dc.DrawLine(0, row, panel_width, row);
        }
    }

    // 8A: Drop shadow on right edge (4 graduated bands: 8%→4%→2%→1% black)
    {
        constexpr int kShadowBands = 4;
        constexpr std::array<unsigned char, kShadowBands> kShadowAlphas = {20, 10, 5, 3};
        for (int band = 0; band < kShadowBands; ++band)
        {
            paint_dc.SetPen(
                wxPen(wxColour(0, 0, 0, kShadowAlphas.at(static_cast<size_t>(band))), 1));
            paint_dc.DrawLine(panel_width - 1 - band, 0, panel_width - 1 - band, panel_height);
        }
    }

    // 8B: Soft left highlight — 1px BgPanel lighter
    {
        auto highlight = theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(108);
        paint_dc.SetPen(wxPen(highlight, 1));
        paint_dc.DrawLine(0, 0, 0, panel_height);
    }
}

// --- Persistence ---

void LayoutManager::SaveLayoutState()
{
    if (config_ == nullptr)
    {
        return;
    }
    config_->set("layout.sidebar_visible", sidebar_visible_);
    config_->set("layout.sidebar_width", sidebar_width_);
    // Fix 15: Persist active file path for restore on next launch
    config_->set("workspace.last_active_file", active_file_path_);
}

void LayoutManager::RestoreLayoutState()
{
    if (config_ == nullptr)
    {
        return;
    }
    sidebar_visible_ = config_->get_bool("layout.sidebar_visible", true);
    sidebar_width_ = config_->get_int("layout.sidebar_width", kDefaultSidebarWidth);
    sidebar_width_ = std::clamp(sidebar_width_, kMinSidebarWidth, kMaxSidebarWidth);
    sidebar_current_width_ = sidebar_visible_ ? sidebar_width_ : 0;
}

void LayoutManager::ToggleEditorMinimap()
{
    if (split_view_ != nullptr)
    {
        auto* editor = split_view_->GetEditorPanel();
        if (editor != nullptr)
        {
            editor->ToggleMinimap();
        }
    }
}

// --- Multi-file tab management ---

void LayoutManager::OpenFileInTab(const std::string& path)
{
    // If file is already open, just switch to it
    if (tab_bar_ != nullptr && tab_bar_->HasTab(path))
    {
        SwitchToTab(path);
        return;
    }

    // Save current editor state before switching
    if (!active_file_path_.empty() && split_view_ != nullptr)
    {
        auto buf_it = file_buffers_.find(active_file_path_);
        if (buf_it != file_buffers_.end())
        {
            auto* editor = split_view_->GetEditorPanel();
            if (editor != nullptr)
            {
                buf_it->second.content = editor->GetContent();
                auto session = editor->GetSessionState();
                buf_it->second.cursor_position = session.cursor_position;
                buf_it->second.first_visible_line = session.first_visible_line;
            }
        }
    }

    // Read file content
    std::string content;
    try
    {
        std::ifstream file_stream(path);
        if (file_stream.is_open())
        {
            content.assign(std::istreambuf_iterator<char>(file_stream),
                           std::istreambuf_iterator<char>());
        }
        else
        {
            MARKAMP_LOG_ERROR("Failed to open file: {}", path);
            return;
        }
    }
    catch (const std::exception& ex)
    {
        MARKAMP_LOG_ERROR("Error reading file {}: {}", path, ex.what());
        return;
    }

    // Store in buffer
    FileBuffer buffer;
    buffer.content = content;
    buffer.is_modified = false;
    buffer.cursor_position = 0;
    buffer.first_visible_line = 0;
    try
    {
        buffer.last_write_time = std::filesystem::last_write_time(path);
    }
    catch (const std::filesystem::filesystem_error& ex)
    {
        MARKAMP_LOG_WARN("Could not get last write time for {}: {}", path, ex.what());
    }
    file_buffers_[path] = std::move(buffer);

    // Extract display name from path
    const std::string display_name = std::filesystem::path(path).filename().string();

    // Add tab
    if (tab_bar_ != nullptr)
    {
        tab_bar_->AddTab(path, display_name);
    }

    // Load content into editor
    active_file_path_ = path;
    if (split_view_ != nullptr)
    {
        auto* editor = split_view_->GetEditorPanel();
        if (editor != nullptr)
        {
            editor->SetContent(content);
            editor->ClearModified();
            // R2 Fix 11: Editor gains focus after opening a file
            editor->SetFocus();
        }
    }

    // R2 Fix 13: Update status bar filename
    if (statusbar_panel_ != nullptr)
    {
        statusbar_panel_->set_filename(display_name);
        // R2 Fix 14: Language from extension
        const std::string ext = std::filesystem::path(path).extension().string();
        std::string language;
        if (ext == ".md" || ext == ".markdown" || ext == ".mdx")
            language = "Markdown";
        else if (ext == ".json")
            language = "JSON";
        else if (ext == ".cpp" || ext == ".cc" || ext == ".cxx")
            language = "C++";
        else if (ext == ".h" || ext == ".hpp" || ext == ".hxx")
            language = "C++ Header";
        else if (ext == ".c")
            language = "C";
        else if (ext == ".py")
            language = "Python";
        else if (ext == ".js")
            language = "JavaScript";
        else if (ext == ".ts")
            language = "TypeScript";
        else if (ext == ".html" || ext == ".htm")
            language = "HTML";
        else if (ext == ".css")
            language = "CSS";
        else if (ext == ".yaml" || ext == ".yml")
            language = "YAML";
        else if (ext == ".xml")
            language = "XML";
        else if (ext == ".txt")
            language = "Plain Text";
        else if (ext == ".sh" || ext == ".zsh" || ext == ".bash")
            language = "Shell";
        else if (ext == ".cmake")
            language = "CMake";
        else
            language = ext.empty() ? "Plain Text" : ext.substr(1);
        statusbar_panel_->set_language(language);

        // R2 Fix 19: File size
        try
        {
            const auto file_size = std::filesystem::file_size(path);
            statusbar_panel_->set_file_size(file_size);
        }
        catch (const std::filesystem::filesystem_error& /*err*/)
        {
            statusbar_panel_->set_file_size(0);
        }
    }

    MARKAMP_LOG_INFO("Opened file in tab: {}", path);
}

void LayoutManager::CloseTab(const std::string& path)
{
    const auto buf_it = file_buffers_.find(path);
    if (buf_it == file_buffers_.end())
    {
        // Not in our buffers, just remove the tab
        if (tab_bar_ != nullptr)
        {
            tab_bar_->RemoveTab(path);
        }
        return;
    }

    // Check if modified — prompt user
    if (buf_it->second.is_modified)
    {
        const std::string display_name = std::filesystem::path(path).filename().string();
        const int result = wxMessageBox(
            wxString::Format("'%s' has unsaved changes. Save before closing?", display_name),
            "Unsaved Changes",
            wxYES_NO | wxCANCEL | wxICON_QUESTION,
            this);

        if (result == wxCANCEL)
        {
            return;
        }
        if (result == wxYES)
        {
            SaveFile(path);
        }
    }

    // Remove from buffer
    file_buffers_.erase(buf_it);

    // Remove tab (TabBar handles activating adjacent tab)
    if (tab_bar_ != nullptr)
    {
        tab_bar_->RemoveTab(path);
    }

    // Update active path
    if (active_file_path_ == path)
    {
        if (tab_bar_ != nullptr)
        {
            active_file_path_ = tab_bar_->GetActiveTabPath();
        }
        else
        {
            active_file_path_.clear();
        }

        // Load the new active file if any
        if (!active_file_path_.empty())
        {
            SwitchToTab(active_file_path_);
        }
        else if (split_view_ != nullptr)
        {
            // Fix 12: Show empty-state placeholder when last tab closes
            auto* editor = split_view_->GetEditorPanel();
            if (editor != nullptr)
            {
                editor->SetContent("");
                editor->ClearModified();
            }

            // R2 Fix 12: Return to startup screen when all tabs close
            core::events::ShowStartupRequestEvent startup_evt;
            event_bus_.publish(startup_evt);
        }
    }

    MARKAMP_LOG_INFO("Closed tab: {}", path);
}

void LayoutManager::SwitchToTab(const std::string& path)
{
    if (path == active_file_path_)
    {
        return;
    }

    // Save current editor state
    if (!active_file_path_.empty() && split_view_ != nullptr)
    {
        auto buf_it = file_buffers_.find(active_file_path_);
        if (buf_it != file_buffers_.end())
        {
            auto* editor = split_view_->GetEditorPanel();
            if (editor != nullptr)
            {
                buf_it->second.content = editor->GetContent();
                auto session = editor->GetSessionState();
                buf_it->second.cursor_position = session.cursor_position;
                buf_it->second.first_visible_line = session.first_visible_line;
            }
        }
    }

    // Load target file from buffer
    const auto buf_it = file_buffers_.find(path);
    if (buf_it == file_buffers_.end())
    {
        MARKAMP_LOG_WARN("SwitchToTab: file not in buffer: {}", path);
        return;
    }

    active_file_path_ = path;

    // Update tab bar
    if (tab_bar_ != nullptr)
    {
        tab_bar_->SetActiveTab(path);
    }

    // Load content
    if (split_view_ != nullptr)
    {
        auto* editor = split_view_->GetEditorPanel();
        if (editor != nullptr)
        {
            editor->SetContent(buf_it->second.content);
            EditorPanel::SessionState restore_state;
            restore_state.cursor_position = buf_it->second.cursor_position;
            restore_state.first_visible_line = buf_it->second.first_visible_line;
            editor->RestoreSessionState(restore_state);

            if (!buf_it->second.is_modified)
            {
                editor->ClearModified();
            }

            // R2 Fix 10: Editor gains focus after tab switch
            editor->SetFocus();
        }
    }

    // Fix 7: Sync file tree selection with active tab
    if (file_tree_ != nullptr)
    {
        file_tree_->SetActiveFileId(path);
    }

    // Fix 13: Publish content changed event to refresh preview panel
    core::events::EditorContentChangedEvent content_evt;
    content_evt.content = buf_it->second.content;
    event_bus_.publish(content_evt);

    // Fix 16: Update status bar cursor position on tab switch
    if (statusbar_panel_ != nullptr)
    {
        statusbar_panel_->set_cursor_position(buf_it->second.first_visible_line + 1, 1);

        // R2 Fix 13: Update filename in status bar
        const std::string display_name = std::filesystem::path(path).filename().string();
        statusbar_panel_->set_filename(display_name);

        // R2 Fix 14: Update language in status bar
        const std::string ext = std::filesystem::path(path).extension().string();
        std::string language;
        if (ext == ".md" || ext == ".markdown" || ext == ".mdx")
            language = "Markdown";
        else if (ext == ".json")
            language = "JSON";
        else if (ext == ".cpp" || ext == ".cc" || ext == ".cxx")
            language = "C++";
        else if (ext == ".h" || ext == ".hpp" || ext == ".hxx")
            language = "C++ Header";
        else if (ext == ".c")
            language = "C";
        else if (ext == ".py")
            language = "Python";
        else if (ext == ".js")
            language = "JavaScript";
        else if (ext == ".ts")
            language = "TypeScript";
        else if (ext == ".html" || ext == ".htm")
            language = "HTML";
        else if (ext == ".css")
            language = "CSS";
        else if (ext == ".yaml" || ext == ".yml")
            language = "YAML";
        else if (ext == ".xml")
            language = "XML";
        else if (ext == ".txt")
            language = "Plain Text";
        else if (ext == ".sh" || ext == ".zsh" || ext == ".bash")
            language = "Shell";
        else if (ext == ".cmake")
            language = "CMake";
        else
            language = ext.empty() ? "Plain Text" : ext.substr(1);
        statusbar_panel_->set_language(language);

        // R2 Fix 19: Update file size in status bar
        try
        {
            const auto file_size = std::filesystem::file_size(path);
            statusbar_panel_->set_file_size(file_size);
        }
        catch (const std::filesystem::filesystem_error& /*err*/)
        {
            statusbar_panel_->set_file_size(0);
        }
    }

    MARKAMP_LOG_DEBUG("Switched to tab: {}", path);
}

void LayoutManager::SaveActiveFile()
{
    if (!active_file_path_.empty())
    {
        SaveFile(active_file_path_);

        // Mark as not modified
        auto buf_it = file_buffers_.find(active_file_path_);
        if (buf_it != file_buffers_.end())
        {
            buf_it->second.is_modified = false;
            try
            {
                buf_it->second.last_write_time =
                    std::filesystem::last_write_time(active_file_path_);
            }
            catch (const std::filesystem::filesystem_error& /*ex*/)
            {
            }
        }
        if (tab_bar_ != nullptr)
        {
            tab_bar_->SetTabModified(active_file_path_, false);
        }
    }
}

void LayoutManager::SaveActiveFileAs()
{
    wxFileDialog dialog(this,
                        "Save As",
                        wxEmptyString,
                        wxEmptyString,
                        "Markdown files (*.md)|*.md|All files (*.*)|*.*",
                        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    const std::string new_path = dialog.GetPath().ToStdString();

    // Save content to new path
    if (split_view_ != nullptr)
    {
        split_view_->SaveFile(new_path);
    }

    // Update buffer
    if (!active_file_path_.empty())
    {
        auto buf_it = file_buffers_.find(active_file_path_);
        if (buf_it != file_buffers_.end())
        {
            FileBuffer new_buf = std::move(buf_it->second);
            new_buf.is_modified = false;
            file_buffers_.erase(buf_it);
            file_buffers_[new_path] = std::move(new_buf);
        }

        // Update tab
        if (tab_bar_ != nullptr)
        {
            const std::string display_name = std::filesystem::path(new_path).filename().string();
            tab_bar_->RenameTab(active_file_path_, new_path, display_name);
            tab_bar_->SetTabModified(new_path, false);
        }

        active_file_path_ = new_path;
    }
}

auto LayoutManager::GetActiveFilePath() const -> std::string
{
    return active_file_path_;
}

auto LayoutManager::GetTabBar() -> TabBar*
{
    return tab_bar_;
}

// --- Auto-save ---

void LayoutManager::StartAutoSave()
{
    auto_save_timer_.SetOwner(this);
    auto_save_timer_.Start(kAutoSaveIntervalMs);
    MARKAMP_LOG_INFO("Auto-save started (interval={}ms)", kAutoSaveIntervalMs);
}

void LayoutManager::StopAutoSave()
{
    auto_save_timer_.Stop();
    MARKAMP_LOG_INFO("Auto-save stopped");
}

void LayoutManager::OnAutoSaveTimer(wxTimerEvent& /*event*/)
{
    for (auto& [path, buffer] : file_buffers_)
    {
        if (buffer.is_modified)
        {
            const std::string draft_path = path + ".markamp-draft";
            try
            {
                std::ofstream draft(draft_path);
                if (draft.is_open())
                {
                    draft << buffer.content;
                    MARKAMP_LOG_DEBUG("Auto-saved draft: {}", draft_path);
                }
            }
            catch (const std::exception& ex)
            {
                MARKAMP_LOG_WARN("Auto-save failed for {}: {}", path, ex.what());
            }
        }
    }
}

// --- External file change detection ---

void LayoutManager::CheckExternalFileChanges()
{
    if (active_file_path_.empty())
    {
        return;
    }

    auto buf_it = file_buffers_.find(active_file_path_);
    if (buf_it == file_buffers_.end())
    {
        return;
    }

    try
    {
        const auto current_write_time = std::filesystem::last_write_time(active_file_path_);
        if (current_write_time > buf_it->second.last_write_time)
        {
            const std::string display_name =
                std::filesystem::path(active_file_path_).filename().string();
            const int result = wxMessageBox(
                wxString::Format("'%s' has been modified externally. Reload?", display_name),
                "File Changed",
                wxYES_NO | wxICON_QUESTION,
                this);

            if (result == wxYES)
            {
                // Re-read file
                std::ifstream file_stream(active_file_path_);
                if (file_stream.is_open())
                {
                    std::string content((std::istreambuf_iterator<char>(file_stream)),
                                        std::istreambuf_iterator<char>());

                    buf_it->second.content = content;
                    buf_it->second.is_modified = false;
                    buf_it->second.last_write_time = current_write_time;

                    if (split_view_ != nullptr)
                    {
                        auto* editor = split_view_->GetEditorPanel();
                        if (editor != nullptr)
                        {
                            editor->SetContent(content);
                            editor->ClearModified();
                        }
                    }

                    if (tab_bar_ != nullptr)
                    {
                        tab_bar_->SetTabModified(active_file_path_, false);
                    }

                    MARKAMP_LOG_INFO("Reloaded file from disk: {}", active_file_path_);
                }
            }
            else
            {
                // User declined — update timestamp to avoid re-prompting
                buf_it->second.last_write_time = current_write_time;
            }
        }
    }
    catch (const std::filesystem::filesystem_error& ex)
    {
        MARKAMP_LOG_WARN("Error checking file changes: {}", ex.what());
    }
}

auto LayoutManager::HasUnsavedFiles() const -> bool
{
    for (const auto& [path, buffer] : file_buffers_)
    {
        if (buffer.is_modified)
        {
            return true;
        }
    }
    return false;
}

auto LayoutManager::GetOpenFileCount() const -> size_t
{
    return file_buffers_.size();
}

// R2 Fix 15: Save all modified files
void LayoutManager::SaveAllFiles()
{
    for (auto& [path, buffer] : file_buffers_)
    {
        if (buffer.is_modified)
        {
            SaveFile(path);
        }
    }
}

// R2 Fix 16: Revert active file to on-disk content
void LayoutManager::RevertActiveFile()
{
    if (active_file_path_.empty())
    {
        return;
    }

    auto buf_it = file_buffers_.find(active_file_path_);
    if (buf_it == file_buffers_.end())
    {
        return;
    }

    // Re-read from disk
    try
    {
        std::ifstream file_stream(active_file_path_);
        if (!file_stream.is_open())
        {
            return;
        }
        std::string content;
        content.assign(std::istreambuf_iterator<char>(file_stream),
                       std::istreambuf_iterator<char>());

        buf_it->second.content = content;
        buf_it->second.is_modified = false;

        // Reload into editor
        if (split_view_ != nullptr)
        {
            auto* editor = split_view_->GetEditorPanel();
            if (editor != nullptr)
            {
                editor->SetContent(content);
                editor->ClearModified();
            }
        }

        // Update tab modified state
        if (tab_bar_ != nullptr)
        {
            tab_bar_->SetTabModified(active_file_path_, false);
        }
    }
    catch (const std::exception& ex)
    {
        MARKAMP_LOG_ERROR("Error reverting file {}: {}", active_file_path_, ex.what());
    }
}

// R2 Fix 17: Close all open tabs
void LayoutManager::CloseAllTabs()
{
    if (tab_bar_ == nullptr)
    {
        return;
    }

    // Copy paths because CloseTab mutates the container
    const auto all_paths = tab_bar_->GetAllTabPaths();
    for (const auto& path : all_paths)
    {
        CloseTab(path);
    }
}

} // namespace markamp::ui
