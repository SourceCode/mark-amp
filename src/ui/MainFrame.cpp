#include "MainFrame.h"

#include "CommandPalette.h"
#include "LayoutManager.h"
#include "ShortcutOverlay.h"
#include "StartupPanel.h"
#include "StatusBarPanel.h"
#include "TabBar.h"
#include "app/MarkAmpApp.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/Logger.h"
#include "core/ShortcutManager.h"
#include "core/ThemeEngine.h"

#include <wx/aboutdlg.h>
#include <wx/display.h>
#include <wx/filedlg.h>
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
                     markamp::core::RecentWorkspaces* recent_workspaces,
                     markamp::platform::PlatformAbstraction* platform,
                     markamp::core::ThemeEngine* theme_engine)
    : wxFrame(
          nullptr, wxID_ANY, title, pos, size, wxBORDER_NONE | wxRESIZE_BORDER | wxCLIP_CHILDREN)
    , event_bus_(event_bus)
    , config_(config)
    , recent_workspaces_(recent_workspaces)
    , platform_(platform)
    , theme_engine_(theme_engine)
    , shortcut_manager_(*event_bus)
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

    // Default to Startup Screen unless arguments are parsed (logic to be added via MarkAmpApp)
    // For now, we initialize both but show based on logic.
    // Actually, good practice: don't create LayoutManager yet if not needed?
    // User wants: Startup Screen -> Selection -> Editor.

    startup_panel_ = new StartupPanel(this, event_bus_, recent_workspaces_, theme_engine_);
    sizer->Add(startup_panel_, 1, wxEXPAND);

    // We'll create LayoutManager on demand or hide it?
    // Better: create it hidden? Or create on Open.
    // Let's create it hidden for smoother transition if possible, or just create on demand.
    // To keep it simple: Create it hidden.

    if (theme_engine_ != nullptr && event_bus_ != nullptr)
    {
        layout_ = new LayoutManager(this, *theme_engine_, *event_bus_, config_);
        sizer->Add(layout_, 1, wxEXPAND);
        layout_->Hide(); // Hidden by default
    }

    // Determine initial view
    // TODO: if command line args has file, showEditor(); else showStartupScreen();
    showStartupScreen(); // Default for now

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

    // Phase 7: Command Palette & Keyboard UX
    if (theme_engine_ != nullptr)
    {
        command_palette_ = new CommandPalette(this, *theme_engine_);
        shortcut_overlay_ = new ShortcutOverlay(this, *theme_engine_, shortcut_manager_);
    }
    RegisterDefaultShortcuts();
    shortcut_manager_.load_keybindings(core::Config::config_directory());
    RegisterPaletteCommands();

    // Accelerator: Cmd+Shift+P → Command Palette
    wxAcceleratorEntry accel_entries[2];
    accel_entries[0].Set(wxACCEL_CMD | wxACCEL_SHIFT, 'P', wxID_HIGHEST + 100);
    accel_entries[1].Set(wxACCEL_NORMAL, WXK_F1, wxID_HIGHEST + 101);
    wxAcceleratorTable accel_table(2, accel_entries);
    SetAcceleratorTable(accel_table);

    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt) { ShowCommandPalette(); },
        wxID_HIGHEST + 100);
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt) { ToggleShortcutOverlay(); },
        wxID_HIGHEST + 101);

    MARKAMP_LOG_INFO("MainFrame created: {}x{} (frameless)", size.GetWidth(), size.GetHeight());
}

