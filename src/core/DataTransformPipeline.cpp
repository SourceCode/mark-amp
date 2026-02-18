/// @file DataTransformPipeline.cpp
/// @brief V9 Phase 48 — DataTransformPipeline implementation.

#include "DataTransformPipeline.h"

#include <algorithm>

namespace markamp::core
{

void DataTransformPipeline::add_step(TransformStep step)
{
    steps_.push_back(std::move(step));
}

auto DataTransformPipeline::remove_step(int index) -> bool
{
    if (index < 0 || index >= static_cast<int>(steps_.size()))
    {
        return false;
    }
    steps_.erase(steps_.begin() + index);
    return true;
}

void DataTransformPipeline::clear_steps()
{
    steps_.clear();
}

auto DataTransformPipeline::execute(const std::vector<DataRow>& input_rows,
                                    const std::vector<ColumnDef>& columns) const
    -> std::vector<DataRow>
{
    std::vector<DataRow> result = input_rows;

    for (const auto& step : steps_)
    {
        int col_idx = find_column(columns, step.column);
        if (col_idx < 0)
        {
            continue;
        }
        auto col_index = static_cast<size_t>(col_idx);

        switch (step.type)
        {
            case TransformType::kFilter:
            {
                std::vector<DataRow> filtered;
                for (const auto& row : result)
                {
                    if (col_index < row.cells.size() && row.cells[col_index] == step.expression)
                    {
                        filtered.push_back(row);
                    }
                }
                result = std::move(filtered);
                break;
            }
            case TransformType::kSort:
            {
                std::sort(result.begin(),
                          result.end(),
                          [col_index, &step](const DataRow& row_a, const DataRow& row_b)
                          {
                              const auto& val_a =
                                  col_index < row_a.cells.size() ? row_a.cells[col_index] : "";
                              const auto& val_b =
                                  col_index < row_b.cells.size() ? row_b.cells[col_index] : "";
                              return step.ascending ? val_a < val_b : val_a > val_b;
                          });
                break;
            }
            case TransformType::kMap:
            {
                // Map: replace column values with expression (simple placeholder)
                for (auto& row : result)
                {
                    if (col_index < row.cells.size())
                    {
                        row.cells[col_index] = step.expression;
                    }
                }
                break;
            }
            case TransformType::kAggregate:
            case TransformType::kJoin:
                // Reserved for future implementation
                break;
        }
    }

    return result;
}

auto DataTransformPipeline::validate(const std::vector<ColumnDef>& columns) const -> bool
{
    for (const auto& step : steps_)
    {
        if (find_column(columns, step.column) < 0)
        {
            return false;
        }
    }
    return true;
}

auto DataTransformPipeline::step_count() const -> int
{
    return static_cast<int>(steps_.size());
}

auto DataTransformPipeline::get_steps() const -> const std::vector<TransformStep>&
{
    return steps_;
}

auto DataTransformPipeline::find_column(const std::vector<ColumnDef>& columns,
                                        const std::string& name) -> int
{
    for (size_t idx = 0; idx < columns.size(); ++idx)
    {
        if (columns[idx].name == name)
        {
            return static_cast<int>(idx);
        }
    }
    return -1;
}

} // namespace markamp::core
