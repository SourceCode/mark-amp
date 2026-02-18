// ============================================================================
// File: src/core/SecurityCommandProvider.cpp
// Phase 29: Security & Input Validation — Command palette commands
// ============================================================================

#include "SecurityCommandProvider.h"

#include <algorithm>
#include <iterator>
#include <set>

namespace markamp::core
{

// ── Construction ──

SecurityCommandProvider::SecurityCommandProvider()
{
    register_commands();
}

auto SecurityCommandProvider::register_commands() -> void
{
    commands_ = {
        {.id = "security.viewAuditLog",
         .title = "Security: View Audit Log",
         .category = "Security",
         .keybinding = "",
         .description = "View the security audit log with recent events"},

        {.id = "security.clearAuditLog",
         .title = "Security: Clear Audit Log",
         .category = "Security",
         .keybinding = "",
         .description = "Clear all entries from the security audit log"},

        {.id = "security.viewExtensionPermissions",
         .title = "Security: View Extension Permissions",
         .category = "Security",
         .keybinding = "",
         .description = "View sandbox permissions for all installed extensions"},

        {.id = "security.toggleStrictCsp",
         .title = "Security: Toggle Strict CSP Mode",
         .category = "Content Security",
         .keybinding = "",
         .description = "Toggle between strict and relaxed Content Security Policy"},

        {.id = "security.viewRateLimitStatus",
         .title = "Security: View Rate Limit Status",
         .category = "Rate Limiting",
         .keybinding = "",
         .description = "View token bucket status for all rate-limited actions"},

        {.id = "security.resetRateLimits",
         .title = "Security: Reset Rate Limits",
         .category = "Rate Limiting",
         .keybinding = "",
         .description = "Reset all rate limit token buckets to maximum"},

        {.id = "security.exportReport",
         .title = "Security: Export Security Report",
         .category = "Security",
         .keybinding = "",
         .description = "Export a comprehensive security report as JSON"},

        {.id = "security.toggleAuditLogging",
         .title = "Security: Toggle Audit Logging",
         .category = "Security",
         .keybinding = "",
         .description = "Enable or disable security audit event logging"},
    };
}

// ── Queries ──

auto SecurityCommandProvider::commands() const -> const std::vector<SecurityCommand>&
{
    return commands_;
}

auto SecurityCommandProvider::find_command(const std::string& command_id) const
    -> std::optional<SecurityCommand>
{
    auto iter = std::find_if(commands_.begin(),
                             commands_.end(),
                             [&](const SecurityCommand& cmd) { return cmd.id == command_id; });
    if (iter != commands_.end())
    {
        return *iter;
    }
    return std::nullopt;
}

auto SecurityCommandProvider::commands_in_category(const std::string& category) const
    -> std::vector<SecurityCommand>
{
    std::vector<SecurityCommand> result;
    std::copy_if(commands_.begin(),
                 commands_.end(),
                 std::back_inserter(result),
                 [&](const SecurityCommand& cmd) { return cmd.category == category; });
    return result;
}

auto SecurityCommandProvider::categories() const -> std::vector<std::string>
{
    std::set<std::string> unique;
    for (const auto& cmd : commands_)
    {
        unique.insert(cmd.category);
    }
    return {unique.begin(), unique.end()};
}

auto SecurityCommandProvider::command_count() const -> int32_t
{
    return static_cast<int32_t>(commands_.size());
}

auto SecurityCommandProvider::available_commands() const -> std::vector<SecurityCommand>
{
    return commands_;
}

} // namespace markamp::core