void MainFrame::onClose(wxCloseEvent& event)
{
    MARKAMP_LOG_INFO("MainFrame closing.");
    // Save keybindings before closing
    shortcut_manager_.save_keybindings(core::Config::config_directory());

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
    config_->set("last_open_file", last_active_file_);

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

    // Restore last open file (Session Restore)
    std::string last_file = config_->get_string("last_open_file", "");
    if (!last_file.empty() && event_bus_ != nullptr)
    {
        core::events::ActiveFileChangedEvent evt;
        evt.file_id = last_file;
        event_bus_->publish(evt);
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
    kMenuOpenFolder = wxID_HIGHEST + 10,
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
    kMenuToggleZenMode,
    kMenuThemeGallery,
    kMenuFullscreen
};

// (Moved logic to end of file to fix redefinition and structure)

void MainFrame::createMenuBar()
{
    auto* menu_bar = new wxMenuBar();

    // --- File menu ---
    auto* fileMenu = new wxMenu;
    fileMenu->Append(wxID_NEW, "&New\tCtrl+N");
    fileMenu->Append(wxID_OPEN, "&Open Folder...\tCtrl+O");

    auto* recentMenu = new wxMenu;
    fileMenu->AppendSubMenu(recentMenu, "Open &Recent");

    fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt+F4");
    menu_bar->Append(fileMenu, "&File");

    // Bind events
    Bind(wxEVT_MENU, &MainFrame::onOpenFolder, this, wxID_OPEN);
    Bind(
        wxEVT_MENU, [this](wxCommandEvent& /*event*/) { Close(true); }, wxID_EXIT);

    // Initial population of recent menu
    rebuildRecentMenu();

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
    view_menu->AppendCheckItem(kMenuToggleZenMode, "Toggle &Zen Mode\tCtrl+K");
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

    Bind(wxEVT_MENU, &MainFrame::onOpenFolder, this, kMenuOpenFolder);
    Bind(wxEVT_MENU, &MainFrame::onSave, this, kMenuSave);

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

        Bind(
            wxEVT_MENU,
            [this]([[maybe_unused]] wxCommandEvent& evt) { toggleZenMode(); },
            kMenuToggleZenMode);
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

    // --- Startup Events ---
    if (event_bus_ != nullptr)
    {
        subscriptions_.push_back(event_bus_->subscribe<core::events::OpenFolderRequestEvent>(
            [this](const core::events::OpenFolderRequestEvent& evt)
            {
                if (evt.path.empty())
                {
                    // Trigger standard open folder dialog logic
                    wxCommandEvent dummy;
                    onOpenFolder(dummy);
                    // onOpenFolder handles the UI switch if successful
                }
                else
                {
                    // Direct open
                    std::vector<core::FileNode> nodes;
                    scanDirectory(evt.path, nodes);
                    if (layout_ != nullptr)
                    {
                        layout_->setFileTree(nodes);
                        showEditor();
                        // Add to recent workspaces
                        if (recent_workspaces_ != nullptr)
                        {
                            recent_workspaces_->add(evt.path);
                        }
                    }
                }
            }));

        subscriptions_.push_back(event_bus_->subscribe<core::events::WorkspaceOpenRequestEvent>(
            [this](const core::events::WorkspaceOpenRequestEvent& evt)
            {
                std::vector<core::FileNode> nodes;
                scanDirectory(evt.path, nodes);
                if (layout_ != nullptr)
                {
                    layout_->setFileTree(nodes);
                    showEditor();
                    // Add/Bump in recent workspaces
                    if (recent_workspaces_ != nullptr)
                    {
                        recent_workspaces_->add(evt.path);
                    }
                }
            }));

        subscriptions_.push_back(event_bus_->subscribe<core::events::ActiveFileChangedEvent>(
            [this](const core::events::ActiveFileChangedEvent& evt)
            {
                last_active_file_ = evt.file_id;
                // Update Title Bar (Item 19)
                if (!evt.file_id.empty())
                {
                    wxFileName fname(evt.file_id);
                    SetTitle(fname.GetFullName() + " - MarkAmp");
                }
                else
                {
                    SetTitle("MarkAmp");
                }
            }));
    }
}

// --- Folder opening ---

void MainFrame::onOpenFolder(wxCommandEvent& /*event*/)
{
    wxDirDialog dlg(this, "Open Folder", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dlg.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    wxString path = dlg.GetPath();
    MARKAMP_LOG_INFO("Opening folder: {}", path.ToStdString());

    std::vector<core::FileNode> nodes;
    scanDirectory(path.ToStdString(), nodes);

    if (layout_ != nullptr)
    {
        layout_->setFileTree(nodes);
        showEditor();
    }

    if (recent_workspaces_ != nullptr)
    {
        recent_workspaces_->add(path.ToStdString());
    }
}

void MainFrame::scanDirectory(const std::string& path, std::vector<core::FileNode>& out_nodes)
{
    namespace fs = std::filesystem;

    try
    {
        if (!fs::exists(path) || !fs::is_directory(path))
        {
            return;
        }

        // Separate folders and files for sorting
        std::vector<core::FileNode> folders;
        std::vector<core::FileNode> files;

        for (const auto& entry : fs::directory_iterator(path))
        {
            const auto& p = entry.path();
            std::string name = p.filename().string();

            // Skip hidden files/folders (starting with dot)
            if (name.empty() || name[0] == '.')
            {
                continue;
            }

            core::FileNode node;
            node.id = p.string();
            node.name = name;
            node.is_open = false; // Collapsed by default

            if (entry.is_directory())
            {
                // Recursively scan directories
                scanDirectory(p.string(), node.children);
                folders.push_back(node);
            }
            else if (entry.is_regular_file())
            {
                // Filter for Markdown-related files
                std::string ext = p.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                if (ext == ".md" || ext == ".markdown" || ext == ".txt")
                {
                    files.push_back(node);
                }
            }
        }

        // Sort: folders first, then files, both alphabetically
        auto sort_func = [](const core::FileNode& a, const core::FileNode& b)
        {
            // Case-insensitive sort
            std::string sa = a.name;
            std::string sb = b.name;
            std::transform(sa.begin(), sa.end(), sa.begin(), ::tolower);
            std::transform(sb.begin(), sb.end(), sb.begin(), ::tolower);
            return sa < sb;
        };

        std::sort(folders.begin(), folders.end(), sort_func);
        std::sort(files.begin(), files.end(), sort_func);

        // Merge
        out_nodes.reserve(out_nodes.size() + folders.size() + files.size());
        out_nodes.insert(out_nodes.end(), folders.begin(), folders.end());
        out_nodes.insert(out_nodes.end(), files.begin(), files.end());
    }
    catch (const std::exception& e)
    {
        MARKAMP_LOG_ERROR("Failed to scan directory {}: {}", path, e.what());
    }
}

void MainFrame::showStartupScreen()
{
    if (layout_ != nullptr)
    {
        layout_->Hide();
    }
    if (startup_panel_ != nullptr)
    {
        startup_panel_->Show();
        startup_panel_->refreshRecentWorkspaces();
    }
    Layout();
    updateMenuBarForStartup();
}

void MainFrame::showEditor()
{
    if (startup_panel_ != nullptr)
    {
        startup_panel_->Hide();
    }
    if (layout_ != nullptr)
    {
        layout_->Show();
    }
    Layout();
    updateMenuBarForStartup();
}

void MainFrame::updateMenuBarForStartup()
{
    rebuildRecentMenu();
    // Disable editor-specific menus (e.g. Save, Close Editor)
    auto* menuBar = GetMenuBar();
    if (menuBar == nullptr)
    {
        return;
    }

    // Example: Disable "Save" (id: kMenuSave)
    // We need to find the menu item by ID.
    // Assuming standard IDs or ones defined in MainFrame.h
    // For now, let's just log. Real implementation depends on Menu IDs being accessible.
    // menuBar->Enable(kMenuSave, false);
    // menuBar->Enable(kMenuCloseEditor, false);
    MARKAMP_LOG_DEBUG("MainFrame: Menu bar updated for Startup Screen");
}

void MainFrame::updateMenuBarForEditor()
{
    rebuildRecentMenu();
    // Re-enable editor menus
    auto* menuBar = GetMenuBar();
    if (menuBar == nullptr)
    {
        return;
    }

    // menuBar->Enable(kMenuSave, true);
    // menuBar->Enable(kMenuCloseEditor, true);
    MARKAMP_LOG_DEBUG("MainFrame: Menu bar updated for Editor View");
}

void MainFrame::rebuildRecentMenu()
{
    auto* menuBar = GetMenuBar();
    if (menuBar == nullptr)
        return;

    // Find "File" menu
    int fileMenuIdx = menuBar->FindMenu("File");
    if (fileMenuIdx == wxNOT_FOUND)
        return;

    wxMenu* fileMenu = menuBar->GetMenu(static_cast<size_t>(fileMenuIdx));
    if (fileMenu == nullptr)
        return;

    wxMenu* recentMenu = nullptr;
    for (size_t i = 0; i < fileMenu->GetMenuItemCount(); ++i)
    {
        wxMenuItem* item = fileMenu->FindItemByPosition(i);
        if (item && item->IsSubMenu() && item->GetItemLabelText() == "Open Recent")
        {
            recentMenu = item->GetSubMenu();
            break;
        }
    }

    if (!recentMenu)
        return;

    // Clear existing items
    // wxMenu doesn't have DestroyChildren, we must loop and delete
    while (recentMenu->GetMenuItemCount() > 0)
    {
        recentMenu->Destroy(recentMenu->FindItemByPosition(0));
    }

    // Populate with new recent workspaces
    if (recent_workspaces_ != nullptr)
    {
        const auto& recent = recent_workspaces_->list();
        if (recent.empty())
        {
            auto* item = recentMenu->Append(wxID_ANY, "(No recent folders)", "");
            item->Enable(false);
        }
        else
        {
            int id = kMenuOpenRecentBase;
            for (const auto& path : recent)
            {
                recentMenu->Append(id, path.string());
                Bind(
                    wxEVT_MENU,
                    [this, path](wxCommandEvent&)
                    {
                        core::events::WorkspaceOpenRequestEvent evt;
                        evt.path = path.string();
                        if (event_bus_)
                            event_bus_->publish(evt);
                    },
                    id);
                id++;
            }
        }
    }
}

void MainFrame::onSave(wxCommandEvent& /*event*/)
{
    if (layout_ == nullptr)
    {
        return;
    }

    // If no active file, prompt for Save As
    if (last_active_file_.empty())
    {
        wxFileDialog dlg(this,
                         "Save As",
                         "",
                         "Untitled.md",
                         "Markdown files (*.md)|*.md|All files (*.*)|*.*",
                         wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

        if (dlg.ShowModal() == wxID_CANCEL)
        {
            return;
        }

        last_active_file_ = dlg.GetPath().ToStdString();

        // Update Title (Item 19) - or wait for event?
        // Let's set it now for responsiveness, event will confirm later
        SetTitle(dlg.GetFilename() + " - MarkAmp");

        // Notify others that file changed?
        // For now just save to it.
    }

    layout_->SaveFile(last_active_file_);
}

void MainFrame::toggleZenMode()
{
    zen_mode_ = !zen_mode_;

    if (layout_)
    {
        // Hide sidebar in zen mode
        layout_->set_sidebar_visible(!zen_mode_);

        // Hide status bar in zen mode
        auto* status_bar = layout_->statusbar_container();
        if (status_bar)
        {
            status_bar->Show(!zen_mode_);
            layout_->Layout();
        }
    }

    // Update menu state
    auto* menu_bar = GetMenuBar();
    if (menu_bar)
    {
        // Update View -> Zen Mode check
        // We need to find the item ID.
        // wxMenuBar doesn't have FindItem(id) directly on top level, but FindItem(id) works if we
        // have the menu or recursively. wxFrame::FindItemInMenuBar exists? No. But we can just use
        // FindItem.
        wxMenuItem* item = menu_bar->FindItem(kMenuToggleZenMode);
        if (item)
        {
            item->Check(zen_mode_);
        }

        wxMenuItem* sidebar_item = menu_bar->FindItem(kMenuToggleSidebar);
        if (sidebar_item)
        {
            sidebar_item->Check(!zen_mode_);
        }
    }

    // Optional: Enter/Exit fullscreen for true Zen
    // ShowFullScreen(zen_mode_, wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION);
    // For now, let's keep it strictly UI element toggling as requested.
}

// ═══════════════════════════════════════════════════════
// Phase 7A: Command Palette Registration
// ═══════════════════════════════════════════════════════

void MainFrame::RegisterDefaultShortcuts()
{
    using core::Shortcut;
    constexpr int kCmd = wxMOD_CMD;
    constexpr int kCmdShift = wxMOD_CMD | wxMOD_SHIFT;

    // File shortcuts
    shortcut_manager_.register_shortcut(
        {"file.open", "Open Folder", 'O', kCmd, "global", "File", {}});
    shortcut_manager_.register_shortcut({"file.save", "Save", 'S', kCmd, "global", "File", {}});
    shortcut_manager_.register_shortcut({"file.new", "New File", 'N', kCmd, "global", "File", {}});

    // View shortcuts
    shortcut_manager_.register_shortcut(
        {"view.editor", "Editor Mode", '1', kCmd, "global", "View", {}});
    shortcut_manager_.register_shortcut(
        {"view.split", "Split Mode", '2', kCmd, "global", "View", {}});
    shortcut_manager_.register_shortcut(
        {"view.preview", "Preview Mode", '3', kCmd, "global", "View", {}});
    shortcut_manager_.register_shortcut(
        {"view.sidebar", "Toggle Sidebar", 'B', kCmd, "global", "View", {}});
    shortcut_manager_.register_shortcut(
        {"view.zen", "Toggle Zen Mode", 'K', kCmd, "global", "View", {}});
    shortcut_manager_.register_shortcut(
        {"view.fullscreen", "Toggle Fullscreen", WXK_F11, wxMOD_NONE, "global", "View", {}});

    // Edit shortcuts
    shortcut_manager_.register_shortcut({"edit.undo", "Undo", 'Z', kCmd, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut(
        {"edit.redo", "Redo", 'Z', kCmdShift, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut({"edit.find", "Find", 'F', kCmd, "editor", "Edit", {}});

    // Markdown formatting (editor context)
    shortcut_manager_.register_shortcut({"md.bold", "Bold", 'B', kCmd, "editor", "Markdown", {}});
    shortcut_manager_.register_shortcut(
        {"md.italic", "Italic", 'I', kCmd, "editor", "Markdown", {}});
    shortcut_manager_.register_shortcut(
        {"md.code", "Inline Code", 'E', kCmd, "editor", "Markdown", {}});
    shortcut_manager_.register_shortcut(
        {"md.link", "Insert Link", 'K', kCmdShift, "editor", "Markdown", {}});

    // Tools
    shortcut_manager_.register_shortcut(
        {"tools.palette", "Command Palette", 'P', kCmdShift, "global", "Tools", {}});
    shortcut_manager_.register_shortcut(
        {"tools.shortcuts", "Keyboard Shortcuts", WXK_F1, wxMOD_NONE, "global", "Tools", {}});

    // Tab management shortcuts (QoL features 9-10)
    shortcut_manager_.register_shortcut(
        {"tab.close", "Close Tab", 'W', kCmd, "global", "File", {}});
    shortcut_manager_.register_shortcut(
        {"tab.next", "Next Tab", WXK_TAB, wxMOD_CONTROL, "global", "Navigation", {}});
    shortcut_manager_.register_shortcut({"tab.prev",
                                         "Previous Tab",
                                         WXK_TAB,
                                         wxMOD_CONTROL | wxMOD_SHIFT,
                                         "global",
                                         "Navigation",
                                         {}});

    MARKAMP_LOG_DEBUG("Registered {} default shortcuts",
                      shortcut_manager_.get_all_shortcuts().size());
}

void MainFrame::RegisterPaletteCommands()
{
    if (command_palette_ == nullptr)
    {
        return;
    }

    command_palette_->ClearCommands();

    // ── File commands ──
    command_palette_->RegisterCommand({"Open Folder...",
                                       "File",
                                       shortcut_manager_.get_shortcut_text("file.open"),
                                       [this]()
                                       {
                                           wxCommandEvent dummy;
                                           onOpenFolder(dummy);
                                       }});
    command_palette_->RegisterCommand({"Save",
                                       "File",
                                       shortcut_manager_.get_shortcut_text("file.save"),
                                       [this]()
                                       {
                                           wxCommandEvent dummy;
                                           onSave(dummy);
                                       }});

    // ── View commands ──
    command_palette_->RegisterCommand({"Editor Mode",
                                       "View",
                                       shortcut_manager_.get_shortcut_text("view.editor"),
                                       [this]()
                                       {
                                           if (event_bus_ != nullptr)
                                           {
                                               core::events::ViewModeChangedEvent evt{
                                                   core::events::ViewMode::Editor};
                                               event_bus_->publish(evt);
                                           }
                                       }});
    command_palette_->RegisterCommand({"Split Mode",
                                       "View",
                                       shortcut_manager_.get_shortcut_text("view.split"),
                                       [this]()
                                       {
                                           if (event_bus_ != nullptr)
                                           {
                                               core::events::ViewModeChangedEvent evt{
                                                   core::events::ViewMode::Split};
                                               event_bus_->publish(evt);
                                           }
                                       }});
    command_palette_->RegisterCommand({"Preview Mode",
                                       "View",
                                       shortcut_manager_.get_shortcut_text("view.preview"),
                                       [this]()
                                       {
                                           if (event_bus_ != nullptr)
                                           {
                                               core::events::ViewModeChangedEvent evt{
                                                   core::events::ViewMode::Preview};
                                               event_bus_->publish(evt);
                                           }
                                       }});
    command_palette_->RegisterCommand({"Toggle Sidebar",
                                       "View",
                                       shortcut_manager_.get_shortcut_text("view.sidebar"),
                                       [this]()
                                       {
                                           if (event_bus_ != nullptr)
                                           {
                                               core::events::SidebarToggleEvent evt;
                                               evt.visible = true;
                                               event_bus_->publish(evt);
                                           }
                                       }});
    command_palette_->RegisterCommand(
        {"Toggle Zen Mode", "View", shortcut_manager_.get_shortcut_text("view.zen"), [this]() {
             toggleZenMode();
         }});
    command_palette_->RegisterCommand({"Toggle Fullscreen",
                                       "View",
                                       shortcut_manager_.get_shortcut_text("view.fullscreen"),
                                       [this]()
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
                                       }});
    command_palette_->RegisterCommand({"Toggle Minimap",
                                       "View",
                                       "",
                                       [this]()
                                       {
                                           if (layout_ != nullptr)
                                           {
                                               layout_->ToggleEditorMinimap();
                                           }
                                       }});

    // ── Theme commands ──
    command_palette_->RegisterCommand({"Theme Gallery...",
                                       "Theme",
                                       shortcut_manager_.get_shortcut_text("view.theme_gallery"),
                                       [this]()
                                       {
                                           if (event_bus_ != nullptr)
                                           {
                                               core::events::ThemeGalleryRequestEvent evt;
                                               event_bus_->publish(evt);
                                           }
                                       }});

    // ── Tools commands ──
    command_palette_->RegisterCommand({"Keyboard Shortcuts",
                                       "Tools",
                                       shortcut_manager_.get_shortcut_text("tools.shortcuts"),
                                       [this]() { ToggleShortcutOverlay(); }});

    MARKAMP_LOG_DEBUG("Registered {} palette commands",
                      shortcut_manager_.get_all_shortcuts().size());
}

void MainFrame::ShowCommandPalette()
{
    if (command_palette_ != nullptr)
    {
        command_palette_->ShowPalette();
    }
}

void MainFrame::ToggleShortcutOverlay()
{
    if (shortcut_overlay_ == nullptr)
    {
        return;
    }

    if (shortcut_overlay_->IsOverlayVisible())
    {
        shortcut_overlay_->HideOverlay();
    }
    else
    {
        shortcut_overlay_->ShowOverlay();
    }
}

// --- Tab Management (QoL features 9-11) ---

void MainFrame::onCloseActiveTab()
{
    if (layout_ == nullptr || event_bus_ == nullptr)
    {
        return;
    }

    const std::string active_path = layout_->GetActiveFilePath();
    if (!active_path.empty())
    {
        core::events::TabCloseRequestEvent evt(active_path);
        event_bus_->publish(evt);
    }
}

void MainFrame::onCycleTab(bool forward)
{
    if (layout_ == nullptr)
    {
        return;
    }

    auto* tab_bar = layout_->GetTabBar();
    if (tab_bar == nullptr)
    {
        return;
    }

    if (forward)
    {
        tab_bar->ActivateNextTab();
    }
    else
    {
        tab_bar->ActivatePreviousTab();
    }
}

void MainFrame::updateWindowTitle()
{
    if (layout_ == nullptr)
    {
        return;
    }

    const std::string active_path = layout_->GetActiveFilePath();
    if (active_path.empty())
    {
        SetTitle("MarkAmp");
        return;
    }

    const std::string filename = std::filesystem::path(active_path).filename().string();

    // Check if modified via tab bar
    bool is_modified = false;
    auto* tab_bar = layout_->GetTabBar();
    if (tab_bar != nullptr)
    {
        is_modified = tab_bar->IsTabModified(active_path);
    }

    if (is_modified)
    {
        SetTitle(wxString::Format("\u25CF %s - MarkAmp", filename));
    }
    else
    {
        SetTitle(wxString::Format("%s - MarkAmp", filename));
    }
}

} // namespace markamp::ui
