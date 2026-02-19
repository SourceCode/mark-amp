#include "ReleaseAuditModel.h"

namespace markamp::ui
{

void ReleaseAuditModel::set_checks(std::vector<AuditCheck> checks)
{
    checks_ = std::move(checks);
}
auto ReleaseAuditModel::checks() const -> const std::vector<AuditCheck>&
{
    return checks_;
}

auto ReleaseAuditModel::by_category(const std::string& category) const -> std::vector<AuditCheck>
{
    std::vector<AuditCheck> result;
    for (const auto& check : checks_)
    {
        if (check.category == category)
        {
            result.push_back(check);
        }
    }
    return result;
}

auto ReleaseAuditModel::pass_count() const -> int
{
    int count = 0;
    for (const auto& check : checks_)
    {
        if (check.status == AuditStatus::kPass)
        {
            ++count;
        }
    }
    return count;
}

auto ReleaseAuditModel::fail_count() const -> int
{
    int count = 0;
    for (const auto& check : checks_)
    {
        if (check.status == AuditStatus::kFail)
        {
            ++count;
        }
    }
    return count;
}

auto ReleaseAuditModel::skip_count() const -> int
{
    int count = 0;
    for (const auto& check : checks_)
    {
        if (check.status == AuditStatus::kSkipped)
        {
            ++count;
        }
    }
    return count;
}

void ReleaseAuditModel::set_gates(std::vector<ReleaseGate> gates)
{
    gates_ = std::move(gates);
}
auto ReleaseAuditModel::gates() const -> const std::vector<ReleaseGate>&
{
    return gates_;
}

auto ReleaseAuditModel::all_required_gates_passing() const -> bool
{
    for (const auto& gate : gates_)
    {
        if (gate.is_required && !gate.is_passing)
        {
            return false;
        }
    }
    return true;
}

auto ReleaseAuditModel::failing_gates() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& gate : gates_)
    {
        if (gate.is_required && !gate.is_passing)
        {
            result.push_back(gate.name);
        }
    }
    return result;
}

auto ReleaseAuditModel::is_release_ready() const -> bool
{
    return fail_count() == 0 && all_required_gates_passing();
}

} // namespace markamp::ui
