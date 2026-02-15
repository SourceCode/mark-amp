#pragma once

#include "AVColumnType.h"
#include "AVTypes.h"
#include "AVValue.h"

#include <string>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Filter Condition
// ════════════════════════════════════════════════════════════

struct AVFilterCondition
{
    std::string key_id;
    AVFilterOperator op{AVFilterOperator::IsNotEmpty};
    AVValue operand;
};

// ════════════════════════════════════════════════════════════
// Filter Group (compound logic)
// ════════════════════════════════════════════════════════════

enum class AVFilterLogic : std::uint8_t
{
    And,
    Or
};

struct AVFilterGroup
{
    AVFilterLogic logic{AVFilterLogic::And};
    std::vector<AVFilterCondition> conditions;
};

// ════════════════════════════════════════════════════════════
// Filter Engine
// ════════════════════════════════════════════════════════════

struct AttributeView;

class FilterEngine
{
public:
    explicit FilterEngine(const AVColumnTypeRegistry& registry);

    /// Evaluate a single filter condition against a row in the AV.
    [[nodiscard]] auto evaluate_condition(const AVFilterCondition& condition,
                                          const AttributeView& attribute_view,
                                          const std::string& block_id) const -> bool;

    /// Evaluate a filter group (AND/OR) against a row.
    [[nodiscard]] auto evaluate_group(const AVFilterGroup& group,
                                      const AttributeView& attribute_view,
                                      const std::string& block_id) const -> bool;

    /// Filter rows, returning block IDs that pass.
    [[nodiscard]] auto filter_rows(const AVFilterGroup& group,
                                   const AttributeView& attribute_view) const
        -> std::vector<std::string>;

private:
    const AVColumnTypeRegistry& registry_;
};

} // namespace markamp::core::av
