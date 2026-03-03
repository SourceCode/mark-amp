#include "ExtensionDetailPanel.h"

namespace markamp::ui
{

ExtensionDetailPanel::ExtensionDetailPanel(wxWindow* parent,
                                           core::ThemeEngine& theme_engine,
                                           core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    CreateLayout();
    ApplyTheme();
}

void ExtensionDetailPanel::CreateLayout()
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Navigation bar: back button
    auto* nav_sizer = new wxBoxSizer(wxHORIZONTAL);
    back_button_ = new wxButton(
        this, wxID_ANY, "\xE2\x86\x90 Back", wxDefaultPosition, wxSize(80, 28), wxBORDER_NONE);
    back_button_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
    back_button_->Bind(wxEVT_BUTTON,
                       [this](wxCommandEvent& /*evt*/)
                       {
                           if (on_back_)
                           {
                               on_back_();
                           }
                       });
    nav_sizer->AddSpacer(8);
    nav_sizer->Add(back_button_, 0, wxALIGN_CENTER_VERTICAL);
    main_sizer->Add(nav_sizer, 0, wxEXPAND | wxTOP | wxBOTTOM, 6);

    // Header: title + publisher + version
    auto* header_sizer = new wxBoxSizer(wxVERTICAL);

    title_label_ = new wxStaticText(this, wxID_ANY, "");
    title_label_->SetFont(
        theme_engine_.font(core::ThemeFontToken::MonoRegular).Bold().Scaled(1.15F));

