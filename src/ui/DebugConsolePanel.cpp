#include "ui/DebugConsolePanel.h"

#include <iomanip>
#include <sstream>

namespace markamp::ui
{

DebugConsolePanel::DebugConsolePanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
    , service_(&owned_service_)
{
    CreateLayout();

    listener_id_ = service_->on_change([this]() { RefreshContent(); });
}

DebugConsolePanel::DebugConsolePanel(wxWindow* parent, core::DebugConsoleService* service)
    : wxPanel(parent, wxID_ANY)
    , service_(service != nullptr ? service : &owned_service_)
{
    CreateLayout();

    listener_id_ = service_->on_change([this]() { RefreshContent(); });
}

void DebugConsolePanel::CreateLayout()
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // ── Output list ──
    output_list_ = new wxListCtrl(this,
                                  wxID_ANY,
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  wxLC_REPORT | wxLC_NO_HEADER | wxBORDER_NONE);
    output_list_->InsertColumn(0, "Time", wxLIST_FORMAT_LEFT, 64);
    output_list_->InsertColumn(1, "Type", wxLIST_FORMAT_LEFT, 24);
    output_list_->InsertColumn(2, "Content", wxLIST_FORMAT_LEFT, 700);

    wxFont mono_font(
        11, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Menlo");
    output_list_->SetFont(mono_font);

    sizer->Add(output_list_, 1, wxEXPAND);

    // ── Input area ──
    auto* input_sizer = new wxBoxSizer(wxHORIZONTAL);

    prompt_label_ = new wxStaticText(this, wxID_ANY, ">");
    prompt_label_->SetFont(mono_font);
    input_sizer->Add(prompt_label_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 4);

    input_ctrl_ = new wxTextCtrl(this,
                                 wxID_ANY,
                                 wxEmptyString,
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 wxTE_PROCESS_ENTER | wxBORDER_NONE);
    input_ctrl_->SetFont(mono_font);
    input_ctrl_->SetHint("Type a command or expression...");

    input_ctrl_->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent& evt) { OnInputEnter(evt); });

    input_ctrl_->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& evt) { OnInputKey(evt); });

    input_sizer->Add(input_ctrl_, 1, wxEXPAND | wxTOP | wxBOTTOM, 2);

    sizer->Add(input_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 4);

    SetSizer(sizer);

    // Wire context menu
    output_list_->Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& evt) { OnContextMenu(evt); });

    // Add welcome message
    service_->add_output("Debug Console Ready. Type .help for available commands.",
                         core::ConsoleEntryType::kSystem);
}

void DebugConsolePanel::OnInputEnter(wxCommandEvent& /*event*/)
{
    if (input_ctrl_ == nullptr)
    {
        return;
    }

    auto command = input_ctrl_->GetValue().ToStdString();
    if (command.empty())
    {
        return;
    }

    SubmitCommand(command);
    input_ctrl_->Clear();
}

void DebugConsolePanel::OnInputKey(wxKeyEvent& event)
{
    if (service_ == nullptr)
    {
        event.Skip();
        return;
    }

    // Shift+Enter — insert newline (multi-line mode)
    if (event.GetKeyCode() == WXK_RETURN && event.ShiftDown())
    {
        if (input_ctrl_ != nullptr)
        {
            const long insert_pos = input_ctrl_->GetInsertionPoint();
            input_ctrl_->Replace(insert_pos, insert_pos, "\n");
            input_ctrl_->SetInsertionPoint(insert_pos + 1);
        }
        return;
    }

    // Up arrow — previous history
    if (event.GetKeyCode() == WXK_UP)
    {
        auto cmd = service_->navigate_history(-1);
        if (!cmd.empty() && input_ctrl_ != nullptr)
        {
            input_ctrl_->SetValue(wxString(cmd));
            input_ctrl_->SetInsertionPointEnd();
        }
        return;
    }

    // Down arrow — next history
    if (event.GetKeyCode() == WXK_DOWN)
    {
        auto cmd = service_->navigate_history(1);
        if (input_ctrl_ != nullptr)
        {
            input_ctrl_->SetValue(wxString(cmd));
            input_ctrl_->SetInsertionPointEnd();
        }
        return;
    }

    event.Skip();
}

void DebugConsolePanel::SubmitCommand(const std::string& command)
{
    if (service_ == nullptr)
    {
        return;
    }

    // Add input entry
    service_->add_output("> " + command, core::ConsoleEntryType::kInput);
    service_->push_history(command);
    service_->reset_history_position();

    // Evaluate
    auto result = evaluator_.evaluate(command);

    if (result.output == "__CLEAR__")
    {
        service_->clear();
        return;
    }

    if (result.is_error)
    {
        service_->add_output(result.output, core::ConsoleEntryType::kError);
    }
    else
    {
        service_->add_output(result.output, core::ConsoleEntryType::kResult);
    }
}

void DebugConsolePanel::RefreshContent()
{
    RenderEntries();
}

