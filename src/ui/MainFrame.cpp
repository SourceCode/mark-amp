#include "MainFrame.h"

#include "LayoutManager.h"
#include "app/MarkAmpApp.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/Logger.h"
#include "core/ThemeEngine.h"

#include <wx/aboutdlg.h>
#include <wx/display.h>
#include <wx/filename.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/stdpaths.h>

namespace markamp::ui
{

namespace
{

/// Locate the application icon PNG by searching common paths:
/// 1. macOS .app bundle Resources directory
/// 2. Relative to executable (build tree layout)
/// 3. Relative to project root (source tree layout)
wxString findIconPath()
{
    const wxString icon_filename = "markamp.png";

#ifdef __WXOSX__
    // In a .app bundle: Contents/Resources/icons/markamp.png
    wxString bundle_path = wxStandardPaths::Get().GetResourcesDir() + "/icons/" + icon_filename;
    if (wxFileName::FileExists(bundle_path))
    {
        return bundle_path;
    }
#endif

    // Relative to executable: ../resources/icons/markamp.png (build tree)
    wxFileName exe_path(wxStandardPaths::Get().GetExecutablePath());
    wxString exe_dir = exe_path.GetPath();

    // Build tree: build/debug/src/markamp → ../../resources/icons/
    wxString build_path = exe_dir + "/../../../resources/icons/" + icon_filename;
    if (wxFileName::FileExists(build_path))
    {
        return build_path;
    }

    // Also try: build/debug/resources/icons/ (if copied by CMake)
    wxString cmake_copy_path = exe_dir + "/../resources/icons/" + icon_filename;
    if (wxFileName::FileExists(cmake_copy_path))
    {
        return cmake_copy_path;
    }

    // Direct sibling: resources/icons/ (running from project root)
    wxString direct_path = "resources/icons/" + icon_filename;
    if (wxFileName::FileExists(direct_path))
    {
        return direct_path;
    }

    return {};
}

} // namespace

MainFrame::MainFrame(const wxString& title,
                     const wxPoint& pos,
                     const wxSize& size,
                     markamp::core::EventBus* event_bus,
                     markamp::core::Config* config,
                     markamp::platform::PlatformAbstraction* platform,
                     markamp::core::ThemeEngine* theme_engine)
    : wxFrame(
          nullptr, wxID_ANY, title, pos, size, wxBORDER_NONE | wxRESIZE_BORDER | wxCLIP_CHILDREN)
    , event_bus_(event_bus)
    , config_(config)
    , platform_(platform)
    , theme_engine_(theme_engine)
{
    // Minimum size constraints
    SetMinSize(wxSize(app::MarkAmpApp::kMinWidth, app::MarkAmpApp::kMinHeight));

    // Dark background (will be overridden by theme)
    SetBackgroundColour(wxColour(20, 20, 30));

    // Set application icon (window icon on Linux/Windows, dock icon on macOS)
    wxInitAllImageHandlers();
    wxString icon_path = findIconPath();
    if (!icon_path.empty())
    {
        wxImage icon_image(icon_path, wxBITMAP_TYPE_PNG);
        if (icon_image.IsOk())
        {
            wxIcon app_icon;
            app_icon.CopyFromBitmap(wxBitmap(icon_image));
            SetIcon(app_icon);
            MARKAMP_LOG_INFO("Application icon loaded from: {}", icon_path.ToStdString());
        }
        else
        {
            MARKAMP_LOG_WARN("Failed to load icon image: {}", icon_path.ToStdString());
        }
    }
    else
    {
        MARKAMP_LOG_WARN("Application icon not found");
    }

    // Apply platform-specific frameless window style
    if (platform_ != nullptr)
    {
        platform_->set_frameless_window_style(this);
    }

    // Create the application menu bar (required on macOS, useful on all platforms)
    createMenuBar();

    // Create the custom chrome title bar (with optional theme engine)
    chrome_ = new CustomChrome(this, event_bus_, platform_, theme_engine_);

    // Create the layout manager (sidebar + content + status bar)
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(chrome_, 0, wxEXPAND);

    if (theme_engine_ != nullptr && event_bus_ != nullptr)
    {
        layout_ = new LayoutManager(this, *theme_engine_, *event_bus_, config_);
        sizer->Add(layout_, 1, wxEXPAND);
    }
    else
    {
        // Fallback: plain placeholder content panel
        auto* contentPanel = new wxPanel(this);
        contentPanel->SetBackgroundColour(wxColour(25, 25, 35));
        sizer->Add(contentPanel, 1, wxEXPAND);
    }

    SetSizer(sizer);

    // Bind events
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::onClose, this);
    Bind(wxEVT_SIZE, &MainFrame::onSize, this);

