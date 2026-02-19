#include "NodeMigrationEngine.h"

#include <algorithm>

namespace markamp::node_editor
{

void NodeMigrationEngine::register_step(MigrationStep step)
{
    step.step_id = MigrationStepId{next_step_id_++};
    steps_.push_back(std::move(step));
}

auto NodeMigrationEngine::step_count() const -> std::size_t
{
    return steps_.size();
}

auto NodeMigrationEngine::migrate(const std::string& graph_data,
                                  uint32_t from_version,
                                  uint32_t to_version) const -> MigrationResult
{
    MigrationResult result;

    if (from_version == to_version)
    {
        result.success = true;
        result.migrated_data = graph_data;
        return result;
    }

    if (from_version > to_version)
    {
        result.warnings.emplace_back("Downgrade migration is not supported");
        return result;
    }

    if (!can_migrate(from_version, to_version))
    {
        result.warnings.emplace_back("No migration path available");
        return result;
    }

    // Collect applicable steps
    std::vector<const MigrationStep*> applicable;
    for (const auto& step : steps_)
    {
        if (step.from_version >= from_version && step.to_version <= to_version)
        {
            applicable.push_back(&step);
        }
    }

    // Sort by from_version
    std::sort(applicable.begin(),
              applicable.end(),
              [](const MigrationStep* left, const MigrationStep* right)
              { return left->from_version < right->from_version; });

    result.success = true;
    result.migrated_data = graph_data;
    result.steps_applied = applicable.size();

    for (const auto* step : applicable)
    {
        result.warnings.push_back("Applied: " + step->description + " (v" +
                                  std::to_string(step->from_version) + " -> v" +
                                  std::to_string(step->to_version) + ")");
    }

    return result;
}

auto NodeMigrationEngine::can_migrate(uint32_t from_version, uint32_t to_version) const -> bool
{
    if (from_version >= to_version)
    {
        return from_version == to_version;
    }

    // Check if there are steps covering the range
    for (const auto& step : steps_)
    {
        if (step.from_version >= from_version && step.to_version <= to_version)
        {
            return true;
        }
    }
    return false;
}

auto NodeMigrationEngine::migration_path(uint32_t from_version, uint32_t to_version) const
    -> std::vector<std::string>
{
    std::vector<std::string> path;
    for (const auto& step : steps_)
    {
        if (step.from_version >= from_version && step.to_version <= to_version)
        {
            path.push_back(step.description + " (v" + std::to_string(step.from_version) + " -> v" +
                           std::to_string(step.to_version) + ")");
        }
    }
    return path;
}

auto NodeMigrationEngine::current_version() const -> uint32_t
{
    return current_version_;
}

void NodeMigrationEngine::set_current_version(uint32_t version)
{
    current_version_ = version;
}

auto NodeMigrationEngine::check_compatibility(const std::string& /*graph_data*/,
                                              uint32_t graph_version) const -> CompatibilityReport
{
    CompatibilityReport report;
    report.current_version = current_version_;
    report.graph_version = graph_version;

    if (graph_version == current_version_)
    {
        report.compatible = true;
        report.required_steps = 0;
        return report;
    }

    if (graph_version > current_version_)
    {
        report.compatible = false;
        report.step_descriptions.emplace_back("Graph version is newer than runtime");
        return report;
    }

    // Check if migration is possible
    auto path = migration_path(graph_version, current_version_);
    report.compatible = !path.empty();
    report.required_steps = path.size();
    report.step_descriptions = path;

    return report;
}

void NodeMigrationEngine::register_deprecation(const std::string& type_name,
                                               const std::string& replacement,
                                               uint32_t removal_version)
{
    deprecations_.push_back(DeprecationEntry{type_name, replacement, removal_version});
}

auto NodeMigrationEngine::deprecated_types() const -> std::vector<DeprecationEntry>
{
    return deprecations_;
}

auto NodeMigrationEngine::deprecation_count() const -> std::size_t
{
    return deprecations_.size();
}

auto NodeMigrationEngine::is_deprecated(const std::string& type_name) const -> bool
{
    for (const auto& entry : deprecations_)
    {
        if (entry.type_name == type_name)
        {
            return true;
        }
    }
    return false;
}

void NodeMigrationEngine::clear()
{
    steps_.clear();
    deprecations_.clear();
}

} // namespace markamp::node_editor
