#pragma once

#include "core/EventBus.h"
#include "core/ExtensionManagement.h"
#include "core/ExtensionManifest.h"
#include "core/GalleryService.h"
#include "core/ThemeEngine.h"

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Detail panel shown when a user clicks an extension card.
/// Displays full extension info with action buttons and a back button.
class ExtensionDetailPanel : public wxPanel
{
public:
    ExtensionDetailPanel(wxWindow* parent,
                         core::ThemeEngine& theme_engine,
                         core::EventBus& event_bus);

    /// Show details for a local (installed) extension.
    void ShowExtension(const core::LocalExtension& extension, bool has_update = false);

    /// Show details for a gallery extension (not yet installed).
    void ShowGalleryExtension(const core::GalleryExtension& extension, bool is_installed = false);

    /// Clear the detail view.
    void Clear();

    /// Set callback for the back button.
    void SetOnBack(std::function<void()> callback);

    /// Set callback for install/uninstall actions.
    void SetOnInstall(std::function<void(const std::string&)> callback);
    void SetOnUninstall(std::function<void(const std::string&)> callback);
    void SetOnUpdate(std::function<void(const std::string&)> callback);

    /// Apply current theme styling.
    void ApplyTheme();

private:
    core::ThemeEngine& theme_engine_;
    [[maybe_unused]] core::EventBus& event_bus_;

    std::string current_extension_id_;
    bool current_is_installed_{false};
    bool current_has_update_{false};

    // Header
    wxButton* back_button_{nullptr};
    wxStaticText* title_label_{nullptr};
    wxStaticText* publisher_label_{nullptr};
    wxStaticText* version_label_{nullptr};

    // Action buttons
    wxButton* install_button_{nullptr};
    wxButton* uninstall_button_{nullptr};
    wxButton* update_button_{nullptr};

    // Content
    wxScrolledWindow* content_scroll_{nullptr};
    wxStaticText* description_text_{nullptr};
    wxStaticText* deps_header_{nullptr};
    wxStaticText* deps_text_{nullptr};

    // Callbacks
    std::function<void()> on_back_;
    std::function<void(const std::string&)> on_install_;
    std::function<void(const std::string&)> on_uninstall_;
    std::function<void(const std::string&)> on_update_;

    void CreateLayout();
    void UpdateActionButtons();
};

} // namespace markamp::ui