    auto* meta_sizer = new wxBoxSizer(wxHORIZONTAL);
    publisher_label_ = new wxStaticText(this, wxID_ANY, "");
    publisher_label_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.90F));

    version_label_ = new wxStaticText(this, wxID_ANY, "");
    version_label_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));

    meta_sizer->Add(publisher_label_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    meta_sizer->Add(version_label_, 0, wxALIGN_CENTER_VERTICAL);

    header_sizer->Add(title_label_, 0, wxLEFT, 12);
    header_sizer->Add(meta_sizer, 0, wxLEFT | wxTOP, 12);

    main_sizer->Add(header_sizer, 0, wxEXPAND | wxBOTTOM, 8);

    // Action buttons row
    auto* action_sizer = new wxBoxSizer(wxHORIZONTAL);
    action_sizer->AddSpacer(12);

    install_button_ =
        new wxButton(this, wxID_ANY, "Install", wxDefaultPosition, wxSize(90, 30), wxBORDER_NONE);
    install_button_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.90F));
    install_button_->Bind(wxEVT_BUTTON,
                          [this](wxCommandEvent& /*evt*/)
                          {
                              if (on_install_)
                              {
                                  on_install_(current_extension_id_);
                              }
                          });

    uninstall_button_ =
        new wxButton(this, wxID_ANY, "Uninstall", wxDefaultPosition, wxSize(90, 30), wxBORDER_NONE);
    uninstall_button_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.90F));
    uninstall_button_->Bind(wxEVT_BUTTON,
                            [this](wxCommandEvent& /*evt*/)
                            {
                                if (on_uninstall_)
                                {
                                    on_uninstall_(current_extension_id_);
                                }
                            });

    update_button_ =
        new wxButton(this, wxID_ANY, "Update", wxDefaultPosition, wxSize(90, 30), wxBORDER_NONE);
    update_button_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.90F));
    update_button_->Bind(wxEVT_BUTTON,
                         [this](wxCommandEvent& /*evt*/)
                         {
                             if (on_update_)
                             {
                                 on_update_(current_extension_id_);
                             }
                         });

    action_sizer->Add(install_button_, 0, wxRIGHT, 6);
    action_sizer->Add(uninstall_button_, 0, wxRIGHT, 6);
    action_sizer->Add(update_button_, 0);

    main_sizer->Add(action_sizer, 0, wxEXPAND | wxBOTTOM, 8);

    // Phase 20 Task 4: Tab bar
    tab_bar_ = new wxPanel(this, wxID_ANY);
    tab_bar_->SetBackgroundColour(
        theme_engine_.color(core::ThemeColorToken::BgPanel).ChangeLightness(95));
    auto* tab_sizer = new wxBoxSizer(wxHORIZONTAL);

    auto make_tab = [this](const std::string& label, int tab_index) -> wxButton*
    {
        auto* btn = new wxButton(tab_bar_,
                                 wxID_ANY,
                                 label,
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 wxBORDER_NONE | wxBU_EXACTFIT);
        btn->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
        btn->SetMinSize(wxSize(-1, 28));
        btn->Bind(wxEVT_BUTTON,
                  [this, tab_index](wxCommandEvent& /*evt*/)
                  {
                      active_tab_ = tab_index;
                      // Show/hide panels based on active tab
                      if (overview_panel_ != nullptr)
                      {
                          overview_panel_->Show(active_tab_ == 0);
                      }
                      if (changelog_panel_ != nullptr)
                      {
                          changelog_panel_->Show(active_tab_ == 1);
                      }
                      if (deps_panel_ != nullptr)
                      {
                          deps_panel_->Show(active_tab_ == 2);
                      }

                      // Update tab button styling
                      auto accent = theme_engine_.color(core::ThemeColorToken::AccentPrimary);
                      auto muted = theme_engine_.color(core::ThemeColorToken::TextMuted);
                      if (tab_overview_ != nullptr)
                      {
                          tab_overview_->SetForegroundColour(active_tab_ == 0 ? accent : muted);
                      }
                      if (tab_changelog_ != nullptr)
                      {
                          tab_changelog_->SetForegroundColour(active_tab_ == 1 ? accent : muted);
                      }
                      if (tab_deps_ != nullptr)
                      {
                          tab_deps_->SetForegroundColour(active_tab_ == 2 ? accent : muted);
                      }

                      if (content_scroll_ != nullptr)
                      {
                          content_scroll_->Layout();
                          content_scroll_->FitInside();
                      }
                      Layout();
                      Refresh();
                  });
        return btn;
    };

    tab_overview_ = make_tab("Overview", 0);
    tab_changelog_ = make_tab("Changelog", 1);
    tab_deps_ = make_tab("Dependencies", 2);

    tab_sizer->AddSpacer(12);
    tab_sizer->Add(tab_overview_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 16);
    tab_sizer->Add(tab_changelog_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 16);
    tab_sizer->Add(tab_deps_, 0, wxALIGN_CENTER_VERTICAL);
    tab_bar_->SetSizer(tab_sizer);

    main_sizer->Add(tab_bar_, 0, wxEXPAND | wxBOTTOM, 2);

    // Separator line
    auto* separator = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 1));
    separator->SetBackgroundColour(
        theme_engine_.color(core::ThemeColorToken::TextMuted).ChangeLightness(60));
    main_sizer->Add(separator, 0, wxEXPAND | wxLEFT | wxRIGHT, 12);

    // Scrollable content area
    content_scroll_ = new wxScrolledWindow(this, wxID_ANY);
    content_scroll_->SetScrollRate(0, 10);
    auto* scroll_sizer = new wxBoxSizer(wxVERTICAL);

    // ── Overview panel (tab 0) ──
    overview_panel_ = new wxPanel(content_scroll_, wxID_ANY);
    auto* overview_sizer = new wxBoxSizer(wxVERTICAL);
    description_text_ = new wxStaticText(
        overview_panel_, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    description_text_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.90F));
    description_text_->Wrap(300);
    overview_sizer->Add(description_text_, 0, wxEXPAND | wxALL, 12);

    // Phase 20 Task 15: Runtime info section
    runtime_info_text_ = new wxStaticText(
        overview_panel_, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    runtime_info_text_->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
    runtime_info_text_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    overview_sizer->Add(runtime_info_text_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);

    overview_panel_->SetSizer(overview_sizer);
    scroll_sizer->Add(overview_panel_, 0, wxEXPAND);

    // ── Changelog panel (tab 1) ──
    changelog_panel_ = new wxPanel(content_scroll_, wxID_ANY);
    auto* changelog_sizer = new wxBoxSizer(wxVERTICAL);
    changelog_text_ = new wxStaticText(changelog_panel_,
                                       wxID_ANY,
                                       "No changelog available.",
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       wxST_NO_AUTORESIZE);
    changelog_text_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
    changelog_text_->Wrap(300);
    changelog_sizer->Add(changelog_text_, 0, wxEXPAND | wxALL, 12);
    changelog_panel_->SetSizer(changelog_sizer);
    changelog_panel_->Hide(); // Initially hidden
    scroll_sizer->Add(changelog_panel_, 0, wxEXPAND);

    // ── Dependencies panel (tab 2) ──
    deps_panel_ = new wxPanel(content_scroll_, wxID_ANY);
    auto* deps_sizer = new wxBoxSizer(wxVERTICAL);
    deps_header_ = new wxStaticText(deps_panel_, wxID_ANY, "Dependencies");
    deps_header_->SetFont(
        theme_engine_.font(core::ThemeFontToken::MonoRegular).Bold().Scaled(0.95F));
    deps_sizer->Add(deps_header_, 0, wxLEFT | wxTOP, 12);

    deps_text_ = new wxStaticText(deps_panel_, wxID_ANY, "None");
    deps_text_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
    deps_sizer->Add(deps_text_, 0, wxLEFT | wxTOP | wxBOTTOM, 12);

    deps_panel_->SetSizer(deps_sizer);
    deps_panel_->Hide(); // Initially hidden
    scroll_sizer->Add(deps_panel_, 0, wxEXPAND);

    content_scroll_->SetSizer(scroll_sizer);
    main_sizer->Add(content_scroll_, 1, wxEXPAND);

    SetSizer(main_sizer);

    // Initially hide all action buttons until an extension is shown
    install_button_->Hide();
    uninstall_button_->Hide();
    update_button_->Hide();

    // Set initial tab styling
    tab_overview_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::AccentPrimary));
    tab_changelog_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    tab_deps_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
}

