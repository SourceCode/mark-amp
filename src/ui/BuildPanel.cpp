#include "BuildPanel.h"

#include "core/Events.h"

#include <wx/sizer.h>

namespace markamp::ui
{

BuildPanel::BuildPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    CreateLayout();
}

BuildPanel::BuildPanel(wxWindow* parent, core::BuildService* service, core::EventBus* event_bus)
    : wxPanel(parent, wxID_ANY)
    , service_(service)
    , event_bus_(event_bus)
{
    CreateLayout();
    RefreshContent();
}

void BuildPanel::CreateLayout()
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // ── Toolbar ──
    toolbar_ = new wxWindow(this, wxID_ANY);
    auto* top_bar = new wxBoxSizer(wxHORIZONTAL);

    build_btn_ = new wxButton(
        toolbar_, wxID_ANY, "▶ Build", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    build_btn_->SetToolTip("Start build (Ctrl+Shift+B)");
    build_btn_->Bind(wxEVT_BUTTON, [this](wxCommandEvent& /*evt*/) { StartBuild(); });
    top_bar->Add(build_btn_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);

    stop_btn_ =
        new wxButton(toolbar_, wxID_ANY, "■ Stop", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    stop_btn_->SetToolTip("Stop build");
    stop_btn_->Bind(wxEVT_BUTTON, [this](wxCommandEvent& /*evt*/) { StopBuild(); });
    stop_btn_->Enable(false);
    top_bar->Add(stop_btn_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);

    clean_btn_ = new wxButton(
        toolbar_, wxID_ANY, "🗑 Clean", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    clean_btn_->SetToolTip("Clean build artifacts");
    clean_btn_->Bind(wxEVT_BUTTON,
                     [this](wxCommandEvent& /*evt*/)
                     {
                         if (service_ != nullptr)
                         {
                             service_->clean();
                         }
                     });
    top_bar->Add(clean_btn_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

    // Summary
    summary_label_ = new wxStaticText(toolbar_, wxID_ANY, "Ready");
    top_bar->Add(summary_label_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

    top_bar->AddStretchSpacer(1);

    // Build time
    time_label_ = new wxStaticText(toolbar_, wxID_ANY, "");
    top_bar->Add(time_label_, 0, wxALIGN_CENTER_VERTICAL);

    toolbar_->SetSizer(top_bar);
    sizer->Add(toolbar_, 0, wxEXPAND | wxALL, 4);

    // ── Progress bar ──
    progress_bar_ = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 3));
    progress_bar_->SetValue(0);
    progress_bar_->Hide();
    sizer->Add(progress_bar_, 0, wxEXPAND | wxLEFT | wxRIGHT, 4);

    // ── Build output ──
    output_area_ = new wxTextCtrl(this,
                                  wxID_ANY,
                                  wxEmptyString,
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxBORDER_NONE);
    output_area_->SetFont(wxFont(wxFontInfo(11).Family(wxFONTFAMILY_TELETYPE).FaceName("Menlo")));
    output_area_->Bind(wxEVT_LEFT_DCLICK, &BuildPanel::OnOutputDoubleClick, this);
    sizer->Add(output_area_, 1, wxEXPAND);

    SetSizer(sizer);
}

void BuildPanel::RefreshContent()
{
    if (service_ == nullptr)
    {
        return;
    }

    UpdateProgressBar();
    UpdateSummary();

    // Update button states
    const bool is_building = service_->is_building();
    if (build_btn_ != nullptr)
    {
        build_btn_->Enable(!is_building);
    }
    if (stop_btn_ != nullptr)
    {
        stop_btn_->Enable(is_building);
    }
    if (clean_btn_ != nullptr)
    {
        clean_btn_->Enable(!is_building);
    }

    // Update output
    if (output_area_ != nullptr)
    {
        output_area_->Freeze();
        output_area_->SetValue(wxString::FromUTF8(service_->last_result().output));

        // Highlight errors in red
        const wxString kContent = output_area_->GetValue();
        size_t search_pos = 0;
        while (true)
        {
            const size_t err_pos = kContent.find(": error:", search_pos);
            if (err_pos == wxString::npos)
            {
                break;
            }
            size_t line_start_pos = kContent.rfind('\n', err_pos);
            line_start_pos = (line_start_pos == wxString::npos) ? 0 : line_start_pos + 1;
            size_t line_end_pos = kContent.find('\n', err_pos);
            if (line_end_pos == wxString::npos)
            {
                line_end_pos = kContent.length();
            }

            wxTextAttr error_style;
            error_style.SetTextColour(wxColour(235, 87, 87));
            output_area_->SetStyle(
                static_cast<long>(line_start_pos), static_cast<long>(line_end_pos), error_style);
            search_pos = line_end_pos;
        }

        // Highlight warnings in yellow
        search_pos = 0;
        while (true)
        {
            const size_t warn_pos = kContent.find(": warning:", search_pos);
            if (warn_pos == wxString::npos)
            {
                break;
            }
            size_t line_start_pos = kContent.rfind('\n', warn_pos);
            line_start_pos = (line_start_pos == wxString::npos) ? 0 : line_start_pos + 1;
            size_t line_end_pos = kContent.find('\n', warn_pos);
            if (line_end_pos == wxString::npos)
            {
                line_end_pos = kContent.length();
            }

            wxTextAttr warn_style;
            warn_style.SetTextColour(wxColour(242, 201, 76));
            output_area_->SetStyle(
                static_cast<long>(line_start_pos), static_cast<long>(line_end_pos), warn_style);
            search_pos = line_end_pos;
        }

        output_area_->ShowPosition(output_area_->GetLastPosition());
        output_area_->Thaw();
    }
}

void BuildPanel::UpdateProgressBar()
{
    if (progress_bar_ == nullptr || service_ == nullptr)
    {
        return;
    }

    if (service_->is_building())
    {
        progress_bar_->Show();
        auto progress = service_->progress();
        if (progress.total > 0)
        {
            progress_bar_->SetValue(static_cast<int>(progress.percentage * 100.0F));
        }
        else
        {
            progress_bar_->Pulse(); // Indeterminate progress
        }
    }
    else
    {
        progress_bar_->SetValue(0);
        progress_bar_->Hide();
    }
}

void BuildPanel::UpdateSummary()
{
    if (summary_label_ == nullptr || service_ == nullptr)
    {
        return;
    }

    switch (service_->status())
    {
        case core::BuildStatus::kIdle:
        {
            const auto& result = service_->last_result();
            if (result.success)
            {
                summary_label_->SetLabel(
                    wxString::Format("✅ Build succeeded — %d errors, %d warnings",
                                     result.error_count,
                                     result.warning_count));
                summary_label_->SetForegroundColour(wxColour(78, 201, 176));
            }
            else if (result.error_count > 0)
            {
                summary_label_->SetLabel(wxString::Format("✕ Build failed — %d errors, %d warnings",
                                                          result.error_count,
                                                          result.warning_count));
                summary_label_->SetForegroundColour(wxColour(235, 87, 87));
            }
            else
            {
                summary_label_->SetLabel("Ready");
                summary_label_->SetForegroundColour(wxColour(180, 180, 180));
            }
            break;
        }
        case core::BuildStatus::kConfiguring:
            summary_label_->SetLabel("⚙ Configuring...");
            summary_label_->SetForegroundColour(wxColour(86, 156, 214));
            break;
        case core::BuildStatus::kBuilding:
        {
            auto progress = service_->progress();
            if (progress.total > 0)
            {
                summary_label_->SetLabel(wxString::Format("🔨 Building [%d/%d] %s",
                                                          progress.completed,
                                                          progress.total,
                                                          wxString(progress.current_file)));
            }
            else
            {
                summary_label_->SetLabel("🔨 Building...");
            }
            summary_label_->SetForegroundColour(wxColour(86, 156, 214));
            break;
        }
        case core::BuildStatus::kFinished:
            summary_label_->SetLabel("✅ Build finished");
            summary_label_->SetForegroundColour(wxColour(78, 201, 176));
            break;
        case core::BuildStatus::kFailed:
            summary_label_->SetLabel("✕ Build failed");
            summary_label_->SetForegroundColour(wxColour(235, 87, 87));
            break;
    }

    // Update build time display
    if (time_label_ != nullptr)
    {
        const auto& result = service_->last_result();
        if (service_->is_building())
        {
            time_label_->SetLabel("⏱ Building...");
        }
        else if (result.duration_seconds > 0.0F)
        {
            if (result.duration_seconds >= 60.0F)
            {
                const int minutes = static_cast<int>(result.duration_seconds) / 60;
                const int seconds = static_cast<int>(result.duration_seconds) % 60;
                time_label_->SetLabel(wxString::Format("⏱ %dm %ds", minutes, seconds));
            }
            else
            {
                time_label_->SetLabel(
                    wxString::Format("⏱ %.1fs", static_cast<double>(result.duration_seconds)));
            }
        }
        else
        {
            time_label_->SetLabel("");
        }
    }
}

void BuildPanel::set_service(core::BuildService* service)
{
    service_ = service;
}

void BuildPanel::StartBuild(const std::string& target, const std::string& config)
{
    if (service_ == nullptr)
    {
        return;
    }
    service_->start_build(target, config);
    RefreshContent();
}

void BuildPanel::StopBuild()
{
    if (service_ == nullptr)
    {
        return;
    }
    service_->stop_build();
    RefreshContent();
}

void BuildPanel::ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour)
{
    SetBackgroundColour(bg_colour);
    if (output_area_ != nullptr)
    {
        output_area_->SetBackgroundColour(bg_colour);
        output_area_->SetForegroundColour(fg_colour);
    }
    if (summary_label_ != nullptr)
    {
        summary_label_->SetForegroundColour(fg_colour);
    }
    if (time_label_ != nullptr)
    {
        time_label_->SetForegroundColour(wxColour(128, 128, 128));
    }
    Refresh();
}

void BuildPanel::OnOutputDoubleClick(wxMouseEvent& event)
{
    event.Skip();
    if (output_area_ == nullptr || event_bus_ == nullptr)
    {
        return;
    }

    // Get the line at the click position
    const long click_pos = output_area_->GetInsertionPoint();
    const long line_num = output_area_->PositionToXY(click_pos, nullptr, nullptr);
    if (line_num < 0)
    {
        return;
    }

    // Retrieve the line text
    const int total_lines = output_area_->GetNumberOfLines();
    if (line_num >= total_lines)
    {
        return;
    }

    const wxString line_text = output_area_->GetLineText(static_cast<int>(line_num));
    const std::string line_str = std::string(line_text.utf8_str());

    // Parse for compiler error
    const auto errors = error_parser_.parse_line(line_str);
    if (errors.empty())
    {
        return;
    }

    // Navigate to the first error on this line
    const auto& err = errors[0];
    core::events::NavigateToProblemEvent nav_evt;
    nav_evt.file_uri = err.file;
    nav_evt.line = err.line;
    nav_evt.character = err.column;
    event_bus_->publish(nav_evt);
}

void BuildPanel::set_compiler(const std::string& compiler)
{
    compiler_ = compiler;
    if (compiler_choice_ != nullptr)
    {
        compiler_choice_->SetStringSelection(wxString(compiler));
    }
}

auto BuildPanel::compiler() const -> const std::string&
{
    return compiler_;
}

void BuildPanel::set_build_target(const std::string& target)
{
    build_target_ = target;
    if (target_choice_ != nullptr)
    {
        target_choice_->SetStringSelection(wxString(target));
    }
}

auto BuildPanel::build_target() const -> const std::string&
{
    return build_target_;
}

void BuildPanel::set_available_targets(const std::vector<std::string>& targets)
{
    if (target_choice_ == nullptr)
    {
        return;
    }
    target_choice_->Clear();
    for (const auto& target : targets)
    {
        target_choice_->Append(wxString(target));
    }
    if (!targets.empty())
    {
        target_choice_->SetSelection(0);
        build_target_ = targets[0];
    }
}

void BuildPanel::set_auto_build(bool enabled)
{
    auto_build_ = enabled;
}

auto BuildPanel::auto_build() const -> bool
{
    return auto_build_;
}

} // namespace markamp::ui
