#include "TableEditorOverlay.h"

#include "core/Color.h"
#include "core/Events.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>

namespace markamp::ui
{

TableEditorOverlay::TableEditorOverlay(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       core::EventBus& event_bus,
                                       CommitCallback on_commit)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , on_commit_(std::move(on_commit))
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    toolbar_panel_ = new wxPanel(this, wxID_ANY);
    grid_panel_ = new wxPanel(this, wxID_ANY);

    CreateToolbar();

    main_sizer->Add(toolbar_panel_, 0, wxEXPAND | wxALL, kPadding);
    main_sizer->Add(grid_panel_, 1, wxEXPAND | wxALL, kPadding);

    SetSizer(main_sizer);
    ApplyTheme();

    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
}

void TableEditorOverlay::CreateToolbar()
{
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    auto make_btn = [this](const wxString& label, const wxString& tip) -> wxButton*
    {
        auto* btn = new wxButton(toolbar_panel_, wxID_ANY, label, wxDefaultPosition, wxDefaultSize);
        btn->SetToolTip(tip);
        return btn;
    };

    auto* add_row_btn = make_btn("+Row", "Add row");
    auto* add_col_btn = make_btn("+Col", "Add column");
    auto* del_row_btn = make_btn("-Row", "Delete last row");
    auto* del_col_btn = make_btn("-Col", "Delete last column");
    auto* commit_btn = make_btn(wxString::FromUTF8("✓ Done"), "Apply changes");
    auto* cancel_btn = make_btn(wxString::FromUTF8("✗ Cancel"), "Discard changes");

    add_row_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { OnAddRow(); });
    add_col_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { OnAddColumn(); });
    del_row_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { OnDeleteRow(); });
    del_col_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { OnDeleteColumn(); });
    commit_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { OnCommit(); });
    cancel_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { OnCancel(); });

    sizer->Add(add_row_btn, 0, wxRIGHT, 4);
    sizer->Add(add_col_btn, 0, wxRIGHT, 4);
    sizer->Add(del_row_btn, 0, wxRIGHT, 4);
    sizer->Add(del_col_btn, 0, wxRIGHT, 12);
    sizer->AddStretchSpacer();
    sizer->Add(commit_btn, 0, wxRIGHT, 4);
    sizer->Add(cancel_btn, 0);

    toolbar_panel_->SetSizer(sizer);
}

auto TableEditorOverlay::LoadTable(const std::vector<std::string>& lines,
                                   int start_line,
                                   int end_line) -> bool
{
    start_line_ = start_line;
    end_line_ = end_line;
    cells_.clear();

    if (lines.empty())
    {
        return false;
    }

    for (const auto& line : lines)
    {
        if (IsSeparatorRow(line))
        {
            continue; // Skip separator rows (e.g., |---|---|)
        }

        auto row = ParseTableRow(line);
        if (row.empty())
        {
            continue;
        }

        cells_.push_back(row);
    }

    if (cells_.empty())
    {
        return false;
    }

    // Normalize: ensure all rows have the same column count
    num_cols_ = 0;
    for (const auto& row : cells_)
    {
        num_cols_ = std::max(num_cols_, static_cast<int>(row.size()));
    }
    for (auto& row : cells_)
    {
        while (static_cast<int>(row.size()) < num_cols_)
        {
            row.emplace_back("");
        }
    }
    num_rows_ = static_cast<int>(cells_.size());

    RebuildGrid();
    return true;
}

