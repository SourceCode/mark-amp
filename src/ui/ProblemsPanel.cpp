#include "ProblemsPanel.h"

#include "PanelContainer.h"

#include <wx/clipbrd.h>
#include <wx/menu.h>
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

    // ── Summary bar ──
    summary_label_ = new wxStaticText(toolbar_, wxID_ANY, "0 Errors, 0 Warnings, 0 Info");
    top_bar->Add(summary_label_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

    top_bar->AddStretchSpacer(1);

    // Severity filter buttons
    auto make_filter_btn = [this, &top_bar](const wxString& label,
                                            core::DiagnosticSeverity sev,
                                            const wxString& tooltip)
    {
        auto* btn = new wxButton(
            toolbar_, wxID_ANY, label, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
        btn->SetToolTip(tooltip);
        btn->Bind(wxEVT_BUTTON,
                  [this, sev](wxCommandEvent& /*evt*/)
                  {
                      set_severity_filter(sev);
                      RefreshContent();
                  });
        top_bar->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    };

    make_filter_btn("⊘", core::DiagnosticSeverity::kError, "Show errors only");
    make_filter_btn("⚠", core::DiagnosticSeverity::kWarning, "Show warnings and errors");
    make_filter_btn("ℹ", core::DiagnosticSeverity::kInformation, "Show info and above");
    make_filter_btn("✱", core::DiagnosticSeverity::kHint, "Show all");

    top_bar->AddSpacer(4);

    // Sort buttons
    auto* sort_sev =
        new wxButton(toolbar_, wxID_ANY, "↕S", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    sort_sev->SetToolTip("Sort by severity");
    sort_sev->Bind(wxEVT_BUTTON,
                   [this](wxCommandEvent& /*evt*/)
                   {
                       set_sort_mode(ProblemSortMode::kSeverity);
                       RefreshContent();
                   });
    top_bar->Add(sort_sev, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);

    auto* sort_file =
        new wxButton(toolbar_, wxID_ANY, "↕F", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    sort_file->SetToolTip("Sort by file");
    sort_file->Bind(wxEVT_BUTTON,
                    [this](wxCommandEvent& /*evt*/)
                    {
                        set_sort_mode(ProblemSortMode::kFile);
                        RefreshContent();
                    });
    top_bar->Add(sort_file, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);

    top_bar->AddSpacer(4);

    // Collapse/Expand all
    auto* collapse_btn =
        new wxButton(toolbar_, wxID_ANY, "⊟", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    collapse_btn->SetToolTip("Collapse all");
    collapse_btn->Bind(wxEVT_BUTTON,
                       [this](wxCommandEvent& /*evt*/)
                       {
                           collapse_all();
                           RefreshContent();
                       });
    top_bar->Add(collapse_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);

    auto* expand_btn =
        new wxButton(toolbar_, wxID_ANY, "⊞", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    expand_btn->SetToolTip("Expand all");
    expand_btn->Bind(wxEVT_BUTTON,
                     [this](wxCommandEvent& /*evt*/)
                     {
                         expand_all();
                         RefreshContent();
                     });
    top_bar->Add(expand_btn, 0, wxALIGN_CENTER_VERTICAL);

    toolbar_->SetSizer(top_bar);

    if (panel_container)
    {
        panel_container->RegisterActionToolbar("problems", toolbar_);
    }
    else
    {
        sizer->Add(toolbar_, 0, wxEXPAND | wxALL, 4);
    }

    // ── Search bar ──
    search_ctrl_ = new wxSearchCtrl(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    search_ctrl_->SetDescriptiveText("Filter problems…");
    search_ctrl_->Bind(wxEVT_TEXT,
                       [this](wxCommandEvent& /*evt*/)
                       {
                           set_text_filter(search_ctrl_->GetValue().ToStdString());
                           RefreshContent();
                       });
    sizer->Add(search_ctrl_, 0, wxEXPAND | wxLEFT | wxRIGHT, 4);

    // ── Problem list ──
    list_ctrl_ = new wxListCtrl(this,
                                wxID_ANY,
                                wxDefaultPosition,
                                wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);
    list_ctrl_->InsertColumn(0, "Severity", wxLIST_FORMAT_LEFT, 26);
    list_ctrl_->InsertColumn(1, "Message", wxLIST_FORMAT_LEFT, 400);
    list_ctrl_->InsertColumn(2, "File", wxLIST_FORMAT_LEFT, 200);
    list_ctrl_->InsertColumn(3, "Line", wxLIST_FORMAT_RIGHT, 50);
    list_ctrl_->InsertColumn(4, "Source", wxLIST_FORMAT_LEFT, 80);

    // Navigate to file on double-click or Enter
    list_ctrl_->Bind(wxEVT_LIST_ITEM_ACTIVATED,
                     [this](wxListEvent& /*evt*/) { NavigateToSelectedProblem(); });

    // Keyboard navigation (F8 = next problem, Shift+F8 = prev)
    list_ctrl_->Bind(wxEVT_LIST_KEY_DOWN, [this](wxListEvent& evt) { OnKeyDown(evt); });

    sizer->Add(list_ctrl_, 1, wxEXPAND);

    // Wire context menu
    list_ctrl_->Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& evt) { OnContextMenu(evt); });

    SetSizer(sizer);
}

void ProblemsPanel::RefreshContent()
{
    // Rebuild tree in data mode
    if (service_ != nullptr)
    {
        tree_model_.set_severity_filter(filter_);
        tree_model_.rebuild(*service_);
    }

    // Update summary
    if (summary_label_ != nullptr)
    {
        auto errors = tree_model_.total_error_count();
        auto warnings = tree_model_.total_warning_count();
        auto infos = tree_model_.total_info_count();

        if (errors == 0 && warnings == 0 && infos == 0)
        {
            summary_label_->SetLabel("✅ No problems found in workspace");
        }
        else
        {
            summary_label_->SetLabel(
                wxString::Format("%zu Errors, %zu Warnings, %zu Info", errors, warnings, infos));
        }
    }

    RenderTreeToList();
}

void ProblemsPanel::RenderTreeToList()
{
    if (list_ctrl_ == nullptr)
    {
        return;
    }

    list_ctrl_->DeleteAllItems();

    const auto& nodes = tree_model_.file_nodes();
    for (const auto& node : nodes)
    {
        // File header row
        long file_row = list_ctrl_->InsertItem(list_ctrl_->GetItemCount(), wxEmptyString);
        wxString file_summary =
            wxString::Format("▸ %s (%zu)", wxString(node.display_name), node.diagnostics.size());
        if (node.collapsed)
        {
            file_summary.Replace("▸", "▸");
        }
        else
        {
            file_summary.Replace("▸", "▾");
        }
        list_ctrl_->SetItem(file_row, 1, file_summary);

        // Style file header
        wxColour header_col(180, 180, 180);
        switch (node.max_severity())
        {
            case core::DiagnosticSeverity::kError:
                header_col = wxColour(235, 87, 87);
                break;
            case core::DiagnosticSeverity::kWarning:
                header_col = wxColour(242, 201, 76);
                break;
            default:
                break;
        }
        list_ctrl_->SetItemTextColour(file_row, header_col);

        // Diagnostic items (if not collapsed)
        if (!node.collapsed)
        {
            for (const auto& diag : node.diagnostics)
            {
                long row = list_ctrl_->InsertItem(list_ctrl_->GetItemCount(), wxEmptyString);

                wxString severity_icon;
                wxColour row_col;
                switch (diag.severity)
                {
                    case core::DiagnosticSeverity::kError:
                        severity_icon = "⊘";
                        row_col = wxColour(235, 87, 87);
                        break;
                    case core::DiagnosticSeverity::kWarning:
                        severity_icon = "⚠";
                        row_col = wxColour(242, 201, 76);
                        break;
                    case core::DiagnosticSeverity::kInformation:
                        severity_icon = "ℹ";
                        row_col = wxColour(75, 156, 220);
                        break;
                    case core::DiagnosticSeverity::kHint:
                        severity_icon = "💡";
                        row_col = wxColour(150, 150, 150);
                        break;
                }

                list_ctrl_->SetItem(row, 0, severity_icon);
                list_ctrl_->SetItem(row, 1, wxString("  " + diag.message));
                list_ctrl_->SetItem(row, 2, wxString(node.file_uri));
                list_ctrl_->SetItem(row,
                                    3,
                                    wxString::Format("%d:%d",
                                                     diag.range.start.line + 1,
                                                     diag.range.start.character + 1));
                list_ctrl_->SetItem(row, 4, wxString(diag.source));

                list_ctrl_->SetItemTextColour(row, row_col);
            }
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
    if (search_ctrl_ != nullptr)
    {
        search_ctrl_->SetBackgroundColour(bg_colour);
        search_ctrl_->SetForegroundColour(fg_colour);
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
    tree_model_.set_severity_filter(min_severity);
}

auto ProblemsPanel::severity_filter() const -> core::DiagnosticSeverity
{
    return filter_;
}

// ── V2 API ──

void ProblemsPanel::set_sort_mode(ProblemSortMode mode)
{
    tree_model_.set_sort_mode(mode);
}

auto ProblemsPanel::sort_mode() const -> ProblemSortMode
{
    return tree_model_.sort_mode();
}

void ProblemsPanel::set_text_filter(const std::string& text)
{
    tree_model_.set_text_filter(text);
}

auto ProblemsPanel::text_filter() const -> const std::string&
{
    return tree_model_.text_filter();
}

void ProblemsPanel::set_source_filter(const std::string& source)
{
    tree_model_.set_source_filter(source);
}

auto ProblemsPanel::source_filter() const -> const std::string&
{
    return tree_model_.source_filter();
}

void ProblemsPanel::collapse_all()
{
    tree_model_.collapse_all();
}

void ProblemsPanel::expand_all()
{
    tree_model_.expand_all();
}

auto ProblemsPanel::tree_model() -> ProblemsTreeModel&
{
    return tree_model_;
}

auto ProblemsPanel::tree_model() const -> const ProblemsTreeModel&
{
    return tree_model_;
}

// ── Context Menu ──

void ProblemsPanel::OnContextMenu(wxContextMenuEvent& /*event*/)
{
    if (list_ctrl_ == nullptr)
    {
        return;
    }

    wxMenu menu;
    enum MenuId : int
    {
        kCopyMessage = wxID_HIGHEST + 1,
        kCollapseAll,
        kExpandAll,
    };

    menu.Append(kCopyMessage, "Copy Problem Message");
    menu.AppendSeparator();
    menu.Append(kCollapseAll, "Collapse All");
    menu.Append(kExpandAll, "Expand All");

    const long selected = list_ctrl_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    menu.Enable(kCopyMessage, selected >= 0);

    menu.Bind(wxEVT_MENU,
              [this](wxCommandEvent& evt)
              {
                  switch (evt.GetId())
                  {
                      case kCopyMessage:
                          CopySelectedToClipboard();
                          break;
                      case kCollapseAll:
                          collapse_all();
                          RefreshContent();
                          break;
                      case kExpandAll:
                          expand_all();
                          RefreshContent();
                          break;
                      default:
                          break;
                  }
              });

    PopupMenu(&menu);
}

void ProblemsPanel::CopySelectedToClipboard()
{
    if (list_ctrl_ == nullptr)
    {
        return;
    }
    const long selected = list_ctrl_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selected < 0)
    {
        return;
    }
    const wxString message = list_ctrl_->GetItemText(selected, 1);
    if (message.empty())
    {
        return;
    }
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(message));
        wxTheClipboard->Close();
    }
}

void ProblemsPanel::OnKeyDown(wxListEvent& event)
{
    if (list_ctrl_ == nullptr)
    {
        event.Skip();
        return;
    }

    const int key_code = event.GetKeyCode();

    // F8 = next problem, Shift+F8 = previous problem
    if (key_code == WXK_F8)
    {
        const long current = list_ctrl_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        const long count = list_ctrl_->GetItemCount();
        if (count == 0)
        {
            return;
        }

        const bool shift = wxGetKeyState(WXK_SHIFT);
        long next = shift ? (current - 1) : (current + 1);

        // Wrap around
        if (next < 0)
        {
            next = count - 1;
        }
        else if (next >= count)
        {
            next = 0;
        }

        // Deselect current, select next
        if (current >= 0)
        {
            list_ctrl_->SetItemState(current, 0, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
        }
        list_ctrl_->SetItemState(next,
                                 wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
                                 wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
        list_ctrl_->EnsureVisible(next);
        return;
    }

    event.Skip();
}

void ProblemsPanel::NavigateToSelectedProblem()
{
    if (list_ctrl_ == nullptr)
    {
        return;
    }
    const long selected = list_ctrl_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selected < 0)
    {
        return;
    }
    // Get file URI and line from the selected row
    // (file is column 2, line is column 3)
    auto file_uri = list_ctrl_->GetItemText(selected, 2).ToStdString();
    auto line_str = list_ctrl_->GetItemText(selected, 3).ToStdString();
    // Navigation event would be fired via EventBus with file_uri and line
    // This provides the hook point for the event system
}

auto ProblemsPanel::available_quick_fixes(std::size_t /*diag_index*/) const
    -> std::vector<QuickFixAction>
{
    // Quick fixes are provided by language servers / linters.
    // This returns an empty vector until a language server protocol is wired in.
    return {};
}

void ProblemsPanel::apply_quick_fix(const QuickFixAction& /*action*/)
{
    // Apply quick fix via command execution:
    // event_bus_->publish(ExecuteCommandEvent{action.command_id});
    // Placeholder until command bus is integrated.
}

void ProblemsPanel::autofix_all()
{
    // Iterate all diagnostics, gather available quick fixes, apply each.
    // This is a batch operation that requires language server support.
    // Implementation deferred until quick fix providers are registered.
}

auto ProblemsPanel::file_decoration_for_uri(const std::string& uri) const
    -> core::DiagnosticSeverity
{
    if (service_ == nullptr)
    {
        return core::DiagnosticSeverity::kHint;
    }

    const auto& diags = service_->get(uri);
    auto max_sev = core::DiagnosticSeverity::kHint;
    for (const auto& diag : diags)
    {
        if (static_cast<int>(diag.severity) < static_cast<int>(max_sev))
        {
            max_sev = diag.severity;
        }
    }
    return max_sev;
}

auto ProblemsPanel::gutter_markers_for_uri(const std::string& uri) const
    -> std::vector<GutterMarker>
{
    std::vector<GutterMarker> markers;
    if (service_ == nullptr)
    {
        return markers;
    }

    const auto& diags = service_->get(uri);
    for (const auto& diag : diags)
    {
        GutterMarker marker;
        marker.line = diag.range.start.line;
        marker.severity = diag.severity;
        marker.tooltip = diag.message;
        markers.push_back(std::move(marker));
    }
    return markers;
}

} // namespace markamp::ui
