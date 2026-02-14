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
#include <wx/dnd.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/html/htmprint.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/stdpaths.h>

#include <filesystem>

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
                     markamp::core::ThemeEngine* theme_engine,
                     markamp::core::FeatureRegistry* feature_registry,
                     markamp::core::IMermaidRenderer* mermaid_renderer,
                     markamp::core::IMathRenderer* math_renderer)
    : wxFrame(
          nullptr, wxID_ANY, title, pos, size, wxBORDER_NONE | wxRESIZE_BORDER | wxCLIP_CHILDREN)
    , event_bus_(event_bus)
    , config_(config)
    , recent_workspaces_(recent_workspaces)
    , platform_(platform)
    , theme_engine_(theme_engine)
    , feature_registry_(feature_registry)
    , mermaid_renderer_(mermaid_renderer)
    , math_renderer_(math_renderer)
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
        layout_ = new LayoutManager(this,
                                    *theme_engine_,
                                    *event_bus_,
                                    config_,
                                    feature_registry_,
                                    mermaid_renderer_,
                                    math_renderer_);
        sizer->Add(layout_, 1, wxEXPAND);
        layout_->Hide(); // Hidden by default
    }

    // Open file from command-line args, or show startup screen
    bool opened_from_args = false;
    if (wxTheApp->argc > 1)
    {
        std::string arg_path = wxTheApp->argv[1].ToStdString();
        if (std::filesystem::exists(arg_path))
        {
            showEditor();
            layout_->OpenFileInTab(arg_path);
            opened_from_args = true;
        }
    }
    if (!opened_from_args)
    {
        showStartupScreen();
    }

    SetSizer(sizer);

    // Bind events
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::onClose, this);
    Bind(wxEVT_SIZE, &MainFrame::onSize, this);

    // Edge-resize mouse events on the frame itself
    Bind(wxEVT_MOTION, &MainFrame::onFrameMouseMove, this);
    Bind(wxEVT_LEFT_DOWN, &MainFrame::onFrameMouseDown, this);

    // Fix 18: Enable drag-and-drop file opening
    class FileDropTarget : public wxFileDropTarget
    {
    public:
        explicit FileDropTarget(MainFrame* frame)
            : frame_(frame)
        {
        }
        auto OnDropFiles(wxCoord /*x_pos*/, wxCoord /*y_pos*/, const wxArrayString& filenames)
            -> bool override
        {
            if (frame_->layout_ == nullptr)
            {
                return false;
            }
            frame_->showEditor();
            for (const auto& file : filenames)
            {
                frame_->layout_->OpenFileInTab(file.ToStdString());
            }
            return true;
        }

    private:
        MainFrame* frame_;
    };
    SetDropTarget(new FileDropTarget(this));

    // Restore saved window state, then center if no saved state
    restoreWindowState();

    // Log DPI info
    logDpiInfo();

    // Phase 7: Command Palette & Keyboard UX
    if (theme_engine_ != nullptr && event_bus_ != nullptr)
    {
        command_palette_ = new CommandPalette(this, *theme_engine_, *event_bus_);
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

    // Fix 12: Prompt if unsaved files exist
    if (layout_ != nullptr && layout_->HasUnsavedFiles())
    {
        const int result = wxMessageBox("You have unsaved changes. Save all before closing?",
                                        "Unsaved Changes",
                                        wxYES_NO | wxCANCEL | wxICON_WARNING,
                                        this);

        if (result == wxCANCEL)
        {
            event.Veto();
            return;
        }
        if (result == wxYES)
        {
            layout_->SaveActiveFile();
        }
        // wxNO = discard and close
    }

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
    kMenuFullscreen,
    kMenuOpenFile, // Fix 15: dedicated ID for Open File
    kMenuCloseTab, // Fix 17: dedicated ID for Close Tab
    // R2 Fixes 15-17
    kMenuSaveAll,
    kMenuRevertFile,
    kMenuCloseAllTabs,
    // R3 Fixes
    kMenuCloseFolder, // R3 Fix 11
    kMenuGoToLine,    // R3 Fix 8
    // R6 Fixes
    kMenuFind,          // R6 Fix 1
    kMenuReplace,       // R6 Fix 1
    kMenuDuplicateLine, // R6 Fix 6
    kMenuToggleComment, // R6 Fix 7
    kMenuWordWrap,      // R6 Fix 10
    kMenuPrint,         // R6 Fix 18
    kMenuDeleteLine,    // R6 Fix 15
    kMenuClearRecent,   // R6 Fix 8
    // R7 Fixes
    kMenuMoveLineUp,           // R7 Fix 1
    kMenuMoveLineDown,         // R7 Fix 2
    kMenuJoinLines,            // R7 Fix 3
    kMenuTransformUpper,       // R7 Fix 4
    kMenuTransformLower,       // R7 Fix 5
    kMenuSelectAllOccurrences, // R7 Fix 6
    kMenuExpandLineSelection,  // R7 Fix 7
    kMenuTransformTitle,       // R7 Fix 8
    kMenuSortAsc,              // R7 Fix 9
    kMenuSortDesc,             // R7 Fix 9
    kMenuInsertLineAbove,      // R7 Fix 10
    kMenuInsertLineBelow,      // R7 Fix 11
    kMenuToggleLineNumbers,    // R7 Fix 12
    kMenuToggleWhitespace,     // R7 Fix 13
    kMenuFoldAll,              // R7 Fix 14
    kMenuUnfoldAll,            // R7 Fix 14
    kMenuWelcomeScreen,        // R7 Fix 20
    // R8 Fixes
    kMenuCopyLineUp,
    kMenuCopyLineDown,
    kMenuDeleteAllLeft,
    kMenuDeleteAllRight,
    kMenuReverseLines,
    kMenuDeleteDuplicateLines,
    kMenuTransposeChars,
    kMenuIndentSelection,
    kMenuOutdentSelection,
    kMenuSelectWord,
    kMenuSelectParagraph,
    kMenuToggleReadOnly,
    kMenuConvertIndentSpaces,
    kMenuConvertIndentTabs,
    kMenuJumpToBracket,
    kMenuToggleMinimap,
    kMenuFoldCurrent,
    kMenuUnfoldCurrent,
    kMenuAddLineComment,
    kMenuRemoveLineComment,
    // R9 Fixes
    kMenuTrimTrailingWS,
    kMenuExpandSelection,
    kMenuShrinkSelection,
    kMenuCursorUndo,
    kMenuCursorRedo,
    kMenuMoveTextLeft,
    kMenuMoveTextRight,
    kMenuToggleAutoIndent,
    kMenuToggleBracketMatching,
    kMenuToggleCodeFolding,
    kMenuToggleIndentGuides,
    kMenuSelectToBracket,
    kMenuToggleBlockComment,
    kMenuInsertDateTime,
    kMenuBold,
    kMenuItalic,
    kMenuInlineCode,
    kMenuBlockquote,
    kMenuCycleHeading,
    kMenuInsertTable,
    // R10 Fixes
    kMenuToggleSmartList,
    kMenuCloseOtherTabs,
    kMenuCloseSavedTabs,
    kMenuInsertLink,
    kMenuAddCursorBelow,
    kMenuAddCursorAbove,
    kMenuAddCursorNextOccurrence,
    kMenuRemoveSurroundingBrackets,
    kMenuDuplicateSelectionOrLine,
    kMenuShowTableEditor,
    kMenuToggleScrollBeyond,
    kMenuToggleHighlightLine,
    kMenuToggleAutoCloseBrackets,
    kMenuToggleStickyScroll,
    kMenuToggleFontLigatures,
    kMenuToggleSmoothCaret,
    kMenuToggleColorPreview,
    kMenuToggleEdgeRuler,
    kMenuEnsureFinalNewline,
    kMenuInsertSnippet,
    // R11 Fixes
    kMenuToggleSmoothScrolling,
    kMenuToggleTrailingWSHighlight,
    kMenuToggleAutoTrimWS,
    kMenuToggleGutterSeparator,
    kMenuToggleInsertFinalNewline,
    kMenuToggleWhitespaceBoundary,
    kMenuToggleLinkAutoComplete,
    kMenuToggleDragDrop,
    kMenuToggleAutoSave,
    kMenuToggleEmptySelClipboard,
    kMenuCycleRenderWhitespace,
    kMenuDeleteCurrentLine,
    kMenuCopyLineNoSel,
    kMenuAddSelNextMatch,
    kMenuSmartBackspace,
    kMenuHideTableEditor,
    kMenuAutoPairBold,
    kMenuAutoPairItalic,
    kMenuAutoPairCode,
    kMenuToggleMinimapR11,
    // R12 — remaining TabBar ops
    kMenuCloseTabsToLeft,
    kMenuCloseTabsToRight,
    kMenuPinTab,
    kMenuUnpinTab,
    // R13 — remaining features
    kMenuCopyFilePath,
    kMenuRevealInFinder,
    kMenuZoomIn,
    kMenuZoomOut,
    kMenuZoomReset,
    kMenuConvertEolLf,
    kMenuConvertEolCrlf
};

// (Moved logic to end of file to fix redefinition and structure)

