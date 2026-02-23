#include "CommandPalette.h"

#include "accessibility/AccessibilityController.h"
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
    void OnDrawItem(wxDC& draw_dc, const wxRect& rect, size_t n) const override
    {
        parent_palette_->DrawListItem(draw_dc, rect, n);
    }

    wxCoord OnMeasureItem(size_t n) const override
    {
        return parent_palette_->MeasureListItem(n);
    }

    void OnDrawBackground(wxDC& draw_dc, const wxRect& rect, size_t n) const override
    {
        parent_palette_->DrawListItemBackground(draw_dc, rect, n);
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
    input_->SetToolTip("Type to search commands...");
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

    // Animation configs
    animation::AnimationConfig open_cfg;
    open_cfg.duration = std::chrono::milliseconds(150);
    open_cfg.easing_type = animation::EasingType::EaseOutCubic;
    transition_manager_.register_transition("palette_open", open_cfg);

    animation::AnimationConfig close_cfg;
    close_cfg.duration = std::chrono::milliseconds(100);
    close_cfg.easing_type = animation::EasingType::EaseInQuad;
    transition_manager_.register_transition("palette_close", close_cfg);

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
    is_closing_ = false;

    ApplyTheme();
    input_->Clear();
    ApplyFilter();

    SetTransparent(0);
    SetSize(475, 332);
    CenterOnParent();

    Show(true);

    CallAfter(
        [this]()
        {
            if (input_ != nullptr)
            {
                input_->SetFocus();
            }
        });

    transition_manager_.start<float>(
        "palette_open",
        0.0F,
        1.0F,
        [this](float t)
        {
            SetTransparent(static_cast<wxByte>(255.0F * t));
            const float scale = 0.95F + 0.05F * t;
            SetSize(static_cast<int>(500.0F * scale), static_cast<int>(350.0F * scale));
            CenterOnParent();
        },
        [this]()
        {
            SetTransparent(255);
            SetSize(500, 350);
            CenterOnParent();
        });
}

