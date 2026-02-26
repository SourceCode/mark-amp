// ============================================================================
// File: src/ui/DiffPanel.cpp
// Phase 12 / 33: Version Diff & Comparison — DiffPanel implementation
// ============================================================================
#include "DiffPanel.h"

#include <wx/sizer.h>

#include <fstream>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(DiffPanel, wxPanel) wxEND_EVENT_TABLE()

    DiffPanel::DiffPanel(wxWindow* parent, core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY)
    , event_bus_(event_bus)
{
    SetBackgroundColour(wxColour(30, 30, 30));

    auto* sizer = new wxBoxSizer(wxVERTICAL);

    splitter_ = new wxSplitterWindow(
        this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE);

    left_editor_ =
        new wxStyledTextCtrl(splitter_, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    right_editor_ =
        new wxStyledTextCtrl(splitter_, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);

    InitializeEditor(left_editor_, true);
    InitializeEditor(right_editor_, false);

    splitter_->SplitVertically(left_editor_, right_editor_);
    splitter_->SetSashGravity(0.5);

    sizer->Add(splitter_, 1, wxEXPAND);
    SetSizer(sizer);

    left_editor_->Bind(wxEVT_STC_UPDATEUI, &DiffPanel::OnLeftScroll, this);
    right_editor_->Bind(wxEVT_STC_UPDATEUI, &DiffPanel::OnRightScroll, this);
}

void DiffPanel::InitializeEditor(wxStyledTextCtrl* editor, bool read_only)
{
    editor->StyleClearAll();
    editor->SetLexer(wxSTC_LEX_NULL);
    editor->SetMarginWidth(0, 40);
    editor->SetMarginType(0, wxSTC_MARGIN_NUMBER);

    // Some basic tokens for visual contrast
    editor->StyleSetForeground(wxSTC_STYLE_DEFAULT, wxColour(200, 200, 200));
    editor->StyleSetBackground(wxSTC_STYLE_DEFAULT, wxColour(30, 30, 30));

    editor->SetReadOnly(read_only);
}

void DiffPanel::open_diff(const std::string& left_path, const std::string& right_path)
{
    left_editor_->SetReadOnly(false);
    LoadFileSide(left_editor_, left_path);
    left_editor_->SetReadOnly(true);

    LoadFileSide(right_editor_, right_path);
}

void DiffPanel::LoadFileSide(wxStyledTextCtrl* editor, const std::string& path)
{
    std::string content;
    std::ifstream file_stream(path);
    if (file_stream.is_open())
    {
        content.assign(std::istreambuf_iterator<char>(file_stream),
                       std::istreambuf_iterator<char>());
    }
    editor->SetText(content);
}

void DiffPanel::show_diff(const core::BlockDiffResult& diff_result)
{
    total_changes_ = diff_result.stats.changed_blocks();
    current_change_index_ = total_changes_ > 0 ? 0 : -1;
}

void DiffPanel::set_view_mode(core::DiffViewMode mode)
{
    view_mode_ = mode;
}

auto DiffPanel::view_mode() const -> core::DiffViewMode
{
    return view_mode_;
}

void DiffPanel::navigate_next_change()
{
    if (current_change_index_ < total_changes_ - 1)
    {
        ++current_change_index_;
    }
}

void DiffPanel::navigate_prev_change()
{
    if (current_change_index_ > 0)
    {
        --current_change_index_;
    }
}

void DiffPanel::SyncScroll(wxStyledTextCtrl* source, wxStyledTextCtrl* target)
{
    if (is_syncing_scroll_)
        return;
    is_syncing_scroll_ = true;

    int first_line = source->GetFirstVisibleLine();
    target->SetFirstVisibleLine(first_line);

    is_syncing_scroll_ = false;
}

void DiffPanel::OnLeftScroll(wxStyledTextEvent& evt)
{
    if (evt.GetUpdated() & wxSTC_UPDATE_V_SCROLL)
    {
        SyncScroll(left_editor_, right_editor_);
    }
    evt.Skip();
}

void DiffPanel::OnRightScroll(wxStyledTextEvent& evt)
{
    if (evt.GetUpdated() & wxSTC_UPDATE_V_SCROLL)
    {
        SyncScroll(right_editor_, left_editor_);
    }
    evt.Skip();
}

} // namespace markamp::ui
