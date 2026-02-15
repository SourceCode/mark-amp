#include "AVFilter.h"

#include "AttributeView.h"

namespace markamp::core::av
{

FilterEngine::FilterEngine(const AVColumnTypeRegistry& registry)
    : registry_(registry)
{
}

auto FilterEngine::evaluate_condition(const AVFilterCondition& condition,
                                      const AttributeView& attribute_view,
                                      const std::string& block_id) const -> bool
{
    const auto* value = attribute_view.get_value(condition.key_id, block_id);
    if (!value)
    {
        // No value: empty-check operators return true, others return false
        return condition.op == AVFilterOperator::IsEmpty;
    }

    const auto* key = attribute_view.find_key(condition.key_id);
    if (!key)
    {
        return false;
    }

    return registry_.evaluate_filter(*value, condition.op, condition.operand, key->type);
}

auto FilterEngine::evaluate_group(const AVFilterGroup& group,
                                  const AttributeView& attribute_view,
                                  const std::string& block_id) const -> bool
{
    if (group.conditions.empty())
    {
        return true; // No conditions = pass all
    }

    if (group.logic == AVFilterLogic::And)
    {
        for (const auto& condition : group.conditions)
        {
            if (!evaluate_condition(condition, attribute_view, block_id))
            {
                return false;
            }
        }
        return true;
    }

    // OR logic
    for (const auto& condition : group.conditions)
    {
        if (evaluate_condition(condition, attribute_view, block_id))
        {
            return true;
        }
    }
    return false;
}

auto FilterEngine::filter_rows(const AVFilterGroup& group,
                               const AttributeView& attribute_view) const
    -> std::vector<std::string>
{
    const auto all_rows = attribute_view.row_block_ids();

    if (group.conditions.empty())
    {
        return all_rows;
    }

    std::vector<std::string> result;
    result.reserve(all_rows.size());

    for (const auto& block_id : all_rows)
    {
        if (evaluate_group(group, attribute_view, block_id))
        {
            result.push_back(block_id);
        }
    }

    return result;
}

} // namespace markamp::core::av
