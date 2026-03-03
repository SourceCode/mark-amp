#pragma once

#include "ExtensionManifest.h"
#include "ExtensionScanner.h"
#include "GalleryService.h"
#include "VsixService.h"

#include <chrono>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;

/// Information about an available extension update.
struct ExtensionUpdateInfo
{
    std::string extension_id; // publisher.name
    std::string current_version;
    std::string available_version;
    GalleryExtension gallery_entry;
};

/// Phase 20 Task 20: Represents a conflict between extensions.
struct ExtensionConflict
{
    std::string extension_a;   // First extension ID
    std::string extension_b;   // Second extension ID
    std::string conflict_type; // "grammar", "command", "language", "keybinding"
    std::string detail;        // Human-readable conflict description
};

/// Interface for the extension management service.
/// Orchestrates install/uninstall/update flows combining gallery,
/// VSIX, scanner, and enablement services.
class IExtensionManagementService
{
public:
    virtual ~IExtensionManagementService() = default;

    IExtensionManagementService(const IExtensionManagementService&) = delete;
    auto operator=(const IExtensionManagementService&) -> IExtensionManagementService& = delete;
    IExtensionManagementService(IExtensionManagementService&&) = delete;
    auto operator=(IExtensionManagementService&&) -> IExtensionManagementService& = delete;

    /// Install an extension from a local VSIX file.
    [[nodiscard]] virtual auto install(const std::filesystem::path& vsix_path)
        -> std::expected<LocalExtension, std::string> = 0;

    /// Install an extension from the marketplace.
    [[nodiscard]] virtual auto install_from_gallery(const GalleryExtension& gallery_ext)
        -> std::expected<LocalExtension, std::string> = 0;

    /// Uninstall an extension by ID (publisher.name).
    /// Validates dependencies before removal.
    [[nodiscard]] virtual auto uninstall(const std::string& extension_id)
        -> std::expected<void, std::string> = 0;

    /// Get all installed extensions (refreshes from disk).
    [[nodiscard]] virtual auto get_installed() -> std::vector<LocalExtension> = 0;

    /// Check for available updates from the marketplace.
    [[nodiscard]] virtual auto check_updates()
        -> std::expected<std::vector<ExtensionUpdateInfo>, std::string> = 0;

    /// Update a specific extension to the latest gallery version.
    [[nodiscard]] virtual auto update(const std::string& extension_id)
        -> std::expected<LocalExtension, std::string> = 0;

    /// Phase 20 Task 5: Enable a previously disabled extension.
    [[nodiscard]] virtual auto enable(const std::string& extension_id)
        -> std::expected<void, std::string> = 0;

    /// Phase 20 Task 5: Disable an extension without uninstalling.
    [[nodiscard]] virtual auto disable(const std::string& extension_id)
        -> std::expected<void, std::string> = 0;

    /// Phase 20 Task 5: Check if an extension is enabled.
    [[nodiscard]] virtual auto is_enabled(const std::string& extension_id) const -> bool = 0;

    /// Phase 20 Task 20: Detect conflicts between installed extensions.
    [[nodiscard]] virtual auto detect_conflicts() -> std::vector<ExtensionConflict> = 0;

protected:
    IExtensionManagementService() = default;
};

/// Concrete implementation of IExtensionManagementService.
/// Orchestrates: gallery download → VSIX extract → scanner refresh → events.
class ExtensionManagementService : public IExtensionManagementService
{
public:
    /// Construct with all required sub-services.
    ExtensionManagementService(VsixInstallService& vsix_service,
                               ExtensionScannerService& scanner_service,
                               IExtensionGalleryService& gallery_service,
                               EventBus& event_bus);

    auto install(const std::filesystem::path& vsix_path)
        -> std::expected<LocalExtension, std::string> override;

    auto install_from_gallery(const GalleryExtension& gallery_ext)
        -> std::expected<LocalExtension, std::string> override;

    auto uninstall(const std::string& extension_id) -> std::expected<void, std::string> override;

    auto get_installed() -> std::vector<LocalExtension> override;

    auto check_updates() -> std::expected<std::vector<ExtensionUpdateInfo>, std::string> override;

    auto update(const std::string& extension_id)
        -> std::expected<LocalExtension, std::string> override;

    auto enable(const std::string& extension_id) -> std::expected<void, std::string> override;

    auto disable(const std::string& extension_id) -> std::expected<void, std::string> override;

    [[nodiscard]] auto is_enabled(const std::string& extension_id) const -> bool override;

    auto detect_conflicts() -> std::vector<ExtensionConflict> override;

    // ── Auto-update scheduler (#43) ──

    /// Schedule periodic update checks at the given interval.
    /// Calls check_updates() internally and emits events for available updates.
    void schedule_update_check(std::chrono::seconds interval);

    /// Cancel the periodic update check.
    void cancel_update_check();

    /// Whether an update check is currently scheduled.
    [[nodiscard]] auto is_update_check_scheduled() const -> bool;

    /// Get the configured check interval (zero if not scheduled).
    [[nodiscard]] auto update_check_interval() const -> std::chrono::seconds;

private:
    VsixInstallService& vsix_service_;
    ExtensionScannerService& scanner_service_;
    IExtensionGalleryService& gallery_service_;
    EventBus& event_bus_;

    /// Cached list of installed extensions. Refreshed on install/uninstall/scan.
    std::vector<LocalExtension> installed_cache_;

    /// Auto-update scheduling state.
    bool update_check_scheduled_{false};
    std::chrono::seconds update_check_interval_{0};

    /// Refresh the installed extensions cache from disk.
    void refresh_cache();

    /// Check if any installed extension depends on the given extension_id.
    [[nodiscard]] auto find_dependents(const std::string& extension_id) const
        -> std::vector<std::string>;
};

} // namespace markamp::core
