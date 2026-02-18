// ============================================================================
// File: src/canvas/WidgetPermissionGuard.cpp
// Phase 14: Canvas Extensibility — widget permission enforcement
// ============================================================================
#include "canvas/WidgetPermissionGuard.h"

#include "core/Events.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace markamp::canvas
{

namespace
{

auto current_timestamp() -> std::string
{
    const auto now_time = std::chrono::system_clock::now();
    const auto time_value = std::chrono::system_clock::to_time_t(now_time);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time_value), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

} // anonymous namespace

WidgetPermissionGuard::WidgetPermissionGuard(core::EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ── Widget registration ───────────────────────────────────────────

auto WidgetPermissionGuard::register_widget(
    const std::string& widget_id, const std::vector<WidgetPermission>& declared_permissions) -> void
{
    std::unordered_set<uint8_t> perms;
    for (auto perm : declared_permissions)
    {
        perms.insert(static_cast<uint8_t>(perm));
    }
    permissions_[widget_id] = std::move(perms);
}

auto WidgetPermissionGuard::unregister_widget(const std::string& widget_id) -> void
{
    permissions_.erase(widget_id);
}

// ── Permission checks ─────────────────────────────────────────────

auto WidgetPermissionGuard::check_permission(const std::string& widget_id,
                                             WidgetPermission permission) const
    -> PermissionCheckResult
{
    auto iter = permissions_.find(widget_id);
    if (iter == permissions_.end())
    {
        record_audit(widget_id, permission, PermissionCheckResult::kNotFound);
        return PermissionCheckResult::kNotFound;
    }

    const auto& perms = iter->second;
    const auto perm_value = static_cast<uint8_t>(permission);

    if (perms.contains(perm_value))
    {
        record_audit(widget_id, permission, PermissionCheckResult::kGranted);
        return PermissionCheckResult::kGranted;
    }

    record_audit(widget_id, permission, PermissionCheckResult::kDenied);

    // Publish denial event
    core::events::CanvasWidgetPermissionDeniedEvent evt;
    evt.app_id = widget_id;
    evt.requested_scope = permission_name(permission);
    evt.denied_reason = "Permission not declared in manifest";
    event_bus_.publish(evt);

    return PermissionCheckResult::kDenied;
}

auto WidgetPermissionGuard::check_permissions(
    const std::string& widget_id, const std::vector<WidgetPermission>& permissions) const
    -> PermissionCheckResult
{
    for (auto perm : permissions)
    {
        auto result = check_permission(widget_id, perm);
        if (result != PermissionCheckResult::kGranted)
        {
            return result;
        }
    }
    return PermissionCheckResult::kGranted;
}

// ── Runtime grants/revocations ────────────────────────────────────

auto WidgetPermissionGuard::grant_permission(const std::string& widget_id,
                                             WidgetPermission permission) -> bool
{
    auto iter = permissions_.find(widget_id);
    if (iter == permissions_.end())
    {
        return false;
    }

    iter->second.insert(static_cast<uint8_t>(permission));
    return true;
}

auto WidgetPermissionGuard::revoke_permission(const std::string& widget_id,
                                              WidgetPermission permission) -> bool
{
    auto iter = permissions_.find(widget_id);
    if (iter == permissions_.end())
    {
        return false;
    }

    return iter->second.erase(static_cast<uint8_t>(permission)) > 0;
}

// ── Query ─────────────────────────────────────────────────────────

auto WidgetPermissionGuard::widget_permissions(const std::string& widget_id) const
    -> std::vector<WidgetPermission>
{
    std::vector<WidgetPermission> result;
    auto iter = permissions_.find(widget_id);
    if (iter != permissions_.end())
    {
        for (auto perm_value : iter->second)
        {
            result.push_back(static_cast<WidgetPermission>(perm_value));
        }
    }
    return result;
}

auto WidgetPermissionGuard::is_tracked(const std::string& widget_id) const -> bool
{
    return permissions_.contains(widget_id);
}

auto WidgetPermissionGuard::tracked_count() const -> size_t
{
    return permissions_.size();
}

// ── Audit log ─────────────────────────────────────────────────────

auto WidgetPermissionGuard::audit_log() const -> const std::vector<PermissionAuditEntry>&
{
    return audit_log_;
}

auto WidgetPermissionGuard::audit_log_for_widget(const std::string& widget_id) const
    -> std::vector<PermissionAuditEntry>
{
    std::vector<PermissionAuditEntry> result;
    for (const auto& entry : audit_log_)
    {
        if (entry.widget_id == widget_id)
        {
            result.push_back(entry);
        }
    }
    return result;
}

auto WidgetPermissionGuard::clear_audit_log() -> void
{
    audit_log_.clear();
}

// ── Utility ───────────────────────────────────────────────────────

auto WidgetPermissionGuard::permission_name(WidgetPermission permission) -> std::string
{
    switch (permission)
    {
        case WidgetPermission::kReadData:
            return "read_data";
        case WidgetPermission::kWriteData:
            return "write_data";
        case WidgetPermission::kNetworkAccess:
            return "network_access";
        case WidgetPermission::kClipboardAccess:
            return "clipboard_access";
        case WidgetPermission::kFileAccess:
            return "file_access";
        case WidgetPermission::kCanvasModify:
            return "canvas_modify";
    }
    return "unknown";
}

auto WidgetPermissionGuard::clear() -> void
{
    permissions_.clear();
    audit_log_.clear();
}

// ── Private ───────────────────────────────────────────────────────

auto WidgetPermissionGuard::record_audit(const std::string& widget_id,
                                         WidgetPermission permission,
                                         PermissionCheckResult result) const -> void
{
    PermissionAuditEntry entry;
    entry.widget_id = widget_id;
    entry.permission = permission;
    entry.result = result;
    entry.timestamp = current_timestamp();

    audit_log_.push_back(std::move(entry));
}

} // namespace markamp::canvas
