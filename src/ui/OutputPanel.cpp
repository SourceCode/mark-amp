#include "OutputPanel.h"

#include "PanelContainer.h"
#include "core/DebugConsoleService.h"

#include <wx/button.h>
#include <wx/clipbrd.h>
#include <wx/menu.h>
#include <wx/sizer.h>

namespace markamp::ui
{

// ── Data-only constructor (for tests) ──

OutputPanel::OutputPanel()
    : wxPanel()
{
}

// ── UI constructor ──

OutputPanel::OutputPanel(wxWindow* parent, core::OutputChannelService* service)
    : wxPanel(parent, wxID_ANY)
    , service_(service)
{
    CreateLayout(parent);
    if (service_ != nullptr)
    {
        auto names = service_->channel_names();
        if (!names.empty())
        {
            active_channel_ = names.front();
        }
    }
    RefreshContent();
}

void OutputPanel::CreateLayout(wxWindow* parent)
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* panel_container = dynamic_cast<PanelContainer*>(parent->GetParent());
    wxWindow* toolbar_parent = panel_container ? panel_container->GetActionToolbarArea() : this;

    toolbar_ = new wxWindow(toolbar_parent, wxID_ANY);
    auto* top_bar = new wxBoxSizer(wxHORIZONTAL);

    // Channel selector
    channel_selector_ = new wxChoice(toolbar_, wxID_ANY);
    top_bar->Add(channel_selector_, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);

