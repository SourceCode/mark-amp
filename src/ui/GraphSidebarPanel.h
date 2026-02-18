#pragma once

#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/stattext.h>

#include <string>

namespace markamp::ui
{

/// Graph sidebar panel — placeholder for the backlink/knowledge graph view.
/// Shows a list of document links and a "Coming Soon" indicator.
class GraphSidebarPanel : public wxPanel
{
public:
    GraphSidebarPanel(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    /// Add a document link to the graph list.
    void AddDocumentLink(const std::string& label);

    /// Clear all document links.
    void ClearLinks();

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;

    wxStaticText* title_label_{nullptr};
    wxStaticText* status_label_{nullptr};
    wxListBox* link_list_{nullptr};

    core::Subscription theme_sub_;

    void ApplyTheme();
};

} // namespace markamp::ui
