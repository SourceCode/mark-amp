/// @file CrossModuleTestSuite.cpp
/// @brief V9 Phase 49 — CrossModuleTestSuite implementation.

#include "CrossModuleTestSuite.h"

namespace markamp::core
{

void CrossModuleTestSuite::register_module(const std::string& module_name)
{
    modules_.insert(module_name);
}

auto CrossModuleTestSuite::has_module(const std::string& module_name) const -> bool
{
    return modules_.find(module_name) != modules_.end();
}

void CrossModuleTestSuite::add_dependency(const std::string& from, const std::string& to_module)
{
    ModuleDependency dep;
    dep.from_module = from;
    dep.to_module = to_module;
    dep.is_satisfied = has_module(from) && has_module(to_module);
    dependencies_.push_back(std::move(dep));
}

auto CrossModuleTestSuite::check_dependencies() const -> std::vector<ModuleDependency>
{
    std::vector<ModuleDependency> results;
    results.reserve(dependencies_.size());
    for (const auto& dep : dependencies_)
    {
        ModuleDependency checked = dep;
        checked.is_satisfied = has_module(dep.from_module) && has_module(dep.to_module);
        results.push_back(std::move(checked));
    }
    return results;
}

auto CrossModuleTestSuite::run_compatibility_check() const -> std::vector<CompatibilityResult>
{
    std::vector<CompatibilityResult> results;
    for (const auto& module_name : modules_)
    {
        CompatibilityResult result;
        result.module_name = module_name;
        result.compatible = true;
        result.message = "OK";

        // Check if all dependencies are satisfied
        for (const auto& dep : dependencies_)
        {
            if (dep.from_module == module_name && !has_module(dep.to_module))
            {
                result.compatible = false;
                result.message = "Missing dependency: " + dep.to_module;
                break;
            }
        }
        results.push_back(std::move(result));
    }
    return results;
}

auto CrossModuleTestSuite::module_count() const -> int
{
    return static_cast<int>(modules_.size());
}

auto CrossModuleTestSuite::dependency_count() const -> int
{
    return static_cast<int>(dependencies_.size());
}

void CrossModuleTestSuite::clear()
{
    modules_.clear();
    dependencies_.clear();
}

} // namespace markamp::core