void ExtensionDetailPanel::ShowExtension(const core::LocalExtension& extension, bool has_update)
{
    current_extension_id_ = extension.manifest.publisher + "." + extension.manifest.name;
    current_is_installed_ = true;
    current_has_update_ = has_update;

    title_label_->SetLabel(extension.manifest.name);
    publisher_label_->SetLabel(extension.manifest.publisher);
    version_label_->SetLabel("v" + extension.manifest.version);
    description_text_->SetLabel(extension.manifest.description);
    description_text_->Wrap(GetClientSize().GetWidth() - 24);

    // Phase 20 Task 15: Runtime info for installed extensions
    if (runtime_info_text_ != nullptr)
    {
        std::string info;
        info += "Runtime Information\n";
        info += "  Status: ";
        info += extension.enabled ? "Active" : "Disabled";
        info += "\n";
        info += "  Location: " + extension.location.string() + "\n";
        info += "  Built-in: ";
        info += extension.is_builtin ? "Yes" : "No";
        info += "\n";
        if (!extension.manifest.main.empty())
        {
            info += "  Entry Point: " + extension.manifest.main + "\n";
        }
        if (!extension.manifest.engines_vscode.empty())
        {
            info += "  Engine: " + extension.manifest.engines_vscode + "\n";
        }
        runtime_info_text_->SetLabel(info);
        runtime_info_text_->Show();
    }

    // Dependencies tab
    if (extension.manifest.extension_dependencies.empty())
    {
        deps_text_->SetLabel("None");
    }
    else
    {
        std::string deps_str;
        for (const auto& dep : extension.manifest.extension_dependencies)
        {
            if (!deps_str.empty())
            {
                deps_str += "\n";
            }
            deps_str += "  \xE2\x80\xA2 " + dep; // bullet
        }
        deps_text_->SetLabel(deps_str);
    }

    // Phase 20 Task 13: Changelog tab
    if (changelog_text_ != nullptr)
    {
        if (extension.manifest.changelog.empty())
        {
            changelog_text_->SetLabel("No changelog available.");
        }
        else
        {
            changelog_text_->SetLabel(extension.manifest.changelog);
            changelog_text_->Wrap(GetClientSize().GetWidth() - 24);
        }
    }

    // Phase 20 Task 21: Contributed commands — append to dependencies panel
    if (!extension.manifest.contributes.commands.empty())
    {
        std::string cmds_str = "\n\nCONTRIBUTED COMMANDS\n";
        for (const auto& cmd : extension.manifest.contributes.commands)
        {
            cmds_str += "  \xE2\x80\xA2 ";
            if (!cmd.category.empty())
            {
                cmds_str += cmd.category + ": ";
            }
            cmds_str += cmd.title;
            if (!cmd.command.empty())
            {
                cmds_str += "  (" + cmd.command + ")";
            }
            cmds_str += "\n";
        }
        if (deps_text_ != nullptr)
        {
            deps_text_->SetLabel(deps_text_->GetLabel().ToStdString() + cmds_str);
        }
    }

    UpdateActionButtons();
    Layout();
    Refresh();
}

