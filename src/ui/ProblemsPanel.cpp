#include "ProblemsPanel.h"

#include "PanelContainer.h"

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
    CreateLayout(parent);
    RefreshContent();
}

void ProblemsPanel::CreateLayout(wxWindow* parent)
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* panel_container = dynamic_cast<PanelContainer*>(parent->GetParent());
    wxWindow* toolbar_parent = panel_container ? panel_container->GetActionToolbarArea() : this;

    toolbar_ = new wxWindow(toolbar_parent, wxID_ANY);
    auto* top_bar = new wxBoxSizer(wxHORIZONTAL);

    // ── Summary bar (now in toolbar) ──
    summary_label_ = new wxStaticText(toolbar_, wxID_ANY, "0 Errors, 0 Warnings, 0 Info");
    top_bar->Add(summary_label_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);

    toolbar_->SetSizer(top_bar);

    if (panel_container)
    {
        panel_container->RegisterActionToolbar("problems", toolbar_);
    }
    else
    {
        sizer->Add(toolbar_, 0, wxEXPAND | wxALL, 4);
    }

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

        wxColour row_col;
        switch (item.severity)
        {
            case core::DiagnosticSeverity::kError:
                row_col = wxColour(235, 87, 87); // Red
                break;
            case core::DiagnosticSeverity::kWarning:
                row_col = wxColour(242, 201, 76); // Yellow/Orange
                break;
            default:
                row_col = list_ctrl_->GetForegroundColour(); // Default foreground
                break;
        }
        list_ctrl_->SetItemTextColour(row, row_col);
    }

    // Update summary
    if (summary_label_ != nullptr)
    {
        if (error_count() == 0 && warning_count() == 0 && info_count() == 0)
        {
            summary_label_->SetLabel("✅ No problems found in workspace");
        }
        else
        {
            summary_label_->SetLabel(wxString::Format("%zu Errors, %zu Warnings, %zu Info",
                                                      error_count(),
                                                      warning_count(),
                                                      info_count()));
        }
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