    // Log level filter buttons
    auto make_filter_btn = [this, &top_bar](const wxString& label, core::LogLevel level)
    {
        auto* filter_btn = new wxButton(
            toolbar_, wxID_ANY, label, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
        filter_btn->SetToolTip(wxString::Format("Show %s and above", label));
        filter_btn->Bind(wxEVT_BUTTON,
                         [this, level](wxCommandEvent& /*evt*/)
                         {
                             set_log_level_filter(level);
                             RefreshContent();
                         });
        top_bar->Add(filter_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    };

    make_filter_btn("V", core::LogLevel::kTrace);   // Verbose (all)
    make_filter_btn("I", core::LogLevel::kInfo);    // Info+
    make_filter_btn("W", core::LogLevel::kWarning); // Warnings+
    make_filter_btn("E", core::LogLevel::kError);   // Errors only

    top_bar->AddSpacer(8);

    // Clear button
    auto* clear_btn =
        new wxButton(toolbar_, wxID_ANY, "✕", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    clear_btn->SetToolTip("Clear output");
    clear_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& /*evt*/) { clear_active_channel(); });
    top_bar->Add(clear_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);

    // Lock scroll toggle
    auto* lock_btn =
        new wxButton(toolbar_, wxID_ANY, "⇩", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    lock_btn->SetToolTip("Toggle auto-scroll");
    lock_btn->Bind(wxEVT_BUTTON,
                   [this](wxCommandEvent& /*evt*/)
                   {
                       auto_scroll_ = !auto_scroll_;
                       RefreshContent();
                   });
    top_bar->Add(lock_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);

    // Word wrap toggle
    wrap_btn_ = new wxToggleButton(
        toolbar_, wxID_ANY, "↩", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    wrap_btn_->SetToolTip("Toggle word wrap");
    wrap_btn_->SetValue(word_wrap_);
    wrap_btn_->Bind(wxEVT_TOGGLEBUTTON,
                    [this](wxCommandEvent& /*evt*/)
                    {
                        word_wrap_ = wrap_btn_->GetValue();
                        RefreshContent();
                    });
    top_bar->Add(wrap_btn_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);

    // Timestamp toggle
    timestamp_btn_ = new wxToggleButton(
        toolbar_, wxID_ANY, "⏱", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    timestamp_btn_->SetToolTip("Show timestamps");
    timestamp_btn_->SetValue(show_timestamps_);
    timestamp_btn_->Bind(wxEVT_TOGGLEBUTTON,
                         [this](wxCommandEvent& /*evt*/)
                         {
                             show_timestamps_ = timestamp_btn_->GetValue();
                             RefreshContent();
                         });
    top_bar->Add(timestamp_btn_, 0, wxALIGN_CENTER_VERTICAL);

    toolbar_->SetSizer(top_bar);

    if (panel_container)
    {
        panel_container->RegisterActionToolbar("output", toolbar_);
    }
    else
    {
        sizer->Add(toolbar_, 0, wxEXPAND | wxALL, 4);
    }

    // ── Search bar ──
    search_ctrl_ = new wxSearchCtrl(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    search_ctrl_->SetDescriptiveText("Filter output…");
    search_ctrl_->Bind(wxEVT_SEARCHCTRL_SEARCH_BTN,
                       [this](wxCommandEvent& /*evt*/)
                       {
                           search_text_ = search_ctrl_->GetValue().ToStdString();
                           RefreshContent();
                       });
    search_ctrl_->Bind(wxEVT_TEXT,
                       [this](wxCommandEvent& /*evt*/)
                       {
                           search_text_ = search_ctrl_->GetValue().ToStdString();
                           RefreshContent();
                       });
    search_ctrl_->Hide(); // Hidden by default, shown via Cmd+F
    sizer->Add(search_ctrl_, 0, wxEXPAND | wxLEFT | wxRIGHT, 4);

    // ── Text display area ──
    long text_style = wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxBORDER_NONE;
    if (!word_wrap_)
    {
        text_style |= wxTE_DONTWRAP;
    }
    text_area_ =
        new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, text_style);
    text_area_->SetFont(wxFont(wxFontInfo(11).Family(wxFONTFAMILY_TELETYPE).FaceName("Menlo")));
    sizer->Add(text_area_, 1, wxEXPAND);

    SetSizer(sizer);

    // Wire channel selector
    channel_selector_->Bind(wxEVT_CHOICE, [this](wxCommandEvent& evt) { OnChannelChanged(evt); });

    // Wire context menu
    text_area_->Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& evt) { OnContextMenu(evt); });
}

void OutputPanel::OnChannelChanged(wxCommandEvent& /*event*/)
{
    if (channel_selector_ == nullptr)
    {
        return;
    }
    const int sel = channel_selector_->GetSelection();
    if (sel != wxNOT_FOUND)
    {
        active_channel_ =
            channel_selector_->GetString(static_cast<unsigned int>(sel)).ToStdString();

        // Mark channel as read when selected
        if (service_ != nullptr)
        {
            auto* channel = service_->get_channel(active_channel_);
            if (channel != nullptr)
            {
                channel->mark_read();
            }
        }

        RefreshContent();
    }
}

void OutputPanel::RefreshContent()
{
    if (text_area_ == nullptr)
    {
        return;
    }

    // Update channel selector items (with unread badges)
    if (channel_selector_ != nullptr && service_ != nullptr)
    {
        auto names = service_->channel_names();
        channel_selector_->Clear();
        int active_idx = 0;
        for (std::size_t idx = 0; idx < names.size(); ++idx)
        {
            wxString display_name = wxString(names[idx]);
            auto* channel = service_->get_channel(names[idx]);
            if (channel != nullptr && channel->unread_count() > 0)
            {
                display_name += wxString::Format(" (%zu)", channel->unread_count());
            }
            channel_selector_->Append(display_name);
            if (names[idx] == active_channel_)
            {
                active_idx = static_cast<int>(idx);
            }
        }
        if (!names.empty())
        {
            channel_selector_->SetSelection(active_idx);
        }
    }

    RenderLines();

    if (auto_scroll_)
    {
        text_area_->ShowPosition(text_area_->GetLastPosition());
    }
}

void OutputPanel::RenderLines()
{
    if (text_area_ == nullptr || service_ == nullptr || active_channel_.empty())
    {
        return;
    }

    auto* channel = service_->get_channel(active_channel_);
    if (channel == nullptr)
    {
        text_area_->SetValue(wxEmptyString);
        return;
    }

    // Get filtered lines
    auto lines = channel->lines_filtered(log_level_filter_);

    text_area_->Freeze();
    text_area_->Clear();

    for (const auto& line : lines)
    {
        // Apply text search filter
        if (!search_text_.empty())
        {
            if (line.text.find(search_text_) == std::string::npos &&
                line.ansi_text.find(search_text_) == std::string::npos)
            {
                continue;
            }
        }

        // Build display line
        wxString display_line;

        if (show_timestamps_)
        {
            display_line += wxString(OutputPanelRenderer::format_timestamp(line.timestamp)) + " ";
        }

        // Render ANSI segments
        auto segments = renderer_.render_line(line);

        long line_start = text_area_->GetLastPosition();

        // Add plain text first
        for (const auto& seg : segments)
        {
            display_line += wxString::FromUTF8(seg.text);
        }
        display_line += "\n";
        text_area_->AppendText(display_line);

        // Apply color styles to segments
        long pos = line_start;
        if (show_timestamps_)
        {
            // Style the timestamp prefix in gray
            auto ts_text = OutputPanelRenderer::format_timestamp(line.timestamp);
            wxTextAttr ts_style;
            ts_style.SetTextColour(wxColour(128, 128, 128));
            text_area_->SetStyle(pos, pos + static_cast<long>(ts_text.size()), ts_style);
            pos += static_cast<long>(ts_text.size()) + 1; // +1 for space
        }

        for (const auto& seg : segments)
        {
            if (seg.text.empty())
            {
                continue;
            }
            wxTextAttr style;
            style.SetTextColour(wxColour(seg.fg_red, seg.fg_green, seg.fg_blue));
            if (seg.use_bg)
            {
                style.SetBackgroundColour(wxColour(seg.bg_red, seg.bg_green, seg.bg_blue));
            }
            if (seg.bold)
            {
                style.SetFontWeight(wxFONTWEIGHT_BOLD);
            }
            if (seg.italic)
            {
                style.SetFontStyle(wxFONTSTYLE_ITALIC);
            }
            if (seg.underline)
            {
                style.SetFontUnderlined(true);
            }
            auto seg_len = static_cast<long>(seg.text.size());
            text_area_->SetStyle(pos, pos + seg_len, style);
            pos += seg_len;
        }
    }

    text_area_->Thaw();
}

void OutputPanel::ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour)
{
    SetBackgroundColour(bg_colour);
    if (text_area_ != nullptr)
    {
        text_area_->SetBackgroundColour(bg_colour);
        text_area_->SetForegroundColour(fg_colour);
    }
    if (search_ctrl_ != nullptr)
    {
        search_ctrl_->SetBackgroundColour(bg_colour);
        search_ctrl_->SetForegroundColour(fg_colour);
    }
    Refresh();
}

// ── Data-layer API (unchanged for test compatibility) ──

void OutputPanel::set_service(core::OutputChannelService* service)
{
    service_ = service;
    if (service_ != nullptr)
    {
        auto names = service_->channel_names();
        if (!names.empty() && active_channel_.empty())
        {
            active_channel_ = names.front();
        }
    }
}

auto OutputPanel::active_channel() const -> const std::string&
{
    return active_channel_;
}

void OutputPanel::set_active_channel(const std::string& channel_name)
{
    active_channel_ = channel_name;
}

auto OutputPanel::channel_names() const -> std::vector<std::string>
{
    return service_ != nullptr ? service_->channel_names() : std::vector<std::string>{};
}

auto OutputPanel::active_content() const -> std::string
{
    if (service_ == nullptr || active_channel_.empty())
    {
        return {};
    }
    auto* channel = service_->get_channel(active_channel_);
    return channel != nullptr ? channel->content() : std::string{};
}

auto OutputPanel::auto_scroll() const -> bool
{
    return auto_scroll_;
}

void OutputPanel::set_auto_scroll(bool enabled)
{
    auto_scroll_ = enabled;
}

void OutputPanel::clear_active_channel()
{
    if (service_ == nullptr || active_channel_.empty())
    {
        return;
    }
    auto* channel = service_->get_channel(active_channel_);
    if (channel != nullptr)
    {
        channel->clear();
    }
    RefreshContent();
}

void OutputPanel::lock_scroll()
{
    auto_scroll_ = false;
}

void OutputPanel::unlock_scroll()
{
    auto_scroll_ = true;
}

// ── V2 API ──

auto OutputPanel::log_level_filter() const -> core::LogLevel
{
    return log_level_filter_;
}

void OutputPanel::set_log_level_filter(core::LogLevel level)
{
    log_level_filter_ = level;
}

auto OutputPanel::word_wrap() const -> bool
{
    return word_wrap_;
}

void OutputPanel::set_word_wrap(bool enabled)
{
    word_wrap_ = enabled;
}

auto OutputPanel::show_timestamps() const -> bool
{
    return show_timestamps_;
}

void OutputPanel::set_show_timestamps(bool enabled)
{
    show_timestamps_ = enabled;
}

auto OutputPanel::search_text() const -> const std::string&
{
    return search_text_;
}

void OutputPanel::set_search_text(const std::string& text)
{
    search_text_ = text;
}

// ── Context Menu ──

void OutputPanel::OnContextMenu(wxContextMenuEvent& /*event*/)
{
    if (text_area_ == nullptr)
    {
        return;
    }

    wxMenu menu;
    enum MenuId : int
    {
        kCopySelection = wxID_HIGHEST + 1,
        kCopyAll,
        kSelectAll,
        kClear,
        kOpenInEditor,
    };

    menu.Append(kCopySelection, "Copy Selection\tCtrl+C");
    menu.Append(kCopyAll, "Copy All");
    menu.AppendSeparator();
    menu.Append(kOpenInEditor, "Open in Editor");
    menu.AppendSeparator();
    menu.Append(kSelectAll, "Select All\tCtrl+A");
    menu.AppendSeparator();
    menu.Append(kClear, "Clear Output");

    // Enable/disable based on state
    menu.Enable(kCopySelection, text_area_->GetStringSelection().length() > 0);
    menu.Enable(kCopyAll, text_area_->GetValue().length() > 0);

    menu.Bind(wxEVT_MENU,
              [this](wxCommandEvent& evt)
              {
                  switch (evt.GetId())
                  {
                      case kCopySelection:
                          CopySelectedToClipboard();
                          break;
                      case kCopyAll:
                          CopyAllToClipboard();
                          break;
                      case kSelectAll:
                          if (text_area_ != nullptr)
                          {
                              text_area_->SelectAll();
                          }
                          break;
                      case kClear:
                          clear_active_channel();
                          break;
                      case kOpenInEditor:
                          OpenSelectedInEditor();
                          break;
                      default:
                          break;
                  }
              });

    PopupMenu(&menu);
}

void OutputPanel::CopySelectedToClipboard()
{
    if (text_area_ == nullptr)
    {
        return;
    }
    const wxString selection = text_area_->GetStringSelection();
    if (selection.empty())
    {
        return;
    }
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(selection));
        wxTheClipboard->Close();
    }
}