void TableEditorOverlay::RebuildGrid()
{
    // Clear existing widgets
    cell_widgets_.clear();
    grid_panel_->DestroyChildren();

    auto* grid_sizer = new wxFlexGridSizer(num_cols_, 4, 4);
    for (int col = 0; col < num_cols_; ++col)
    {
        grid_sizer->AddGrowableCol(static_cast<size_t>(col), 1);
    }

    cell_widgets_.resize(static_cast<size_t>(num_rows_));
    for (int row = 0; row < num_rows_; ++row)
    {
        auto row_idx = static_cast<size_t>(row);
        cell_widgets_[row_idx].resize(static_cast<size_t>(num_cols_));
        for (int col = 0; col < num_cols_; ++col)
        {
            auto col_idx = static_cast<size_t>(col);
            auto* cell = new wxTextCtrl(grid_panel_,
                                        wxID_ANY,
                                        wxString::FromUTF8(cells_[row_idx][col_idx]),
                                        wxDefaultPosition,
                                        wxSize(kCellMinWidth, kCellHeight));

            // Style header row differently
            if (row == 0)
            {
                auto font = cell->GetFont();
                font.SetWeight(wxFONTWEIGHT_BOLD);
                cell->SetFont(font);
                // R21 Fix 21: Header row accent background
                auto header_bg = theme_engine_.color(core::ThemeColorToken::BgHeader);
                cell->SetBackgroundColour(header_bg);
            }

            // R21 Fix 22: Active cell border highlight on focus
            cell->Bind(wxEVT_SET_FOCUS,
                       [this, cell](wxFocusEvent& evt)
                       {
                           auto accent = theme_engine_.color(core::ThemeColorToken::AccentPrimary);
                           cell->SetBackgroundColour(accent.ChangeLightness(170));
                           cell->Refresh();
                           evt.Skip();
                       });
            cell->Bind(wxEVT_KILL_FOCUS,
                       [this, cell, row](wxFocusEvent& evt)
                       {
                           auto bg = (row == 0)
                                         ? theme_engine_.color(core::ThemeColorToken::BgHeader)
                                         : theme_engine_.color(core::ThemeColorToken::BgInput);
                           cell->SetBackgroundColour(bg);
                           cell->Refresh();
                           evt.Skip();
                       });

            cell_widgets_[row_idx][col_idx] = cell;
            grid_sizer->Add(cell, 1, wxEXPAND);
        }
    }

    grid_panel_->SetSizer(grid_sizer);
    ApplyTheme();

    GetSizer()->Layout();
    Fit();
}

void TableEditorOverlay::SyncCellsFromWidgets()
{
    for (int row = 0; row < num_rows_; ++row)
    {
        auto row_idx = static_cast<size_t>(row);
        for (int col = 0; col < num_cols_; ++col)
        {
            auto col_idx = static_cast<size_t>(col);
            if (cell_widgets_[row_idx][col_idx] != nullptr)
            {
                cells_[row_idx][col_idx] =
                    cell_widgets_[row_idx][col_idx]->GetValue().ToStdString();
            }
        }
    }
}

auto TableEditorOverlay::SerializeToMarkdown() const -> std::string
{
    if (cells_.empty() || num_cols_ == 0)
    {
        return "";
    }

    // Calculate column widths
    std::vector<int> col_widths(static_cast<size_t>(num_cols_), 3); // minimum 3 for "---"
    for (const auto& row : cells_)
    {
        for (int col = 0; col < num_cols_; ++col)
        {
            auto col_idx = static_cast<size_t>(col);
            col_widths[col_idx] =
                std::max(col_widths[col_idx], static_cast<int>(row[col_idx].size()));
        }
    }

    std::ostringstream oss;

    // Header row
    oss << "|";
    for (int col = 0; col < num_cols_; ++col)
    {
        auto col_idx = static_cast<size_t>(col);
        oss << " ";
        oss << std::left << std::setw(col_widths[col_idx]) << cells_[0][col_idx];
        oss << " |";
    }
    oss << "\n";

    // Separator row
    oss << "|";
    for (int col = 0; col < num_cols_; ++col)
    {
        auto col_idx = static_cast<size_t>(col);
        oss << " ";
        oss << std::string(static_cast<size_t>(col_widths[col_idx]), '-');
        oss << " |";
    }
    oss << "\n";

    // Data rows
    for (int row = 1; row < static_cast<int>(cells_.size()); ++row)
    {
        auto row_idx = static_cast<size_t>(row);
        oss << "|";
        for (int col = 0; col < num_cols_; ++col)
        {
            auto col_idx = static_cast<size_t>(col);
            oss << " ";
            oss << std::left << std::setw(col_widths[col_idx]) << cells_[row_idx][col_idx];
            oss << " |";
        }
        oss << "\n";
    }

    return oss.str();
}

