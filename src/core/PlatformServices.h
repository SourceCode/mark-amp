/// @file PlatformServices.h
/// @brief V9 Phase 32 – Cross-platform services for credentials, notifications,
///        file watching, auto-update, system tray, and shell integration.

#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Credential Management
// ============================================================================

/// Stored credential for a service.
struct Credential
{
    std::string service;
    std::string username;
    std::string password;
};

/// Cross-platform credential manager.
/// Uses platform-native stores (Keychain on macOS, Credential Manager on Windows,
/// Secret Service on Linux) when available; falls back to in-memory storage.
class CredentialManager
{
public:
    /// Store a credential.  Returns true on success.
    auto store(const Credential& cred) -> bool;

    /// Retrieve a credential by service name.
    [[nodiscard]] auto retrieve(const std::string& service) const -> Credential;

    /// Remove a credential by service name.  Returns true if removed.
    auto remove(const std::string& service) -> bool;

    /// Check whether a credential exists for the given service.
    [[nodiscard]] auto has(const std::string& service) const -> bool;

    /// List all stored service names.
    [[nodiscard]] auto list_services() const -> std::vector<std::string>;

private:
    std::unordered_map<std::string, Credential> store_;
};

// ============================================================================
// Notifications
// ============================================================================

/// Urgency level for a desktop notification.
enum class NotificationUrgency : uint8_t
{
    kLow,
    kNormal,
    kCritical
};

/// Describes a desktop notification.
struct NotificationInfo
{
    std::string title;
    std::string body;
    std::string icon_path;
    NotificationUrgency urgency{NotificationUrgency::kNormal};
    std::string notification_id; ///< Unique ID for dismiss/update
};

/// Cross-platform notification service.
class NotificationService
{
public:
    /// Show a desktop notification.  Returns the notification ID.
    auto show(const NotificationInfo& info) -> std::string;

    /// Dismiss a notification by ID.
    void dismiss(const std::string& notification_id);

    /// Get the notification history.
    [[nodiscard]] auto history() const -> const std::vector<NotificationInfo>&;

    /// Clear history.
    void clear_history();

private:
    std::vector<NotificationInfo> history_;
    int next_id_{1};
};

// ============================================================================
// File Watching
// ============================================================================

/// Type of file-system change.
enum class FileChangeType : uint8_t
{
    kCreated,
    kModified,
    kDeleted,
    kRenamed
};

/// Describes a single file-system change event.
struct FileWatchEvent
{
    std::string path;
    FileChangeType change_type{FileChangeType::kModified};
    std::string new_path; ///< For rename events
};

/// Cross-platform file watcher.
/// Uses FSEvents (macOS), ReadDirectoryChangesW (Windows), inotify (Linux)
/// when compiled for the respective platform; generic polling otherwise.
class FileWatcher
{
public:
    /// Start watching a path (file or directory, optionally recursive).
    auto watch(const std::string& path, bool recursive = true) -> bool;

    /// Stop watching a path.
    auto unwatch(const std::string& path) -> bool;

    /// Poll for pending events (non-blocking).
    [[nodiscard]] auto poll_events() -> std::vector<FileWatchEvent>;

    /// Inject a synthetic event (for testing).
    void inject_event(const FileWatchEvent& evt);

    /// List all currently watched paths.
    [[nodiscard]] auto watched_paths() const -> std::vector<std::string>;

    /// Check if a path is being watched.
    [[nodiscard]] auto is_watching(const std::string& path) const -> bool;

private:
    std::vector<std::string> watched_;
    std::vector<FileWatchEvent> pending_;
};

// ============================================================================
// Auto-Update
// ============================================================================

/// Update channel.
enum class UpdateChannel : uint8_t
{
    kStable,
    kBeta,
    kNightly
};

/// Information about an available update.
struct AutoUpdateInfo
{
    UpdateChannel channel{UpdateChannel::kStable};
    std::string current_version;
    std::string latest_version;
    std::string changelog;
    std::string download_url;
    bool update_available{false};
};

/// Cross-platform auto-update service.
class AutoUpdateService
{
public:
    /// Set the update channel.
    void set_channel(UpdateChannel channel);

    /// Get the current channel.
    [[nodiscard]] auto channel() const -> UpdateChannel;

    /// Set the current application version.
    void set_current_version(const std::string& version);

    /// Check for updates.  Returns info about the latest version.
    [[nodiscard]] auto check_for_update() const -> AutoUpdateInfo;

    /// Simulate downloading an update (in production, backgrounds a download).
    auto download_update(const AutoUpdateInfo& info) -> bool;

    /// Mark the update for installation on next restart.
    auto apply_on_restart(const AutoUpdateInfo& info) -> bool;

    /// Compare two semver strings.  Returns true if latest > current.
    [[nodiscard]] static auto is_newer(const std::string& current, const std::string& latest)
        -> bool;

private:
    UpdateChannel channel_{UpdateChannel::kStable};
    std::string current_version_;
    std::string pending_update_version_;
    bool pending_install_{false};
};

// ============================================================================
// System Tray
// ============================================================================

/// System tray menu item.
struct TrayMenuItem
{
    std::string label;
    std::string action_id;
    bool enabled{true};
    bool separator{false}; ///< If true, renders as separator (label ignored)
};

/// System tray display mode.
enum class TrayVisibility : uint8_t
{
    kAlways,
    kWhenMinimized,
    kNever
};

/// Cross-platform system tray service.
class SystemTrayService
{
public:
    /// Show the tray icon.
    void show();

    /// Hide the tray icon.
    void hide();

    /// Set the tray icon path.
    void set_icon(const std::string& icon_path);

    /// Set the tray tooltip.
    void set_tooltip(const std::string& tooltip);

    /// Set the context menu items.
    void set_menu(const std::vector<TrayMenuItem>& items);

    /// Set visibility mode.
    void set_visibility_mode(TrayVisibility mode);

    /// Query state.
    [[nodiscard]] auto is_visible() const -> bool;
    [[nodiscard]] auto icon_path() const -> const std::string&;
    [[nodiscard]] auto tooltip() const -> const std::string&;
    [[nodiscard]] auto menu_items() const -> const std::vector<TrayMenuItem>&;
    [[nodiscard]] auto visibility_mode() const -> TrayVisibility;

private:
    bool visible_{false};
    std::string icon_path_;
    std::string tooltip_;
    std::vector<TrayMenuItem> menu_items_;
    TrayVisibility visibility_mode_{TrayVisibility::kWhenMinimized};
};

// ============================================================================
// Shell Integration
// ============================================================================

/// Cross-platform shell integration service.
class ShellIntegration
{
public:
    /// Register file associations for MarkAmp file types.
    /// Extensions: .md, .markamp-nb, .markamp-board
    auto register_file_associations() -> bool;

    /// Register the markamp:// protocol handler.
    auto register_protocol_handler() -> bool;

    /// Open a file with the default system handler.
    auto open_with_system(const std::string& path) -> bool;

    /// Get the default icon path for a given file extension.
    [[nodiscard]] auto get_file_icon(const std::string& extension) const -> std::string;

    /// Check if file associations are currently registered.
    [[nodiscard]] auto are_associations_registered() const -> bool;

    /// Check if protocol handler is registered.
    [[nodiscard]] auto is_protocol_registered() const -> bool;

private:
    bool associations_registered_{false};
    bool protocol_registered_{false};
};

} // namespace markamp::core
