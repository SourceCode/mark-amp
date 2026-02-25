#include "SearchSidebarPanel.h"

#include "core/Logger.h"
#include "ui/DesignSystemContext.h"
#include "ui/EmptyPanelState.h"
#include "ui/IconManager.h"
#include "ui/PanelHeader.h"
#include "ui/SidebarFooter.h"
#include "ui/SidebarSection.h"
#include "ui/ThemedScrollbar.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/sizer.h>

namespace markamp::ui
{
class MatchBadge : public ThemeAwareWindow
{
public:
    MatchBadge(wxWindow* parent, markamp::core::ThemeEngine& theme_engine)
        : ThemeAwareWindow(
              parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetMinSize(wxSize(60, 20));
    }

    void SetMatchCount(int count)
    {
        count_ = count;
        Refresh();
    }

protected:
    void OnPaint(wxPaintEvent& /*event*/)
    {
        wxAutoBufferedPaintDC dc(this);
        auto& theme = theme_engine();
        dc.SetBackground(
            wxBrush(theme.resolve_token("sidebar.bg")
                        .value_or(theme.color(markamp::core::ThemeColorToken::BgPanel))));
        dc.Clear();

        wxSize size = GetClientSize();

        std::string text = count_ == 1 ? "1 result" : std::to_string(count_) + " results";

        dc.SetFont(theme.font(markamp::core::ThemeFontToken::MonoRegular).Scaled(0.8f));

        wxCoord tw, th;
        dc.GetTextExtent(text, &tw, &th);

        int padding_x = 8;
        int padding_y = 2;
        int rect_w = tw + padding_x * 2;
        int rect_h = th + padding_y * 2;

        int x = 0;
        int y = (size.GetHeight() - rect_h) / 2;

        auto bg_color = theme.color(markamp::core::ThemeColorToken::ActivityBarBadgeBg);
        auto fg_color = theme.color(markamp::core::ThemeColorToken::ActivityBarBadgeFg);

        if (count_ == 0)
        {
            bg_color = theme.color(markamp::core::ThemeColorToken::BgPanel);
            fg_color = theme.color(markamp::core::ThemeColorToken::TextMuted);
        }

        std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));
        if (gc)
        {
            gc->SetBrush(wxBrush(bg_color));
            gc->SetPen(*wxTRANSPARENT_PEN);
            gc->DrawRoundedRectangle(x, y, rect_w, rect_h, rect_h / 2.0);

            gc->SetFont(theme.font(markamp::core::ThemeFontToken::MonoRegular).Scaled(0.8f),
                        fg_color);
            gc->DrawText(text, x + padding_x, y + padding_y);
        }

        SetMinSize(wxSize(rect_w, rect_h));
    }

    void OnThemeChanged(const markamp::core::Theme& /*new_theme*/) override
    {
        Refresh();
    }

