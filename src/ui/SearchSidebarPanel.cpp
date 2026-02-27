#include "SearchSidebarPanel.h"

#include "core/Logger.h"
#include "core/WorkspaceSearchEngine.h"
#include "ui/DesignSystemContext.h"
#include "ui/EmptyPanelState.h"
#include "ui/IconManager.h"
#include "ui/PanelHeader.h"
#include "ui/SidebarFooter.h"
#include "ui/SidebarSection.h"
#include "ui/ThemedScrollbar.h"

#include <wx/app.h>
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
        wxAutoBufferedPaintDC paint_dc(this);
        auto& theme = theme_engine();
        paint_dc.SetBackground(
            wxBrush(theme.resolve_token("sidebar.bg")
                        .value_or(theme.color(markamp::core::ThemeColorToken::BgPanel))));
        paint_dc.Clear();

        wxSize size = GetClientSize();

        const std::string kText = count_ == 1 ? "1 result" : std::to_string(count_) + " results";

        paint_dc.SetFont(theme.font(markamp::core::ThemeFontToken::MonoRegular).Scaled(0.8f));

        wxCoord text_width = 0;
        wxCoord text_height = 0;
        paint_dc.GetTextExtent(kText, &text_width, &text_height);

        const int kPaddingX = 8;
        const int kPaddingY = 2;
        const int kRectW = text_width + kPaddingX * 2;
        const int kRectH = text_height + kPaddingY * 2;

        const int kX = 0;
        const int kY = (size.GetHeight() - kRectH) / 2;

        auto bg_color = theme.color(markamp::core::ThemeColorToken::ActivityBarBadgeBg);
        auto fg_color = theme.color(markamp::core::ThemeColorToken::ActivityBarBadgeFg);

        if (count_ == 0)
        {
            bg_color = theme.color(markamp::core::ThemeColorToken::BgPanel);
            fg_color = theme.color(markamp::core::ThemeColorToken::TextMuted);
        }

        std::unique_ptr<wxGraphicsContext> graphics_context(wxGraphicsContext::Create(paint_dc));
        if (graphics_context)
        {
            graphics_context->SetBrush(wxBrush(bg_color));
            graphics_context->SetPen(*wxTRANSPARENT_PEN);
            graphics_context->DrawRoundedRectangle(kX, kY, kRectW, kRectH, kRectH / 2.0);

            graphics_context->SetFont(
                theme.font(markamp::core::ThemeFontToken::MonoRegular).Scaled(0.8F), fg_color);
            graphics_context->DrawText(kText, kX + kPaddingX, kY + kPaddingY);
        }

        SetMinSize(wxSize(kRectW, kRectH));
    }

    void OnThemeChanged(const markamp::core::Theme& /*new_theme*/) override
    {
        Refresh();
    }

private:
    int count_{0};
    // NOLINTBEGIN
    wxDECLARE_EVENT_TABLE();
    // NOLINTEND
};

// NOLINTBEGIN
wxBEGIN_EVENT_TABLE(MatchBadge, ThemeAwareWindow) EVT_PAINT(MatchBadge::OnPaint) wxEND_EVENT_TABLE()
    // NOLINTEND

    namespace
{

