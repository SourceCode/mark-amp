/// RuntimePolicy.cpp — V7 Phase 39: Enterprise runtime protections

#include "RuntimePolicy.h"

#include <algorithm>
#include <chrono>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Runtime Policy
// ══════════════════════════════════════════════════════════════════════════════

auto RuntimePolicy::is_extension_allowed(const std::string& extension_id) const -> bool
{
    // If blocklisted, always deny
    if (blocklist_.contains(extension_id))
    {
        return false;
    }

    // If allowlist is non-empty, only allowlisted extensions pass
    if (!allowlist_.empty())
    {
        return allowlist_.contains(extension_id);
    }

    return true;
}

void RuntimePolicy::block_extension(const std::string& extension_id)
{
    blocklist_.insert(extension_id);
}

void RuntimePolicy::unblock_extension(const std::string& extension_id)
{
    blocklist_.erase(extension_id);
}

void RuntimePolicy::allow_extension(const std::string& extension_id)
{
    allowlist_.insert(extension_id);
}

auto RuntimePolicy::blocklist() const -> std::vector<std::string>
{
    return {blocklist_.begin(), blocklist_.end()};
}

auto RuntimePolicy::allowlist() const -> std::vector<std::string>
{
    return {allowlist_.begin(), allowlist_.end()};
}

void RuntimePolicy::process_args(const std::vector<std::string>& args)
{
    for (const auto& arg : args)
    {
        if (arg == "--immutable")
        {
            immutable_ = true;
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// Network Interceptor
// ══════════════════════════════════════════════════════════════════════════════

auto NetworkInterceptor::has_permission(const std::string& extension_id) const -> bool
{
    return permitted_.contains(extension_id);
}

void NetworkInterceptor::grant_permission(const std::string& extension_id)
{
    permitted_.insert(extension_id);
}

void NetworkInterceptor::revoke_permission(const std::string& extension_id)
{
    permitted_.erase(extension_id);
}

auto NetworkInterceptor::intercept(const std::string& extension_id, const std::string& url)
    -> Result<void>
{
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();

    bool allowed = has_permission(extension_id);

    audit_log_.push_back(AuditEntry{
        .extension_id = extension_id,
        .url = url,
        .allowed = allowed,
        .timestamp_ms = now_ms,
    });

    if (!allowed)
    {
        blocked_count_++;
        return std::unexpected(
            make_error(ErrorCode::PermissionDenied,
                       SubsystemId::ExtensionHost,
                       "Extension '" + extension_id + "' lacks network permission for: " + url));
    }

    return {};
}

} // namespace markamp::core
