#include "StartupPanel.h"

#include "core/EventBus.h"
#include "core/Events.h" // Will need to accept/define events here later
#include "core/Logger.h"
#include "core/RecentWorkspaces.h"
#include "core/ThemeEngine.h"

#include <wx/dcclient.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/stdpaths.h>
#include <wx/wrapsizer.h>

namespace markamp::ui
{

namespace
{
// Helper to finding the icon - duplicating logic from MainFrame for now
// In a real refactor, this should be in a generic utility.
wxString findIconPath()
{
    const wxString icon_filename = "markamp.png";
#ifdef __WXOSX__
    wxString bundle_path = wxStandardPaths::Get().GetResourcesDir() + "/icons/" + icon_filename;
    if (wxFileName::FileExists(bundle_path))
        return bundle_path;
#endif
    wxFileName exe_path(wxStandardPaths::Get().GetExecutablePath());
    wxString exe_dir = exe_path.GetPath();
    wxString build_path = exe_dir + "/../../../resources/icons/" + icon_filename;
    if (wxFileName::FileExists(build_path))
        return build_path;
    wxString cmake_copy_path = exe_dir + "/../resources/icons/" + icon_filename;
    if (wxFileName::FileExists(cmake_copy_path))
        return cmake_copy_path;
    wxString direct_path = "resources/icons/" + icon_filename;
    if (wxFileName::FileExists(direct_path))
        return direct_path;
    return {};
}
} // namespace

StartupPanel::StartupPanel(wxWindow* parent,
                           markamp::core::EventBus* event_bus,
                           markamp::core::RecentWorkspaces* recent_workspaces,
                           markamp::core::ThemeEngine* theme_engine)
    : wxPanel(parent)
    , event_bus_(event_bus)
    , recent_workspaces_(recent_workspaces)
    , theme_engine_(theme_engine)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT); // For custom theming
    initUi();

    Bind(wxEVT_PAINT, &StartupPanel::onPaint, this);

    if (event_bus_ != nullptr)
    {
        theme_sub_ = event_bus_->subscribe<core::events::ThemeChangedEvent>(
            [this](const core::events::ThemeChangedEvent& /*evt*/) { Refresh(); });
    }
}

void StartupPanel::initUi()
{
    main_sizer_ = new wxBoxSizer(wxVERTICAL);

    // Spacer top
    main_sizer_->AddStretchSpacer(1);

    // Logo
    createLogo(main_sizer_);
    main_sizer_->AddSpacer(40);

    // Buttons
    createButtons(main_sizer_);
    main_sizer_->AddSpacer(40);

    // Recent Workspaces
    createRecentList(main_sizer_);

    // Spacer bottom
    main_sizer_->AddStretchSpacer(2);

    SetSizer(main_sizer_);
}

void StartupPanel::createLogo(wxSizer* parent)
{
    wxString icon_path = findIconPath();
    if (icon_path.empty())
        return;

    wxImage image(icon_path);
    if (!image.IsOk())
        return;

    // Scale up for splash
    image.Rescale(128, 128, wxIMAGE_QUALITY_HIGH);
    auto* bitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap(image));
    parent->Add(bitmap, 0, wxALIGN_CENTER_HORIZONTAL);

    // R6 Fix 19: Version label below logo
    auto version_str = wxString::Format(
        "v%d.%d.%d", MARKAMP_VERSION_MAJOR, MARKAMP_VERSION_MINOR, MARKAMP_VERSION_PATCH);
    auto* version_label = new wxStaticText(this, wxID_ANY, version_str);
    if (theme_engine_ != nullptr)
    {
        version_label->SetFont(
            theme_engine_->font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
        version_label->SetForegroundColour(theme_engine_->color(core::ThemeColorToken::TextMuted));
    }
    parent->Add(version_label, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 8);
}

