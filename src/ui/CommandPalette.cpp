#include "CommandPalette.h"

#include "core/Events.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/panel.h>
#include <wx/sizer.h>

#include <algorithm>
#include <cctype>

namespace markamp::ui
{

class PaletteListBox : public wxVListBox
{
public:
    PaletteListBox(CommandPalette* logic_parent, wxWindow* ui_parent)
        : wxVListBox(
              ui_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxVSCROLL)
        , parent_palette_(logic_parent)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
    }

protected:
    void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const override
    {
        parent_palette_->DrawListItem(dc, rect, n);
    }

    wxCoord OnMeasureItem(size_t n) const override
    {
        return parent_palette_->MeasureListItem(n);
    }

    void OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const override
    {
        parent_palette_->DrawListItemBackground(dc, rect, n);
    }

private:
    CommandPalette* parent_palette_;
};

CommandPalette::CommandPalette(wxWindow* parent,
                               core::ThemeEngine& theme_engine,
                               core::EventBus& event_bus)
    : wxDialog(parent,
               wxID_ANY,
               wxEmptyString,
               wxDefaultPosition,
               wxSize(500, 350),
               wxBORDER_NONE | wxSTAY_ON_TOP)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    // 30. Drop Shadow / Pop wrapper: 1px accent border wrapper around the main content
    auto* outer_sizer = new wxBoxSizer(wxVERTICAL);
    auto* content_panel = new wxPanel(this);
    outer_sizer->Add(content_panel, 1, wxEXPAND | wxALL, 1);
    SetSizer(outer_sizer);

    auto* sizer = new wxBoxSizer(wxVERTICAL);

    input_ = new wxTextCtrl(content_panel,
                            wxID_ANY,
                            wxEmptyString,
                            wxDefaultPosition,
                            wxDefaultSize,
                            wxTE_PROCESS_ENTER | wxBORDER_NONE);
    // 29. Input Padding: Increase internal padding via sizer margin
    sizer->Add(input_, 0, wxEXPAND | wxALL, 12);

    list_ = new PaletteListBox(this, content_panel);
    sizer->Add(list_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    content_panel->SetSizer(sizer);

    // Event bindings
    input_->Bind(wxEVT_TEXT, &CommandPalette::OnFilterChanged, this);
    input_->Bind(wxEVT_KEY_DOWN, &CommandPalette::OnKeyDown, this);
    list_->Bind(wxEVT_LISTBOX_DCLICK, &CommandPalette::OnCommandSelected, this);
    list_->Bind(wxEVT_KEY_DOWN, &CommandPalette::OnKeyDown, this);

    ApplyTheme();

    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
}

void CommandPalette::RegisterCommand(PaletteCommand command)
{
    all_commands_.push_back(std::move(command));
}

void CommandPalette::RegisterCommands(std::vector<PaletteCommand> commands)
{
    for (auto& cmd : commands)
    {
        all_commands_.push_back(std::move(cmd));
    }
}

void CommandPalette::ClearCommands()
{
    all_commands_.clear();
}

void CommandPalette::ShowPalette()
{
    ApplyTheme();
    input_->Clear();
    ApplyFilter();

    // Center on parent
    CenterOnParent();
    Show(true);
    input_->SetFocus();
}

void CommandPalette::OnFilterChanged(wxCommandEvent& /*event*/)
{
    ApplyFilter();
}

void CommandPalette::OnCommandSelected(wxCommandEvent& /*event*/)
{
    ExecuteSelected();
}

void CommandPalette::OnKeyDown(wxKeyEvent& event)
{
    int key = event.GetKeyCode();

    if (key == WXK_ESCAPE)
    {
        Hide();
        return;
    }

    if (key == WXK_RETURN || key == WXK_NUMPAD_ENTER)
    {
        ExecuteSelected();
        return;
    }

    if (key == WXK_DOWN)
    {
        int sel = list_->GetSelection();
        for (int i = sel + 1; i < static_cast<int>(display_items_.size()); ++i)
        {
            if (display_items_[static_cast<std::size_t>(i)].type == ItemType::Command)
            {
                list_->SetSelection(i);
                list_->Refresh();
                break;
            }
        }
        return;
    }

    if (key == WXK_UP)
    {
        int sel = list_->GetSelection();
        for (int i = sel - 1; i >= 0; --i)
        {
            if (display_items_[static_cast<std::size_t>(i)].type == ItemType::Command)
            {
                list_->SetSelection(i);
                list_->Refresh();
                break;
            }
        }
        return;
    }

    event.Skip();
}

