/// RuntimePolicy.h — V7 Phase 39: Enterprise runtime protections
///
/// Immutable config mode, extension blocklist/allowlist, policy config file.

#pragma once

#include "Result.h"

#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Runtime Policy
// ══════════════════════════════════════════════════════════════════════════════

/// Enterprise runtime policy configuration.
class RuntimePolicy
{
public:
    RuntimePolicy() = default;

    /// Enable immutable config mode (--immutable flag).
    void set_immutable(bool immutable)
    {
        immutable_ = immutable;
    }
    [[nodiscard]] auto is_immutable() const noexcept -> bool
    {
        return immutable_;
    }

    /// Check if an extension is allowed.
    [[nodiscard]] auto is_extension_allowed(const std::string& extension_id) const -> bool;

    /// Add an extension to the blocklist.
    void block_extension(const std::string& extension_id);

    /// Remove an extension from the blocklist.
    void unblock_extension(const std::string& extension_id);

    /// Add an extension to the allowlist. When the allowlist is non-empty,
    /// only allowlisted extensions are permitted.
    void allow_extension(const std::string& extension_id);

    /// Get the blocklist.
    [[nodiscard]] auto blocklist() const -> std::vector<std::string>;

    /// Get the allowlist.
    [[nodiscard]] auto allowlist() const -> std::vector<std::string>;

    /// Process command-line flags (e.g., --immutable).
    void process_args(const std::vector<std::string>& args);

private:
    bool immutable_{false};
    std::unordered_set<std::string> blocklist_;
    std::unordered_set<std::string> allowlist_;
};

// ══════════════════════════════════════════════════════════════════════════════
// Network Interceptor
// ══════════════════════════════════════════════════════════════════════════════

/// Intercepts extension network calls, enforcing permissions.
class NetworkInterceptor
{
public:
    NetworkInterceptor() = default;

    /// Check if an extension has network permission.
    [[nodiscard]] auto has_permission(const std::string& extension_id) const -> bool;

    /// Grant network permission to an extension.
    void grant_permission(const std::string& extension_id);

    /// Revoke network permission from an extension.
    void revoke_permission(const std::string& extension_id);

    /// Attempt a network call. Returns error if extension lacks permission.
    [[nodiscard]] auto intercept(const std::string& extension_id, const std::string& url)
        -> Result<void>;

    /// Get the audit log of attempted network calls.
    struct AuditEntry
    {
        std::string extension_id;
        std::string url;
        bool allowed{false};
        int64_t timestamp_ms{0};
    };

    [[nodiscard]] auto audit_log() const -> std::vector<AuditEntry>
    {
        return audit_log_;
    }

    /// Get the number of blocked calls.
    [[nodiscard]] auto blocked_count() const noexcept -> size_t
    {
        return blocked_count_;
    }

private:
    std::unordered_set<std::string> permitted_;
    std::vector<AuditEntry> audit_log_;
    size_t blocked_count_{0};
};

} // namespace markamp::core