void StartupPanel::createButtons(wxSizer* parent)
{
    auto* button_sizer = new wxBoxSizer(wxHORIZONTAL);

    // Determine colors
    wxColour accent_bg(100, 60, 140); // Default purple
    wxColour text_fg(*wxWHITE);
    if (theme_engine_ != nullptr)
    {
        accent_bg = theme_engine_->color(core::ThemeColorToken::AccentPrimary);
    }

    // Font for button labels
    wxFont btn_font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    // Helper lambda to create a custom-drawn button panel
    auto make_button = [&](const wxString& label) -> wxPanel*
    {
        auto* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(220, 48));
        panel->SetMinSize(wxSize(220, 48));
        panel->SetBackgroundStyle(wxBG_STYLE_PAINT);
        panel->SetCursor(wxCursor(wxCURSOR_HAND));

        // Proper paint using the panel pointer directly
        panel->Bind(wxEVT_PAINT,
                    [panel, accent_bg, text_fg, btn_font, label](wxPaintEvent&)
                    {
                        wxPaintDC paint_dc(panel);
                        const wxSize size = panel->GetClientSize();

                        // Fill with accent color
                        paint_dc.SetBrush(wxBrush(accent_bg));
                        paint_dc.SetPen(*wxTRANSPARENT_PEN);
                        paint_dc.DrawRoundedRectangle(0, 0, size.GetWidth(), size.GetHeight(), 6);

                        // Draw centered text
                        paint_dc.SetFont(btn_font);
                        paint_dc.SetTextForeground(text_fg);
                        const wxSize text_size = paint_dc.GetTextExtent(label);
                        const int text_x = (size.GetWidth() - text_size.GetWidth()) / 2;
                        const int text_y = (size.GetHeight() - text_size.GetHeight()) / 2;
                        paint_dc.DrawText(label, text_x, text_y);
                    });

        return panel;
    };

    auto* open_folder_panel = make_button("Open Folder  (\u2318O)");
    auto* open_repo_panel = make_button("Open Repository");

    // Bind click events
    open_folder_panel->Bind(wxEVT_LEFT_DOWN,
                            [this](wxMouseEvent&)
                            {
                                wxCommandEvent evt(wxEVT_BUTTON);
                                onOpenFolder(evt);
                            });
    open_repo_panel->Bind(wxEVT_LEFT_DOWN,
                          [this](wxMouseEvent&)
                          {
                              wxCommandEvent evt(wxEVT_BUTTON);
                              onOpenRepository(evt);
                          });

    button_sizer->Add(open_folder_panel, 1, wxRIGHT, 10);
    button_sizer->Add(open_repo_panel, 1, wxLEFT, 10);

    // Center the button row
    parent->Add(button_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 40);
}

void StartupPanel::createRecentList(wxSizer* parent)
{
    if (!recent_workspaces_)
    {
        return;
    }

    // Outer container to hold label + list, centered on screen
    auto* section_sizer = new wxBoxSizer(wxVERTICAL);

    // "Workspaces" heading — left-aligned within the centered container
    auto* label = new wxStaticText(this, wxID_ANY, "Workspaces");
    wxFont heading_font = label->GetFont();
    heading_font.SetPointSize(14);
    heading_font.SetWeight(wxFONTWEIGHT_BOLD);
    if (theme_engine_ != nullptr)
    {
        label->SetForegroundColour(theme_engine_->color(core::ThemeColorToken::TextMain));
        heading_font = theme_engine_->font(core::ThemeFontToken::UIHeading);
        heading_font.SetPointSize(14);
        heading_font.SetWeight(wxFONTWEIGHT_BOLD);
    }
    label->SetFont(heading_font);
    section_sizer->Add(label, 0, wxBOTTOM, 12);

    // List container (transparent background, holds workspace items)
    recent_list_container_ = new wxPanel(this, wxID_ANY);
    recent_list_container_->SetBackgroundStyle(wxBG_STYLE_PAINT);
    recent_list_sizer_ = new wxBoxSizer(wxVERTICAL);
    recent_list_container_->SetSizer(recent_list_sizer_);

    section_sizer->Add(recent_list_container_, 0, wxEXPAND);

    // Set a fixed width matching the button row and center it all
    recent_list_container_->SetMinSize(wxSize(460, -1));
    parent->Add(section_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 40);
}

