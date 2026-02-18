#pragma once

/// @file WidgetPermissionGuard.h
/// @brief Phase 14: Permission enforcement for canvas widgets.
///
/// Enforces a default-deny permission model for widget operations.
/// Widgets must declare required permissions in their manifest.
/// Runtime grants/revocations are tracked with an audit log.

#include "core/EventBus.h"

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::canvas
{

/// Fine-grained permission scopes for widgets.
enum class WidgetPermission : uint8_t
{
    kReadData,        ///< Read data from provider
    kWriteData,       ///< Write/modify data
    kNetworkAccess,   ///< Make network requests
    kClipboardAccess, ///< Read/write clipboard
    kFileAccess,      ///< Access filesystem
    kCanvasModify     ///< Modify other canvas objects
};

/// Result of a permission check.
enum class PermissionCheckResult : uint8_t
{
    kGranted, ///< Permission explicitly granted
    kDenied,  ///< Permission denied (not declared or revoked)
    kNotFound ///< Widget not tracked
};

/// Entry in the permission audit log.
struct PermissionAuditEntry
{
    std::string widget_id;
    WidgetPermission permission;
    PermissionCheckResult result;
    std::string timestamp;
};

/// Enforces widget permissions with default-deny policy and audit trail.
class WidgetPermissionGuard
{
public:
    explicit WidgetPermissionGuard(core::EventBus& event_bus);

    // ── Widget registration ───────────────────────────────────────

    /// Register a widget with its declared permissions.
    auto register_widget(const std::string& widget_id,
                         const std::vector<WidgetPermission>& declared_permissions) -> void;

    /// Remove a widget from permission tracking.
    auto unregister_widget(const std::string& widget_id) -> void;

    // ── Permission checks ─────────────────────────────────────────

    /// Check if a widget has a specific permission.
    [[nodiscard]] auto check_permission(const std::string& widget_id,
                                        WidgetPermission permission) const -> PermissionCheckResult;

    /// Check if a widget has all specified permissions.
    [[nodiscard]] auto check_permissions(const std::string& widget_id,
                                         const std::vector<WidgetPermission>& permissions) const
        -> PermissionCheckResult;

    // ── Runtime grants/revocations ────────────────────────────────

    /// Grant an additional permission to a widget at runtime.
    auto grant_permission(const std::string& widget_id, WidgetPermission permission) -> bool;

    /// Revoke a specific permission from a widget.
    auto revoke_permission(const std::string& widget_id, WidgetPermission permission) -> bool;

    // ── Query ─────────────────────────────────────────────────────

    /// All granted permissions for a widget.
    [[nodiscard]] auto widget_permissions(const std::string& widget_id) const
        -> std::vector<WidgetPermission>;

    /// Whether a widget is registered.
    [[nodiscard]] auto is_tracked(const std::string& widget_id) const -> bool;

    /// Number of tracked widgets.
    [[nodiscard]] auto tracked_count() const -> size_t;

    // ── Audit log ─────────────────────────────────────────────────

    /// Get the full audit log.
    [[nodiscard]] auto audit_log() const -> const std::vector<PermissionAuditEntry>&;

    /// Audit entries for a specific widget.
    [[nodiscard]] auto audit_log_for_widget(const std::string& widget_id) const
        -> std::vector<PermissionAuditEntry>;

    /// Clear the audit log.
    auto clear_audit_log() -> void;

    // ── Utility ───────────────────────────────────────────────────

    /// Human-readable name for a permission.
    [[nodiscard]] static auto permission_name(WidgetPermission permission) -> std::string;

    /// Remove all tracked widgets and audit entries.
    auto clear() -> void;

private:
    /// Record an audit entry.
    auto record_audit(const std::string& widget_id,
                      WidgetPermission permission,
                      PermissionCheckResult result) const -> void;

    core::EventBus& event_bus_;

    /// widget_id -> set of granted permissions
    std::unordered_map<std::string, std::unordered_set<uint8_t>> permissions_;

    /// Audit trail (mutable for const check methods to record checks)
    mutable std::vector<PermissionAuditEntry> audit_log_;
};

} // namespace markamp::canvas
