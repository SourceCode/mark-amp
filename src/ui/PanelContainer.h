/**
 * @file PanelContainer.h
 */

#pragma once

#include "core/EventBus.h"
#include "core/Events.h"
#include "ui/DesignSystemContext.h"
#include "ui/EmptyPanelState.h"
#include "ui/PanelAreaModel.h"
#include "ui/PanelTabBar.h"
#include "ui/ThemeAwareWindow.h"

#include <wx/bookctrl.h>
#include <wx/panel.h>
#include <wx/simplebook.h>
#include <wx/sizer.h>
#include <wx/window.h>

#include <string>
#include <unordered_map>

namespace markamp::ui
{

/**
 * @brief The primary container for the bottom panel area.
 *
 * It hosts the PanelTabBar at the top and manages the display of the active
 * panel's actual window content beneath it using a wxSimplebook.
 */
class PanelContainer : public ThemeAwareWindow
{
public:
    PanelContainer(wxWindow* parent,
                   DesignSystemContext& design_system,
                   core::EventBus& event_bus,
                   PanelAreaModel& model);

    ~PanelContainer() override;

    // Disallow copies
    PanelContainer(const PanelContainer&) = delete;
    auto operator=(const PanelContainer&) -> PanelContainer& = delete;

    // Disallow moves
    PanelContainer(PanelContainer&&) = delete;
    auto operator=(PanelContainer&&) -> PanelContainer& = delete;

    /**
     * @brief Theme update response
     */
    void OnThemeChanged(const core::Theme& new_theme) override;

    /**
     * @brief Registers a physical window for a specific panel ID.
     *
     * The window should be created as a child of the `GetContentArea()` window.
     *
     * @param panel_id The unique panel identifier.
     * @param window The wxWindow associated with this panel.
     */
    void RegisterPanelWindow(const std::string& panel_id, wxWindow* window);

    using PanelFactory = std::function<wxWindow*(wxWindow*)>;

    /**
     * @brief Registers a factory function to lazily initialize a panel when activated.
     */
    void RegisterDeferredPanel(const std::string& panel_id, PanelFactory factory);

    /**
     * @brief Returns the window area where panel content should be parented.
     */
    auto GetContentArea() const -> wxWindow*
    {
        return content_book_;
    }

    /**
     * @brief Returns the window area where action toolbars should be parented.
     */
    auto GetActionToolbarArea() const -> wxWindow*
    {
        return action_toolbar_container_;
    }

    /**
     * @brief Registers an action toolbar for a specific panel ID.
     */
    void RegisterActionToolbar(const std::string& panel_id, wxWindow* toolbar);

private:
    void SyncWithModel();
    void OnTabsChanged(const core::events::PanelAreaTabsChangedEvent& event);

    DesignSystemContext& ds_;
    core::EventBus& event_bus_;
    PanelAreaModel& model_;

    PanelTabBar* tab_bar_{nullptr};
    wxWindow* action_toolbar_container_{nullptr};
    wxBoxSizer* header_sizer_{nullptr};
    wxSimplebook* content_book_{nullptr};
    wxBoxSizer* main_sizer_{nullptr};

    std::unordered_map<std::string, wxWindow*> panel_windows_;
    std::unordered_map<std::string, wxWindow*> action_toolbars_;
    std::unordered_map<std::string, size_t> page_indices_;
    std::unordered_map<std::string, PanelFactory> deferred_panels_;

    EmptyPanelState* empty_state_{nullptr};

    core::Subscription model_sub_;
};

} // namespace markamp::ui