    class ToggleTextButton : public ThemeAwareWindow
    {
    public:
        ToggleTextButton(wxWindow* parent,
                         markamp::core::ThemeEngine& theme_engine,
                         const std::string& text,
                         bool initial_state)
            : ThemeAwareWindow(
                  parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
            , text_(text)
            , is_toggled_(initial_state)
        {
            SetBackgroundStyle(wxBG_STYLE_PAINT);
            SetMinSize(wxSize(24, 24));
        }

        void set_on_toggle(std::function<void(bool)> callback)
        {
            on_toggle_ = std::move(callback);
        }
        [[nodiscard]] auto is_toggled() const -> bool
        {
            return is_toggled_;
        }
        void set_toggled(bool state)
        {
            is_toggled_ = state;
            Refresh();
        }
        void set_text(const std::string& text)
        {
            text_ = text;
            Refresh();
        }

    protected:
        void OnPaint(wxPaintEvent& /*event*/)
        {
            wxAutoBufferedPaintDC paint_dc(this);
            auto& theme = theme_engine();
            const wxSize size = GetClientSize();

            auto bg_color = theme.resolve_token("sidebar.bg")
                                .value_or(theme.color(markamp::core::ThemeColorToken::BgPanel));
            if (is_toggled_)
            {
                bg_color = theme.color(markamp::core::ThemeColorToken::ControlBgSelected);
            }
            else if (is_hovered_)
            {
                bg_color = theme.color(markamp::core::ThemeColorToken::ControlBgHover);
            }

            paint_dc.SetBackground(wxBrush(bg_color));
            paint_dc.Clear();

            std::unique_ptr<wxGraphicsContext> graphics_context(
                wxGraphicsContext::Create(paint_dc));
            if (graphics_context)
            {
                graphics_context->SetBrush(wxBrush(bg_color));
                if (is_toggled_)
                {
                    graphics_context->SetPen(
                        wxPen(theme.color(markamp::core::ThemeColorToken::ControlBorderFocus), 1));
                }
                else
                {
                    graphics_context->SetPen(*wxTRANSPARENT_PEN);
                }
                graphics_context->DrawRoundedRectangle(
                    0, 0, size.GetWidth(), size.GetHeight(), 3.0);

                auto fg_color = theme.color(markamp::core::ThemeColorToken::TextMain);
                if (is_toggled_)
                {
                    fg_color = theme.color(markamp::core::ThemeColorToken::ControlFgNormal);
                }
                else if (!is_hovered_)
                {
                    fg_color = theme.color(markamp::core::ThemeColorToken::TextMuted);
                }

                graphics_context->SetFont(
                    theme.font(markamp::core::ThemeFontToken::MonoRegular).Scaled(0.85F), fg_color);
                double text_width = 0;
                double text_height = 0;
                double text_descent = 0;
                double text_ext_leading = 0;
                graphics_context->GetTextExtent(
                    text_, &text_width, &text_height, &text_descent, &text_ext_leading);
                graphics_context->DrawText(text_,
                                           (size.GetWidth() - text_width) / 2.0,
                                           (size.GetHeight() - text_height) / 2.0);
            }
        }

        void OnMouseLeftDown(wxMouseEvent& /*event*/)
        {
            is_toggled_ = !is_toggled_;
            Refresh();
            if (on_toggle_)
            {
                on_toggle_(is_toggled_);
            }
        }

        void OnMouseEnter(wxMouseEvent& /*event*/)
        {
            is_hovered_ = true;
            Refresh();
        }
        void OnMouseLeave(wxMouseEvent& /*event*/)
        {
            is_hovered_ = false;
            Refresh();
        }

        void OnThemeChanged(const markamp::core::Theme& /*new_theme*/) override
        {
            Refresh();
        }

    private:
        std::string text_;
        bool is_toggled_{false};
        bool is_hovered_{false};
        std::function<void(bool)> on_toggle_;

        // NOLINTBEGIN
        wxDECLARE_EVENT_TABLE();
        // NOLINTEND
    };

