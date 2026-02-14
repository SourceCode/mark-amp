#include "ProblemsPanel.h"

#include <wx/sizer.h>

namespace markamp::ui
{

// ── Data-only constructor (for tests) ──

ProblemsPanel::ProblemsPanel()
    : wxPanel()
{
}

// ── UI constructor ──

ProblemsPanel::ProblemsPanel(wxWindow* parent, core::DiagnosticsService* service)
    : wxPanel(parent, wxID_ANY)
    , service_(service)
{
    CreateLayout();
    RefreshContent();
}

void ProblemsPanel::CreateLayout()
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // ── Problem list ──
    list_ctrl_ = new wxListCtrl(this,
                                wxID_ANY,
                                wxDefaultPosition,
                                wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);
    list_ctrl_->InsertColumn(0, "Severity", wxLIST_FORMAT_LEFT, 70);
    list_ctrl_->InsertColumn(1, "File", wxLIST_FORMAT_LEFT, 200);
    list_ctrl_->InsertColumn(2, "Line", wxLIST_FORMAT_RIGHT, 50);
    list_ctrl_->InsertColumn(3, "Message", wxLIST_FORMAT_LEFT, 400);
    list_ctrl_->InsertColumn(4, "Source", wxLIST_FORMAT_LEFT, 100);

    sizer->Add(list_ctrl_, 1, wxEXPAND);

    // ── Summary bar ──
    summary_label_ = new wxStaticText(this, wxID_ANY, "0 Errors, 0 Warnings, 0 Info");
    sizer->Add(summary_label_, 0, wxEXPAND | wxALL, 4);

    SetSizer(sizer);
}

void ProblemsPanel::RefreshContent()
{
    if (list_ctrl_ == nullptr)
    {
        return;
    }

    list_ctrl_->DeleteAllItems();

    auto items = problems(filter_);
    for (std::size_t idx = 0; idx < items.size(); ++idx)
    {
        const auto& item = items[idx];
        long row = list_ctrl_->InsertItem(static_cast<long>(idx), wxEmptyString);

        wxString severity_str;
        switch (item.severity)
        {
            case core::DiagnosticSeverity::kError:
                severity_str = "Error";
                break;
            case core::DiagnosticSeverity::kWarning:
                severity_str = "Warning";
                break;
            case core::DiagnosticSeverity::kInformation:
                severity_str = "Info";
                break;
            case core::DiagnosticSeverity::kHint:
                severity_str = "Hint";
                break;
        }

        list_ctrl_->SetItem(row, 0, severity_str);
        list_ctrl_->SetItem(row, 1, wxString(item.file_uri));
        list_ctrl_->SetItem(row, 2, wxString::Format("%d", item.line));
        list_ctrl_->SetItem(row, 3, wxString(item.message));
        list_ctrl_->SetItem(row, 4, wxString(item.source));
    }

    // Update summary
    if (summary_label_ != nullptr)
    {
        summary_label_->SetLabel(wxString::Format(
            "%zu Errors, %zu Warnings, %zu Info", error_count(), warning_count(), info_count()));
    }
}

void ProblemsPanel::ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour)
{
    SetBackgroundColour(bg_colour);
    if (list_ctrl_ != nullptr)
    {
        list_ctrl_->SetBackgroundColour(bg_colour);
        list_ctrl_->SetForegroundColour(fg_colour);
    }
    if (summary_label_ != nullptr)
    {
        summary_label_->SetForegroundColour(fg_colour);
    }
    Refresh();
}

// ── Data-layer API (unchanged for test compatibility) ──

void ProblemsPanel::set_service(core::DiagnosticsService* service)
{
    service_ = service;
}

auto ProblemsPanel::problems(core::DiagnosticSeverity min_severity) const
    -> std::vector<ProblemItem>
{
    std::vector<ProblemItem> result;
    if (service_ == nullptr)
    {
        return result;
    }

    for (const auto& uri : service_->uris())
    {
        for (const auto& diag : service_->get(uri))
        {
            if (diag.severity <= min_severity)
            {
                result.push_back({
                    .file_uri = uri,
                    .message = diag.message,
                    .severity = diag.severity,
                    .line = diag.range.start.line,
                    .character = diag.range.start.character,
                    .source = diag.source,
                });
            }
        }
    }
    return result;
}

auto ProblemsPanel::error_count() const -> std::size_t
{
    return service_ != nullptr ? service_->count_by_severity(core::DiagnosticSeverity::kError) : 0;
}

auto ProblemsPanel::warning_count() const -> std::size_t
{
    return service_ != nullptr ? service_->count_by_severity(core::DiagnosticSeverity::kWarning)
                               : 0;
}

auto ProblemsPanel::info_count() const -> std::size_t
{
    return service_ != nullptr ? service_->count_by_severity(core::DiagnosticSeverity::kInformation)
                               : 0;
}

void ProblemsPanel::set_severity_filter(core::DiagnosticSeverity min_severity)
{
    filter_ = min_severity;
}

auto ProblemsPanel::severity_filter() const -> core::DiagnosticSeverity
{
    return filter_;
}

} // namespace markamp::ui