void StartupPanel::refreshRecentWorkspaces()
{
    if (recent_list_sizer_ == nullptr || recent_workspaces_ == nullptr)
    {
        return;
    }

    recent_list_sizer_->Clear(true); // Delete old children

    const auto& list = recent_workspaces_->list();
    int count = 0;
    for (const auto& path : list)
    {
        if (count++ >= 3)
            break; // Mockup shows 3, implementation plan helper logic says 3 here?
        // Task said "list of the last opened areas so we can quickly open the last 3 prior".

        auto* item_panel = new wxPanel(recent_list_container_, wxID_ANY);

        wxColour item_bg(40, 40, 50);
        if (theme_engine_ != nullptr)
        {
            item_bg = theme_engine_->color(core::ThemeColorToken::BgPanel);
        }
        item_panel->SetBackgroundColour(item_bg);

        auto* item_sizer = new wxBoxSizer(wxVERTICAL);

        auto name = path.filename().string();
        if (name.empty())
            name = path.string();

        auto* name_txt = new wxStaticText(item_panel, wxID_ANY, name);
        // Make name bold/larger
        wxFont nameFont = wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        if (theme_engine_ != nullptr)
        {
            name_txt->SetForegroundColour(theme_engine_->color(core::ThemeColorToken::TextMain));
            // name_txt->SetFont(theme_engine_->font(core::ThemeFontToken::UIHeading)); // Use theme
            // font if available, else fallback
            nameFont = theme_engine_->font(core::ThemeFontToken::UIHeading);
        }
        else
        {
            name_txt->SetForegroundColour(*wxWHITE);
        }
        name_txt->SetFont(nameFont);

        auto* path_txt = new wxStaticText(item_panel, wxID_ANY, path.string());
        if (theme_engine_ != nullptr)
        {
            path_txt->SetForegroundColour(theme_engine_->color(core::ThemeColorToken::TextMuted));
            path_txt->SetFont(theme_engine_->font(core::ThemeFontToken::UISmall));
        }
        else
        {
            path_txt->SetForegroundColour(wxColour(150, 150, 150));
        }

        item_sizer->Add(name_txt, 0, wxEXPAND | wxLEFT | wxTOP | wxRIGHT, 10);
        item_sizer->Add(path_txt, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 10);

        item_panel->SetSizer(item_sizer);

        // Click handler
        item_panel->Bind(wxEVT_LEFT_DOWN, [this, path](wxMouseEvent&) { onWorkspaceClick(path); });
        name_txt->Bind(wxEVT_LEFT_DOWN, [this, path](wxMouseEvent&) { onWorkspaceClick(path); });
        path_txt->Bind(wxEVT_LEFT_DOWN, [this, path](wxMouseEvent&) { onWorkspaceClick(path); });

        // R17 Fix 36: Hover highlight on workspace items
        item_panel->SetCursor(wxCursor(wxCURSOR_HAND));
        item_panel->Bind(wxEVT_ENTER_WINDOW,
                         [item_panel, item_bg](wxMouseEvent&)
                         {
                             item_panel->SetBackgroundColour(item_bg.ChangeLightness(120));
                             item_panel->Refresh();
                         });
        item_panel->Bind(wxEVT_LEAVE_WINDOW,
                         [item_panel, item_bg](wxMouseEvent&)
                         {
                             item_panel->SetBackgroundColour(item_bg);
                             item_panel->Refresh();
                         });

        // Add to list sizer with spacing
        recent_list_sizer_->Add(item_panel, 0, wxEXPAND | wxBOTTOM, 10);
    }

    recent_list_container_->Layout();
}

void StartupPanel::onOpenFolder(wxCommandEvent& /*event*/)
{
    MARKAMP_LOG_INFO("StartupPanel: Open Folder clicked");
    if (event_bus_ != nullptr)
    {
        core::events::OpenFolderRequestEvent evt;
        event_bus_->publish(evt);
    }
}

// ReSharper disable once CppMemberFunctionMayBeStatic
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void StartupPanel::onOpenRepository(wxCommandEvent& /*event*/)
{
    wxTextEntryDialog dialog(this,
                             "Enter a GitHub or GitLab repository URL:",
                             "Open Remote Repository",
                             "https://github.com/");
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString url = dialog.GetValue().Trim();
        if (!url.IsEmpty())
        {
            wxLaunchDefaultBrowser(url);
        }
    }
}

void StartupPanel::onWorkspaceClick(const std::filesystem::path& path)
{
    MARKAMP_LOG_INFO("StartupPanel: Opening workspace {}", path.string());
    if (event_bus_ != nullptr)
    {
        core::events::WorkspaceOpenRequestEvent evt;
        evt.path = path.string();
        event_bus_->publish(evt);
    }
}

void StartupPanel::onPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC dc(this);

    wxColour bg = wxColour(20, 20, 30); // Default dark
    if (theme_engine_ != nullptr)
    {
        bg = theme_engine_->color(core::ThemeColorToken::BgApp);
    }

    dc.SetBackground(wxBrush(bg));
    dc.Clear();

    // retro-futuristic grid (subtle)
    wxColour grid_color(255, 255, 255, 10); // Very faint white
    if (theme_engine_ != nullptr)
    {
        // Use a derived color if possible, or just hardcode for now
        grid_color = theme_engine_->color(core::ThemeColorToken::BorderDark);
        // Make it very transparent
        grid_color = wxColour(grid_color.Red(), grid_color.Green(), grid_color.Blue(), 20);
    }

    dc.SetPen(wxPen(grid_color, 1));

    wxSize size = GetClientSize();
    int step = 40;

    for (int x = 0; x < size.GetWidth(); x += step)
        dc.DrawLine(x, 0, x, size.GetHeight());

    for (int y = 0; y < size.GetHeight(); y += step)
        dc.DrawLine(0, y, size.GetWidth(), y);
}

} // namespace markamp::ui