void ExtensionDetailPanel::ShowGalleryExtension(const core::GalleryExtension& extension,
                                                bool is_installed)
{
    current_extension_id_ = extension.identifier;
    current_is_installed_ = is_installed;
    current_has_update_ = false;

    title_label_->SetLabel(extension.display_name.empty() ? extension.identifier
                                                          : extension.display_name);
    publisher_label_->SetLabel(extension.publisher_display);
    version_label_->SetLabel("v" + extension.version);
    description_text_->SetLabel(extension.description);
    description_text_->Wrap(GetClientSize().GetWidth() - 24);

    deps_text_->SetLabel("None");

    // Phase 20 Task 13: Changelog tab (no changelog for gallery extensions)
    if (changelog_text_ != nullptr)
    {
        changelog_text_->SetLabel("No changelog available for gallery extensions.");
    }

    UpdateActionButtons();
    Layout();
    Refresh();
}

void ExtensionDetailPanel::Clear()
{
    current_extension_id_.clear();
    title_label_->SetLabel("");
    publisher_label_->SetLabel("");
    version_label_->SetLabel("");
    description_text_->SetLabel("");
    deps_text_->SetLabel("");

    install_button_->Hide();
    uninstall_button_->Hide();
    update_button_->Hide();

    Layout();
}

void ExtensionDetailPanel::SetOnBack(std::function<void()> callback)
{
    on_back_ = std::move(callback);
}

void ExtensionDetailPanel::SetOnInstall(std::function<void(const std::string&)> callback)
{
    on_install_ = std::move(callback);
}

void ExtensionDetailPanel::SetOnUninstall(std::function<void(const std::string&)> callback)
{
    on_uninstall_ = std::move(callback);
}

void ExtensionDetailPanel::SetOnUpdate(std::function<void(const std::string&)> callback)
{
    on_update_ = std::move(callback);
}

void ExtensionDetailPanel::UpdateActionButtons()
{
    if (current_is_installed_)
    {
        install_button_->Hide();
        uninstall_button_->Show();
        if (current_has_update_)
        {
            update_button_->Show();
        }
        else
        {
            update_button_->Hide();
        }
    }
    else
    {
        install_button_->Show();
        uninstall_button_->Hide();
        update_button_->Hide();
    }
    Layout();
}

void ExtensionDetailPanel::ApplyTheme()
{
    auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    SetBackgroundColour(bg_color);
    if (content_scroll_ != nullptr)
    {
        content_scroll_->SetBackgroundColour(bg_color);
    }

    if (back_button_ != nullptr)
    {
        back_button_->SetBackgroundColour(bg_color);
        back_button_->SetForegroundColour(
            theme_engine_.color(core::ThemeColorToken::AccentPrimary));
    }

    if (title_label_ != nullptr)
    {
        title_label_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMain));
    }
    if (publisher_label_ != nullptr)
    {
        publisher_label_->SetForegroundColour(
            theme_engine_.color(core::ThemeColorToken::TextMuted));
    }
    if (version_label_ != nullptr)
    {
        version_label_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    }
    if (description_text_ != nullptr)
    {
        description_text_->SetForegroundColour(
            theme_engine_.color(core::ThemeColorToken::TextMain));
    }
    if (deps_header_ != nullptr)
    {
        deps_header_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMain));
    }
    if (deps_text_ != nullptr)
    {
        deps_text_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    }
    if (changelog_text_ != nullptr)
    {
        changelog_text_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    }
    if (tab_bar_ != nullptr)
    {
        tab_bar_->SetBackgroundColour(
            theme_engine_.color(core::ThemeColorToken::BgPanel).ChangeLightness(95));
    }
    if (overview_panel_ != nullptr)
    {
        overview_panel_->SetBackgroundColour(bg_color);
    }
    if (changelog_panel_ != nullptr)
    {
        changelog_panel_->SetBackgroundColour(bg_color);
    }
    if (deps_panel_ != nullptr)
    {
        deps_panel_->SetBackgroundColour(bg_color);
    }

    // Style action buttons
    if (install_button_ != nullptr)
    {
        install_button_->SetBackgroundColour(
            theme_engine_.color(core::ThemeColorToken::AccentPrimary));
        install_button_->SetForegroundColour(wxColour(255, 255, 255));
    }
    if (uninstall_button_ != nullptr)
    {
        uninstall_button_->SetBackgroundColour(
            theme_engine_.color(core::ThemeColorToken::BgPanel).ChangeLightness(110));
        uninstall_button_->SetForegroundColour(
            theme_engine_.color(core::ThemeColorToken::TextMain));
    }
    if (update_button_ != nullptr)
    {
        update_button_->SetBackgroundColour(
            theme_engine_.color(core::ThemeColorToken::AccentPrimary));
        update_button_->SetForegroundColour(wxColour(255, 255, 255));
    }

    Refresh();
}

} // namespace markamp::ui
