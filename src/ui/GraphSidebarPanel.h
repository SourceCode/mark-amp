#pragma once

#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/stattext.h>

#include <string>
#include <vector>

namespace markamp::ui
{

/// Graph sidebar panel — shows backlinks for the active document
/// and supports double-click navigation to linked documents.
class GraphSidebarPanel : public wxPanel
{
public:
    GraphSidebarPanel(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    /// Add a document link to the graph list.
    void AddDocumentLink(const std::string& label);

    /// Clear all document links.
    void ClearLinks();

    /// Refresh the backlink list for the given file path.
    void RefreshBacklinks(const std::string& file_path);

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;

    wxStaticText* title_label_{nullptr};
    wxStaticText* status_label_{nullptr};
    wxListBox* link_list_{nullptr};

    core::Subscription theme_sub_;
    core::Subscription file_opened_sub_;

    /// Cached link paths for navigation on double-click
    std::vector<std::string> link_paths_;
    std::string current_file_path_;

    void ApplyTheme();
    void OnLinkDoubleClicked(wxCommandEvent& event);
};

} // namespace markamp::ui
