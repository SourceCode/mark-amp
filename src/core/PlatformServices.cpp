/// @file PlatformServices.cpp
/// @brief V9 Phase 32 – Cross-platform services implementation.

#include "PlatformServices.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// CredentialManager
// ============================================================================

auto CredentialManager::store(const Credential& cred) -> bool
{
    store_[cred.service] = cred;
    return true;
}

auto CredentialManager::retrieve(const std::string& service) const -> Credential
{
    auto iter = store_.find(service);
    if (iter != store_.end())
    {
        return iter->second;
    }
    return Credential{};
}

auto CredentialManager::remove(const std::string& service) -> bool
{
    return store_.erase(service) > 0;
}

auto CredentialManager::has(const std::string& service) const -> bool
{
    return store_.contains(service);
}

auto CredentialManager::list_services() const -> std::vector<std::string>
{
    std::vector<std::string> services;
    services.reserve(store_.size());
    for (const auto& [svc, _cred] : store_)
    {
        services.push_back(svc);
    }
    std::sort(services.begin(), services.end());
    return services;
}

// ============================================================================
// NotificationService
// ============================================================================

auto NotificationService::show(const NotificationInfo& info) -> std::string
{
    NotificationInfo stored = info;
    if (stored.notification_id.empty())
    {
        stored.notification_id = "notif_" + std::to_string(next_id_++);
    }
    history_.push_back(stored);
    return stored.notification_id;
}

void NotificationService::dismiss(const std::string& notification_id)
{
    auto iter = std::remove_if(history_.begin(),
                               history_.end(),
                               [&](const NotificationInfo& notif)
                               { return notif.notification_id == notification_id; });
    history_.erase(iter, history_.end());
}

auto NotificationService::history() const -> const std::vector<NotificationInfo>&
{
    return history_;
}

void NotificationService::clear_history()
{
    history_.clear();
}

// ============================================================================
// FileWatcher
// ============================================================================

auto FileWatcher::watch(const std::string& path, bool /*recursive*/) -> bool
{
    if (is_watching(path))
    {
        return false;
    }
    watched_.push_back(path);
    return true;
}

auto FileWatcher::unwatch(const std::string& path) -> bool
{
    auto iter = std::find(watched_.begin(), watched_.end(), path);
    if (iter != watched_.end())
    {
        watched_.erase(iter);
        return true;
    }
    return false;
}

auto FileWatcher::poll_events() -> std::vector<FileWatchEvent>
{
    auto events = std::move(pending_);
    pending_.clear();
    return events;
}

void FileWatcher::inject_event(const FileWatchEvent& evt)
{
    pending_.push_back(evt);
}

auto FileWatcher::watched_paths() const -> std::vector<std::string>
{
    return watched_;
}

auto FileWatcher::is_watching(const std::string& path) const -> bool
{
    return std::find(watched_.begin(), watched_.end(), path) != watched_.end();
}

// ============================================================================
// AutoUpdateService
// ============================================================================

void AutoUpdateService::set_channel(UpdateChannel channel)
{
    channel_ = channel;
}

auto AutoUpdateService::channel() const -> UpdateChannel
{
    return channel_;
}

void AutoUpdateService::set_current_version(const std::string& version)
{
    current_version_ = version;
}

auto AutoUpdateService::check_for_update() const -> AutoUpdateInfo
{
    AutoUpdateInfo info;
    info.channel = channel_;
    info.current_version = current_version_;
    // In production, this would query a server.
    // For testing, return a stub result.
    info.latest_version = current_version_;
    info.update_available = false;
    return info;
}

auto AutoUpdateService::download_update(const AutoUpdateInfo& /*info*/) -> bool
{
    // Stub: in production, this would background-download the update.
    return true;
}

auto AutoUpdateService::apply_on_restart(const AutoUpdateInfo& info) -> bool
{
    pending_update_version_ = info.latest_version;
    pending_install_ = true;
    return true;
}

auto AutoUpdateService::is_newer(const std::string& current, const std::string& latest) -> bool
{
    // Simple semver comparison: split on '.' and compare numerically.
    auto parse_version = [](const std::string& ver) -> std::vector<int>
    {
        std::vector<int> parts;
        std::istringstream stream(ver);
        std::string segment;
        while (std::getline(stream, segment, '.'))
        {
            try
            {
                parts.push_back(std::stoi(segment));
            }
            catch (...)
            {
                parts.push_back(0);
            }
        }
        // Pad to at least 3 parts
        while (parts.size() < 3)
        {
            parts.push_back(0);
        }
        return parts;
    };

    auto cur = parse_version(current);
    auto lat = parse_version(latest);

    for (size_t idx = 0; idx < std::min(cur.size(), lat.size()); ++idx)
    {
        if (lat[idx] > cur[idx])
        {
            return true;
        }
        if (lat[idx] < cur[idx])
        {
            return false;
        }
    }
    return false; // Equal
}

// ============================================================================
// SystemTrayService
// ============================================================================

void SystemTrayService::show()
{
    visible_ = true;
}

void SystemTrayService::hide()
{
    visible_ = false;
}

void SystemTrayService::set_icon(const std::string& icon_path)
{
    icon_path_ = icon_path;
}

void SystemTrayService::set_tooltip(const std::string& tooltip)
{
    tooltip_ = tooltip;
}

void SystemTrayService::set_menu(const std::vector<TrayMenuItem>& items)
{
    menu_items_ = items;
}

void SystemTrayService::set_visibility_mode(TrayVisibility mode)
{
    visibility_mode_ = mode;
}

auto SystemTrayService::is_visible() const -> bool
{
    return visible_;
}

auto SystemTrayService::icon_path() const -> const std::string&
{
    return icon_path_;
}

auto SystemTrayService::tooltip() const -> const std::string&
{
    return tooltip_;
}

auto SystemTrayService::menu_items() const -> const std::vector<TrayMenuItem>&
{
    return menu_items_;
}

auto SystemTrayService::visibility_mode() const -> TrayVisibility
{
    return visibility_mode_;
}

// ============================================================================
// ShellIntegration
// ============================================================================

auto ShellIntegration::register_file_associations() -> bool
{
    associations_registered_ = true;
    return true;
}

auto ShellIntegration::register_protocol_handler() -> bool
{
    protocol_registered_ = true;
    return true;
}

auto ShellIntegration::open_with_system(const std::string& /*path*/) -> bool
{
    // Stub: in production, calls platform-specific open (NSWorkspace, ShellExecute, xdg-open).
    return true;
}

auto ShellIntegration::get_file_icon(const std::string& extension) const -> std::string
{
    if (extension == ".md" || extension == "md")
    {
        return "icons/markdown.png";
    }
    if (extension == ".markamp-nb" || extension == "markamp-nb")
    {
        return "icons/notebook.png";
    }
    if (extension == ".markamp-board" || extension == "markamp-board")
    {
        return "icons/board.png";
    }
    return "icons/generic.png";
}

auto ShellIntegration::are_associations_registered() const -> bool
{
    return associations_registered_;
}

auto ShellIntegration::is_protocol_registered() const -> bool
{
    return protocol_registered_;
}

} // namespace markamp::core