void MainFrame::createMenuBar()
{
    auto* menu_bar = new wxMenuBar();

    // --- File menu ---
    auto* fileMenu = new wxMenu;
    fileMenu->Append(wxID_NEW, "&New\tCtrl+N");
    fileMenu->Append(wxID_OPEN, "&Open Folder...\tCtrl+O");
    fileMenu->Append(kMenuOpenFile, "Open &File...\tCtrl+Shift+O");

    auto* recentMenu = new wxMenu;
    fileMenu->AppendSubMenu(recentMenu, "Open &Recent");

    fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S");
    fileMenu->Append(kMenuSaveAs, "Save &As...\tCtrl+Shift+S");
    // R2 Fix 15: Save All
    fileMenu->Append(kMenuSaveAll, "Save A&ll\tCtrl+Alt+S");
    fileMenu->AppendSeparator();
    // R2 Fix 16: Revert File
    fileMenu->Append(kMenuRevertFile, "Re&vert File");
    fileMenu->AppendSeparator();
    // Fix 17: Close Tab shortcut
    fileMenu->Append(kMenuCloseTab, "&Close Tab\tCtrl+W");
    // R2 Fix 17: Close All Tabs
    fileMenu->Append(kMenuCloseAllTabs, "Close All Ta&bs\tCtrl+Shift+W");
    // R10: Close Other / Close Saved Tabs
    fileMenu->Append(kMenuCloseOtherTabs, "Close Ot&her Tabs");
    fileMenu->Append(kMenuCloseSavedTabs, "Close Sa&ved Tabs");
    // R12: Close Tabs to Left/Right, Pin/Unpin
    fileMenu->Append(kMenuCloseTabsToLeft, "Close Tabs to &Left");
    fileMenu->Append(kMenuCloseTabsToRight, "Close Tabs to Ri&ght");
    fileMenu->Append(kMenuPinTab, "&Pin Current Tab");
    fileMenu->Append(kMenuUnpinTab, "U&npin Current Tab");
    // R3 Fix 11: Close Folder (return to startup screen)
    fileMenu->Append(kMenuCloseFolder, "Close &Folder");
    fileMenu->AppendSeparator();
    // R6 Fix 18: Print
    fileMenu->Append(kMenuPrint, "&Print...\tCtrl+P");
    fileMenu->AppendSeparator();
    // R13: Copy Path / Reveal in Finder
    fileMenu->Append(kMenuCopyFilePath, "Copy File Pat&h");
    fileMenu->Append(kMenuRevealInFinder, "Reveal in &Finder");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt+F4");
    menu_bar->Append(fileMenu, "&File");

    // Bind events
    Bind(wxEVT_MENU, &MainFrame::onOpenFolder, this, wxID_OPEN);
    Bind(
        wxEVT_MENU, [this](wxCommandEvent& /*event*/) { Close(true); }, wxID_EXIT);

    // Fix 19: File → New creates a new untitled tab + showEditor transition
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            showEditor();
            if (layout_ != nullptr)
            {
                static int untitled_count = 1;
                std::string untitled_path = "Untitled-" + std::to_string(untitled_count++) + ".md";
                layout_->OpenFileInTab(untitled_path);
            }
        },
        wxID_NEW);

    // Fix 20: File → Open File dialog + showEditor transition
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            wxFileDialog dialog(this,
                                "Open File",
                                wxEmptyString,
                                wxEmptyString,
                                "Markdown files (*.md)|*.md|All files (*.*)|*.*",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);
            if (dialog.ShowModal() == wxID_OK)
            {
                showEditor();
                const std::string path = dialog.GetPath().ToStdString();
                if (layout_ != nullptr)
                {
                    layout_->OpenFileInTab(path);
                }
            }
        },
        kMenuOpenFile);

    // R2 Fix 15: Save All binding
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (layout_ != nullptr)
            {
                layout_->SaveAllFiles();
            }
        },
        kMenuSaveAll);

    // R2 Fix 16: Revert File binding
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (layout_ != nullptr)
            {
                layout_->RevertActiveFile();
            }
        },
        kMenuRevertFile);

    // R2 Fix 17: Close All Tabs binding
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (layout_ != nullptr)
            {
                layout_->CloseAllTabs();
            }
        },
        kMenuCloseAllTabs);

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
    edit_menu->AppendSeparator();
    // R6 Fix 1: Find & Replace
    edit_menu->Append(kMenuFind, "&Find...\tCtrl+F");
    edit_menu->Append(kMenuReplace, "Find and &Replace...\tCtrl+H");
    edit_menu->AppendSeparator();
    // R6 Fix 6: Duplicate Line
    edit_menu->Append(kMenuDuplicateLine, "&Duplicate Line\tCtrl+Shift+D");
    // R6 Fix 7: Toggle Comment
    edit_menu->Append(kMenuToggleComment, "Toggle C&omment\tCtrl+/");
    // R6 Fix 15: Delete Line
    edit_menu->Append(kMenuDeleteLine, "Delete Li&ne\tCtrl+Shift+K");
    edit_menu->AppendSeparator();
    // R7 Fixes: Move, Join, Select, Expand, Insert
    edit_menu->Append(kMenuMoveLineUp, "Move Line &Up\tAlt+Up");
    edit_menu->Append(kMenuMoveLineDown, "Move Line Do&wn\tAlt+Down");
    edit_menu->Append(kMenuJoinLines, "&Join Lines\tCtrl+J");
    edit_menu->Append(kMenuSelectAllOccurrences, "Select All &Occurrences\tCtrl+Shift+L");
    edit_menu->Append(kMenuExpandLineSelection, "E&xpand Line Selection\tCtrl+L");
    edit_menu->Append(kMenuInsertLineAbove, "Insert Line A&bove\tCtrl+Shift+Return");
    edit_menu->Append(kMenuInsertLineBelow, "Insert Line Belo&w\tCtrl+Return");
    edit_menu->AppendSeparator();
    // R8: Copy/Delete/Transpose/Indent/Bracket/Comment/ReadOnly
    edit_menu->Append(kMenuCopyLineUp, "Copy Line U&p\tShift+Alt+Up");
    edit_menu->Append(kMenuCopyLineDown, "Copy Line Dow&n\tShift+Alt+Down");
    edit_menu->Append(kMenuDeleteAllLeft, "Delete All Lef&t\tCtrl+Backspace");
    edit_menu->Append(kMenuDeleteAllRight, "Delete All Ri&ght\tCtrl+Delete");
    edit_menu->Append(kMenuTransposeChars, "Transpose Charac&ters\tCtrl+T");
    edit_menu->AppendSeparator();
    edit_menu->Append(kMenuIndentSelection, "&Indent Selection\tCtrl+]");
    edit_menu->Append(kMenuOutdentSelection, "O&utdent Selection\tCtrl+[");
    edit_menu->Append(kMenuJumpToBracket, "Jump to Matching &Bracket\tCtrl+Shift+\\");
    edit_menu->AppendSeparator();
    edit_menu->Append(kMenuAddLineComment, "Add Line Commen&t");
    edit_menu->Append(kMenuRemoveLineComment, "Remove Line Comme&nt");
    edit_menu->AppendCheckItem(kMenuToggleReadOnly, "Toggle Read-Onl&y");
    edit_menu->AppendSeparator();
    // R8: Selection submenu
    auto* selection_menu = new wxMenu();
    selection_menu->Append(kMenuSelectWord, "Select &Word\tCtrl+D");
    selection_menu->Append(kMenuSelectParagraph, "Select &Paragraph");
    // R9: Expand/Shrink Selection
    selection_menu->Append(kMenuExpandSelection, "&Expand Selection\tShift+Alt+Right");
    selection_menu->Append(kMenuShrinkSelection, "&Shrink Selection\tShift+Alt+Left");
    edit_menu->AppendSubMenu(selection_menu, "Se&lection");
    // R7: Transform submenu
    auto* transform_menu = new wxMenu();
    transform_menu->Append(kMenuTransformUpper, "&Uppercase");
    transform_menu->Append(kMenuTransformLower, "&Lowercase");
    transform_menu->Append(kMenuTransformTitle, "&Title Case");
    transform_menu->AppendSeparator();
    // R8: Indentation conversion in Transform
    transform_menu->Append(kMenuConvertIndentSpaces, "Convert Indentation to &Spaces");
    transform_menu->Append(kMenuConvertIndentTabs, "Convert Indentation to &Tabs");
    edit_menu->AppendSubMenu(transform_menu, "Transfor&m");
    // R7: Sort submenu
    auto* sort_menu = new wxMenu();
    sort_menu->Append(kMenuSortAsc, "Sort Lines &Ascending");
    sort_menu->Append(kMenuSortDesc, "Sort Lines &Descending");
    sort_menu->AppendSeparator();
    // R8: Reverse / Delete Duplicates
    sort_menu->Append(kMenuReverseLines, "&Reverse Lines");
    sort_menu->Append(kMenuDeleteDuplicateLines, "Delete &Duplicate Lines");
    edit_menu->AppendSubMenu(sort_menu, "S&ort Lines");
    edit_menu->AppendSeparator();
    // R3 Fix 8: Go To Line
    edit_menu->Append(kMenuGoToLine, "Go to &Line...\tCtrl+G");
    // R9: Edit additions
    edit_menu->Append(kMenuTrimTrailingWS, "Trim Trailing &Whitespace");
    edit_menu->Append(kMenuCursorUndo, "Cursor &Undo\tCtrl+U");
    edit_menu->Append(kMenuCursorRedo, "Cursor Red&o\tCtrl+Shift+U");
    edit_menu->AppendSeparator();
    edit_menu->Append(kMenuMoveTextLeft, "Move Text Le&ft");
    edit_menu->Append(kMenuMoveTextRight, "Move Text Ri&ght");
    edit_menu->Append(kMenuSelectToBracket, "Select to &Matching Bracket");
    edit_menu->Append(kMenuToggleBlockComment, "Toggle Bloc&k Comment\tShift+Alt+A");
    edit_menu->Append(kMenuInsertDateTime, "Insert &Date/Time");
    edit_menu->AppendSeparator();
    // R10: Multi-cursor and bracket operations
    edit_menu->Append(kMenuAddCursorBelow, "Add Cursor Bel&ow\tAlt+Cmd+Down");
    edit_menu->Append(kMenuAddCursorAbove, "Add Cursor Abo&ve\tAlt+Cmd+Up");
    edit_menu->Append(kMenuAddCursorNextOccurrence, "Add Cursor at Ne&xt Match");
    edit_menu->Append(kMenuRemoveSurroundingBrackets, "&Remove Surrounding Brackets");
    edit_menu->Append(kMenuDuplicateSelectionOrLine, "Duplicate Selectio&n or Line");
    edit_menu->Append(kMenuEnsureFinalNewline, "Ensure Final New&line");
    // R11: Edit additions
    edit_menu->Append(kMenuDeleteCurrentLine, "Delete C&urrent Line");
    edit_menu->Append(kMenuCopyLineNoSel, "Copy Line (No Se&lection)");
    edit_menu->Append(kMenuAddSelNextMatch, "Add Selection to Next &Match");
    edit_menu->Append(kMenuSmartBackspace, "Smart &Backspace");
    edit_menu->AppendSeparator();
    // R13: EOL conversion
    auto* eol_menu = new wxMenu();
    eol_menu->Append(kMenuConvertEolLf, "LF (Unix/macOS)");
    eol_menu->Append(kMenuConvertEolCrlf, "CRLF (Windows)");
    edit_menu->AppendSubMenu(eol_menu, "Line Endin&gs");
    menu_bar->Append(edit_menu, "&Edit");

    // --- Format menu (Markdown) ---
    auto* format_menu = new wxMenu();
    format_menu->Append(kMenuBold, "&Bold\tCtrl+B");
    format_menu->Append(kMenuItalic, "&Italic\tCtrl+I");
    format_menu->Append(kMenuInlineCode, "Inline &Code\tCtrl+E");
    format_menu->AppendSeparator();
    format_menu->Append(kMenuBlockquote, "Block&quote\tCtrl+Shift+.");
    format_menu->Append(kMenuCycleHeading, "Cycle &Heading\tCtrl+Shift+H");
    format_menu->Append(kMenuInsertTable, "Insert &Table");
    format_menu->AppendSeparator();
    // R10: Format additions
    format_menu->Append(kMenuInsertLink, "Insert &Link\tCtrl+K");
    format_menu->Append(kMenuShowTableEditor, "Show Table &Editor");
    format_menu->AppendCheckItem(kMenuToggleSmartList, "Smart &List Continuation");
    format_menu->Check(kMenuToggleSmartList, true);
    format_menu->Append(kMenuInsertSnippet, "Insert Sni&ppet");
    // R11: Format additions
    format_menu->Append(kMenuHideTableEditor, "&Hide Table Editor");
    format_menu->Append(kMenuAutoPairBold, "Auto-Pair Bol&d (**)");
    format_menu->Append(kMenuAutoPairItalic, "Auto-Pair Ital&ic (*)");
    format_menu->Append(kMenuAutoPairCode, "Auto-Pair Cod&e (`)");
    menu_bar->Append(format_menu, "F&ormat");

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
    // R6 Fix 10: Word Wrap toggle
    view_menu->AppendCheckItem(kMenuWordWrap, "&Word Wrap\tCtrl+Alt+W");
    view_menu->Check(kMenuWordWrap, true); // default on
    view_menu->AppendSeparator();
    // R9: View additions — toggles for editor features
    view_menu->AppendCheckItem(kMenuToggleAutoIndent, "Toggle &Auto-Indent");
    view_menu->Check(kMenuToggleAutoIndent, true);
    view_menu->AppendCheckItem(kMenuToggleBracketMatching, "Toggle Brac&ket Matching");
    view_menu->Check(kMenuToggleBracketMatching, true);
    view_menu->AppendCheckItem(kMenuToggleCodeFolding, "Toggle Code &Folding");
    view_menu->Check(kMenuToggleCodeFolding, true);
    view_menu->AppendCheckItem(kMenuToggleIndentGuides, "Toggle Inden&t Guides");
    view_menu->Check(kMenuToggleIndentGuides, true);
    // R10: View additions — more toggles
    view_menu->AppendSeparator();
    view_menu->AppendCheckItem(kMenuToggleScrollBeyond, "Scroll &Beyond Last Line");
    view_menu->Check(kMenuToggleScrollBeyond, true);
    view_menu->AppendCheckItem(kMenuToggleHighlightLine, "&Highlight Current Line");
    view_menu->Check(kMenuToggleHighlightLine, true);
    view_menu->AppendCheckItem(kMenuToggleAutoCloseBrackets, "Auto-&Close Brackets");
    view_menu->Check(kMenuToggleAutoCloseBrackets, true);
    view_menu->AppendCheckItem(kMenuToggleStickyScroll, "Stick&y Scroll");
    view_menu->Check(kMenuToggleStickyScroll, true);
    view_menu->AppendCheckItem(kMenuToggleFontLigatures, "Font Li&gatures");
    view_menu->AppendCheckItem(kMenuToggleSmoothCaret, "S&mooth Caret");
    view_menu->AppendCheckItem(kMenuToggleColorPreview, "Inline Color Pre&view");
    view_menu->AppendCheckItem(kMenuToggleEdgeRuler, "Edge Column R&uler");
    // R11: View additions
    view_menu->AppendSeparator();
    view_menu->AppendCheckItem(kMenuToggleSmoothScrolling, "Smooth Scro&lling");
    view_menu->AppendCheckItem(kMenuToggleTrailingWSHighlight, "Trailing &Whitespace Highlight");
    view_menu->AppendCheckItem(kMenuToggleAutoTrimWS, "Auto-&Trim Trailing WS");
    view_menu->AppendCheckItem(kMenuToggleGutterSeparator, "Gutt&er Separator");
    view_menu->Check(kMenuToggleGutterSeparator, true);
    view_menu->AppendCheckItem(kMenuToggleInsertFinalNewline, "Insert Final &Newline on Save");
    view_menu->AppendCheckItem(kMenuToggleWhitespaceBoundary, "Whitespace &Boundary");
    view_menu->AppendCheckItem(kMenuToggleLinkAutoComplete, "Lin&k Auto-Complete");
    view_menu->AppendCheckItem(kMenuToggleDragDrop, "Drag && &Drop");
    view_menu->Check(kMenuToggleDragDrop, true);
    view_menu->AppendCheckItem(kMenuToggleAutoSave, "&Auto-Save");
    view_menu->AppendCheckItem(kMenuToggleEmptySelClipboard, "Empty Selection Cli&pboard");
    view_menu->Append(kMenuCycleRenderWhitespace, "Cycle Render Whi&tespace");
    view_menu->AppendCheckItem(kMenuToggleMinimapR11, "Toggle M&inimap");
    // R7 Fixes: Toggle Line Numbers, Whitespace, Fold/Unfold
    view_menu->AppendCheckItem(kMenuToggleLineNumbers, "Toggle &Line Numbers");
    view_menu->Check(kMenuToggleLineNumbers, true); // default on
    view_menu->AppendCheckItem(kMenuToggleWhitespace, "Toggle W&hitespace");
    view_menu->AppendSeparator();
    view_menu->Append(kMenuFoldAll, "&Fold All Regions");
    view_menu->Append(kMenuUnfoldAll, "&Unfold All Regions");
    // R8: Fold/Unfold Current + Minimap
    view_menu->Append(kMenuFoldCurrent, "Fold C&urrent Region\tCtrl+Alt+[");
    view_menu->Append(kMenuUnfoldCurrent, "Unfold Cu&rrent Region\tCtrl+Alt+]");
    view_menu->AppendSeparator();
    view_menu->AppendCheckItem(kMenuToggleMinimap, "Toggle &Minimap");
    view_menu->AppendSeparator();
    view_menu->Append(kMenuFullscreen, "Toggle &Fullscreen\tF11");
    view_menu->AppendSeparator();
    // R13: Zoom menu items
    view_menu->Append(kMenuZoomIn, "Zoom &In\tCtrl+=");
    view_menu->Append(kMenuZoomOut, "Zoom &Out\tCtrl+-");
    view_menu->Append(kMenuZoomReset, "&Reset Zoom\tCtrl+0");
    menu_bar->Append(view_menu, "&View");

    // --- Window menu ---
    auto* window_menu = new wxMenu();
    window_menu->Append(kMenuThemeGallery, "Theme &Gallery...\tCtrl+T");
    menu_bar->Append(window_menu, "&Window");

    // --- Help menu ---
    auto* help_menu = new wxMenu();
    help_menu->Append(kMenuAbout, "&About MarkAmp");
    help_menu->AppendSeparator();
    // R7 Fix 20: Welcome Screen
    help_menu->Append(kMenuWelcomeScreen, "&Welcome Screen");
    menu_bar->Append(help_menu, "&Help");

    SetMenuBar(menu_bar);

    // --- Bind menu events ---
    Bind(
        wxEVT_MENU, [this]([[maybe_unused]] wxCommandEvent& evt) { Close(); }, kMenuQuit);

    Bind(wxEVT_MENU, &MainFrame::onOpenFolder, this, kMenuOpenFolder);
    Bind(wxEVT_MENU, &MainFrame::onSave, this, kMenuSave);

    // R3 Fix 11: Close Folder → return to startup screen
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            workspace_folder_name_ = "MarkAmp";
            showStartupScreen();
        },
        kMenuCloseFolder);

    // R3 Fix 8: Go To Line handler
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (event_bus_ != nullptr)
            {
                const core::events::GoToLineRequestEvent go_evt;
                event_bus_->publish(go_evt);
            }
        },
        kMenuGoToLine);

    // R6 Fix 1: Find
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (event_bus_ != nullptr)
            {
                const core::events::FindRequestEvent find_evt;
                event_bus_->publish(find_evt);
            }
        },
        kMenuFind);

    // R6 Fix 1: Replace
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (event_bus_ != nullptr)
            {
                const core::events::ReplaceRequestEvent repl_evt;
                event_bus_->publish(repl_evt);
            }
        },
        kMenuReplace);

    // R6 Fix 6: Duplicate Line
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (event_bus_ != nullptr)
            {
                const core::events::DuplicateLineRequestEvent dup_evt;
                event_bus_->publish(dup_evt);
            }
        },
        kMenuDuplicateLine);

    // R6 Fix 7: Toggle Comment
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (event_bus_ != nullptr)
            {
                const core::events::ToggleCommentRequestEvent cmt_evt;
                event_bus_->publish(cmt_evt);
            }
        },
        kMenuToggleComment);

    // R6 Fix 15: Delete Line
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (event_bus_ != nullptr)
            {
                const core::events::DeleteLineRequestEvent del_evt;
                event_bus_->publish(del_evt);
            }
        },
        kMenuDeleteLine);

    // R6 Fix 18: Print — real wxHtmlEasyPrinting
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (layout_ == nullptr)
            {
                return;
            }
            auto content = layout_->GetActiveFileContent();
            if (content.empty())
            {
                return;
            }
            wxHtmlEasyPrinting printer("MarkAmp Print");
            printer.SetHeader("<b>MarkAmp</b> — @TITLE@", wxPAGE_ALL);
            printer.SetFooter("Page @PAGENUM@ / @PAGESCNT@", wxPAGE_ALL);
            printer.PrintText("<pre>" + content + "</pre>");
        },
        kMenuPrint);

    // R6 Fix 8: Clear Recent Workspaces
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (recent_workspaces_ != nullptr)
            {
                recent_workspaces_->clear();
                rebuildRecentMenu();
            }
        },
        kMenuClearRecent);

    // --- R7 event bindings ---
    auto publish_r7 = [this](auto make_event, int menu_id)
    {
        Bind(
            wxEVT_MENU,
            [this, make_event]([[maybe_unused]] wxCommandEvent& evt)
            {
                if (event_bus_ != nullptr)
                {
                    const auto e = make_event();
                    event_bus_->publish(e);
                }
            },
            menu_id);
    };
    publish_r7([]() { return core::events::MoveLineUpRequestEvent{}; }, kMenuMoveLineUp);
    publish_r7([]() { return core::events::MoveLineDownRequestEvent{}; }, kMenuMoveLineDown);
    publish_r7([]() { return core::events::JoinLinesRequestEvent{}; }, kMenuJoinLines);
    publish_r7([]() { return core::events::SortLinesAscRequestEvent{}; }, kMenuSortAsc);
    publish_r7([]() { return core::events::SortLinesDescRequestEvent{}; }, kMenuSortDesc);
    publish_r7([]() { return core::events::TransformUpperRequestEvent{}; }, kMenuTransformUpper);
    publish_r7([]() { return core::events::TransformLowerRequestEvent{}; }, kMenuTransformLower);
    publish_r7([]() { return core::events::TransformTitleRequestEvent{}; }, kMenuTransformTitle);
    publish_r7([]() { return core::events::SelectAllOccurrencesRequestEvent{}; },
               kMenuSelectAllOccurrences);
    publish_r7([]() { return core::events::ExpandLineSelectionRequestEvent{}; },
               kMenuExpandLineSelection);
    publish_r7([]() { return core::events::InsertLineAboveRequestEvent{}; }, kMenuInsertLineAbove);
    publish_r7([]() { return core::events::InsertLineBelowRequestEvent{}; }, kMenuInsertLineBelow);
    publish_r7([]() { return core::events::FoldAllRequestEvent{}; }, kMenuFoldAll);
    publish_r7([]() { return core::events::UnfoldAllRequestEvent{}; }, kMenuUnfoldAll);
    publish_r7([]() { return core::events::ToggleLineNumbersRequestEvent{}; },
               kMenuToggleLineNumbers);
    publish_r7([]() { return core::events::ToggleWhitespaceRequestEvent{}; },
               kMenuToggleWhitespace);

    // R8 event bindings
    auto publish_r8 = [this](auto make_event, int menu_id)
    {
        Bind(
            wxEVT_MENU,
            [this, make_event]([[maybe_unused]] wxCommandEvent& evt)
            {
                if (event_bus_ != nullptr)
                {
                    const auto e = make_event();
                    event_bus_->publish(e);
                }
            },
            menu_id);
    };
    publish_r8([]() { return core::events::CopyLineUpRequestEvent{}; }, kMenuCopyLineUp);
    publish_r8([]() { return core::events::CopyLineDownRequestEvent{}; }, kMenuCopyLineDown);
    publish_r8([]() { return core::events::DeleteAllLeftRequestEvent{}; }, kMenuDeleteAllLeft);
    publish_r8([]() { return core::events::DeleteAllRightRequestEvent{}; }, kMenuDeleteAllRight);
    publish_r8([]() { return core::events::ReverseLinesRequestEvent{}; }, kMenuReverseLines);
    publish_r8([]() { return core::events::DeleteDuplicateLinesRequestEvent{}; },
               kMenuDeleteDuplicateLines);
    publish_r8([]() { return core::events::TransposeCharsRequestEvent{}; }, kMenuTransposeChars);
    publish_r8([]() { return core::events::IndentSelectionRequestEvent{}; }, kMenuIndentSelection);
    publish_r8([]() { return core::events::OutdentSelectionRequestEvent{}; },
               kMenuOutdentSelection);
    publish_r8([]() { return core::events::SelectWordRequestEvent{}; }, kMenuSelectWord);
    publish_r8([]() { return core::events::SelectParagraphRequestEvent{}; }, kMenuSelectParagraph);
    publish_r8([]() { return core::events::ToggleReadOnlyRequestEvent{}; }, kMenuToggleReadOnly);
    publish_r8([]() { return core::events::ConvertIndentSpacesRequestEvent{}; },
               kMenuConvertIndentSpaces);
    publish_r8([]() { return core::events::ConvertIndentTabsRequestEvent{}; },
               kMenuConvertIndentTabs);
    publish_r8([]() { return core::events::JumpToBracketRequestEvent{}; }, kMenuJumpToBracket);
    publish_r8([]() { return core::events::ToggleMinimapRequestEvent{}; }, kMenuToggleMinimap);
    publish_r8([]() { return core::events::FoldCurrentRequestEvent{}; }, kMenuFoldCurrent);
    publish_r8([]() { return core::events::UnfoldCurrentRequestEvent{}; }, kMenuUnfoldCurrent);
    publish_r8([]() { return core::events::AddLineCommentRequestEvent{}; }, kMenuAddLineComment);
    publish_r8([]() { return core::events::RemoveLineCommentRequestEvent{}; },
               kMenuRemoveLineComment);

    // R9 event bindings
    auto publish_r9 = [this](auto make_event, int menu_id)
    {
        Bind(
            wxEVT_MENU,
            [this, make_event]([[maybe_unused]] wxCommandEvent& evt)
            {
                if (event_bus_ != nullptr)
                {
                    const auto e = make_event();
                    event_bus_->publish(e);
                }
            },
            menu_id);
    };
    publish_r9([]() { return core::events::TrimTrailingWSRequestEvent{}; }, kMenuTrimTrailingWS);
    publish_r9([]() { return core::events::ExpandSelectionRequestEvent{}; }, kMenuExpandSelection);
    publish_r9([]() { return core::events::ShrinkSelectionRequestEvent{}; }, kMenuShrinkSelection);
    publish_r9([]() { return core::events::CursorUndoRequestEvent{}; }, kMenuCursorUndo);
    publish_r9([]() { return core::events::CursorRedoRequestEvent{}; }, kMenuCursorRedo);
    publish_r9([]() { return core::events::MoveTextLeftRequestEvent{}; }, kMenuMoveTextLeft);
    publish_r9([]() { return core::events::MoveTextRightRequestEvent{}; }, kMenuMoveTextRight);
    publish_r9([]() { return core::events::ToggleAutoIndentRequestEvent{}; },
               kMenuToggleAutoIndent);
    publish_r9([]() { return core::events::ToggleBracketMatchingRequestEvent{}; },
               kMenuToggleBracketMatching);
    publish_r9([]() { return core::events::ToggleCodeFoldingRequestEvent{}; },
               kMenuToggleCodeFolding);
    publish_r9([]() { return core::events::ToggleIndentGuidesRequestEvent{}; },
               kMenuToggleIndentGuides);
    publish_r9([]() { return core::events::SelectToBracketRequestEvent{}; }, kMenuSelectToBracket);
    publish_r9([]() { return core::events::ToggleBlockCommentRequestEvent{}; },
               kMenuToggleBlockComment);
    publish_r9([]() { return core::events::InsertDateTimeRequestEvent{}; }, kMenuInsertDateTime);
    publish_r9([]() { return core::events::BoldRequestEvent{}; }, kMenuBold);
    publish_r9([]() { return core::events::ItalicRequestEvent{}; }, kMenuItalic);
    publish_r9([]() { return core::events::InlineCodeRequestEvent{}; }, kMenuInlineCode);
    publish_r9([]() { return core::events::BlockquoteRequestEvent{}; }, kMenuBlockquote);
    publish_r9([]() { return core::events::CycleHeadingRequestEvent{}; }, kMenuCycleHeading);
    publish_r9([]() { return core::events::InsertTableRequestEvent{}; }, kMenuInsertTable);

    // R10 event bindings
    auto publish_r10 = [this](auto make_event, int menu_id)
    {
        Bind(
            wxEVT_MENU,
            [this, make_event]([[maybe_unused]] wxCommandEvent& evt)
            {
                if (event_bus_ != nullptr)
                {
                    const auto e = make_event();
                    event_bus_->publish(e);
                }
            },
            menu_id);
    };
    publish_r10([]() { return core::events::ToggleSmartListContinuationRequestEvent{}; },
                kMenuToggleSmartList);
    publish_r10([]() { return core::events::CloseOtherTabsRequestEvent{}; }, kMenuCloseOtherTabs);
    publish_r10([]() { return core::events::CloseSavedTabsRequestEvent{}; }, kMenuCloseSavedTabs);
    publish_r10([]() { return core::events::InsertLinkRequestEvent{}; }, kMenuInsertLink);
    publish_r10([]() { return core::events::AddCursorBelowRequestEvent{}; }, kMenuAddCursorBelow);
    publish_r10([]() { return core::events::AddCursorAboveRequestEvent{}; }, kMenuAddCursorAbove);
    publish_r10([]() { return core::events::AddCursorNextOccurrenceRequestEvent{}; },
                kMenuAddCursorNextOccurrence);
    publish_r10([]() { return core::events::RemoveSurroundingBracketsRequestEvent{}; },
                kMenuRemoveSurroundingBrackets);
    publish_r10([]() { return core::events::DuplicateSelectionOrLineRequestEvent{}; },
                kMenuDuplicateSelectionOrLine);
    publish_r10([]() { return core::events::ShowTableEditorRequestEvent{}; }, kMenuShowTableEditor);
    publish_r10([]() { return core::events::ToggleScrollBeyondLastLineRequestEvent{}; },
                kMenuToggleScrollBeyond);
    publish_r10([]() { return core::events::ToggleHighlightCurrentLineRequestEvent{}; },
                kMenuToggleHighlightLine);
    publish_r10([]() { return core::events::ToggleAutoClosingBracketsRequestEvent{}; },
                kMenuToggleAutoCloseBrackets);
    publish_r10([]() { return core::events::ToggleStickyScrollRequestEvent{}; },
                kMenuToggleStickyScroll);
    publish_r10([]() { return core::events::ToggleFontLigaturesRequestEvent{}; },
                kMenuToggleFontLigatures);
    publish_r10([]() { return core::events::ToggleSmoothCaretRequestEvent{}; },
                kMenuToggleSmoothCaret);
    publish_r10([]() { return core::events::ToggleInlineColorPreviewRequestEvent{}; },
                kMenuToggleColorPreview);
    publish_r10([]() { return core::events::ToggleEdgeColumnRulerRequestEvent{}; },
                kMenuToggleEdgeRuler);
    publish_r10([]() { return core::events::EnsureFinalNewlineRequestEvent{}; },
                kMenuEnsureFinalNewline);
    publish_r10([]() { return core::events::InsertSnippetRequestEvent{}; }, kMenuInsertSnippet);

    // R11 event bindings
    auto publish_r11 = [this](auto make_event, int menu_id)
    {
        Bind(
            wxEVT_MENU,
            [this, make_event]([[maybe_unused]] wxCommandEvent& evt)
            {
                if (event_bus_ != nullptr)
                {
                    const auto e = make_event();
                    event_bus_->publish(e);
                }
            },
            menu_id);
    };
    publish_r11([]() { return core::events::ToggleSmoothScrollingRequestEvent{}; },
                kMenuToggleSmoothScrolling);
    publish_r11([]() { return core::events::ToggleTrailingWSHighlightRequestEvent{}; },
                kMenuToggleTrailingWSHighlight);
    publish_r11([]() { return core::events::ToggleAutoTrimWSRequestEvent{}; },
                kMenuToggleAutoTrimWS);
    publish_r11([]() { return core::events::ToggleGutterSeparatorRequestEvent{}; },
                kMenuToggleGutterSeparator);
    publish_r11([]() { return core::events::ToggleInsertFinalNewlineRequestEvent{}; },
                kMenuToggleInsertFinalNewline);
    publish_r11([]() { return core::events::ToggleWhitespaceBoundaryRequestEvent{}; },
                kMenuToggleWhitespaceBoundary);
    publish_r11([]() { return core::events::ToggleLinkAutoCompleteRequestEvent{}; },
                kMenuToggleLinkAutoComplete);
    publish_r11([]() { return core::events::ToggleDragDropRequestEvent{}; }, kMenuToggleDragDrop);
    publish_r11([]() { return core::events::ToggleAutoSaveRequestEvent{}; }, kMenuToggleAutoSave);
    publish_r11([]() { return core::events::ToggleEmptySelClipboardRequestEvent{}; },
                kMenuToggleEmptySelClipboard);
    publish_r11([]() { return core::events::CycleRenderWhitespaceRequestEvent{}; },
                kMenuCycleRenderWhitespace);
    publish_r11([]() { return core::events::DeleteLineRequestEvent{}; }, kMenuDeleteCurrentLine);
    publish_r11([]() { return core::events::CopyLineNoSelRequestEvent{}; }, kMenuCopyLineNoSel);
    publish_r11([]() { return core::events::AddSelNextMatchRequestEvent{}; }, kMenuAddSelNextMatch);
    publish_r11([]() { return core::events::SmartBackspaceRequestEvent{}; }, kMenuSmartBackspace);
    publish_r11([]() { return core::events::HideTableEditorRequestEvent{}; }, kMenuHideTableEditor);
    publish_r11([]() { return core::events::AutoPairBoldRequestEvent{}; }, kMenuAutoPairBold);
    publish_r11([]() { return core::events::AutoPairItalicRequestEvent{}; }, kMenuAutoPairItalic);
    publish_r11([]() { return core::events::AutoPairCodeRequestEvent{}; }, kMenuAutoPairCode);
    publish_r11([]() { return core::events::ToggleMinimapRequestEvent{}; }, kMenuToggleMinimapR11);

    // R12 event bindings
    auto publish_r12 = [this](auto make_event, int menu_id)
    {
        Bind(
            wxEVT_MENU,
            [this, make_event]([[maybe_unused]] wxCommandEvent& evt)
            {
                if (event_bus_ != nullptr)
                {
                    const auto e = make_event();
                    event_bus_->publish(e);
                }
            },
            menu_id);
    };
    publish_r12([]() { return core::events::CloseTabsToLeftRequestEvent{}; }, kMenuCloseTabsToLeft);
    publish_r12([]() { return core::events::CloseTabsToRightRequestEvent{}; },
                kMenuCloseTabsToRight);
    publish_r12([]() { return core::events::PinTabRequestEvent{}; }, kMenuPinTab);
    publish_r12([]() { return core::events::UnpinTabRequestEvent{}; }, kMenuUnpinTab);

    // ── R13 bindings ──
    auto publish_r13 = [this](auto make_event, int menu_id)
    {
        Bind(
            wxEVT_MENU,
            [this, make_event]([[maybe_unused]] wxCommandEvent& evt)
            {
                if (event_bus_ != nullptr)
                {
                    const auto evnt = make_event();
                    event_bus_->publish(evnt);
                }
            },
            menu_id);
    };
    publish_r13([]() { return core::events::CopyFilePathRequestEvent{}; }, kMenuCopyFilePath);
    publish_r13([]() { return core::events::RevealInFinderRequestEvent{}; }, kMenuRevealInFinder);
    publish_r13([]() { return core::events::ZoomInRequestEvent{}; }, kMenuZoomIn);
    publish_r13([]() { return core::events::ZoomOutRequestEvent{}; }, kMenuZoomOut);
    publish_r13([]() { return core::events::ZoomResetRequestEvent{}; }, kMenuZoomReset);
    publish_r13([]() { return core::events::ConvertEolLfRequestEvent{}; }, kMenuConvertEolLf);
    publish_r13([]() { return core::events::ConvertEolCrlfRequestEvent{}; }, kMenuConvertEolCrlf);

    // R7 Fix 20: Help → Welcome Screen
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt) { showStartupScreen(); },
        kMenuWelcomeScreen);

    // Fix 18: Save As binding
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (layout_ != nullptr)
            {
                layout_->SaveActiveFileAs();
            }
        },
        kMenuSaveAs);

    // Fix 17: Close Tab binding (Cmd+W / Ctrl+W)
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            if (layout_ != nullptr && event_bus_ != nullptr)
            {
                const std::string active = layout_->GetActiveFilePath();
                if (!active.empty())
                {
                    const core::events::TabCloseRequestEvent close_evt(active);
                    event_bus_->publish(close_evt);
                }
            }
        },
        kMenuCloseTab);

    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent& evt)
        {
            wxAboutDialogInfo info;
            info.SetName("MarkAmp");
            // R6 Fix 5: Use CMake-injected version macros
            info.SetVersion(wxString::Format(
                "%d.%d.%d", MARKAMP_VERSION_MAJOR, MARKAMP_VERSION_MINOR, MARKAMP_VERSION_PATCH));
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

        // R6 Fix 10: Word Wrap toggle
        Bind(
            wxEVT_MENU,
            [this]([[maybe_unused]] wxCommandEvent& evt)
            {
                const core::events::WrapToggleRequestEvent wrap_evt;
                event_bus_->publish(wrap_evt);
            },
            kMenuWordWrap);
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
                updateWindowTitle();
            }));

        // Fix 19: Update window title on tab switch and close
        subscriptions_.push_back(event_bus_->subscribe<core::events::TabSwitchedEvent>(
            [this](const core::events::TabSwitchedEvent& /*evt*/) { updateWindowTitle(); }));

        subscriptions_.push_back(event_bus_->subscribe<core::events::TabCloseRequestEvent>(
            [this](const core::events::TabCloseRequestEvent& /*evt*/)
            {
                // Delay title update to next event loop iteration
                // so the tab is actually closed before we query the active path
                CallAfter([this]() { updateWindowTitle(); });
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
        // Fix 15: Set workspace root for relative path computation
        layout_->SetWorkspaceRoot(path.ToStdString());
        showEditor();

        // R3 Fix 12: Store workspace folder name for window title
        workspace_folder_name_ = std::filesystem::path(path.ToStdString()).filename().string();
        SetTitle(wxString::Format("%s \u2014 MarkAmp", workspace_folder_name_));
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
                node.type = core::FileNodeType::Folder;
                // Recursively scan directories
                scanDirectory(p.string(), node.children);
                folders.push_back(node);
            }
            else if (entry.is_regular_file())
            {
                node.type = core::FileNodeType::File;
                files.push_back(node);
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
    // Fix 16: Was calling updateMenuBarForStartup — should be editor
    updateMenuBarForEditor();
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

    // R6 Fix 8: Clear Recent Workspaces at bottom
    recentMenu->AppendSeparator();
    recentMenu->Append(kMenuClearRecent, "Clear Recent Workspaces");
}

void MainFrame::onSave(wxCommandEvent& /*event*/)
{
    if (layout_ == nullptr)
    {
        return;
    }

    // Fix 20: If the active file is untitled or no file is active, use Save As
    const std::string active_path = layout_->GetActiveFilePath();
    if (active_path.empty() || active_path.find("Untitled") != std::string::npos)
    {
        layout_->SaveActiveFileAs();
    }
    else
    {
        layout_->SaveActiveFile();
    }
    updateWindowTitle();
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

    // R3 Fix 8: Go To Line shortcut
    shortcut_manager_.register_shortcut(
        {"edit.goto_line", "Go to Line", 'G', kCmd, "editor", "Edit", {}});

    // R6 shortcuts
    shortcut_manager_.register_shortcut({"edit.find", "Find", 'F', kCmd, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut(
        {"edit.replace", "Replace", 'H', kCmd, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut(
        {"edit.duplicate_line", "Duplicate Line", 'D', kCmdShift, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut(
        {"edit.toggle_comment", "Toggle Comment", '/', kCmd, "editor", "Markdown", {}});
    shortcut_manager_.register_shortcut(
        {"edit.delete_line", "Delete Line", 'K', kCmdShift, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut(
        {"view.word_wrap", "Word Wrap", 'W', wxMOD_CMD | wxMOD_ALT, "global", "View", {}});

    // R7 shortcuts
    shortcut_manager_.register_shortcut(
        {"edit.move_line_up", "Move Line Up", WXK_UP, wxMOD_ALT, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut(
        {"edit.move_line_down", "Move Line Down", WXK_DOWN, wxMOD_ALT, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut(
        {"edit.join_lines", "Join Lines", 'J', kCmd, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut({"edit.select_all_occurrences",
                                         "Select All Occurrences",
                                         'L',
                                         kCmdShift,
                                         "editor",
                                         "Edit",
                                         {}});
    shortcut_manager_.register_shortcut(
        {"edit.expand_line_selection", "Expand Line Selection", 'L', kCmd, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut({"edit.insert_line_above",
                                         "Insert Line Above",
                                         WXK_RETURN,
                                         kCmdShift,
                                         "editor",
                                         "Edit",
                                         {}});
    shortcut_manager_.register_shortcut(
        {"edit.insert_line_below", "Insert Line Below", WXK_RETURN, kCmd, "editor", "Edit", {}});

    // R8 shortcuts
    shortcut_manager_.register_shortcut(
        {"edit.copy_line_up", "Copy Line Up", WXK_UP, kCmdShift | wxMOD_ALT, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut({"edit.copy_line_down",
                                         "Copy Line Down",
                                         WXK_DOWN,
                                         kCmdShift | wxMOD_ALT,
                                         "editor",
                                         "Edit",
                                         {}});
    shortcut_manager_.register_shortcut(
        {"edit.transpose_chars", "Transpose Characters", 'T', wxMOD_CONTROL, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut(
        {"edit.indent_selection", "Indent Selection", ']', kCmd, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut(
        {"edit.outdent_selection", "Outdent Selection", '[', kCmd, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut(
        {"edit.select_word", "Select Word", 'D', kCmd, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut({"edit.jump_to_bracket",
                                         "Jump to Matching Bracket",
                                         '\\',
                                         kCmdShift,
                                         "editor",
                                         "Edit",
                                         {}});
    shortcut_manager_.register_shortcut(
        {"view.fold_current", "Fold Current Region", '[', kCmd | wxMOD_ALT, "editor", "View", {}});
    shortcut_manager_.register_shortcut({"view.unfold_current",
                                         "Unfold Current Region",
                                         ']',
                                         kCmd | wxMOD_ALT,
                                         "editor",
                                         "View",
                                         {}});

    // R9 shortcuts
    shortcut_manager_.register_shortcut(
        {"edit.cursor_undo", "Cursor Undo", 'U', kCmd, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut(
        {"edit.cursor_redo", "Cursor Redo", 'U', kCmdShift, "editor", "Edit", {}});
    shortcut_manager_.register_shortcut({"edit.expand_selection",
                                         "Expand Selection",
                                         WXK_RIGHT,
                                         wxMOD_SHIFT | wxMOD_ALT,
                                         "editor",
                                         "Edit",
                                         {}});
    shortcut_manager_.register_shortcut({"edit.shrink_selection",
                                         "Shrink Selection",
                                         WXK_LEFT,
                                         wxMOD_SHIFT | wxMOD_ALT,
                                         "editor",
                                         "Edit",
                                         {}});
    shortcut_manager_.register_shortcut({"edit.block_comment",
                                         "Toggle Block Comment",
                                         'A',
                                         wxMOD_SHIFT | wxMOD_ALT,
                                         "editor",
                                         "Edit",
                                         {}});
    shortcut_manager_.register_shortcut({"format.bold", "Bold", 'B', kCmd, "editor", "Format", {}});
    shortcut_manager_.register_shortcut(
        {"format.italic", "Italic", 'I', kCmd, "editor", "Format", {}});
    shortcut_manager_.register_shortcut(
        {"format.inline_code", "Inline Code", 'E', kCmd, "editor", "Format", {}});
    shortcut_manager_.register_shortcut(
        {"format.blockquote", "Blockquote", '.', kCmdShift, "editor", "Format", {}});
    shortcut_manager_.register_shortcut(
        {"format.cycle_heading", "Cycle Heading", 'H', kCmdShift, "editor", "Format", {}});

    // R10 shortcuts
    shortcut_manager_.register_shortcut(
        {"format.insert_link", "Insert Link", 'K', kCmd, "editor", "Format", {}});
    shortcut_manager_.register_shortcut({"edit.cursor_below",
                                         "Add Cursor Below",
                                         WXK_DOWN,
                                         wxMOD_ALT | wxMOD_CMD,
                                         "editor",
                                         "Edit",
                                         {}});
    shortcut_manager_.register_shortcut({"edit.cursor_above",
                                         "Add Cursor Above",
                                         WXK_UP,
                                         wxMOD_ALT | wxMOD_CMD,
                                         "editor",
                                         "Edit",
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

    // ── R6 Edit commands ──
    command_palette_->RegisterCommand({"Find",
                                       "Edit",
                                       shortcut_manager_.get_shortcut_text("edit.find"),
                                       [this]()
                                       {
                                           if (event_bus_ != nullptr)
                                           {
                                               event_bus_->publish(
                                                   core::events::FindRequestEvent{});
                                           }
                                       }});
    command_palette_->RegisterCommand({"Replace",
                                       "Edit",
                                       shortcut_manager_.get_shortcut_text("edit.replace"),
                                       [this]()
                                       {
                                           if (event_bus_ != nullptr)
                                           {
                                               event_bus_->publish(
                                                   core::events::ReplaceRequestEvent{});
                                           }
                                       }});
    command_palette_->RegisterCommand({"Duplicate Line",
                                       "Edit",
                                       shortcut_manager_.get_shortcut_text("edit.duplicate_line"),
                                       [this]()
                                       {
                                           if (event_bus_ != nullptr)
                                           {
                                               event_bus_->publish(
                                                   core::events::DuplicateLineRequestEvent{});
                                           }
                                       }});
    command_palette_->RegisterCommand({"Toggle Comment",
                                       "Edit",
                                       shortcut_manager_.get_shortcut_text("edit.toggle_comment"),
                                       [this]()
                                       {
                                           if (event_bus_ != nullptr)
                                           {
                                               event_bus_->publish(
                                                   core::events::ToggleCommentRequestEvent{});
                                           }
                                       }});
    command_palette_->RegisterCommand({"Delete Line",
                                       "Edit",
                                       shortcut_manager_.get_shortcut_text("edit.delete_line"),
                                       [this]()
                                       {
                                           if (event_bus_ != nullptr)
                                           {
                                               event_bus_->publish(
                                                   core::events::DeleteLineRequestEvent{});
                                           }
                                       }});
    command_palette_->RegisterCommand({"Word Wrap",
                                       "View",
                                       shortcut_manager_.get_shortcut_text("view.word_wrap"),
                                       [this]()
                                       {
                                           if (event_bus_ != nullptr)
                                           {
                                               event_bus_->publish(
                                                   core::events::WrapToggleRequestEvent{});
                                           }
                                       }});

    // ── R7 palette commands ──
    auto reg_r7 = [this](const char* name, const char* cat, const char* sc_id, auto make_event)
    {
        command_palette_->RegisterCommand({name,
                                           cat,
                                           shortcut_manager_.get_shortcut_text(sc_id),
                                           [this, make_event]()
                                           {
                                               if (event_bus_ != nullptr)
                                               {
                                                   event_bus_->publish(make_event());
                                               }
                                           }});
    };
    reg_r7("Move Line Up",
           "Edit",
           "edit.move_line_up",
           []() { return core::events::MoveLineUpRequestEvent{}; });
    reg_r7("Move Line Down",
           "Edit",
           "edit.move_line_down",
           []() { return core::events::MoveLineDownRequestEvent{}; });
    reg_r7("Join Lines",
           "Edit",
           "edit.join_lines",
           []() { return core::events::JoinLinesRequestEvent{}; });
    reg_r7("Sort Lines Ascending",
           "Edit",
           "",
           []() { return core::events::SortLinesAscRequestEvent{}; });
    reg_r7("Sort Lines Descending",
           "Edit",
           "",
           []() { return core::events::SortLinesDescRequestEvent{}; });
    reg_r7("Transform to Uppercase",
           "Edit",
           "",
           []() { return core::events::TransformUpperRequestEvent{}; });
    reg_r7("Transform to Lowercase",
           "Edit",
           "",
           []() { return core::events::TransformLowerRequestEvent{}; });
    reg_r7("Transform to Title Case",
           "Edit",
           "",
           []() { return core::events::TransformTitleRequestEvent{}; });
    reg_r7("Select All Occurrences",
           "Edit",
           "edit.select_all_occurrences",
           []() { return core::events::SelectAllOccurrencesRequestEvent{}; });
    reg_r7("Expand Line Selection",
           "Edit",
           "edit.expand_line_selection",
           []() { return core::events::ExpandLineSelectionRequestEvent{}; });
    reg_r7("Insert Line Above",
           "Edit",
           "edit.insert_line_above",
           []() { return core::events::InsertLineAboveRequestEvent{}; });
    reg_r7("Insert Line Below",
           "Edit",
           "edit.insert_line_below",
           []() { return core::events::InsertLineBelowRequestEvent{}; });
    reg_r7("Fold All Regions", "View", "", []() { return core::events::FoldAllRequestEvent{}; });
    reg_r7(
        "Unfold All Regions", "View", "", []() { return core::events::UnfoldAllRequestEvent{}; });
    reg_r7("Toggle Line Numbers",
           "View",
           "",
           []() { return core::events::ToggleLineNumbersRequestEvent{}; });
    reg_r7("Toggle Whitespace",
           "View",
           "",
           []() { return core::events::ToggleWhitespaceRequestEvent{}; });
    command_palette_->RegisterCommand(
        {"Welcome Screen", "Help", "", [this]() { showStartupScreen(); }});

    // ── R8 palette commands ──
    auto reg_r8 = [this](const char* name, const char* cat, const char* sc_id, auto make_event)
    {
        command_palette_->RegisterCommand({name,
                                           cat,
                                           shortcut_manager_.get_shortcut_text(sc_id),
                                           [this, make_event]()
                                           {
                                               if (event_bus_ != nullptr)
                                               {
                                                   event_bus_->publish(make_event());
                                               }
                                           }});
    };
    reg_r8("Copy Line Up",
           "Edit",
           "edit.copy_line_up",
           []() { return core::events::CopyLineUpRequestEvent{}; });
    reg_r8("Copy Line Down",
           "Edit",
           "edit.copy_line_down",
           []() { return core::events::CopyLineDownRequestEvent{}; });
    reg_r8(
        "Delete All Left", "Edit", "", []() { return core::events::DeleteAllLeftRequestEvent{}; });
    reg_r8("Delete All Right",
           "Edit",
           "",
           []() { return core::events::DeleteAllRightRequestEvent{}; });
    reg_r8("Reverse Lines", "Edit", "", []() { return core::events::ReverseLinesRequestEvent{}; });
    reg_r8("Delete Duplicate Lines",
           "Edit",
           "",
           []() { return core::events::DeleteDuplicateLinesRequestEvent{}; });
    reg_r8("Transpose Characters",
           "Edit",
           "edit.transpose_chars",
           []() { return core::events::TransposeCharsRequestEvent{}; });
    reg_r8("Indent Selection",
           "Edit",
           "edit.indent_selection",
           []() { return core::events::IndentSelectionRequestEvent{}; });
    reg_r8("Outdent Selection",
           "Edit",
           "edit.outdent_selection",
           []() { return core::events::OutdentSelectionRequestEvent{}; });
    reg_r8("Select Word",
           "Edit",
           "edit.select_word",
           []() { return core::events::SelectWordRequestEvent{}; });
    reg_r8("Select Paragraph",
           "Edit",
           "",
           []() { return core::events::SelectParagraphRequestEvent{}; });
    reg_r8("Toggle Read-Only",
           "Edit",
           "",
           []() { return core::events::ToggleReadOnlyRequestEvent{}; });
    reg_r8("Convert Indentation to Spaces",
           "Edit",
           "",
           []() { return core::events::ConvertIndentSpacesRequestEvent{}; });
    reg_r8("Convert Indentation to Tabs",
           "Edit",
           "",
           []() { return core::events::ConvertIndentTabsRequestEvent{}; });
    reg_r8("Jump to Matching Bracket",
           "Edit",
           "edit.jump_to_bracket",
           []() { return core::events::JumpToBracketRequestEvent{}; });
    reg_r8(
        "Toggle Minimap", "View", "", []() { return core::events::ToggleMinimapRequestEvent{}; });
    reg_r8("Fold Current Region",
           "View",
           "view.fold_current",
           []() { return core::events::FoldCurrentRequestEvent{}; });
    reg_r8("Unfold Current Region",
           "View",
           "view.unfold_current",
           []() { return core::events::UnfoldCurrentRequestEvent{}; });
    reg_r8("Add Line Comment",
           "Edit",
           "",
           []() { return core::events::AddLineCommentRequestEvent{}; });
    reg_r8("Remove Line Comment",
           "Edit",
           "",
           []() { return core::events::RemoveLineCommentRequestEvent{}; });

    // ── R9 palette commands ──
    auto reg_r9 = [this](const char* name, const char* cat, const char* sc_id, auto make_event)
    {
        command_palette_->RegisterCommand({name,
                                           cat,
                                           shortcut_manager_.get_shortcut_text(sc_id),
                                           [this, make_event]()
                                           {
                                               if (event_bus_ != nullptr)
                                               {
                                                   event_bus_->publish(make_event());
                                               }
                                           }});
    };
    reg_r9("Trim Trailing Whitespace",
           "Edit",
           "",
           []() { return core::events::TrimTrailingWSRequestEvent{}; });
    reg_r9("Expand Selection",
           "Edit",
           "edit.expand_selection",
           []() { return core::events::ExpandSelectionRequestEvent{}; });
    reg_r9("Shrink Selection",
           "Edit",
           "edit.shrink_selection",
           []() { return core::events::ShrinkSelectionRequestEvent{}; });
    reg_r9("Cursor Undo",
           "Edit",
           "edit.cursor_undo",
           []() { return core::events::CursorUndoRequestEvent{}; });
    reg_r9("Cursor Redo",
           "Edit",
           "edit.cursor_redo",
           []() { return core::events::CursorRedoRequestEvent{}; });
    reg_r9("Move Text Left", "Edit", "", []() { return core::events::MoveTextLeftRequestEvent{}; });
    reg_r9(
        "Move Text Right", "Edit", "", []() { return core::events::MoveTextRightRequestEvent{}; });
    reg_r9("Toggle Auto-Indent",
           "View",
           "",
           []() { return core::events::ToggleAutoIndentRequestEvent{}; });
    reg_r9("Toggle Bracket Matching",
           "View",
           "",
           []() { return core::events::ToggleBracketMatchingRequestEvent{}; });
    reg_r9("Toggle Code Folding",
           "View",
           "",
           []() { return core::events::ToggleCodeFoldingRequestEvent{}; });
    reg_r9("Toggle Indentation Guides",
           "View",
           "",
           []() { return core::events::ToggleIndentGuidesRequestEvent{}; });
    reg_r9("Select to Matching Bracket",
           "Edit",
           "",
           []() { return core::events::SelectToBracketRequestEvent{}; });
    reg_r9("Toggle Block Comment",
           "Edit",
           "edit.block_comment",
           []() { return core::events::ToggleBlockCommentRequestEvent{}; });
    reg_r9("Insert Date/Time",
           "Edit",
           "",
           []() { return core::events::InsertDateTimeRequestEvent{}; });
    reg_r9("Bold", "Format", "format.bold", []() { return core::events::BoldRequestEvent{}; });
    reg_r9(
        "Italic", "Format", "format.italic", []() { return core::events::ItalicRequestEvent{}; });
    reg_r9("Inline Code",
           "Format",
           "format.inline_code",
           []() { return core::events::InlineCodeRequestEvent{}; });
    reg_r9("Blockquote",
           "Format",
           "format.blockquote",
           []() { return core::events::BlockquoteRequestEvent{}; });
    reg_r9("Cycle Heading",
           "Format",
           "format.cycle_heading",
           []() { return core::events::CycleHeadingRequestEvent{}; });
    reg_r9("Insert Table", "Format", "", []() { return core::events::InsertTableRequestEvent{}; });

    // ── R10 palette commands ──
    auto reg_r10 = [this](const char* name, const char* cat, const char* sc_id, auto make_event)
    {
        command_palette_->RegisterCommand({name,
                                           cat,
                                           shortcut_manager_.get_shortcut_text(sc_id),
                                           [this, make_event]()
                                           {
                                               if (event_bus_ != nullptr)
                                               {
                                                   event_bus_->publish(make_event());
                                               }
                                           }});
    };
    reg_r10("Toggle Smart List Continuation",
            "Format",
            "",
            []() { return core::events::ToggleSmartListContinuationRequestEvent{}; });
    reg_r10("Close Other Tabs",
            "File",
            "",
            []() { return core::events::CloseOtherTabsRequestEvent{}; });
    reg_r10("Close Saved Tabs",
            "File",
            "",
            []() { return core::events::CloseSavedTabsRequestEvent{}; });
    reg_r10("Insert Link",
            "Format",
            "format.insert_link",
            []() { return core::events::InsertLinkRequestEvent{}; });
    reg_r10("Add Cursor Below",
            "Edit",
            "edit.cursor_below",
            []() { return core::events::AddCursorBelowRequestEvent{}; });
    reg_r10("Add Cursor Above",
            "Edit",
            "edit.cursor_above",
            []() { return core::events::AddCursorAboveRequestEvent{}; });
    reg_r10("Add Cursor at Next Occurrence",
            "Edit",
            "",
            []() { return core::events::AddCursorNextOccurrenceRequestEvent{}; });
    reg_r10("Remove Surrounding Brackets",
            "Edit",
            "",
            []() { return core::events::RemoveSurroundingBracketsRequestEvent{}; });
    reg_r10("Duplicate Selection or Line",
            "Edit",
            "",
            []() { return core::events::DuplicateSelectionOrLineRequestEvent{}; });
    reg_r10("Show Table Editor",
            "Format",
            "",
            []() { return core::events::ShowTableEditorRequestEvent{}; });
    reg_r10("Toggle Scroll Beyond Last Line",
            "View",
            "",
            []() { return core::events::ToggleScrollBeyondLastLineRequestEvent{}; });
    reg_r10("Toggle Highlight Current Line",
            "View",
            "",
            []() { return core::events::ToggleHighlightCurrentLineRequestEvent{}; });
    reg_r10("Toggle Auto-Closing Brackets",
            "View",
            "",
            []() { return core::events::ToggleAutoClosingBracketsRequestEvent{}; });
    reg_r10("Toggle Sticky Scroll",
            "View",
            "",
            []() { return core::events::ToggleStickyScrollRequestEvent{}; });
    reg_r10("Toggle Font Ligatures",
            "View",
            "",
            []() { return core::events::ToggleFontLigaturesRequestEvent{}; });
    reg_r10("Toggle Smooth Caret",
            "View",
            "",
            []() { return core::events::ToggleSmoothCaretRequestEvent{}; });
    reg_r10("Toggle Inline Color Preview",
            "View",
            "",
            []() { return core::events::ToggleInlineColorPreviewRequestEvent{}; });
    reg_r10("Toggle Edge Column Ruler",
            "View",
            "",
            []() { return core::events::ToggleEdgeColumnRulerRequestEvent{}; });
    reg_r10("Ensure Final Newline",
            "Edit",
            "",
            []() { return core::events::EnsureFinalNewlineRequestEvent{}; });
    reg_r10(
        "Insert Snippet", "Format", "", []() { return core::events::InsertSnippetRequestEvent{}; });

    // ── R11 palette commands ──
    auto reg_r11 = [this](const char* name, const char* cat, auto make_event)
    {
        command_palette_->RegisterCommand({name,
                                           cat,
                                           "",
                                           [this, make_event]()
                                           {
                                               if (event_bus_ != nullptr)
                                               {
                                                   event_bus_->publish(make_event());
                                               }
                                           }});
    };
    reg_r11("Toggle Smooth Scrolling",
            "View",
            []() { return core::events::ToggleSmoothScrollingRequestEvent{}; });
    reg_r11("Toggle Trailing WS Highlight",
            "View",
            []() { return core::events::ToggleTrailingWSHighlightRequestEvent{}; });
    reg_r11("Toggle Auto-Trim Trailing WS",
            "View",
            []() { return core::events::ToggleAutoTrimWSRequestEvent{}; });
    reg_r11("Toggle Gutter Separator",
            "View",
            []() { return core::events::ToggleGutterSeparatorRequestEvent{}; });
    reg_r11("Toggle Insert Final Newline",
            "View",
            []() { return core::events::ToggleInsertFinalNewlineRequestEvent{}; });
    reg_r11("Toggle Whitespace Boundary",
            "View",
            []() { return core::events::ToggleWhitespaceBoundaryRequestEvent{}; });
    reg_r11("Toggle Link Auto-Complete",
            "View",
            []() { return core::events::ToggleLinkAutoCompleteRequestEvent{}; });
    reg_r11(
        "Toggle Drag-Drop", "View", []() { return core::events::ToggleDragDropRequestEvent{}; });
    reg_r11(
        "Toggle Auto-Save", "View", []() { return core::events::ToggleAutoSaveRequestEvent{}; });
    reg_r11("Toggle Empty Selection Clipboard",
            "View",
            []() { return core::events::ToggleEmptySelClipboardRequestEvent{}; });
    reg_r11("Cycle Render Whitespace",
            "View",
            []() { return core::events::CycleRenderWhitespaceRequestEvent{}; });
    reg_r11("Delete Current Line", "Edit", []() { return core::events::DeleteLineRequestEvent{}; });
    reg_r11("Copy Line (No Selection)",
            "Edit",
            []() { return core::events::CopyLineNoSelRequestEvent{}; });
    reg_r11("Add Selection to Next Match",
            "Edit",
            []() { return core::events::AddSelNextMatchRequestEvent{}; });
    reg_r11("Smart Backspace", "Edit", []() { return core::events::SmartBackspaceRequestEvent{}; });
    reg_r11("Hide Table Editor",
            "Format",
            []() { return core::events::HideTableEditorRequestEvent{}; });
    reg_r11(
        "Auto-Pair Bold (**)", "Format", []() { return core::events::AutoPairBoldRequestEvent{}; });
    reg_r11("Auto-Pair Italic (*)",
            "Format",
            []() { return core::events::AutoPairItalicRequestEvent{}; });
    reg_r11(
        "Auto-Pair Code (`)", "Format", []() { return core::events::AutoPairCodeRequestEvent{}; });
    reg_r11("Toggle Minimap", "View", []() { return core::events::ToggleMinimapRequestEvent{}; });

    // ── R12 palette commands ──
    auto reg_r12 = [this](const char* name, const char* cat, auto make_event)
    {
        command_palette_->RegisterCommand({name,
                                           cat,
                                           "",
                                           [this, make_event]()
                                           {
                                               if (event_bus_ != nullptr)
                                               {
                                                   event_bus_->publish(make_event());
                                               }
                                           }});
    };
    reg_r12(
        "Close Tabs to Left", "File", []() { return core::events::CloseTabsToLeftRequestEvent{}; });
    reg_r12("Close Tabs to Right",
            "File",
            []() { return core::events::CloseTabsToRightRequestEvent{}; });
    reg_r12("Pin Current Tab", "File", []() { return core::events::PinTabRequestEvent{}; });
    reg_r12("Unpin Current Tab", "File", []() { return core::events::UnpinTabRequestEvent{}; });

    // ── R13 palette commands ──
    auto reg_r13 = [this](const char* name, const char* cat, auto make_event)
    {
        command_palette_->RegisterCommand({name,
                                           cat,
                                           "",
                                           [this, make_event]()
                                           {
                                               if (event_bus_ != nullptr)
                                               {
                                                   event_bus_->publish(make_event());
                                               }
                                           }});
    };
    reg_r13("Copy File Path", "File", []() { return core::events::CopyFilePathRequestEvent{}; });
    reg_r13(
        "Reveal in Finder", "File", []() { return core::events::RevealInFinderRequestEvent{}; });
    reg_r13("Zoom In", "View", []() { return core::events::ZoomInRequestEvent{}; });
    reg_r13("Zoom Out", "View", []() { return core::events::ZoomOutRequestEvent{}; });
    reg_r13("Reset Zoom", "View", []() { return core::events::ZoomResetRequestEvent{}; });
    reg_r13("Convert Line Endings to LF",
            "Edit",
            []() { return core::events::ConvertEolLfRequestEvent{}; });
    reg_r13("Convert Line Endings to CRLF",
            "Edit",
            []() { return core::events::ConvertEolCrlfRequestEvent{}; });
    reg_r13("Print...", "File", []() { return core::events::PrintDocumentRequestEvent{}; });

    // ── R14 palette commands ──
    auto reg_r14 = [this](const char* name, const char* cat, auto make_event)
    {
        command_palette_->RegisterCommand({name,
                                           cat,
                                           "",
                                           [this, make_event]()
                                           {
                                               if (event_bus_ != nullptr)
                                               {
                                                   event_bus_->publish(make_event());
                                               }
                                           }});
    };
    reg_r14(
        "Fold Current Region", "Edit", []() { return core::events::FoldCurrentRequestEvent{}; });
    reg_r14("Unfold Current Region",
            "Edit",
            []() { return core::events::UnfoldCurrentRequestEvent{}; });
    reg_r14("Transpose Characters",
            "Edit",
            []() { return core::events::TransposeCharsRequestEvent{}; });
    reg_r14("Reverse Selected Lines",
            "Edit",
            []() { return core::events::ReverseLinesRequestEvent{}; });
    reg_r14(
        "Sort Lines Ascending", "Edit", []() { return core::events::SortLinesAscRequestEvent{}; });
    reg_r14(
        "Reload File from Disk", "File", []() { return core::events::FileReloadRequestEvent{}; });

    // ── R15 palette commands ──
    auto reg_r15 = [this](const char* name, const char* cat, auto make_event)
    {
        command_palette_->RegisterCommand({name,
                                           cat,
                                           "",
                                           [this, make_event]()
                                           {
                                               if (event_bus_ != nullptr)
                                               {
                                                   event_bus_->publish(make_event());
                                               }
                                           }});
    };
    // Fix 10: New File palette command
    command_palette_->RegisterCommand(
        {"New File",
         "File",
         "",
         [this]()
         {
             showEditor();
             if (layout_ != nullptr)
             {
                 static int untitled_idx = 100;
                 std::string path = "Untitled-" + std::to_string(untitled_idx++) + ".md";
                 layout_->OpenFileInTab(path);
             }
         }});
    reg_r15("Sort Lines Descending",
            "Edit",
            []() { return core::events::SortLinesDescRequestEvent{}; });
    reg_r15("Remove Duplicate Lines",
            "Edit",
            []() { return core::events::DeleteDuplicateLinesRequestEvent{}; });
    reg_r15(
        "Select Line", "Edit", []() { return core::events::ExpandLineSelectionRequestEvent{}; });
    reg_r15("Jump to Matching Bracket",
            "Edit",
            []() { return core::events::JumpToBracketRequestEvent{}; });
    reg_r15("Select to Matching Bracket",
            "Edit",
            []() { return core::events::SelectToBracketRequestEvent{}; });

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
        SetTitle(wxString::Format(
            "\u25CF %s \u2014 %s \u2014 MarkAmp", filename, workspace_folder_name_));
    }
    else
    {
        SetTitle(wxString::Format("%s \u2014 %s \u2014 MarkAmp", filename, workspace_folder_name_));
    }

    // Fix 18: Update status bar with active filename
    if (layout_ != nullptr)
    {
        auto* status_bar = layout_->statusbar_container();
        if (status_bar != nullptr)
        {
            status_bar->set_ready_state(filename);
        }
    }
}

} // namespace markamp::ui
