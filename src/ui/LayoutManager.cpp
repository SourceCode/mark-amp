#include "LayoutManager.h"

#include "FileTreeCtrl.h"
#include "SplitView.h"
#include "SplitterBar.h"
#include "StatusBarPanel.h"
#include "ThemeGallery.h"
#include "Toolbar.h"
#include "core/Config.h"
#include "core/Events.h"
#include "core/Logger.h"
#include "core/SampleFiles.h"

#include <wx/dcbuffer.h>
#include <wx/sizer.h>

#include <algorithm>
#include <cmath>

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

    // Bind animation timer
    Bind(wxEVT_TIMER, &LayoutManager::OnSidebarAnimTimer, this, sidebar_anim_timer_.GetId());

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
    sidebar_sizer->Add(header_panel, 0, wxEXPAND);

    // Spacer content area -> FileTreeCtrl
    file_tree_ = new FileTreeCtrl(sidebar_panel_, theme_engine(), event_bus_);
    sidebar_sizer->Add(file_tree_, 1, wxEXPAND);

    // Load sample file tree
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

    // Footer
    auto* footer_panel = new wxPanel(sidebar_panel_, wxID_ANY, wxDefaultPosition, wxSize(-1, 48));
    footer_panel->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgApp));
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
    sidebar_panel_->Refresh();
}

void LayoutManager::OnSidebarPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(sidebar_panel_);
    auto sz = sidebar_panel_->GetClientSize();
    int w = sz.GetWidth();
    int h = sz.GetHeight();

    // Background
    dc.SetBrush(theme_engine().brush(core::ThemeColorToken::BgPanel));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(sz);

    // Right border: 2px border_dark
    dc.SetPen(theme_engine().pen(core::ThemeColorToken::BorderDark, 2));
    dc.DrawLine(w - 1, 0, w - 1, h);

    // Left border: 1px border_light (bevel)
    dc.SetPen(theme_engine().pen(core::ThemeColorToken::BorderLight, 1));
    dc.DrawLine(0, 0, 0, h);
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

} // namespace markamp::ui
