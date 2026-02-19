// V11 Phase 09: Evaluation IR & Execution Planning

#include "node_editor/ExecutionPlan.h"

#include <algorithm>
#include <sstream>
#include <unordered_map>

namespace markamp::node_editor
{

auto ExecutionPlan::build(const EvaluationIR& evaluation_ir) -> ExecutionPlan
{
    ExecutionPlan plan;

    if (!evaluation_ir.is_valid())
    {
        plan.valid_ = false;
        return plan;
    }

    const auto& ir_nodes = evaluation_ir.nodes();
    const auto& ir_links = evaluation_ir.links();

    // Build a map from NodeId -> step index for dependency resolution
    std::unordered_map<NodeId, std::size_t> node_to_step;
    plan.steps_.reserve(ir_nodes.size());

    for (std::size_t idx = 0; idx < ir_nodes.size(); ++idx)
    {
        const auto& ir_node = ir_nodes[idx];

        ExecutionStep step;
        step.node_id = ir_node.node_id;
        step.type_name = ir_node.type_name;

        // Build cache key from type + input slot IDs
        std::ostringstream cache_oss;
        cache_oss << ir_node.type_name;
        for (const auto& sid : ir_node.input_slots)
        {
            cache_oss << ":" << sid.value;
        }
        step.cache_key = cache_oss.str();

        node_to_step[ir_node.node_id] = idx;
        plan.steps_.push_back(std::move(step));
    }

    // Resolve dependency indices from IR links
    for (const auto& link : ir_links)
    {
        const auto target_it = node_to_step.find(link.target_node);
        const auto source_it = node_to_step.find(link.source_node);

        if (target_it != node_to_step.end() && source_it != node_to_step.end())
        {
            auto& target_step = plan.steps_[target_it->second];
            const auto dep_index = source_it->second;

            // Avoid duplicate dependency entries
            const bool already_present =
                std::find(target_step.dependency_indices.begin(),
                          target_step.dependency_indices.end(),
                          dep_index) != target_step.dependency_indices.end();
            if (!already_present)
            {
                target_step.dependency_indices.push_back(dep_index);
            }
        }
    }

    plan.valid_ = true;
    return plan;
}

auto ExecutionPlan::steps() const -> const std::vector<ExecutionStep>&
{
    return steps_;
}

auto ExecutionPlan::step_count() const -> std::size_t
{
    return steps_.size();
}

auto ExecutionPlan::is_valid() const -> bool
{
    return valid_;
}

auto ExecutionPlan::find_step_index(NodeId node_id) const -> int
{
    for (std::size_t idx = 0; idx < steps_.size(); ++idx)
    {
        if (steps_[idx].node_id == node_id)
        {
            return static_cast<int>(idx);
        }
    }
    return -1;
}

auto ExecutionPlan::root_steps() const -> std::vector<std::size_t>
{
    std::vector<std::size_t> roots;
    for (std::size_t idx = 0; idx < steps_.size(); ++idx)
    {
        if (steps_[idx].dependency_indices.empty())
        {
            roots.push_back(idx);
        }
    }
    return roots;
}

auto ExecutionPlan::leaf_steps() const -> std::vector<std::size_t>
{
    // A leaf step is one that no other step depends on
    std::unordered_set<std::size_t> has_dependents;
    for (const auto& step : steps_)
    {
        for (const auto dep_idx : step.dependency_indices)
        {
            has_dependents.insert(dep_idx);
        }
    }

    std::vector<std::size_t> leaves;
    for (std::size_t idx = 0; idx < steps_.size(); ++idx)
    {
        if (!has_dependents.contains(idx))
        {
            leaves.push_back(idx);
        }
    }
    return leaves;
}

} // namespace markamp::node_editor