private:
    int count_{0};
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MatchBadge, ThemeAwareWindow) EVT_PAINT(MatchBadge::OnPaint) wxEND_EVENT_TABLE()

    SearchSidebarPanel::SearchSidebarPanel(wxWindow* parent,
                                           core::ThemeEngine& theme_engine,
                                           core::EventBus& event_bus,
                                           core::Config* config,
                                           DesignSystemContext& ds,
                                           IconManager& icon_manager)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    , event_bus_(event_bus)
    , config_(config)
    , ds_(ds)
    , icon_manager_(icon_manager)
{
    SetBackgroundColour(theme_engine.resolve_token("sidebar.bg")
                            .value_or(theme_engine.color(core::ThemeColorToken::BgPanel)));

    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // 1. Panel Header
    auto* header = new PanelHeader(this, ds_, icon_manager_, event_bus_);
    header->set_title("SEARCH");
    const std::vector<PanelHeader::ActionIcon> actions = {
        {"search.refresh", "refresh", "Refresh Search"},
        {"search.clear", "clear-all", "Clear Search Results"},
        {"search.collapse", "panel-collapse", "Collapse All"}};
    header->set_actions(actions);
    main_sizer->Add(header, 0, wxEXPAND);

    // Toolbar container for inputs
    auto* toolbar_panel = new wxPanel(header, wxID_ANY);
    toolbar_panel->SetBackgroundColour(GetBackgroundColour());
    auto* toolbar_sizer = new wxBoxSizer(wxVERTICAL);
    toolbar_panel->SetSizer(toolbar_sizer);

    // Search input
    search_input_ = new wxSearchCtrl(toolbar_panel,
                                     wxID_ANY,
                                     wxEmptyString,
                                     wxDefaultPosition,
                                     wxSize(-1, 28),
                                     wxTE_PROCESS_ENTER);
    search_input_->SetDescriptiveText("Search files\u2026");
    search_input_->ShowCancelButton(true);
    search_input_->SetBackgroundColour(
        theme_engine.resolve_token("sidebar.bg")
            .value_or(theme_engine.color(core::ThemeColorToken::BgPanel))
            .ChangeLightness(115));
    search_input_->SetForegroundColour(
        theme_engine.resolve_token("text.main")
            .value_or(theme_engine.color(core::ThemeColorToken::TextMain)));
    search_input_->SetFont(theme_engine.font(core::ThemeFontToken::MonoRegular));
    toolbar_sizer->Add(search_input_, 0, wxEXPAND | wxALL, 0);

    // Replace input
    replace_input_ = new wxTextCtrl(toolbar_panel,
                                    wxID_ANY,
                                    wxEmptyString,
                                    wxDefaultPosition,
                                    wxSize(-1, 28),
                                    wxTE_PROCESS_ENTER);
    replace_input_->SetHint("Replace\u2026");
    replace_input_->SetBackgroundColour(
        theme_engine.resolve_token("sidebar.bg")
            .value_or(theme_engine.color(core::ThemeColorToken::BgPanel))
            .ChangeLightness(115));
    replace_input_->SetForegroundColour(
        theme_engine.resolve_token("text.main")
            .value_or(theme_engine.color(core::ThemeColorToken::TextMain)));
    replace_input_->SetFont(theme_engine.font(core::ThemeFontToken::MonoRegular));
    toolbar_sizer->Add(replace_input_, 0, wxEXPAND | wxTOP, 4);

    // Options bar
    auto* opts = new wxBoxSizer(wxHORIZONTAL);
    auto tick_color = theme_engine.resolve_token("text.muted")
                          .value_or(theme_engine.color(core::ThemeColorToken::TextMuted));
    auto tick_font = theme_engine.font(core::ThemeFontToken::MonoRegular).Scaled(0.75f);

    regex_cb_ = new wxCheckBox(toolbar_panel, wxID_ANY, "Regex");
    regex_cb_->SetForegroundColour(tick_color);
    regex_cb_->SetFont(tick_font);
    opts->Add(regex_cb_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

    case_cb_ = new wxCheckBox(toolbar_panel, wxID_ANY, "Case");
    case_cb_->SetForegroundColour(tick_color);
    case_cb_->SetFont(tick_font);
    opts->Add(case_cb_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

    word_cb_ = new wxCheckBox(toolbar_panel, wxID_ANY, "Word");
    word_cb_->SetForegroundColour(tick_color);
    word_cb_->SetFont(tick_font);
    opts->Add(word_cb_, 0, wxALIGN_CENTER_VERTICAL);
    toolbar_sizer->Add(opts, 0, wxEXPAND | wxTOP | wxBOTTOM, 6);

    header->set_toolbar(toolbar_panel);

    // Search Details Section
    auto* details_section = new SidebarSection(
        this, ds_, icon_manager_, event_bus_, config_, "Search Details", "search_details");
    auto* details_panel = new wxPanel(details_section, wxID_ANY);
    details_panel->SetBackgroundColour(GetBackgroundColour());
    auto* details_sizer = new wxBoxSizer(wxVERTICAL);

    auto lbl_font = theme_engine.font(core::ThemeFontToken::UISmall);
    auto lbl_color = theme_engine.color(core::ThemeColorToken::TextMuted);

    auto* inc_lbl = new wxStaticText(details_panel, wxID_ANY, "files to include");
    inc_lbl->SetFont(lbl_font);
    inc_lbl->SetForegroundColour(lbl_color);
    details_sizer->Add(inc_lbl, 0, wxLEFT | wxRIGHT | wxTOP, 4);

    files_include_input_ =
        new wxTextCtrl(details_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 24));
    files_include_input_->SetBackgroundColour(replace_input_->GetBackgroundColour());
    files_include_input_->SetForegroundColour(replace_input_->GetForegroundColour());
    files_include_input_->SetFont(theme_engine.font(core::ThemeFontToken::MonoRegular));
    details_sizer->Add(files_include_input_, 0, wxEXPAND | wxALL, 4);

    auto* exc_lbl = new wxStaticText(details_panel, wxID_ANY, "files to exclude");
    exc_lbl->SetFont(lbl_font);
    exc_lbl->SetForegroundColour(lbl_color);
    details_sizer->Add(exc_lbl, 0, wxLEFT | wxRIGHT | wxTOP, 4);

    files_exclude_input_ =
        new wxTextCtrl(details_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 24));
    files_exclude_input_->SetBackgroundColour(replace_input_->GetBackgroundColour());
    files_exclude_input_->SetForegroundColour(replace_input_->GetForegroundColour());
    files_exclude_input_->SetFont(theme_engine.font(core::ThemeFontToken::MonoRegular));
    details_sizer->Add(files_exclude_input_, 0, wxEXPAND | wxALL, 4);

    details_panel->SetSizer(details_sizer);
    details_section->set_content(details_panel);
    details_section->set_expanded(false); // collapsed by default

    main_sizer->Add(details_section, 0, wxEXPAND | wxBOTTOM, 8);

    // Results container
    // Match count label (Stylized Badge)
    match_badge_ = new MatchBadge(this, theme_engine);
    main_sizer->Add(match_badge_, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    // Results list
    results_list_ = new wxListBox(this, wxID_ANY);
    results_list_->SetBackgroundColour(GetBackgroundColour());
    results_list_->SetForegroundColour(theme_engine.color(core::ThemeColorToken::TextMain));
    main_sizer->Add(results_list_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    // Empty state
    empty_state_ = new EmptyPanelState(this, ds_, icon_manager_);
    empty_state_->set_icon("search-stop"); // Fallback icon or appropriate ID
    empty_state_->set_message("No results found. Try adjusting your search criteria.");
    empty_state_->Hide();
    main_sizer->Add(empty_state_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    // Footer
    footer_ = new SidebarFooter(this, ds_, event_bus_);
    footer_->set_text("Search ready");
    main_sizer->Add(footer_, 0, wxEXPAND);

    SetSizer(main_sizer);

    // Load persisted state
    if (config_)
    {
        search_input_->SetValue(config_->get_string("sidebar.search.state.query"));
        replace_input_->SetValue(config_->get_string("sidebar.search.state.replace"));
        files_include_input_->SetValue(config_->get_string("sidebar.search.state.include"));
        files_exclude_input_->SetValue(config_->get_string("sidebar.search.state.exclude"));
        regex_cb_->SetValue(config_->get_bool("sidebar.search.state.regex"));
        case_cb_->SetValue(config_->get_bool("sidebar.search.state.case_sensitive"));
        word_cb_->SetValue(config_->get_bool("sidebar.search.state.whole_word"));
    }

    // Bind events
    search_input_->Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &SearchSidebarPanel::OnSearch, this);
    search_input_->Bind(wxEVT_TEXT_ENTER, &SearchSidebarPanel::OnSearch, this);
    results_list_->Bind(wxEVT_LISTBOX, &SearchSidebarPanel::OnResultSelected, this);

    // ThemedScrollbar overlaying results list
    scrollbar_ = new ThemedScrollbar(this, theme_engine, this);
    scrollbar_->Hide();

    action_sub_ = event_bus_.subscribe<core::events::PanelHeaderActionEvent>(
        [this](const core::events::PanelHeaderActionEvent& evt)
        {
            if (evt.action_id == "search.clear")
                ClearResults();
        });

    Bind(wxEVT_SCROLLWIN_THUMBTRACK, &SearchSidebarPanel::OnScrollbarDrag, this);
    Bind(wxEVT_SCROLLWIN_THUMBRELEASE, &SearchSidebarPanel::OnScrollbarDrag, this);

    scroll_sync_timer_.SetOwner(this);
    Bind(wxEVT_TIMER, &SearchSidebarPanel::OnScrollSyncTimer, this);
    scroll_sync_timer_.Start(50); // Poll listbox scroll state

    Bind(wxEVT_SIZE,
         [this](wxSizeEvent& event)
         {
             if (scrollbar_ != nullptr && results_list_ != nullptr)
             {
                 auto list_rect = results_list_->GetRect();
                 scrollbar_->SetSize(list_rect.GetRight() - ThemedScrollbar::kWidth,
                                     list_rect.GetTop(),
                                     ThemedScrollbar::kWidth,
                                     list_rect.GetHeight());
                 UpdateScrollbar();
             }
             event.Skip();
         });
}

void SearchSidebarPanel::ClearResults()
{
    if (results_list_ != nullptr)
    {
        results_list_->Clear();
        results_list_->Hide();
    }
    match_count_ = 0;
    if (match_badge_ != nullptr)
    {
        match_badge_->SetMatchCount(0);
    }
    if (empty_state_ != nullptr)
    {
        empty_state_->Show();
    }
    Layout();
}

void SearchSidebarPanel::SetQuery(const std::string& query)
{
    if (search_input_ != nullptr)
    {
        search_input_->SetValue(query);
    }
}

void SearchSidebarPanel::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    SetBackgroundColour(theme_engine()
                            .resolve_token("sidebar.bg")
                            .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));

    if (match_badge_ != nullptr)
    {
        match_badge_->Refresh();
    }

    Refresh();
}