void CommandPalette::ApplyFilter()
{
    auto filter = input_->GetValue().ToStdString();
    std::string filter_lower;
    filter_lower.reserve(filter.size());
    for (char chr : filter)
    {
        filter_lower += static_cast<char>(std::tolower(static_cast<unsigned char>(chr)));
    }
    current_filter_ = filter_lower;

    // Score all commands
    struct ScoredIndex
    {
        size_t index;
        int score;
    };
    std::vector<ScoredIndex> scored;
    scored.reserve(all_commands_.size());

    for (size_t idx = 0; idx < all_commands_.size(); ++idx)
    {
        if (filter_lower.empty())
        {
            // R18 Fix 17: Score by MRU position when no filter
            int mru_score = 100;
            auto mru_it =
                std::find(mru_history_.begin(), mru_history_.end(), all_commands_[idx].label);
            if (mru_it != mru_history_.end())
            {
                mru_score = 200 - static_cast<int>(std::distance(mru_history_.begin(), mru_it));
            }
            scored.push_back({idx, mru_score});
        }
        else
        {
            auto combined = all_commands_[idx].category + ": " + all_commands_[idx].label;
            int sc = FuzzyScore(filter_lower, combined);
            if (sc > 0)
            {
                scored.push_back({idx, sc});
            }
        }
    }

    // Sort by score descending
    std::sort(scored.begin(),
              scored.end(),
              [](const ScoredIndex& left, const ScoredIndex& right)
              { return left.score > right.score; });

    display_items_.clear();
    std::string last_category;
    for (const auto& entry : scored)
    {
        const auto& cmd = all_commands_[entry.index];

        // R18 Fix 18: Insert category header when category changes
        if (cmd.category != last_category)
        {
            last_category = cmd.category;
            display_items_.push_back({ItemType::Header, 0, cmd.category});
        }
        display_items_.push_back({ItemType::Command, entry.index, ""});
    }

    if (display_items_.empty() && !filter_lower.empty())
    {
        // 28. Empty State: Display an aesthetic "No results found" placeholder
        display_items_.push_back({ItemType::Empty, 0, "✦ No commands found ✦"});
        display_items_.push_back({ItemType::Empty, 0, "Try a different search term"});
    }

    list_->SetItemCount(display_items_.size());
    list_->SetSelection(-1);

    // Auto-select first command
    for (size_t i = 0; i < display_items_.size(); ++i)
    {
        if (display_items_[i].type == ItemType::Command)
        {
            list_->SetSelection(static_cast<int>(i));
            break;
        }
    }

    list_->Refresh();
}

void CommandPalette::ExecuteSelected()
{
    int sel = list_->GetSelection();
    if (sel == wxNOT_FOUND || sel >= static_cast<int>(display_items_.size()))
    {
        return;
    }

    if (display_items_[static_cast<std::size_t>(sel)].type != ItemType::Command)
    {
        return;
    }

    size_t cmd_index = display_items_[static_cast<std::size_t>(sel)].cmd_index;
    Hide();

    // R18 Fix 17: Update MRU history
    const auto& label = all_commands_[cmd_index].label;
    auto mru_it = std::find(mru_history_.begin(), mru_history_.end(), label);
    if (mru_it != mru_history_.end())
    {
        mru_history_.erase(mru_it);
    }
    mru_history_.insert(mru_history_.begin(), label);
    if (mru_history_.size() > 20)
    {
        mru_history_.resize(20);
    }

    if (all_commands_[cmd_index].action)
    {
        all_commands_[cmd_index].action();
    }
}

void CommandPalette::ApplyTheme()
{
    auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    auto input_bg = theme_engine_.color(core::ThemeColorToken::BgInput);
    auto accent = theme_engine_.color(core::ThemeColorToken::AccentPrimary);

    // 30. Drop Shadow / Window pop: Set Dialog BG to accent to create 1px border
    SetBackgroundColour(accent.ChangeLightness(50));
    GetSizer()->GetItem(size_t(0))->GetWindow()->SetBackgroundColour(bg_color); // content_panel

    input_->SetBackgroundColour(input_bg);
    input_->SetForegroundColour(fg_color);
    list_->SetBackgroundColour(bg_color);
    list_->SetForegroundColour(fg_color);
}

void CommandPalette::DrawListItemBackground(wxDC& dc, const wxRect& rect, size_t n) const
{
    if (list_->IsSelected(n) && display_items_[n].type == ItemType::Command)
    {
        // 31. Selection Indicator: Prominent background highlight
        auto select_bg =
            theme_engine_.color(core::ThemeColorToken::AccentPrimary).ChangeLightness(70);
        dc.SetBrush(wxBrush(select_bg));
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(rect);

        // Subtly brighter left edge border for selected items
        dc.SetBrush(wxBrush(
            theme_engine_.color(core::ThemeColorToken::AccentPrimary).ChangeLightness(140)));
        dc.DrawRectangle(rect.x, rect.y, 3, rect.height);
    }
    else
    {
        dc.SetBrush(wxBrush(theme_engine_.color(core::ThemeColorToken::BgPanel)));
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(rect);
    }
}

wxCoord CommandPalette::MeasureListItem(size_t n) const
{
    if (display_items_[n].type == ItemType::Header)
        return 24;
    if (display_items_[n].type == ItemType::Empty)
        return 30;
    return 26;
}

