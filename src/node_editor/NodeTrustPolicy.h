#pragma once

// V11 Phase 47: Security Hardening Trust Policy And Execution Guards
// Trust policy engine: controls which packs and operations are permitted.

#include "NodeEditorTypes.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Strong-typed rule identifier.
struct RuleId
{
    uint64_t value{0};

    constexpr RuleId() = default;
    constexpr explicit RuleId(uint64_t v_arg)
        : value(v_arg)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }
    constexpr auto operator==(const RuleId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const RuleId&) const noexcept = default;
};

/// Trust action: what to do when a permission is checked.
enum class TrustAction : uint8_t
{
    kAllow,
    kDeny,
    kPrompt
};

/// Permissions that can be requested by extensions.
enum class ExecutionPermission : uint8_t
{
    kFileRead,
    kFileWrite,
    kNetwork,
    kProcess,
    kClipboard,
    kEnvironment
};

/// A trust rule defines allowed/denied permissions for a scope.
struct TrustRule
{
    RuleId rule_id;
    std::string scope; // Pack ID or "*" for global
    TrustAction action{TrustAction::kDeny};
    std::vector<ExecutionPermission> permissions;
};

/// An audit log entry recording a permission check.
struct AuditEntry
{
    std::string pack_id;
    ExecutionPermission permission;
    bool granted{false};
    std::chrono::steady_clock::time_point timestamp;
};

/// Trust policy engine managing security for node execution.
class NodeTrustPolicy
{
public:
    NodeTrustPolicy();

    // --- Rule management ---
    auto add_rule(TrustRule rule) -> RuleId;
    auto remove_rule(RuleId rule_id) -> bool;
    [[nodiscard]] auto find_rule(RuleId rule_id) const -> const TrustRule*;
    [[nodiscard]] auto rule_count() const -> std::size_t;

    // --- Policy check ---
    [[nodiscard]] auto check_permission(const std::string& pack_id,
                                        ExecutionPermission permission) const -> TrustAction;
    [[nodiscard]] auto is_allowed(const std::string& pack_id, ExecutionPermission permission) const
        -> bool;

    // --- Audit ---
    void log_access(const std::string& pack_id, ExecutionPermission permission, bool granted);
    [[nodiscard]] auto audit_log() const -> const std::vector<AuditEntry>&;
    [[nodiscard]] auto audit_count() const -> std::size_t;
    void clear_audit_log();

    // --- Bulk ---
    void clear();

private:
    uint64_t next_id_{1};
    std::unordered_map<uint64_t, TrustRule> rules_;
    std::vector<AuditEntry> audit_entries_;

    void register_default_rules();
};

} // namespace markamp::node_editor
