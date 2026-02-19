#include "NodeRolloutController.h"

#include <algorithm>
#include <numeric>
#include <set>

namespace markamp::node_editor
{

auto NodeRolloutController::add_stage(RolloutStage stage) -> StageId
{
    StageId sid{next_stage_id_++};
    stage.stage_id = sid;
    stages_.push_back(std::move(stage));
    return sid;
}

auto NodeRolloutController::remove_stage(StageId stage_id) -> bool
{
    auto iter = std::find_if(stages_.begin(),
                             stages_.end(),
                             [&](const RolloutStage& stg) { return stg.stage_id == stage_id; });
    if (iter == stages_.end())
    {
        return false;
    }

    auto removed_index = static_cast<std::size_t>(std::distance(stages_.begin(), iter));
    stages_.erase(iter);

    // Adjust current stage index if needed
    if (current_stage_index_ >= stages_.size() && !stages_.empty())
    {
        current_stage_index_ = stages_.size() - 1;
    }
    else if (current_stage_index_ > removed_index && current_stage_index_ > 0)
    {
        --current_stage_index_;
    }

    return true;
}

auto NodeRolloutController::current_stage() const -> const RolloutStage*
{
    if (stages_.empty() || current_stage_index_ >= stages_.size())
    {
        return nullptr;
    }
    return &stages_[current_stage_index_];
}

auto NodeRolloutController::advance_stage() -> bool
{
    if (current_stage_index_ + 1 >= stages_.size())
    {
        return false;
    }
    ++current_stage_index_;
    return true;
}

auto NodeRolloutController::rollback_stage() -> bool
{
    if (current_stage_index_ == 0)
    {
        return false;
    }
    --current_stage_index_;
    return true;
}

auto NodeRolloutController::stage_count() const -> std::size_t
{
    return stages_.size();
}

auto NodeRolloutController::is_feature_enabled(const std::string& feature_name) const -> bool
{
    // Check extra enabled features first
    for (const auto& feat : extra_enabled_features_)
    {
        if (feat == feature_name)
        {
            return true;
        }
    }

    // Check current stage
    const auto* stage = current_stage();
    if (stage == nullptr)
    {
        return false;
    }
    for (const auto& feat : stage->enabled_features)
    {
        if (feat == feature_name)
        {
            return true;
        }
    }
    return false;
}

void NodeRolloutController::enable_feature(const std::string& feature_name)
{
    // Avoid duplicates
    for (const auto& feat : extra_enabled_features_)
    {
        if (feat == feature_name)
        {
            return;
        }
    }
    extra_enabled_features_.push_back(feature_name);
}

void NodeRolloutController::disable_feature(const std::string& feature_name)
{
    extra_enabled_features_.erase(
        std::remove(extra_enabled_features_.begin(), extra_enabled_features_.end(), feature_name),
        extra_enabled_features_.end());
}

auto NodeRolloutController::enabled_features() const -> std::vector<std::string>
{
    std::set<std::string> features(extra_enabled_features_.begin(), extra_enabled_features_.end());

    const auto* stage = current_stage();
    if (stage != nullptr)
    {
        for (const auto& feat : stage->enabled_features)
        {
            features.insert(feat);
        }
    }

    return {features.begin(), features.end()};
}

void NodeRolloutController::report_health(const std::string& signal_name, float value)
{
    HealthSignal signal;
    signal.name = signal_name;
    signal.value = value;
    signal.timestamp = std::chrono::steady_clock::now();
    health_signals_.push_back(std::move(signal));
}

auto NodeRolloutController::health_score() const -> float
{
    if (health_signals_.empty())
    {
        return 1.0F; // Healthy by default
    }

    float sum = 0.0F;
    for (const auto& signal : health_signals_)
    {
        sum += signal.value;
    }
    return sum / static_cast<float>(health_signals_.size());
}

auto NodeRolloutController::health_signals() const -> const std::vector<HealthSignal>&
{
    return health_signals_;
}

auto NodeRolloutController::is_healthy() const -> bool
{
    const auto* stage = current_stage();
    const float threshold = (stage != nullptr) ? stage->health_threshold : 0.9F;
    return health_score() >= threshold;
}

auto NodeRolloutController::run_validation_suite() const -> ValidationReport
{
    ValidationReport report;

    // Check 1: At least one stage defined
    report.total_checks++;
    if (!stages_.empty())
    {
        report.passed++;
        report.details.emplace_back("PASS: Stages defined (" + std::to_string(stages_.size()) +
                                    ")");
    }
    else
    {
        report.failed++;
        report.details.emplace_back("FAIL: No stages defined");
    }

    // Check 2: Current stage is valid
    report.total_checks++;
    if (current_stage() != nullptr)
    {
        report.passed++;
        report.details.emplace_back("PASS: Current stage is valid");
    }
    else
    {
        report.failed++;
        report.details.emplace_back("FAIL: No valid current stage");
    }

    // Check 3: Health is above threshold
    report.total_checks++;
    if (is_healthy())
    {
        report.passed++;
        report.details.emplace_back("PASS: Health score is acceptable");
    }
    else
    {
        report.failed++;
        report.details.emplace_back("FAIL: Health score below threshold (" +
                                    std::to_string(health_score()) + ")");
    }

    // Check 4: Health signals reported
    report.total_checks++;
    if (!health_signals_.empty())
    {
        report.passed++;
        report.details.emplace_back("PASS: Health signals are being reported");
    }
    else
    {
        report.failed++;
        report.details.emplace_back("FAIL: No health signals reported");
    }

    return report;
}

void NodeRolloutController::clear()
{
    stages_.clear();
    current_stage_index_ = 0;
    extra_enabled_features_.clear();
    health_signals_.clear();
}

} // namespace markamp::node_editor