    // Edge-resize mouse events on the frame itself
    Bind(wxEVT_MOTION, &MainFrame::onFrameMouseMove, this);
    Bind(wxEVT_LEFT_DOWN, &MainFrame::onFrameMouseDown, this);

    // Restore saved window state, then center if no saved state
    restoreWindowState();

    // Log DPI info
    logDpiInfo();

    MARKAMP_LOG_INFO("MainFrame created: {}x{} (frameless)", size.GetWidth(), size.GetHeight());
}

void MainFrame::onClose(wxCloseEvent& event)
{
    MARKAMP_LOG_INFO("MainFrame closing.");
    saveWindowState();
    Destroy();
    event.Skip();
}

void MainFrame::onSize(wxSizeEvent& event)
{
    // Update chrome maximize state
    if (chrome_ != nullptr && platform_ != nullptr)
    {
        chrome_->set_maximized(platform_->is_maximized(this));
    }
    event.Skip();
}

// --- Edge resize detection ---

auto MainFrame::detectResizeEdge(const wxPoint& pos) const -> markamp::platform::ResizeEdge
{
    using markamp::platform::ResizeEdge;

    auto sz = GetClientSize();
    const int w = sz.GetWidth();
    const int h = sz.GetHeight();

    const bool at_top = pos.y < kResizeZone;
    const bool at_bottom = pos.y > h - kResizeZone;
    const bool at_left = pos.x < kResizeZone;
    const bool at_right = pos.x > w - kResizeZone;

    // Corners (larger hit zone)
    const bool corner_top = pos.y < kCornerZone;
    const bool corner_bottom = pos.y > h - kCornerZone;
    const bool corner_left = pos.x < kCornerZone;
    const bool corner_right = pos.x > w - kCornerZone;

    if (corner_top && corner_left)
    {
        return ResizeEdge::TopLeft;
    }
    if (corner_top && corner_right)
    {
        return ResizeEdge::TopRight;
    }
    if (corner_bottom && corner_left)
    {
        return ResizeEdge::BottomLeft;
    }
    if (corner_bottom && corner_right)
    {
        return ResizeEdge::BottomRight;
    }
    if (at_top)
    {
        return ResizeEdge::Top;
    }
    if (at_bottom)
    {
        return ResizeEdge::Bottom;
    }
    if (at_left)
    {
        return ResizeEdge::Left;
    }
    if (at_right)
    {
        return ResizeEdge::Right;
    }

    return ResizeEdge::None;
}

void MainFrame::onFrameMouseMove(wxMouseEvent& event)
{
    using markamp::platform::ResizeEdge;

    auto edge = detectResizeEdge(event.GetPosition());

    switch (edge)
    {
        case ResizeEdge::Top:
        case ResizeEdge::Bottom:
            SetCursor(wxCursor(wxCURSOR_SIZENS));
            break;
        case ResizeEdge::Left:
        case ResizeEdge::Right:
            SetCursor(wxCursor(wxCURSOR_SIZEWE));
            break;
        case ResizeEdge::TopLeft:
        case ResizeEdge::BottomRight:
            SetCursor(wxCursor(wxCURSOR_SIZENWSE));
            break;
        case ResizeEdge::TopRight:
        case ResizeEdge::BottomLeft:
            SetCursor(wxCursor(wxCURSOR_SIZENESW));
            break;
        case ResizeEdge::None:
            SetCursor(wxNullCursor);
            break;
    }

    event.Skip();
}

void MainFrame::onFrameMouseDown(wxMouseEvent& event)
{
    using markamp::platform::ResizeEdge;

    auto edge = detectResizeEdge(event.GetPosition());
    if (edge != ResizeEdge::None && platform_ != nullptr)
    {
        platform_->begin_native_resize(this, edge);
        // macOS handles resize natively via wxRESIZE_BORDER, so no manual handling
    }

    event.Skip();
}

