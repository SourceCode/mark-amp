#include "CommandPalette.h"

#include <wx/sizer.h>

#include <algorithm>
#include <cctype>

namespace markamp::ui
{

CommandPalette::CommandPalette(wxWindow* parent, core::ThemeEngine& theme_engine)
    : wxDialog(parent,
               wxID_ANY,
               wxEmptyString,
               wxDefaultPosition,
               wxSize(500, 350),
               wxBORDER_NONE | wxSTAY_ON_TOP)
    , theme_engine_(theme_engine)
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    input_ = new wxTextCtrl(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    sizer->Add(input_, 0, wxEXPAND | wxALL, 8);

    list_ = new wxListBox(
        this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_SINGLE | wxLB_NEEDED_SB);
    sizer->Add(list_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    SetSizer(sizer);

    // Event bindings
    input_->Bind(wxEVT_TEXT, &CommandPalette::OnFilterChanged, this);
    input_->Bind(wxEVT_KEY_DOWN, &CommandPalette::OnKeyDown, this);
    list_->Bind(wxEVT_LISTBOX_DCLICK, &CommandPalette::OnCommandSelected, this);
    list_->Bind(wxEVT_KEY_DOWN, &CommandPalette::OnKeyDown, this);

    ApplyTheme();
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
        if (sel < static_cast<int>(list_->GetCount()) - 1)
        {
            list_->SetSelection(sel + 1);
        }
        return;
    }

    if (key == WXK_UP)
    {
        int sel = list_->GetSelection();
        if (sel > 0)
        {
            list_->SetSelection(sel - 1);
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
            scored.push_back({idx, 1});
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

    // Update list
    list_->Clear();
    filtered_indices_.clear();
    for (const auto& entry : scored)
    {
        const auto& cmd = all_commands_[entry.index];
        auto display = cmd.category + ": " + cmd.label;
        if (!cmd.shortcut.empty())
        {
            display += "  (" + cmd.shortcut + ")";
        }
        list_->Append(wxString::FromUTF8(display));
        filtered_indices_.push_back(entry.index);
    }

    if (!filtered_indices_.empty())
    {
        list_->SetSelection(0);
    }
}

void CommandPalette::ExecuteSelected()
{
    int sel = list_->GetSelection();
    if (sel == wxNOT_FOUND || sel >= static_cast<int>(filtered_indices_.size()))
    {
        return;
    }

    size_t cmd_index = filtered_indices_[static_cast<size_t>(sel)];
    Hide();

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

    SetBackgroundColour(bg_color);
    input_->SetBackgroundColour(input_bg);
    input_->SetForegroundColour(fg_color);
    list_->SetBackgroundColour(bg_color);
    list_->SetForegroundColour(fg_color);
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