void OutputPanel::CopyAllToClipboard()
{
    if (text_area_ == nullptr)
    {
        return;
    }
    const wxString all_text = text_area_->GetValue();
    if (all_text.empty())
    {
        return;
    }
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(all_text));
        wxTheClipboard->Close();
    }
}

void OutputPanel::set_font_size(int size)
{
    font_size_ = size;
    if (text_area_ != nullptr)
    {
        text_area_->SetFont(wxFont(
            wxFontInfo(size).Family(wxFONTFAMILY_TELETYPE).FaceName(wxString(font_family_))));
        text_area_->Refresh();
    }
}

void OutputPanel::set_font_family(const std::string& family)
{
    font_family_ = family;
    if (text_area_ != nullptr)
    {
        text_area_->SetFont(wxFont(
            wxFontInfo(font_size_).Family(wxFONTFAMILY_TELETYPE).FaceName(wxString(family))));
        text_area_->Refresh();
    }
}

void OutputPanel::OpenSelectedInEditor()
{
    if (text_area_ == nullptr)
    {
        return;
    }

    // Get the current line at cursor position
    const long pos = text_area_->GetInsertionPoint();
    long col = 0;
    long line_idx = 0;
    text_area_->PositionToXY(pos, &col, &line_idx);
    if (line_idx < 0 || line_idx >= text_area_->GetNumberOfLines())
    {
        return;
    }

    const std::string line_text =
        std::string(text_area_->GetLineText(static_cast<int>(line_idx)).utf8_str());

    // Detect file:line references in this line
    auto links = core::DebugConsoleService::detect_stack_traces(line_text);
    if (links.empty())
    {
        return;
    }

    // Open the first detected file reference
    // In a full implementation, this would publish a NavigateToProblemEvent via EventBus
    // For now, the link data (file, line, column) is available for the event system
    [[maybe_unused]] const auto& link = links[0];
}

void OutputPanel::set_virtual_list_mode(bool enabled)
{
    virtual_list_mode_ = enabled;
    // In a full implementation, this would switch between wxTextCtrl and
    // a custom-painted wxVListBox for rendering large outputs with reduced
    // memory overhead and improved scroll performance.
}

auto OutputPanel::virtual_list_mode() const -> bool
{
    return virtual_list_mode_;
}

} // namespace markamp::ui