// --- Window state persistence ---

void MainFrame::saveWindowState()
{
    if (config_ == nullptr)
    {
        return;
    }

    auto pos = GetPosition();
    auto sz = GetSize();
    bool maximized = (platform_ != nullptr) ? platform_->is_maximized(this) : IsMaximized();

    config_->set("window_x", pos.x);
    config_->set("window_y", pos.y);
    config_->set("window_width", sz.GetWidth());
    config_->set("window_height", sz.GetHeight());
    config_->set("window_maximized", maximized);

    auto result = config_->save();
    if (!result)
    {
        MARKAMP_LOG_WARN("Failed to save window state: {}", result.error());
    }
}

void MainFrame::restoreWindowState()
{
    if (config_ == nullptr)
    {
        Centre();
        return;
    }

    int x = config_->get_int("window_x", -1);
    int y = config_->get_int("window_y", -1);
    int w = config_->get_int("window_width", 0);
    int h = config_->get_int("window_height", 0);
    bool maximized = config_->get_bool("window_maximized", false);

    if (w > 0 && h > 0 && x >= 0 && y >= 0)
    {
        // Validate that the saved position is still on a valid display
        wxRect restored_rect(x, y, w, h);
        bool on_screen = false;

        for (unsigned int i = 0; i < wxDisplay::GetCount(); ++i)
        {
            wxDisplay display(i);
            if (display.GetGeometry().Intersects(restored_rect))
            {
                on_screen = true;
                break;
            }
        }

        if (on_screen)
        {
            SetPosition(wxPoint(x, y));
            SetSize(wxSize(w, h));
            MARKAMP_LOG_DEBUG("Window state restored: {}x{} at ({},{})", w, h, x, y);
        }
        else
        {
            MARKAMP_LOG_WARN("Saved window position is off-screen, centering");
            Centre();
        }
    }
    else
    {
        Centre();
    }

    if (maximized && platform_ != nullptr)
    {
        platform_->toggle_maximize(this);
    }
}

void MainFrame::logDpiInfo()
{
    auto scaleFactor = GetDPIScaleFactor();
    auto dpi = GetDPI();
    auto displayIdx = wxDisplay::GetFromWindow(this);

    MARKAMP_LOG_INFO(
        "DPI info: scale={:.2f}, dpi={}x{}, display={}", scaleFactor, dpi.x, dpi.y, displayIdx);

    if (displayIdx != wxNOT_FOUND)
    {
        wxDisplay display(static_cast<unsigned int>(displayIdx));
        auto geometry = display.GetGeometry();
        MARKAMP_LOG_INFO("Display geometry: {}x{}", geometry.GetWidth(), geometry.GetHeight());
    }
}

// --- Menu bar ---

enum MenuId : int
{
    kMenuOpenFile = wxID_OPEN,
    kMenuSave = wxID_SAVE,
    kMenuSaveAs = wxID_SAVEAS,
    kMenuQuit = wxID_EXIT,
    kMenuUndo = wxID_UNDO,
    kMenuRedo = wxID_REDO,
    kMenuCut = wxID_CUT,
    kMenuCopy = wxID_COPY,
    kMenuPaste = wxID_PASTE,
    kMenuSelectAll = wxID_SELECTALL,
    kMenuAbout = wxID_ABOUT,
    // Custom IDs
    kMenuViewEditor = wxID_HIGHEST + 1,
    kMenuViewSplit,
    kMenuViewPreview,
    kMenuToggleSidebar,
    kMenuThemeGallery,
    kMenuFullscreen
};

