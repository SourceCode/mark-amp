#pragma once

#include "EventBus.h"
#include "Events.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Options for showing a notification message to the user.
/// Mirrors VS Code's `vscode.window.showInformationMessage()` /
/// `showWarningMessage()` / `showErrorMessage()` family.
struct NotificationOptions
{
    std::string message;
    events::NotificationLevel level{events::NotificationLevel::Info};
    int duration_ms{3000};

    /// Optional action buttons shown alongside the message.
    std::vector<std::string> actions;
};

/// Result of a notification with action buttons.
struct NotificationResult
{
    /// The action label the user clicked, or empty if dismissed/timed-out.
    std::string selected_action;
    bool was_dismissed{true};
};

/// Extension-facing service for showing notifications.
///
/// This is a lightweight wrapper that publishes `ShowNotificationEvent`
/// on the EventBus. The `NotificationManager` UI widget already listens
/// for these events and renders the toast.
///
/// Injected into `PluginContext` so extensions can call:
///   `ctx.notification_service->show_info("Hello!")`
class NotificationService
{
public:
    explicit NotificationService(EventBus& event_bus);

    /// Show an informational message.
    void show_info(const std::string& message, int duration_ms = 3000);

    /// Show a warning message.
    void show_warning(const std::string& message, int duration_ms = 5000);

    /// Show an error message.
    void show_error(const std::string& message, int duration_ms = 8000);

    /// Show a notification with full options including action buttons.
    void show(const NotificationOptions& options);

    /// Show a notification and return index of selected action via callback.
    /// The callback receives the selected action label, or empty string if dismissed.
    void show_with_actions(const NotificationOptions& options,
                           const std::function<void(const std::string&)>& on_action);

private:
    EventBus& event_bus_;
};

} // namespace markamp::core