void SearchSidebarPanel::OnScrollSyncTimer(wxTimerEvent& /*event*/)
{
    UpdateScrollbar();
}

void SearchSidebarPanel::OnScrollbarDrag(wxScrollWinEvent& event)
{
    int pos = event.GetPosition();
    if (results_list_ != nullptr && results_list_->GetCount() > 0)
    {
        int item_height = results_list_->GetCharHeight() + 4; // approximate row height
        int top_item = pos / item_height;
        top_item = std::clamp(top_item, 0, static_cast<int>(results_list_->GetCount()) - 1);
        results_list_->SetFirstItem(top_item);
    }
}

void SearchSidebarPanel::UpdateScrollbar()
{
    if (scrollbar_ == nullptr || results_list_ == nullptr)
        return;

    if (!results_list_->IsShown() || results_list_->GetCount() == 0)
    {
        scrollbar_->Hide();
        return;
    }

    int item_height = results_list_->GetCharHeight() + 4;
    int total_range = static_cast<int>(results_list_->GetCount()) * item_height;
    int visible_range = results_list_->GetClientSize().GetHeight();

    int top_item = results_list_->HitTest(wxPoint(5, 5));
    if (top_item == wxNOT_FOUND)
    {
        top_item = 0;
    }

    int position = top_item * item_height;
    scrollbar_->UpdateScrollPosition(position, visible_range, total_range);

    if (total_range > visible_range)
    {
        if (!scrollbar_->IsShown())
        {
            scrollbar_->Show();
            scrollbar_->Raise(); // Ensure it stays on top
        }
    }
    else
    {
        scrollbar_->Hide();
    }
}