void MainFrame::createMenuBar()
{
    auto* menu_bar = new wxMenuBar();

    // --- File menu ---
    auto* file_menu = new wxMenu();
    file_menu->Append(kMenuOpenFile, "&Open...\tCtrl+O");
    file_menu->AppendSeparator();
    file_menu->Append(kMenuSave, "&Save\tCtrl+S");
    file_menu->Append(kMenuSaveAs, "Save &As...\tCtrl+Shift+S");
    file_menu->AppendSeparator();
    file_menu->Append(kMenuQuit, "&Quit\tCtrl+Q");
    menu_bar->Append(file_menu, "&File");

    // --- Edit menu ---
    auto* edit_menu = new wxMenu();
    edit_menu->Append(kMenuUndo, "&Undo\tCtrl+Z");
    edit_menu->Append(kMenuRedo, "&Redo\tCtrl+Shift+Z");
    edit_menu->AppendSeparator();
    edit_menu->Append(kMenuCut, "Cu&t\tCtrl+X");
    edit_menu->Append(kMenuCopy, "&Copy\tCtrl+C");
    edit_menu->Append(kMenuPaste, "&Paste\tCtrl+V");
    edit_menu->AppendSeparator();
    edit_menu->Append(kMenuSelectAll, "Select &All\tCtrl+A");
    menu_bar->Append(edit_menu, "&Edit");

    // --- View menu ---
    auto* view_menu = new wxMenu();
    view_menu->AppendRadioItem(kMenuViewEditor, "&Editor Mode\tCtrl+1");
    view_menu->AppendRadioItem(kMenuViewSplit, "&Split Mode\tCtrl+2");
    view_menu->AppendRadioItem(kMenuViewPreview, "&Preview Mode\tCtrl+3");
    view_menu->Check(kMenuViewSplit, true);
    view_menu->AppendSeparator();
    view_menu->AppendCheckItem(kMenuToggleSidebar, "Toggle &Sidebar\tCtrl+B");
    view_menu->Check(kMenuToggleSidebar, true);
    view_menu->AppendSeparator();
    view_menu->Append(kMenuFullscreen, "Toggle &Fullscreen\tF11");
    menu_bar->Append(view_menu, "&View");

    // --- Window menu ---
    auto* window_menu = new wxMenu();
    window_menu->Append(kMenuThemeGallery, "Theme &Gallery...\tCtrl+T");
    menu_bar->Append(window_menu, "&Window");

    // --- Help menu ---
    auto* help_menu = new wxMenu();
    help_menu->Append(kMenuAbout, "&About MarkAmp");
    menu_bar->Append(help_menu, "&Help");

    SetMenuBar(menu_bar);

    // --- Bind menu events ---
    Bind(
        wxEVT_MENU, [this]([[maybe_unused]] wxCommandEvent& evt) { Close(); }, kMenuQuit);

    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            wxAboutDialogInfo info;
            info.SetName("MarkAmp");
            info.SetVersion("1.0.0");
            info.SetDescription("A cross-platform Markdown viewer and editor.");
            info.SetCopyright("(C) 2026 MarkAmp");
            wxAboutBox(info, this);
        },
        kMenuAbout);

    // View mode menu items → EventBus
    if (event_bus_ != nullptr)
    {
        Bind(
            wxEVT_MENU,
            [this]([[maybe_unused]] wxCommandEvent& evt)
            {
                core::events::ViewModeChangedEvent e{core::events::ViewMode::Editor};
                event_bus_->publish(e);
            },
            kMenuViewEditor);

        Bind(
            wxEVT_MENU,
            [this]([[maybe_unused]] wxCommandEvent& evt)
            {
                core::events::ViewModeChangedEvent e{core::events::ViewMode::Split};
                event_bus_->publish(e);
            },
            kMenuViewSplit);

        Bind(
            wxEVT_MENU,
            [this]([[maybe_unused]] wxCommandEvent& evt)
            {
                core::events::ViewModeChangedEvent e{core::events::ViewMode::Preview};
                event_bus_->publish(e);
            },
            kMenuViewPreview);

        Bind(
            wxEVT_MENU,
            [this]([[maybe_unused]] wxCommandEvent& evt)
            {
                core::events::SidebarToggleEvent e;
                e.visible = true; // Will be toggled by receiver
                event_bus_->publish(e);
            },
            kMenuToggleSidebar);
    }

    // Fullscreen toggle
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (IsFullScreen())
            {
                if (platform_ != nullptr)
                {
                    platform_->exit_fullscreen(this);
                }
                else
                {
                    ShowFullScreen(false);
                }
            }
            else
            {
                if (platform_ != nullptr)
                {
                    platform_->enter_fullscreen(this);
                }
                else
                {
                    ShowFullScreen(true);
                }
            }
        },
        kMenuFullscreen);

    MARKAMP_LOG_DEBUG("Menu bar created with File/Edit/View/Window/Help menus");
}

} // namespace markamp::ui