void TableEditorOverlay::ApplyTheme()
{
    auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    auto input_bg = theme_engine_.color(core::ThemeColorToken::BgInput);

    SetBackgroundColour(bg_color);
    toolbar_panel_->SetBackgroundColour(bg_color);
    grid_panel_->SetBackgroundColour(bg_color);

    // Style toolbar buttons
    for (auto* child : toolbar_panel_->GetChildren())
    {
        auto* btn = dynamic_cast<wxButton*>(child);
        if (btn != nullptr)
        {
            btn->SetBackgroundColour(bg_color);
            btn->SetForegroundColour(fg_color);
        }
    }

    // Style cell inputs
    for (auto& row : cell_widgets_)
    {
        for (auto* cell : row)
        {
            if (cell != nullptr)
            {
                cell->SetBackgroundColour(input_bg);
                cell->SetForegroundColour(fg_color);
            }
        }
    }

    Refresh();
}

// ── Toolbar actions ──

void TableEditorOverlay::OnAddRow()
{
    SyncCellsFromWidgets();
    cells_.emplace_back(static_cast<size_t>(num_cols_), "");
    num_rows_++;
    RebuildGrid();
}

void TableEditorOverlay::OnAddColumn()
{
    SyncCellsFromWidgets();
    num_cols_++;
    for (auto& row : cells_)
    {
        row.emplace_back("");
    }
    RebuildGrid();
}

void TableEditorOverlay::OnDeleteRow()
{
    if (num_rows_ <= 1)
    {
        return; // Keep at least the header
    }
    SyncCellsFromWidgets();
    cells_.pop_back();
    num_rows_--;
    RebuildGrid();
}

void TableEditorOverlay::OnDeleteColumn()
{
    if (num_cols_ <= 1)
    {
        return; // Keep at least one column
    }
    SyncCellsFromWidgets();
    num_cols_--;
    for (auto& row : cells_)
    {
        if (!row.empty())
        {
            row.pop_back();
        }
    }
    RebuildGrid();
}

void TableEditorOverlay::OnCommit()
{
    SyncCellsFromWidgets();
    if (on_commit_)
    {
        on_commit_(SerializeToMarkdown(), start_line_, end_line_);
    }
    Hide();
}

void TableEditorOverlay::OnCancel()
{
    Hide();
}

// ── Parse helpers ──

auto TableEditorOverlay::ParseTableRow(const std::string& line) -> std::vector<std::string>
{
    std::vector<std::string> result;

    // Strip leading/trailing whitespace
    auto trimmed = line;
    while (!trimmed.empty() && trimmed.front() == ' ')
    {
        trimmed.erase(trimmed.begin());
    }
    while (!trimmed.empty() &&
           (trimmed.back() == ' ' || trimmed.back() == '\n' || trimmed.back() == '\r'))
    {
        trimmed.pop_back();
    }

    if (trimmed.empty() || trimmed.front() != '|')
    {
        return result;
    }

    // Remove leading and trailing pipes
    if (trimmed.front() == '|')
    {
        trimmed.erase(trimmed.begin());
    }
    if (!trimmed.empty() && trimmed.back() == '|')
    {
        trimmed.pop_back();
    }

    // Split by pipe
    std::istringstream stream(trimmed);
    std::string cell;
    while (std::getline(stream, cell, '|'))
    {
        result.push_back(TrimCell(cell));
    }

    return result;
}

auto TableEditorOverlay::IsSeparatorRow(const std::string& line) -> bool
{
    // A separator row contains only pipes, dashes, colons, and spaces
    for (char chr : line)
    {
        if (chr != '|' && chr != '-' && chr != ':' && chr != ' ' && chr != '\n' && chr != '\r')
        {
            return false;
        }
    }
    // Must contain at least one dash
    return line.find('-') != std::string::npos;
}

auto TableEditorOverlay::TrimCell(const std::string& cell) -> std::string
{
    auto result = cell;
    while (!result.empty() && result.front() == ' ')
    {
        result.erase(result.begin());
    }
    while (!result.empty() && result.back() == ' ')
    {
        result.pop_back();
    }
    return result;
}

} // namespace markamp::ui