void DebugConsolePanel::RenderEntries()
{
    if (output_list_ == nullptr || service_ == nullptr)
    {
        return;
    }

    output_list_->Freeze();
    output_list_->DeleteAllItems();

    const auto& entries = service_->entries();
    for (const auto& entry : entries)
    {
        // Apply log level filter
        if (entry.level < log_level_filter_)
        {
            continue;
        }

        const long row = output_list_->InsertItem(output_list_->GetItemCount(), wxEmptyString);

        // Format timestamp as HH:MM:SS
        const auto time_t_val = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::tm local_tm{};
        localtime_r(&time_t_val, &local_tm);
        std::ostringstream ts_stream;
        ts_stream << std::put_time(&local_tm, "%H:%M:%S");

        wxString type_icon;
        wxColour text_colour;

        switch (entry.type)
        {
            case core::ConsoleEntryType::kOutput:
                type_icon = " ";
                text_colour = wxColour(204, 204, 204);
                break;
            case core::ConsoleEntryType::kInput:
                type_icon = "▶";
                text_colour = wxColour(78, 201, 176); // Teal
                break;
            case core::ConsoleEntryType::kResult:
                type_icon = "◀";
                text_colour = wxColour(156, 220, 254); // Light blue
                break;
            case core::ConsoleEntryType::kError:
                type_icon = "✕";
                text_colour = wxColour(235, 87, 87); // Red
                break;
            case core::ConsoleEntryType::kSystem:
                type_icon = "ℹ";
                text_colour = wxColour(128, 128, 128); // Gray
                break;
            case core::ConsoleEntryType::kDebug:
                type_icon = "⊙";
                text_colour = wxColour(86, 156, 214); // Blue
                break;
        }

        output_list_->SetItem(row, 0, wxString(ts_stream.str()));
        output_list_->SetItem(row, 1, type_icon);
        output_list_->SetItem(row, 2, wxString(entry.text));
        output_list_->SetItemTextColour(row, text_colour);
    }

    // Scroll to bottom
    if (output_list_->GetItemCount() > 0)
    {
        output_list_->EnsureVisible(output_list_->GetItemCount() - 1);
    }

    output_list_->Thaw();
}

void DebugConsolePanel::set_service(core::DebugConsoleService* service)
{
    if (service_ != nullptr && listener_id_ > 0)
    {
        service_->remove_listener(listener_id_);
    }
    service_ = service != nullptr ? service : &owned_service_;
    listener_id_ = service_->on_change([this]() { RefreshContent(); });
}

auto DebugConsolePanel::evaluator() -> core::ExpressionEvaluator&
{
    return evaluator_;
}

void DebugConsolePanel::set_log_level_filter(core::LogLevel level)
{
    log_level_filter_ = level;
    RefreshContent();
}

auto DebugConsolePanel::log_level_filter() const -> core::LogLevel
{
    return log_level_filter_;
}

void DebugConsolePanel::ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour)
{
    SetBackgroundColour(bg_colour);
    if (output_list_ != nullptr)
    {
        output_list_->SetBackgroundColour(bg_colour);
        output_list_->SetForegroundColour(fg_colour);
    }
    if (input_ctrl_ != nullptr)
    {
        input_ctrl_->SetBackgroundColour(bg_colour);
        input_ctrl_->SetForegroundColour(fg_colour);
    }
    if (prompt_label_ != nullptr)
    {
        prompt_label_->SetForegroundColour(wxColour(78, 201, 176)); // Teal prompt
    }
    Refresh();
}

// ── Context Menu ──

void DebugConsolePanel::OnContextMenu(wxContextMenuEvent& /*event*/)
{
    if (output_list_ == nullptr)
    {
        return;
    }

    wxMenu menu;
    enum MenuId : int
    {
        kCopyValue = wxID_HIGHEST + 1,
        kClearConsole,
        kSelectAll,
    };

    menu.Append(kCopyValue, "Copy Value");
    menu.AppendSeparator();
    menu.Append(kSelectAll, "Select All");
    menu.AppendSeparator();
    menu.Append(kClearConsole, "Clear Console");

    const long selected = output_list_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    menu.Enable(kCopyValue, selected >= 0);

    menu.Bind(wxEVT_MENU,
              [this](wxCommandEvent& evt)
              {
                  switch (evt.GetId())
                  {
                      case kCopyValue:
                          CopySelectedToClipboard();
                          break;
                      case kSelectAll:
                          if (output_list_ != nullptr)
                          {
                              for (long idx = 0; idx < output_list_->GetItemCount(); ++idx)
                              {
                                  output_list_->SetItemState(
                                      idx, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                              }
                          }
                          break;
                      case kClearConsole:
                          if (service_ != nullptr)
                          {
                              service_->clear();
                          }
                          break;
                      default:
                          break;
                  }
              });

    PopupMenu(&menu);
}

void DebugConsolePanel::CopySelectedToClipboard()
{
    if (output_list_ == nullptr)
    {
        return;
    }
    const long selected = output_list_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selected < 0)
    {
        return;
    }
    const wxString value = output_list_->GetItemText(selected, 1);
    if (value.empty())
    {
        return;
    }
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(value));
        wxTheClipboard->Close();
    }
}

void DebugConsolePanel::set_font_size(int size)
{
    font_size_ = size;
    wxFont new_font(size,
                    wxFONTFAMILY_TELETYPE,
                    wxFONTSTYLE_NORMAL,
                    wxFONTWEIGHT_NORMAL,
                    false,
                    wxString(font_family_));
    if (output_list_ != nullptr)
    {
        output_list_->SetFont(new_font);
        output_list_->Refresh();
    }
    if (input_ctrl_ != nullptr)
    {
        input_ctrl_->SetFont(new_font);
    }
}

void DebugConsolePanel::set_font_family(const std::string& family)
{
    font_family_ = family;
    wxFont new_font(font_size_,
                    wxFONTFAMILY_TELETYPE,
                    wxFONTSTYLE_NORMAL,
                    wxFONTWEIGHT_NORMAL,
                    false,
                    wxString(family));
    if (output_list_ != nullptr)
    {
        output_list_->SetFont(new_font);
        output_list_->Refresh();
    }
    if (input_ctrl_ != nullptr)
    {
        input_ctrl_->SetFont(new_font);
    }
}

} // namespace markamp::ui