void CommandPalette::HidePalette()
{
    if (is_closing_)
        return;
    is_closing_ = true;

    transition_manager_.start<float>(
        "palette_close",
        1.0F,
        0.0F,
        [this](float t)
        {
            SetTransparent(static_cast<wxByte>(255.0F * t));
            const float scale = 0.95F + 0.05F * t;
            SetSize(static_cast<int>(500.0F * scale), static_cast<int>(350.0F * scale));
            CenterOnParent();
        },
        [this]()
        {
            Hide();
            is_closing_ = false;
        });
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
    const int key_code = event.GetKeyCode();

    if (key_code == WXK_ESCAPE)
    {
        HidePalette();
        return;
    }

    if (key_code == WXK_RETURN || key_code == WXK_NUMPAD_ENTER)
    {
        ExecuteSelected();
        return;
    }

    if (key_code == WXK_DOWN)
    {
        const int current_sel = list_->GetSelection();
        for (int i = current_sel + 1; i < static_cast<int>(display_items_.size()); ++i)
        {
            if (display_items_[static_cast<std::size_t>(i)].type == ItemType::Command)
            {
                list_->SetSelection(i);
                list_->Refresh();

                const size_t cmd_idx = display_items_[static_cast<std::size_t>(i)].cmd_index;
                accessibility::AccessibilityController::get().announce_focus(
                    all_commands_[cmd_idx].label, "Command", "Selected");
                break;
            }
        }
        return;
    }

    if (key_code == WXK_UP)
    {
        const int current_sel = list_->GetSelection();
        for (int i = current_sel - 1; i >= 0; --i)
        {
            if (display_items_[static_cast<std::size_t>(i)].type == ItemType::Command)
            {
                list_->SetSelection(i);
                list_->Refresh();

                const size_t cmd_idx = display_items_[static_cast<std::size_t>(i)].cmd_index;
                accessibility::AccessibilityController::get().announce_focus(
                    all_commands_[cmd_idx].label, "Command", "Selected");
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
            const int score_val = FuzzyScore(filter_lower, combined);
            if (score_val > 0)
            {
                scored.push_back({idx, score_val});
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
        display_items_.push_back({ItemType::Empty, 0, "No commands match your search"});
        display_items_.push_back({ItemType::Empty, 0, "Try entering different keywords"});
    }

    list_->SetItemCount(display_items_.size());
    list_->SetSelection(-1);

    // Auto-select first command
    for (size_t i = 0; i < display_items_.size(); ++i)
    {
        if (display_items_[i].type == ItemType::Command)
        {
            list_->SetSelection(static_cast<int>(i));
            const size_t cmd_idx = display_items_[i].cmd_index;
            accessibility::AccessibilityController::get().announce_focus(
                all_commands_[cmd_idx].label, "Command", "Selected");
            break;
        }
    }

    list_->Refresh();
}

void CommandPalette::ExecuteSelected()
{
    const int current_sel = list_->GetSelection();
    if (current_sel == wxNOT_FOUND || current_sel >= static_cast<int>(display_items_.size()))
    {
        return;
    }

    if (display_items_[static_cast<std::size_t>(current_sel)].type != ItemType::Command)
    {
        return;
    }

    const size_t cmd_index = display_items_[static_cast<std::size_t>(current_sel)].cmd_index;
    HidePalette();

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

void CommandPalette::DrawListItemBackground(wxDC& draw_dc, const wxRect& rect, size_t n) const
{
    if (list_->IsSelected(n) && display_items_[n].type == ItemType::Command)
    {
        // 31. Selection Indicator: Prominent background highlight
        auto select_bg =
            theme_engine_.color(core::ThemeColorToken::AccentPrimary).ChangeLightness(50);
        draw_dc.SetBrush(wxBrush(select_bg));
        draw_dc.SetPen(*wxTRANSPARENT_PEN);
        draw_dc.DrawRectangle(rect);

        // Subtly brighter left edge border for selected items
        draw_dc.SetBrush(wxBrush(
            theme_engine_.color(core::ThemeColorToken::AccentPrimary).ChangeLightness(120)));
        draw_dc.DrawRectangle(rect.x, rect.y, 4, rect.height);
    }
    else
    {
        draw_dc.SetBrush(wxBrush(theme_engine_.color(core::ThemeColorToken::BgPanel)));
        draw_dc.SetPen(*wxTRANSPARENT_PEN);
        draw_dc.DrawRectangle(rect);
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

void CommandPalette::DrawListItem(wxDC& draw_dc, const wxRect& rect, size_t n) const
{
    const auto& item = display_items_[n];
    auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    auto muted_color = theme_engine_.color(core::ThemeColorToken::TextMuted);
    auto accent = theme_engine_.color(core::ThemeColorToken::AccentPrimary);

    if (item.type == ItemType::Empty)
    {
        draw_dc.SetTextForeground(muted_color);
        const wxString empty_text = item.label;
        const auto extent = draw_dc.GetTextExtent(empty_text);

        if (extent.GetWidth() > 0 && extent.GetHeight() > 0)
        {
            draw_dc.DrawText(empty_text,
                             rect.x + (rect.width - extent.GetWidth()) / 2,
                             rect.y + (rect.height - extent.GetHeight()) / 2);
        }
        return;
    }
    else if (item.type == ItemType::Header)
    {
        draw_dc.SetTextForeground(muted_color);
        auto font = draw_dc.GetFont();
        font.SetWeight(wxFONTWEIGHT_BOLD);
        draw_dc.SetFont(font);

        const wxString empty_cat = item.label;
        const auto extent_cat = draw_dc.GetTextExtent(empty_cat);

        if (extent_cat.GetWidth() > 0 && extent_cat.GetHeight() > 0)
        {
            draw_dc.DrawText(
                item.label, rect.x + 8, rect.y + (rect.height - extent_cat.GetHeight()) / 2);
        }
        return;
    }

    const bool is_selected = list_->IsSelected(n);
    if (is_selected)
    {
        draw_dc.SetTextForeground(theme_engine_.color(core::ThemeColorToken::TextMain));
    }
    else
    {
        draw_dc.SetTextForeground(fg_color);
    }

    const int text_y_pos = rect.y + (rect.height - draw_dc.GetCharHeight()) / 2;

    const auto& cmd = all_commands_[item.cmd_index];

    // Split category and label
    const wxString cat_text = wxString::FromUTF8(cmd.category) + ": ";
    draw_dc.DrawText(cat_text, rect.x + 12, text_y_pos);

    int cat_w = draw_dc.GetTextExtent(cat_text).GetWidth();
    int draw_x = rect.x + 12 + cat_w;

    auto font_normal = draw_dc.GetFont();

    // Draw label, bolding matching characters
    if (!current_filter_.empty())
    {
        wxFont font_bold = font_normal;
        font_bold.SetWeight(wxFONTWEIGHT_BOLD);

        std::string label_lower;
        for (const char chr : cmd.label)
        {
            label_lower += static_cast<char>(std::tolower(static_cast<unsigned char>(chr)));
        }

        for (size_t i = 0; i < cmd.label.size(); ++i)
        {
            const wxString char_str = wxString::FromUTF8(std::string(1, cmd.label[i]));
            bool is_match = false;
            if (current_filter_.find(label_lower[i]) != std::string::npos)
            {
                is_match = true;
            }

            if (is_match)
            {
                draw_dc.SetFont(font_bold);
                draw_dc.SetTextForeground(accent);
            }
            else
            {
                draw_dc.SetFont(font_normal);
                draw_dc.SetTextForeground(
                    is_selected ? theme_engine_.color(core::ThemeColorToken::TextMain) : fg_color);
            }

            draw_dc.DrawText(char_str, draw_x, text_y_pos);
            draw_x += draw_dc.GetTextExtent(char_str).GetWidth();
        }
        draw_dc.SetFont(font_normal);
    }
    else
    {
        const wxString label_str = wxString::FromUTF8(cmd.label);
        draw_dc.DrawText(label_str, draw_x, text_y_pos);
    }

    // Shortcut rendering on right side
    if (!cmd.shortcut.empty())
    {
        // 53. Monospace font for hints
        wxFont font_mono = font_normal;
        font_mono.SetFamily(wxFONTFAMILY_TELETYPE); // Use monospace
        draw_dc.SetFont(font_mono);

        const wxString short_str = wxString::FromUTF8(cmd.shortcut);
        const int shortcut_w = draw_dc.GetTextExtent(short_str).GetWidth();
        draw_dc.SetTextForeground(muted_color);
        draw_dc.DrawText(short_str, rect.x + rect.width - shortcut_w - 16, text_y_pos);
        draw_dc.SetFont(font_normal); // restore
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