    // NOLINTBEGIN
    wxBEGIN_EVENT_TABLE(ToggleTextButton, ThemeAwareWindow) EVT_PAINT(ToggleTextButton::OnPaint)
        EVT_LEFT_DOWN(ToggleTextButton::OnMouseLeftDown)
            EVT_ENTER_WINDOW(ToggleTextButton::OnMouseEnter)
                EVT_LEAVE_WINDOW(ToggleTextButton::OnMouseLeave) wxEND_EVENT_TABLE()
    // NOLINTEND

} // namespace

SearchSidebarPanel::SearchSidebarPanel(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       core::EventBus& event_bus,
                                       core::Config* config,
                                       DesignSystemContext& design_system,
                                       IconManager& icon_manager,
                                       const std::string& persistence_id)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    , event_bus_(event_bus)
    , config_(config)
    , ds_(design_system)
    , icon_manager_(icon_manager)
    , persistence_id_(persistence_id)
{
    SetBackgroundColour(theme_engine.resolve_token("sidebar.bg")
                            .value_or(theme_engine.color(core::ThemeColorToken::BgPanel)));

    // Engine setup
    search_engine_ = std::make_unique<core::WorkspaceSearchEngine>();
    search_engine_->SetProgressCallback(
        [this](int scanned, int total, int matches)
        {
            wxTheApp->CallAfter(
                [this, scanned, total, matches]()
                {
                    if (footer_ != nullptr)
                    {
                        footer_->set_text("Scanned " + std::to_string(scanned) + "/" +
                                          std::to_string(total) + " files (" +
                                          std::to_string(matches) + " matches)");
                    }
                    if (match_badge_ != nullptr)
                    {
                        match_count_ = matches;
                        match_badge_->SetMatchCount(match_count_);
                    }
                });
        });

    search_debounce_timer_.SetOwner(this);
    Bind(wxEVT_TIMER, &SearchSidebarPanel::OnDebouncedSearch, this, search_debounce_timer_.GetId());

    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // 1. Panel Header
    auto* header = new PanelHeader(this, ds_, icon_manager_, event_bus_);
    header->set_title("SEARCH");
    const std::vector<PanelHeader::ActionIcon> kActions = {
        {"search.refresh", "refresh", "Refresh Search"},
        {"search.clear", "clear-all", "Clear Search Results"},
        {"search.collapse", "panel-collapse", "Collapse All"}};
    header->set_actions(kActions);
    main_sizer->Add(header, 0, wxEXPAND);

    // Toolbar container for inputs
    auto* toolbar_panel = new wxPanel(header, wxID_ANY);
    toolbar_panel->SetBackgroundColour(GetBackgroundColour());
    auto* toolbar_sizer = new wxBoxSizer(wxVERTICAL);
    toolbar_panel->SetSizer(toolbar_sizer);

    auto input_bg = theme_engine.resolve_token("sidebar.bg")
                        .value_or(theme_engine.color(core::ThemeColorToken::BgPanel))
                        .ChangeLightness(115);

    // --- Search Input Row ---
    auto* search_row = new wxPanel(toolbar_panel, wxID_ANY);
    search_row->SetBackgroundColour(input_bg);
    auto* search_row_sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* btn_toggle_replace = new ToggleTextButton(search_row, theme_engine, ">", false);
    search_row_sizer->Add(btn_toggle_replace, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 2);

    // Load persisted state to initialize toggles correctly
    if (config_ != nullptr)
    {
        const std::string kPrefix =
            "sidebar." + (persistence_id_.empty() ? "" : persistence_id_ + ".") + "search.state.";
        use_regex_ = config_->get_bool(kPrefix + "regex", false);
        match_case_ = config_->get_bool(kPrefix + "case_sensitive", false);
        match_word_ = config_->get_bool(kPrefix + "whole_word", false);
        preserve_case_ = config_->get_bool(kPrefix + "preserve_case", false);
        show_context_ = config_->get_bool(kPrefix + "show_context", false);
    }

    search_input_ = new wxTextCtrl(search_row,
                                   wxID_ANY,
                                   wxEmptyString,
                                   wxDefaultPosition,
                                   wxSize(-1, 28),
                                   wxTE_PROCESS_ENTER | wxBORDER_NONE);
    search_input_->SetHint("Search files...");
    search_input_->SetBackgroundColour(input_bg);
    search_input_->SetForegroundColour(theme_engine.color(core::ThemeColorToken::TextMain));
    search_input_->SetFont(theme_engine.font(core::ThemeFontToken::MonoRegular));
    search_row_sizer->Add(search_input_, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxLEFT, 4);

    auto* btn_regex = new ToggleTextButton(search_row, theme_engine, ".*", use_regex_);
    auto* btn_case = new ToggleTextButton(search_row, theme_engine, "Aa", match_case_);
    auto* btn_word = new ToggleTextButton(search_row, theme_engine, "\\b", match_word_);

    auto* btn_context = new ToggleTextButton(search_row, theme_engine, "C", show_context_);
    btn_context->SetToolTip("Show Context Lines (1 line)");

    btn_toggle_replace->set_on_toggle(
        [this, btn_toggle_replace](bool state)
        {
            if (replace_row_ != nullptr)
            {
                replace_row_->Show(state);
                btn_toggle_replace->set_text(state ? "v" : ">");
                this->Layout();
            }
        });

    auto toggle_cb = [this](bool) { OnSearchOptionsChanged(); };
    btn_regex->set_on_toggle(
        [this, toggle_cb](bool state)
        {
            use_regex_ = state;
            toggle_cb(state);
        });
    btn_case->set_on_toggle(
        [this, toggle_cb](bool state)
        {
            match_case_ = state;
            toggle_cb(state);
        });
    btn_word->set_on_toggle(
        [this, toggle_cb](bool state)
        {
            match_word_ = state;
            toggle_cb(state);
        });
    btn_context->set_on_toggle(
        [this, toggle_cb](bool state)
        {
            show_context_ = state;
            toggle_cb(state);
        });

    search_row_sizer->Add(btn_regex, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    search_row_sizer->Add(btn_case, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    search_row_sizer->Add(btn_word, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    search_row_sizer->Add(btn_context, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);

    search_row->SetSizer(search_row_sizer);
    toolbar_sizer->Add(search_row, 0, wxEXPAND | wxALL, 0);

    // --- Replace Input Row ---
    replace_row_ = new wxPanel(toolbar_panel, wxID_ANY);
    replace_row_->SetBackgroundColour(input_bg);
    auto* replace_row_sizer = new wxBoxSizer(wxHORIZONTAL);

    replace_input_ = new wxTextCtrl(replace_row_,
                                    wxID_ANY,
                                    wxEmptyString,
                                    wxDefaultPosition,
                                    wxSize(-1, 28),
                                    wxTE_PROCESS_ENTER | wxBORDER_NONE);
    replace_input_->SetHint("Replace...");
    replace_input_->SetBackgroundColour(input_bg);
    replace_input_->SetForegroundColour(theme_engine.color(core::ThemeColorToken::TextMain));
    replace_input_->SetFont(theme_engine.font(core::ThemeFontToken::MonoRegular));
    replace_row_sizer->Add(replace_input_, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxLEFT, 4);

    auto* btn_preserve = new ToggleTextButton(replace_row_, theme_engine, "AB", preserve_case_);
    btn_preserve->set_on_toggle([this](bool state) { preserve_case_ = state; });
    replace_row_sizer->Add(btn_preserve, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);

    auto* btn_replace = new ToggleTextButton(replace_row_, theme_engine, "R", false);
    btn_replace->SetToolTip("Replace Matches");
    replace_row_sizer->Add(btn_replace, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);

    auto* btn_replace_all = new ToggleTextButton(replace_row_, theme_engine, "All", false);
    btn_replace_all->SetToolTip("Replace All");
    replace_row_sizer->Add(btn_replace_all, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);

    auto do_replace = [this, btn_replace_all, btn_replace](bool all)
    {
        if (all)
            btn_replace_all->set_toggled(false);
        else
            btn_replace->set_toggled(false);

        if (!search_engine_ || !results_list_)
            return;

        const std::string replacement = replace_input_->GetValue().ToStdString();
        // Since we don't have WorkspaceRoot reliably accessible here outside ExecuteSearch,
        // we use "." to test just like ExecuteSearch does. In the future this comes from
        // WorkspaceService.
        const std::string kWorkspaceRoot = ".";

        int replaced_count = 0;

        if (all)
        {
            auto current_result = results_list_->GetCurrentResult();
            replaced_count =
                search_engine_->ReplaceMatches(current_result.matches, replacement, kWorkspaceRoot);
        }
        else
        {
            auto opt_match = results_list_->GetSelectedMatch();
            if (opt_match.has_value())
            {
                if (search_engine_->ReplaceSingleMatch(
                        opt_match.value(), replacement, kWorkspaceRoot))
                {
                    replaced_count = 1;
                }
            }
        }

        if (replaced_count > 0)
        {
            MARKAMP_LOG_INFO("SearchSidebarPanel: Replaced {} occurrences", replaced_count);
            core::events::SearchReplaceCompletedEvent replace_evt;
            replace_evt.replace_text = replacement;
            replace_evt.replaced_count = replaced_count;
            event_bus_.publish(replace_evt);

            // Re-trigger search to update matches
            ExecuteSearch();
        }
    };

    btn_replace->set_on_toggle([do_replace](bool /*state*/) { do_replace(false); });
    btn_replace_all->set_on_toggle([do_replace](bool /*state*/) { do_replace(true); });

    replace_row_->SetSizer(replace_row_sizer);
    replace_row_->Hide(); // Hidden by default
    toolbar_sizer->Add(replace_row_, 0, wxEXPAND | wxTOP, 4);

    header->set_toolbar(toolbar_panel);

    // Search Details Section
    auto* details_section = new SidebarSection(
        this,
        ds_,
        icon_manager_,
        event_bus_,
        config_,
        "Search Details",
        (persistence_id_.empty() ? "" : persistence_id_ + "_") + "search_details");
    filters_panel_ = new wxPanel(details_section, wxID_ANY);
    filters_panel_->SetBackgroundColour(GetBackgroundColour());
    auto* details_sizer = new wxBoxSizer(wxVERTICAL);

    auto lbl_font = theme_engine.font(core::ThemeFontToken::UISmall);
    auto lbl_color = theme_engine.color(core::ThemeColorToken::TextMuted);

    auto* inc_lbl = new wxStaticText(filters_panel_, wxID_ANY, "files to include");
    inc_lbl->SetFont(lbl_font);
    inc_lbl->SetForegroundColour(lbl_color);
    details_sizer->Add(inc_lbl, 0, wxLEFT | wxRIGHT | wxTOP, 4);

    files_include_input_ =
        new wxTextCtrl(filters_panel_, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 24));
    files_include_input_->SetBackgroundColour(input_bg);
    files_include_input_->SetForegroundColour(theme_engine.color(core::ThemeColorToken::TextMain));
    files_include_input_->SetFont(theme_engine.font(core::ThemeFontToken::MonoRegular));
    details_sizer->Add(files_include_input_, 0, wxEXPAND | wxALL, 4);

    auto* exc_lbl = new wxStaticText(filters_panel_, wxID_ANY, "files to exclude");
    exc_lbl->SetFont(lbl_font);
    exc_lbl->SetForegroundColour(lbl_color);
    details_sizer->Add(exc_lbl, 0, wxLEFT | wxRIGHT | wxTOP, 4);

    files_exclude_input_ =
        new wxTextCtrl(filters_panel_, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 24));
    files_exclude_input_->SetBackgroundColour(input_bg);
    files_exclude_input_->SetForegroundColour(theme_engine.color(core::ThemeColorToken::TextMain));
    files_exclude_input_->SetFont(theme_engine.font(core::ThemeFontToken::MonoRegular));
    details_sizer->Add(files_exclude_input_, 0, wxEXPAND | wxALL, 4);

    filters_panel_->SetSizer(details_sizer);
    details_section->set_content(filters_panel_);
    details_section->set_expanded(false); // collapsed by default

    main_sizer->Add(details_section, 0, wxEXPAND | wxBOTTOM, 8);

    // Results container
    match_badge_ = new MatchBadge(this, theme_engine);
    main_sizer->Add(match_badge_, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    results_list_ = new SearchResultsTree(this, theme_engine);
    results_list_->SetOnResultSelected(
        [](const std::string& file_path, int line, int col)
        {
            MARKAMP_LOG_INFO("SearchSidebarPanel: Result selected {} {}:{}", file_path, line, col);
            // Handle opening file
        });
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
    if (config_ != nullptr)
    {
        const std::string kPrefix =
            "sidebar." + (persistence_id_.empty() ? "" : persistence_id_ + ".") + "search.state.";
        search_input_->SetValue(config_->get_string(kPrefix + "query"));
        replace_input_->SetValue(config_->get_string(kPrefix + "replace"));
        files_include_input_->SetValue(config_->get_string(kPrefix + "include"));
        files_exclude_input_->SetValue(config_->get_string(kPrefix + "exclude"));
    }

    search_input_->Bind(wxEVT_TEXT, &SearchSidebarPanel::OnSearch, this);
    search_input_->Bind(wxEVT_TEXT_ENTER, &SearchSidebarPanel::OnSearch, this);
    files_include_input_->Bind(wxEVT_TEXT, &SearchSidebarPanel::OnSearch, this);
    files_exclude_input_->Bind(wxEVT_TEXT, &SearchSidebarPanel::OnSearch, this);

    // ThemedScrollbar overlaying results list
    scrollbar_ = new ThemedScrollbar(this, theme_engine, this);
    scrollbar_->Hide();

    action_sub_ = event_bus_.subscribe<core::events::PanelHeaderActionEvent>(
        [this](const core::events::PanelHeaderActionEvent& evt)
        {
            if (evt.action_id == "search.clear")
            {
                ClearResults();
            }
            else if (evt.action_id == "search.collapse")
            {
                if (results_list_ != nullptr)
                {
                    results_list_->CollapseAll();
                }
            }
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
                 const auto kListRect = results_list_->GetRect();
                 scrollbar_->SetSize(kListRect.GetRight() - ThemedScrollbar::kWidth,
                                     kListRect.GetTop(),
                                     ThemedScrollbar::kWidth,
                                     kListRect.GetHeight());
                 UpdateScrollbar();
             }
             event.Skip();
         });
}

void SearchSidebarPanel::ClearResults()
{
    search_engine_->Cancel();
    if (results_list_ != nullptr)
    {
        results_list_->ClearResults();
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
    if (footer_ != nullptr)
    {
        footer_->set_text("Search cleared");
    }
    Layout();
}

void SearchSidebarPanel::SelectNextMatch()
{
    if (results_list_ != nullptr)
    {
        results_list_->SelectNextMatch();
    }
}

void SearchSidebarPanel::SelectPreviousMatch()
{
    if (results_list_ != nullptr)
    {
        results_list_->SelectPreviousMatch();
    }
}

void SearchSidebarPanel::SetQuery(const std::string& query)
{
    if (search_input_ != nullptr)
    {
        search_input_->SetValue(query);
    }
}

void SearchSidebarPanel::OnSearchOptionsChanged()
{
    ExecuteSearch();
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
    const int kPos = event.GetPosition();
    if (results_list_ != nullptr && results_list_->GetCount() > 0)
    {
        const int kItemHeight = results_list_->GetCharHeight() + 4; // approximate row height
        int top_item = kPos / kItemHeight;
        top_item = std::clamp(top_item, 0, static_cast<int>(results_list_->GetCount()) - 1);
        results_list_->SetFirstItem(top_item);
    }
}

void SearchSidebarPanel::UpdateScrollbar()
{
    if (scrollbar_ == nullptr || results_list_ == nullptr)
    {
        return;
    }

    if (!results_list_->IsShown() || results_list_->GetCount() == 0)
    {
        scrollbar_->Hide();
        return;
    }

    const int kItemHeight = results_list_->GetCharHeight() + 4;
    const int kTotalRange = static_cast<int>(results_list_->GetCount()) * kItemHeight;
    const int kVisibleRange = results_list_->GetClientSize().GetHeight();

    int top_item = results_list_->HitTestItem(wxPoint(5, 5));
    if (top_item == -1)
    {
        top_item = 0;
    }

    const int kPosition = top_item * kItemHeight;
    scrollbar_->UpdateScrollPosition(kPosition, kVisibleRange, kTotalRange);

    if (kTotalRange > kVisibleRange)
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
    search_debounce_timer_.StartOnce(300);
}

void SearchSidebarPanel::OnDebouncedSearch(wxTimerEvent& /*event*/)
{
    ExecuteSearch();
}

void SearchSidebarPanel::ExecuteSearch()
{
    const auto kQuery = search_input_->GetValue().ToStdString();
    if (kQuery.empty())
    {
        ClearResults();
        return;
    }

    MARKAMP_LOG_INFO("SearchSidebarPanel: Executing search for '{}'", kQuery);

    // Setup search options
    core::WorkspaceSearchEngine::SearchOptions options;
    options.query = kQuery;
    options.regex_mode = use_regex_;
    options.case_sensitive = match_case_;
    options.whole_word = match_word_;
    options.context_lines = show_context_ ? 1 : 0;

    auto ParseAndAddPatterns = [](const std::string& input, std::vector<std::string>& patterns)
    {
        std::string text = input;
        if (!text.empty())
        {
            size_t delim_pos = 0;
            while ((delim_pos = text.find(',')) != std::string::npos)
            {
                patterns.push_back(text.substr(0, delim_pos));
                text.erase(0, delim_pos + 1);
            }
            if (!text.empty())
            {
                patterns.push_back(text);
            }
        }
    };

    // Parse includes
    ParseAndAddPatterns(files_include_input_->GetValue().ToStdString(), options.include_patterns);

    // Parse excludes
    ParseAndAddPatterns(files_exclude_input_->GetValue().ToStdString(), options.exclude_patterns);

    // Note: To truly integrate, we should get the workspace root from core::WorkspaceService
    // For now, using a placeholder string or current working directory if available.
    const std::string kWorkspaceRoot = ".";

    // Cancel any existing search
    search_engine_->Cancel();

    if (search_future_.valid())
    {
        search_future_.wait();
    }

    // Spin up new search task (Temporary placeholder for proper async task management)
    search_future_ = std::async(
        std::launch::async,
        [this, options, kWorkspaceRoot]()
        {
            auto result = search_engine_->Search(options, kWorkspaceRoot);

            wxTheApp->CallAfter(
                [this, result]()
                {
                    if (results_list_ != nullptr)
                    {
                        results_list_->ClearResults();
                        if (result.matches.empty())
                        {
                            results_list_->Hide();
                            if (empty_state_ != nullptr)
                            {
                                empty_state_->Show();
                            }
                        }
                        else
                        {
                            if (empty_state_ != nullptr)
                            {
                                empty_state_->Hide();
                            }
                            results_list_->SetResults(result);
                            results_list_->Show();
                        }
                        Layout();
                    }
                    if (match_badge_ != nullptr)
                    {
                        match_count_ = static_cast<int>(result.matches.size());
                        match_badge_->SetMatchCount(match_count_);
                    }
                    if (footer_ != nullptr)
                    {
                        footer_->set_text("Found " + std::to_string(result.matches.size()) +
                                          " matches in " +
                                          std::to_string(result.files_with_matches) + " files");
                    }
                });
        });
}

void SearchSidebarPanel::OnResultSelected(wxCommandEvent& /*event*/)
{
    // Deprecated for SearchResultsTree, using SetOnResultSelected callback.
}

SearchSidebarPanel::~SearchSidebarPanel()
{
    if (search_engine_)
    {
        search_engine_->Cancel();
    }
    if (search_future_.valid())
    {
        search_future_.wait();
    }

    if (config_ != nullptr)
    {
        const std::string kPrefix =
            "sidebar." + (persistence_id_.empty() ? "" : persistence_id_ + ".") + "search.state.";
        config_->set(kPrefix + "query", search_input_->GetValue().ToStdString());
        config_->set(kPrefix + "replace", replace_input_->GetValue().ToStdString());
        config_->set(kPrefix + "include", files_include_input_->GetValue().ToStdString());
        config_->set(kPrefix + "exclude", files_exclude_input_->GetValue().ToStdString());
        config_->set(kPrefix + "regex", use_regex_);
        config_->set(kPrefix + "case_sensitive", match_case_);
        config_->set(kPrefix + "whole_word", match_word_);
        config_->set(kPrefix + "preserve_case", preserve_case_);
        auto result = config_->save();
        if (!result)
        {
            // Log error
        }
    }
}

} // namespace markamp::ui
