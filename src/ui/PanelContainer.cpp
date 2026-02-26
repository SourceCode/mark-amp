/**
 * @file PanelContainer.cpp
 */

#include "ui/PanelContainer.h"

#include "ui/IconManager.h"

namespace markamp::ui
{

PanelContainer::PanelContainer(wxWindow* parent,
                               DesignSystemContext& design_system,
                               core::EventBus& event_bus,
                               PanelAreaModel& model)
    : ThemeAwareWindow(parent,
                       design_system.theme,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxBORDER_NONE | wxTAB_TRAVERSAL)
    , ds_(design_system)
    , event_bus_(event_bus)
    , model_(model)
    , tab_bar_(nullptr)
    , content_book_(nullptr)
    , main_sizer_(nullptr)
{
    SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));

    main_sizer_ = new wxBoxSizer(wxVERTICAL);

    header_sizer_ = new wxBoxSizer(wxHORIZONTAL);
    tab_bar_ = new PanelTabBar(this, ds_, event_bus_, model_);
    action_toolbar_container_ = new wxWindow(this, wxID_ANY);
    action_toolbar_container_->SetBackgroundColour(
        theme_engine().color(core::ThemeColorToken::BgPanel));
    action_toolbar_container_->SetSizer(new wxBoxSizer(wxHORIZONTAL));

    header_sizer_->Add(tab_bar_, 1, wxEXPAND | wxALL, 0);
    header_sizer_->Add(action_toolbar_container_, 0, wxEXPAND | wxALL, 0);

    main_sizer_->Add(header_sizer_, 0, wxEXPAND | wxALL, 0);

    content_book_ =
        new wxSimplebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    content_book_->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgApp));
    main_sizer_->Add(content_book_, 1, wxEXPAND | wxALL, 0);

    empty_state_ = new EmptyPanelState(this, ds_, IconManager::get());
    empty_state_->set_icon("panel-layout-bottom"); // Using a valid icon name or a generic one
    empty_state_->set_message("No panels are currently active");
    empty_state_->Hide();
    main_sizer_->Add(empty_state_, 1, wxEXPAND | wxALL, 0);

    SetSizer(main_sizer_);

    model_sub_ = event_bus_.subscribe<core::events::PanelAreaTabsChangedEvent>(
        [this](const auto& evt) { OnTabsChanged(evt); });

    SyncWithModel();
}

PanelContainer::~PanelContainer() = default;

void PanelContainer::OnThemeChanged(const core::Theme&)
{
    SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));
    if (action_toolbar_container_)
    {
        action_toolbar_container_->SetBackgroundColour(
            theme_engine().color(core::ThemeColorToken::BgPanel));
    }
    if (content_book_)
    {
        content_book_->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgApp));
    }
    Refresh();
}

void PanelContainer::RegisterPanelWindow(const std::string& panel_id, wxWindow* window)
{
    if (panel_windows_.find(panel_id) != panel_windows_.end())
    {
        return; // Already registered
    }

    // Reparent just in case it wasn't created with content_book_ as parent
    if (window->GetParent() != content_book_)
    {
        window->Reparent(content_book_);
    }

    panel_windows_[panel_id] = window;

    // Add page silently
    content_book_->AddPage(window, panel_id, false);
    page_indices_[panel_id] = content_book_->GetPageCount() - 1;

    SyncWithModel();
}

void PanelContainer::RegisterDeferredPanel(const std::string& panel_id, PanelFactory factory)
{
    if (panel_windows_.contains(panel_id) || deferred_panels_.contains(panel_id))
    {
        return; // Already registered
    }
    deferred_panels_[panel_id] = std::move(factory);
}

void PanelContainer::RegisterActionToolbar(const std::string& panel_id, wxWindow* toolbar)
{
    if (action_toolbars_.find(panel_id) != action_toolbars_.end())
    {
        return; // Already registered
    }

    if (toolbar->GetParent() != action_toolbar_container_)
    {
        toolbar->Reparent(action_toolbar_container_);
    }

    action_toolbars_[panel_id] = toolbar;
    action_toolbar_container_->GetSizer()->Add(toolbar, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    toolbar->Hide(); // Hidden by default

    SyncWithModel();
}

void PanelContainer::OnTabsChanged(const core::events::PanelAreaTabsChangedEvent& /*event*/)
{
    SyncWithModel();
}

void PanelContainer::SyncWithModel()
{
    const std::string active_id = model_.active_panel();

    if (active_id.empty())
    {
        content_book_->Hide();
        action_toolbar_container_->Hide();
        empty_state_->Show();
        Layout();
        Refresh();
        return;
    }

    empty_state_->Hide();
    content_book_->Show();
    action_toolbar_container_->Show();

    // Lazy load check
    if (!panel_windows_.contains(active_id) && deferred_panels_.contains(active_id))
    {
        auto factory = std::move(deferred_panels_[active_id]);
        deferred_panels_.erase(active_id);

        auto* window = factory(content_book_);
        if (window)
        {
            panel_windows_[active_id] = window;
            content_book_->AddPage(window, active_id, false);
            page_indices_[active_id] = content_book_->GetPageCount() - 1;
        }
    }

    auto iter = page_indices_.find(active_id);
    if (iter != page_indices_.end())
    {
        const size_t page_index = iter->second;
        if (content_book_->GetSelection() != static_cast<int>(page_index))
        {
            content_book_->ChangeSelection(static_cast<std::size_t>(page_index));
        }

        // Ensure child window is laid out, if it exists
        if (panel_windows_.contains(active_id))
        {
            auto* window = panel_windows_[active_id];
            if (window)
            {
                window->Layout();
                window->Refresh();
            }
        }

        // Handle action toolbars
        for (auto& [id, toolbar] : action_toolbars_)
        {
            if (id == active_id)
            {
                toolbar->Show();
            }
            else
            {
                toolbar->Hide();
            }
        }
        action_toolbar_container_->Layout();
        action_toolbar_container_->Refresh();
    }
}

} // namespace markamp::ui
