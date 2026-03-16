#pragma once

#include "EventBus.h"
#include "Events.h"

#include <chrono>
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

/// A stored notification entry for the bell badge/dropdown.
struct StoredNotification
{
    enum class Level : uint8_t
    {
        kInfo,
        kWarning,
        kError
    };

    std::string id;
    std::string title;
    std::string message;
    Level level{Level::kInfo};
    std::chrono::system_clock::time_point timestamp;
    bool read{false};
    std::string source; ///< "build", "extension", "system"
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

    // ── Stored notification management (Phase 26: bell badge) ──────

    /// Store a notification for the bell badge dropdown.
    void store(const std::string& title,
               const std::string& message,
               StoredNotification::Level level = StoredNotification::Level::kInfo,
               const std::string& source = {});

    /// Get all stored notifications.
    [[nodiscard]] auto stored_notifications() const -> const std::vector<StoredNotification>&;

    /// Get the number of unread stored notifications.
    [[nodiscard]] auto unread_count() const -> int;

    /// Mark a specific stored notification as read.
    void mark_read(const std::string& id);

    /// Mark all stored notifications as read.
    void mark_all_read();

    /// (#70) Dismiss a specific stored notification.
    void dismiss_notification(const std::string& notification_id);

    /// Remove all stored notifications.
    void clear_all();

private:
    EventBus& event_bus_;
    std::vector<StoredNotification> stored_notifications_;
    int next_stored_id_{1};
};

} // namespace markamp::core