void CommandPalette::DrawListItem(wxDC& dc, const wxRect& rect, size_t n) const
{
    const auto& item = display_items_[n];
    if (item.type == ItemType::Header)
    {
        wxFont bold_font = theme_engine_.font(core::ThemeFontToken::UISmall);
        bold_font.SetWeight(wxFONTWEIGHT_SEMIBOLD);
        dc.SetFont(bold_font);
        dc.SetTextForeground(theme_engine_.color(core::ThemeColorToken::TextMuted));
        wxString text = wxString::FromUTF8("── " + item.label + " ──");
        int w, h;
        dc.GetTextExtent(text, &w, &h);
        dc.DrawText(text, rect.x + 8, rect.y + (rect.height - h) / 2);
        return;
    }
    else if (item.type == ItemType::Empty)
    {
        dc.SetFont(theme_engine_.font(core::ThemeFontToken::SansRegular));
        dc.SetTextForeground(
            theme_engine_.color(core::ThemeColorToken::TextMuted).ChangeLightness(120));
        wxString empty1 = wxString::FromUTF8(item.label);
        int w1, h1;
        dc.GetTextExtent(empty1, &w1, &h1);
        dc.DrawText(empty1, rect.x + (rect.width - w1) / 2, rect.y + (rect.height - h1) / 2);
        return;
    }

    bool is_selected = list_->IsSelected(n);
    const auto& cmd = all_commands_[item.cmd_index];
    dc.SetFont(theme_engine_.font(core::ThemeFontToken::SansRegular));

    int text_y = rect.y + (rect.height - dc.GetCharHeight() + 1) / 2;
    int x_off = rect.x + 12;

    // Draw Category Badge
    dc.SetTextForeground(theme_engine_.color(core::ThemeColorToken::TextMuted));
    wxString cat_text = wxString::FromUTF8("[" + cmd.category + "] ");
    dc.DrawText(cat_text, x_off, text_y);
    x_off += dc.GetTextExtent(cat_text).GetWidth();

    // 27. Match Highlighting
    auto main_col = is_selected
                        ? theme_engine_.color(core::ThemeColorToken::TextMain)
                        : theme_engine_.color(core::ThemeColorToken::TextMain).ChangeLightness(180);
    auto hl_col =
        is_selected
            ? theme_engine_.color(core::ThemeColorToken::TextMain).ChangeLightness(220)
            : theme_engine_.color(core::ThemeColorToken::AccentPrimary).ChangeLightness(140);

    std::string label_lower;
    label_lower.reserve(cmd.label.size());
    for (char c : cmd.label)
        label_lower += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    if (!current_filter_.empty())
    {
        size_t fpos = 0;
        for (size_t ci = 0; ci < cmd.label.size(); ++ci)
        {
            wxString char_str = wxString::FromUTF8(std::string(1, cmd.label[ci]));
            if (fpos < current_filter_.size() && label_lower[ci] == current_filter_[fpos])
            {
                dc.SetTextForeground(hl_col); // Fuzzy match char color
                dc.DrawText(char_str, x_off, text_y);
                x_off += dc.GetTextExtent(char_str).GetWidth();
                ++fpos;
            }
            else
            {
                dc.SetTextForeground(main_col);
                dc.DrawText(char_str, x_off, text_y);
                x_off += dc.GetTextExtent(char_str).GetWidth();
            }
        }
    }
    else
    {
        dc.SetTextForeground(main_col);
        wxString label_str = wxString::FromUTF8(cmd.label);
        dc.DrawText(label_str, x_off, text_y);
    }

    // Shortcut
    if (!cmd.shortcut.empty())
    {
        dc.SetTextForeground(theme_engine_.color(core::ThemeColorToken::TextMuted));
        wxString short_str = wxString::FromUTF8(cmd.shortcut);
        int sw = dc.GetTextExtent(short_str).GetWidth();
        dc.DrawText(short_str, rect.x + rect.width - sw - 12, text_y);
    }
}

auto CommandPalette::FuzzyScore(const std::string& filter, const std::string& candidate) -> int
{
    // Simple subsequence matching with position bonus
    std::string candidate_lower;
    candidate_lower.reserve(candidate.size());
    for (char chr : candidate)
    {
        candidate_lower += static_cast<char>(std::tolower(static_cast<unsigned char>(chr)));
    }

    int score = 0;
    size_t filter_pos = 0;
    size_t last_match_pos = 0;

    for (size_t cand_pos = 0; cand_pos < candidate_lower.size() && filter_pos < filter.size();
         ++cand_pos)
    {
        if (candidate_lower[cand_pos] == filter[filter_pos])
        {
            ++score;
            // Bonus for consecutive matches
            if (cand_pos == last_match_pos + 1 && filter_pos > 0)
            {
                score += 2;
            }
            // Bonus for word boundary matches
            if (cand_pos == 0 || candidate_lower[cand_pos - 1] == ' ' ||
                candidate_lower[cand_pos - 1] == ':')
            {
                score += 3;
            }
            last_match_pos = cand_pos;
            ++filter_pos;
        }
    }

    // All filter characters must match
    if (filter_pos < filter.size())
    {
        return 0;
    }

    return score;
}

} // namespace markamp::ui
