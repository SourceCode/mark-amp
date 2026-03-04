#include "ReleaseGateChecker.h"

namespace markamp::ui
{

auto ReleaseGate::status_name() const -> std::string
{
    switch (status)
    {
        case GateStatus::kPass:
            return "pass";
        case GateStatus::kFail:
            return "fail";
        case GateStatus::kBlocked:
            return "blocked";
        case GateStatus::kPending:
            return "pending";
    }
    return "unknown";
}

auto ReleaseGate::is_blocking() const -> bool
{
    return status == GateStatus::kFail || status == GateStatus::kBlocked;
}

void ReleaseGateChecker::add_gate(const ReleaseGate& gate)
{
    gates_.push_back(gate);
}

void ReleaseGateChecker::set_status(const std::string& gate_id,
                                    GateStatus status,
                                    const std::string& notes)
{
    for (auto& gate : gates_)
    {
        if (gate.gate_id == gate_id)
        {
            gate.status = status;
            if (!notes.empty())
            {
                gate.notes = notes;
            }
            return;
        }
    }
}

auto ReleaseGateChecker::find_gate(const std::string& gate_id) const -> const ReleaseGate*
{
    for (const auto& gate : gates_)
    {
        if (gate.gate_id == gate_id)
        {
            return &gate;
        }
    }
    return nullptr;
}

auto ReleaseGateChecker::all_gates() const -> const std::vector<ReleaseGate>&
{
    return gates_;
}

auto ReleaseGateChecker::gate_count() const -> int
{
    return static_cast<int>(gates_.size());
}

auto ReleaseGateChecker::is_go() const -> bool
{
    if (gates_.empty())
    {
        return false;
    }
    for (const auto& gate : gates_)
    {
        if (gate.status != GateStatus::kPass)
        {
            return false;
        }
    }
    return true;
}

auto ReleaseGateChecker::blocking_count() const -> int
{
    int count = 0;
    for (const auto& gate : gates_)
    {
        if (gate.is_blocking())
        {
            ++count;
        }
    }
    return count;
}

auto ReleaseGateChecker::blocking_gates() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& gate : gates_)
    {
        if (gate.is_blocking())
        {
            result.push_back(gate.gate_id);
        }
    }
    return result;
}

auto ReleaseGateChecker::pass_count() const -> int
{
    int count = 0;
    for (const auto& gate : gates_)
    {
        if (gate.status == GateStatus::kPass)
        {
            ++count;
        }
    }
    return count;
}

auto ReleaseGateChecker::pending_count() const -> int
{
    int count = 0;
    for (const auto& gate : gates_)
    {
        if (gate.status == GateStatus::kPending)
        {
            ++count;
        }
    }
    return count;
}

void ReleaseGateChecker::register_standard_gates()
{
    add_gate({"interaction",
              "Interaction Quality",
              "All primary controls pass interaction checklist",
              GateStatus::kPending,
              ""});
    add_gate({"performance",
              "Performance Budget",
              "Control rendering within latency budget",
              GateStatus::kPending,
              ""});
    add_gate({"accessibility",
              "Accessibility Compliance",
              "WCAG AA compliance for all controls",
              GateStatus::kPending,
              ""});
    add_gate({"regression",
              "Regression Status",
              "No control UX regressions from baseline",
              GateStatus::kPending,
              ""});
    add_gate({"platform",
              "Platform Conformance",
              "Platform-specific behaviors verified",
              GateStatus::kPending,
              ""});
}

} // namespace markamp::ui
