#include "ui/OpenEditorsSection.h"

#include "core/Config.h"
#include "core/ThemeEngine.h"
#include "ui/IconManager.h"

#include <wx/stattext.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(OpenEditorsSection, SidebarSection) wxEND_EVENT_TABLE()

    OpenEditorsSection::OpenEditorsSection(wxWindow* parent,
                                           DesignSystemContext& ds,
                                           IconManager& icon_manager,
                                           core::EventBus& event_bus,
                                           core::Config* config)
    : SidebarSection(parent, ds, icon_manager, event_bus, config, "OPEN EDITORS", "open_editors")
    , event_bus_(event_bus)
{
    scroll_container_ =
        new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    scroll_container_->SetScrollRate(0, 20);
    scroll_container_->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));

    list_sizer_ = new wxBoxSizer(wxVERTICAL);
    scroll_container_->SetSizer(list_sizer_);

    set_content(scroll_container_);

    // Initial state: empty
    BuildEditorsList();

    // Subscribe to events that change the open editors list
    subscriptions_.push_back(event_bus.subscribe<core::events::FileOpenedEvent>(
        [this](const core::events::FileOpenedEvent& evt)
        {
            bool found = false;
            for (auto& ed : open_editors_)
            {
                if (ed.path == evt.file_path)
                {
                    found = true;
                    // Reset active states
                    for (auto& e : open_editors_)
                        e.is_active = false;
                    ed.is_active = true;
                    break;
                }
            }
            if (!found)
            {
                for (auto& e : open_editors_)
                    e.is_active = false;

                // Extract filename
                std::string filename = evt.file_path;
                auto pos = filename.find_last_of("/\\");
                if (pos != std::string::npos)
                    filename = filename.substr(pos + 1);

                open_editors_.push_back({evt.file_path, filename, evt.file_path, true, false});
            }
            BuildEditorsList();
        }));

    subscriptions_.push_back(event_bus.subscribe<core::events::ActiveFileChangedEvent>(
        [this](const core::events::ActiveFileChangedEvent& evt)
        {
            for (auto& ed : open_editors_)
            {
                ed.is_active = (ed.path == evt.file_id);
            }
            BuildEditorsList();
        }));

    subscriptions_.push_back(event_bus.subscribe<core::events::TabCloseRequestEvent>(
        [this](const core::events::TabCloseRequestEvent& evt)
        {
            auto it =
                std::remove_if(open_editors_.begin(),
                               open_editors_.end(),
                               [&evt](const EditorItem& e) { return e.path == evt.file_path; });
            if (it != open_editors_.end())
            {
                open_editors_.erase(it, open_editors_.end());
                BuildEditorsList();
            }
        }));
}

OpenEditorsSection::~OpenEditorsSection() = default;

void OpenEditorsSection::BuildEditorsList()
{
    // Clear existing
    list_sizer_->Clear(true);

    if (open_editors_.empty())
    {
        auto* empty_label = new wxStaticText(scroll_container_, wxID_ANY, "No editors open.");
        empty_label->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMuted));
        empty_label->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));
        list_sizer_->Add(empty_label, 1, wxALIGN_CENTER | wxALL, 10);
    }
    else
    {
        const auto active_bg = theme_engine().color(core::ThemeColorToken::SelectionBg);
        const auto active_fg = theme_engine().color(core::ThemeColorToken::SidebarFg);
        const auto inactive_fg = theme_engine().color(core::ThemeColorToken::TextMuted);

        for (const auto& editor : open_editors_)
        {
            auto* row = new wxPanel(scroll_container_, wxID_ANY);
            auto* row_sizer = new wxBoxSizer(wxHORIZONTAL);

            if (editor.is_active)
            {
                row->SetBackgroundColour(active_bg);
            }
            else
            {
                row->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));
            }

            auto* label = new wxStaticText(row, wxID_ANY, editor.filename);
            label->SetForegroundColour(editor.is_active ? active_fg : inactive_fg);
            label->SetFont(theme_engine().font(core::ThemeFontToken::UISmall));

            auto* close_btn = new wxStaticText(row, wxID_ANY, L"\u00D7");
            close_btn->SetForegroundColour(inactive_fg);
            // close_btn uses a slightly larger font to look like an icon
            close_btn->SetFont(theme_engine().font(core::ThemeFontToken::UISmall).Bold());

            row_sizer->AddStretchSpacer(1);
            row_sizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 8);
            row_sizer->AddStretchSpacer(10); // push right
            row_sizer->Add(close_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

            row->SetSizer(row_sizer);

            row->Bind(wxEVT_LEFT_UP,
                      [&event_bus_ = this->event_bus_, path = editor.path](wxMouseEvent&)
                      {
                          core::events::ActiveFileChangedEvent evt;
                          evt.file_id = path;
                          event_bus_.publish(evt);
                      });

            close_btn->Bind(
                wxEVT_LEFT_UP,
                [&event_bus_ = this->event_bus_, path = editor.path](wxMouseEvent& event)
                {
                    core::events::TabCloseRequestEvent evt;
                    evt.file_path = path;
                    event_bus_.publish(evt);
                    event.StopPropagation();
                });

            close_btn->Bind(wxEVT_ENTER_WINDOW,
                            [close_btn, active_fg](wxMouseEvent& event)
                            {
                                close_btn->SetForegroundColour(active_fg);
                                close_btn->Refresh();
                                event.Skip();
                            });

            close_btn->Bind(wxEVT_LEAVE_WINDOW,
                            [close_btn, inactive_fg](wxMouseEvent& event)
                            {
                                close_btn->SetForegroundColour(inactive_fg);
                                close_btn->Refresh();
                                event.Skip();
                            });

            list_sizer_->Add(row, 0, wxEXPAND | wxTOP | wxBOTTOM, 2);
        }
    }

    scroll_container_->Layout();
    scroll_container_->Refresh();
}

void OpenEditorsSection::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    scroll_container_->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));
    BuildEditorsList();
}

} // namespace markamp::ui
