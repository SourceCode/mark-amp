#include "AuditModel.h"

#include <algorithm>

namespace markamp::canvas
{

void AuditModel::set_gates(std::vector<AuditGate> gates)
{
    gates_ = std::move(gates);
}
auto AuditModel::gates() const -> const std::vector<AuditGate>&
{
    return gates_;
}

void AuditModel::update_gate(const std::string& gate_id,
                             GateStatus status,
                             const std::string& notes)
{
    for (auto& gate : gates_)
    {
        if (gate.gate_id == gate_id)
        {
            gate.status = status;
            gate.notes = notes;
            break;
        }
    }
}

auto AuditModel::gates_in_category(const std::string& category) const -> std::vector<AuditGate>
{
    std::vector<AuditGate> result;
    for (const auto& gate : gates_)
    {
        if (gate.category == category)
        {
            result.push_back(gate);
        }
    }
    return result;
}

auto AuditModel::passed_count() const -> int
{
    return static_cast<int>(std::count_if(gates_.begin(),
                                          gates_.end(),
                                          [](const AuditGate& audit_gate)
                                          { return audit_gate.status == GateStatus::kPassed; }));
}

auto AuditModel::failed_count() const -> int
{
    return static_cast<int>(std::count_if(gates_.begin(),
                                          gates_.end(),
                                          [](const AuditGate& audit_gate)
                                          { return audit_gate.status == GateStatus::kFailed; }));
}

auto AuditModel::not_run_count() const -> int
{
    return static_cast<int>(std::count_if(gates_.begin(),
                                          gates_.end(),
                                          [](const AuditGate& audit_gate)
                                          { return audit_gate.status == GateStatus::kNotRun; }));
}

auto AuditModel::all_passed() const -> bool
{
    return failed_count() == 0 && not_run_count() == 0;
}
auto AuditModel::is_release_ready() const -> bool
{
    return all_passed() && !gates_.empty();
}

} // namespace markamp::canvas
