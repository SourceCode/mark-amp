#include "RegressionHarnessModel.h"

namespace markamp::ui
{

void RegressionHarnessModel::set_baselines(std::vector<SnapshotBaseline> baselines)
{
    baselines_ = std::move(baselines);
}

auto RegressionHarnessModel::baselines() const -> const std::vector<SnapshotBaseline>&
{
    return baselines_;
}

auto RegressionHarnessModel::is_snapshot_current(const std::string& control_id,
                                                 ControlState state,
                                                 const std::string& current_hash) const -> bool
{
    for (const auto& baseline : baselines_)
    {
        if (baseline.control_id == control_id && baseline.state == state)
        {
            return baseline.hash == current_hash;
        }
    }
    return false; // No baseline found
}

auto RegressionHarnessModel::stale_baselines() const -> std::vector<SnapshotBaseline>
{
    std::vector<SnapshotBaseline> result;
    for (const auto& baseline : baselines_)
    {
        if (!baseline.is_current)
        {
            result.push_back(baseline);
        }
    }
    return result;
}

void RegressionHarnessModel::set_keyboard_steps(std::vector<KeyboardStep> steps)
{
    keyboard_steps_ = std::move(steps);
}

auto RegressionHarnessModel::keyboard_steps() const -> const std::vector<KeyboardStep>&
{
    return keyboard_steps_;
}

void RegressionHarnessModel::mark_step_passed(const std::string& step_id)
{
    for (auto& step : keyboard_steps_)
    {
        if (step.step_id == step_id)
        {
            step.passed = true;
            return;
        }
    }
}

auto RegressionHarnessModel::all_keyboard_steps_passed() const -> bool
{
    if (keyboard_steps_.empty())
    {
        return true;
    }
    for (const auto& step : keyboard_steps_)
    {
        if (!step.passed)
        {
            return false;
        }
    }
    return true;
}

auto RegressionHarnessModel::keyboard_pass_rate() const -> double
{
    if (keyboard_steps_.empty())
    {
        return 1.0;
    }
    int passed = 0;
    for (const auto& step : keyboard_steps_)
    {
        if (step.passed)
        {
            ++passed;
        }
    }
    return static_cast<double>(passed) / static_cast<double>(keyboard_steps_.size());
}

void RegressionHarnessModel::set_ci_gate_passing(bool passing)
{
    ci_gate_passing_ = passing;
}
auto RegressionHarnessModel::ci_gate_passing() const -> bool
{
    return ci_gate_passing_;
}

} // namespace markamp::ui
