#include "NodeTrustPolicy.h"

namespace markamp::node_editor
{

NodeTrustPolicy::NodeTrustPolicy()
{
    register_default_rules();
}

void NodeTrustPolicy::register_default_rules()
{
    // Default: deny all for untrusted packs
    TrustRule deny_all;
    deny_all.scope = "*";
    deny_all.action = TrustAction::kDeny;
    deny_all.permissions = {
        ExecutionPermission::kFileRead,
        ExecutionPermission::kFileWrite,
        ExecutionPermission::kNetwork,
        ExecutionPermission::kProcess,
        ExecutionPermission::kClipboard,
        ExecutionPermission::kEnvironment,
    };
    add_rule(std::move(deny_all));
}

auto NodeTrustPolicy::add_rule(TrustRule rule) -> RuleId
{
    RuleId rid{next_id_++};
    rule.rule_id = rid;
    rules_.emplace(rid.value, std::move(rule));
    return rid;
}

auto NodeTrustPolicy::remove_rule(RuleId rule_id) -> bool
{
    return rules_.erase(rule_id.value) > 0;
}

auto NodeTrustPolicy::find_rule(RuleId rule_id) const -> const TrustRule*
{
    auto iter = rules_.find(rule_id.value);
    if (iter == rules_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto NodeTrustPolicy::rule_count() const -> std::size_t
{
    return rules_.size();
}

auto NodeTrustPolicy::check_permission(const std::string& pack_id,
                                       ExecutionPermission permission) const -> TrustAction
{
    // Check specific rules first (matching pack_id), then global ("*") rules
    TrustAction result = TrustAction::kDeny; // Default deny

    for (const auto& [_, rule] : rules_)
    {
        const bool scope_matches = (rule.scope == pack_id) || (rule.scope == "*");
        if (!scope_matches)
        {
            continue;
        }

        for (auto perm : rule.permissions)
        {
            if (perm == permission)
            {
                // Specific pack rules override global
                if (rule.scope == pack_id)
                {
                    return rule.action;
                }
                result = rule.action;
            }
        }
    }

    return result;
}

auto NodeTrustPolicy::is_allowed(const std::string& pack_id, ExecutionPermission permission) const
    -> bool
{
    return check_permission(pack_id, permission) == TrustAction::kAllow;
}

void NodeTrustPolicy::log_access(const std::string& pack_id,
                                 ExecutionPermission permission,
                                 bool granted)
{
    AuditEntry entry;
    entry.pack_id = pack_id;
    entry.permission = permission;
    entry.granted = granted;
    entry.timestamp = std::chrono::steady_clock::now();
    audit_entries_.push_back(std::move(entry));
}

auto NodeTrustPolicy::audit_log() const -> const std::vector<AuditEntry>&
{
    return audit_entries_;
}

auto NodeTrustPolicy::audit_count() const -> std::size_t
{
    return audit_entries_.size();
}

void NodeTrustPolicy::clear_audit_log()
{
    audit_entries_.clear();
}

void NodeTrustPolicy::clear()
{
    rules_.clear();
    audit_entries_.clear();
}

} // namespace markamp::node_editor
