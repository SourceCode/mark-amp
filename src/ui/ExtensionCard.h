#pragma once

#include "core/EventBus.h"
#include "core/ExtensionManagement.h"
#include "core/ExtensionManifest.h"
#include "core/GalleryService.h"
#include "core/ThemeEngine.h"

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include <functional>
#include <string>

namespace markamp::ui
{

/// Compact card widget for displaying an extension in the scrolling list.
/// Shows name, publisher, version, description, and an action button.
class ExtensionCard : public wxPanel
{
public:
    /// Visual state of the card
    enum class State
    {
        NotInstalled,
        Installed,
        UpdateAvailable
    };

    ExtensionCard(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  const std::string& extension_id,
                  const std::string& name,
                  const std::string& publisher,
                  const std::string& version,
                  const std::string& description,
                  State state);

    /// Set callback for when the card body is clicked (open detail view).
    void SetOnClick(std::function<void(const std::string&)> callback);

    /// Set callback for when the action button is clicked.
    void SetOnAction(std::function<void(const std::string&, State)> callback);

    /// Apply current theme colours.
    void ApplyTheme(core::ThemeEngine& theme_engine);

    /// Get the extension ID this card represents.
    [[nodiscard]] auto GetExtensionId() const -> const std::string&;

    /// Update the visual state and button text.
    void SetState(State new_state);

private:
    core::ThemeEngine& theme_engine_;
    std::string extension_id_;
    State state_;

    wxStaticText* name_label_{nullptr};
    wxStaticText* publisher_label_{nullptr};
    wxStaticText* version_label_{nullptr};
    wxStaticText* description_label_{nullptr};
    wxButton* action_button_{nullptr};
    wxPanel* info_panel_{nullptr};

    std::function<void(const std::string&)> on_click_;
    std::function<void(const std::string&, State)> on_action_;

    void CreateLayout(const std::string& name,
                      const std::string& publisher,
                      const std::string& version,
                      const std::string& description);
    void UpdateActionButton();
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnClick(wxMouseEvent& event);

    static constexpr int kCardHeight = 72;
    static constexpr int kCardPadding = 10;
};

} // namespace markamp::ui
