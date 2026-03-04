#include "ControlSnapshotTester.h"

namespace markamp::ui
{

auto ControlSnapshot::state_name() const -> std::string
{
    switch (state)
    {
        case ControlVisualState::kDefault:
            return "default";
        case ControlVisualState::kHover:
            return "hover";
        case ControlVisualState::kFocus:
            return "focus";
        case ControlVisualState::kPressed:
            return "pressed";
        case ControlVisualState::kDisabled:
            return "disabled";
    }
    return "unknown";
}

void ControlSnapshotTester::register_baseline(const SnapshotBaseline& baseline)
{
    baselines_.push_back(baseline);
}

auto ControlSnapshotTester::baseline_count() const -> int
{
    return static_cast<int>(baselines_.size());
}

auto ControlSnapshotTester::find_baseline(const std::string& control_id,
                                          ControlVisualState state) const -> const SnapshotBaseline*
{
    for (const auto& baseline : baselines_)
    {
        if (baseline.control_id == control_id && baseline.state == state)
        {
            return &baseline;
        }
    }
    return nullptr;
}

auto ControlSnapshotTester::compare(const ControlSnapshot& snapshot) const -> bool
{
    const auto* baseline = find_baseline(snapshot.control_id, snapshot.state);
    if (baseline == nullptr)
    {
        return false; // No baseline = fail
    }
    return baseline->expected_hash == snapshot.hash;
}

auto ControlSnapshotTester::validate_all(const std::vector<ControlSnapshot>& snapshots) const -> int
{
    int pass_count = 0;
    for (const auto& snapshot : snapshots)
    {
        if (compare(snapshot))
        {
            ++pass_count;
        }
    }
    return pass_count;
}

auto ControlSnapshotTester::failures(const std::vector<ControlSnapshot>& snapshots) const
    -> std::vector<ControlSnapshot>
{
    std::vector<ControlSnapshot> failed;
    for (const auto& snapshot : snapshots)
    {
        if (!compare(snapshot))
        {
            failed.push_back(snapshot);
        }
    }
    return failed;
}

void ControlSnapshotTester::update_baseline(const std::string& control_id,
                                            ControlVisualState state,
                                            const std::string& new_hash)
{
    for (auto& baseline : baselines_)
    {
        if (baseline.control_id == control_id && baseline.state == state)
        {
            baseline.expected_hash = new_hash;
            return;
        }
    }
    // If not found, add as new
    baselines_.push_back({control_id, state, new_hash});
}

void ControlSnapshotTester::clear()
{
    baselines_.clear();
}

} // namespace markamp::ui