void SearchSidebarPanel::OnSearch(wxCommandEvent& /*event*/)
{
    const auto query = search_input_->GetValue().ToStdString();
    if (query.empty())
    {
        ClearResults();
        return;
    }

    MARKAMP_LOG_INFO("SearchSidebarPanel: Searching for '{}'", query);

    // Publish search request event — FindRequestEvent triggers editor search
    core::events::FindRequestEvent find_evt;
    event_bus_.publish(find_evt);

    // Publish search result count for badge
    core::events::SearchResultCountEvent count_evt;
    count_evt.count = 0; // Will be updated by search engine response
    event_bus_.publish(count_evt);
}

void SearchSidebarPanel::OnResultSelected(wxCommandEvent& /*event*/)
{
    const int sel = results_list_->GetSelection();
    if (sel == wxNOT_FOUND)
    {
        return;
    }

    MARKAMP_LOG_INFO("SearchSidebarPanel: Result selected at index {}", sel);
}

SearchSidebarPanel::~SearchSidebarPanel()
{
    if (config_)
    {
        config_->set("sidebar.search.state.query", search_input_->GetValue().ToStdString());
        config_->set("sidebar.search.state.replace", replace_input_->GetValue().ToStdString());
        config_->set("sidebar.search.state.include",
                     files_include_input_->GetValue().ToStdString());
        config_->set("sidebar.search.state.exclude",
                     files_exclude_input_->GetValue().ToStdString());
        config_->set("sidebar.search.state.regex", regex_cb_->GetValue());
        config_->set("sidebar.search.state.case_sensitive", case_cb_->GetValue());
        config_->set("sidebar.search.state.whole_word", word_cb_->GetValue());
        auto result = config_->save();
        if (!result)
        {
            // Log error
        }
    }
}

} // namespace markamp::ui
