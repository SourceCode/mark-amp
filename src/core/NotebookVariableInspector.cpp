/// @file NotebookVariableInspector.cpp
/// @brief V24 P08-T04: NotebookVariableInspector implementation.
#include "NotebookVariableInspector.h"

#include <algorithm>

namespace markamp::core
{

void NotebookVariableInspector::set_variable(InspectedVariable var)
{
    auto name = var.name;
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        insertion_order_.push_back(name);
    }
    variables_[name] = std::move(var);
}

auto NotebookVariableInspector::remove_variable(const std::string& name) -> bool
{
    auto it = variables_.find(name);
    if (it == variables_.end()) return false;
    variables_.erase(it);
    insertion_order_.erase(
        std::remove(insertion_order_.begin(), insertion_order_.end(), name),
        insertion_order_.end());
    return true;
}

auto NotebookVariableInspector::get_variable(const std::string& name) const
    -> const InspectedVariable*
{
    auto it = variables_.find(name);
    return it != variables_.end() ? &it->second : nullptr;
}

auto NotebookVariableInspector::all_variables() const
    -> std::vector<const InspectedVariable*>
{
    std::vector<const InspectedVariable*> result;
    result.reserve(insertion_order_.size());
    for (const auto& name : insertion_order_) {
        auto it = variables_.find(name);
        if (it != variables_.end()) result.push_back(&it->second);
    }
    return result;
}

auto NotebookVariableInspector::variables_by_type(VariableType type) const
    -> std::vector<const InspectedVariable*>
{
    std::vector<const InspectedVariable*> result;
    for (const auto& name : insertion_order_) {
        auto it = variables_.find(name);
        if (it != variables_.end() && it->second.type == type) {
            result.push_back(&it->second);
        }
    }
    return result;
}

auto NotebookVariableInspector::variables_for_cell(int cell_index) const
    -> std::vector<const InspectedVariable*>
{
    std::vector<const InspectedVariable*> result;
    for (const auto& name : insertion_order_) {
        auto it = variables_.find(name);
        if (it != variables_.end() && it->second.cell_index == cell_index) {
            result.push_back(&it->second);
        }
    }
    return result;
}

void NotebookVariableInspector::mark_cell_stale(int cell_index)
{
    for (auto& [name, var] : variables_) {
        if (var.cell_index == cell_index) var.is_stale = true;
    }
}

void NotebookVariableInspector::clear()
{
    variables_.clear();
    insertion_order_.clear();
}

} // namespace markamp::core
