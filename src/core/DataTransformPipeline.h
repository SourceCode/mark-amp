/// @file DataTransformPipeline.h
/// @brief V9 Phase 48 — Multi-step data transformation pipeline.
#pragma once

#include "DataTableEngine.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Type of transform operation.
enum class TransformType : uint8_t
{
    kMap = 0,
    kFilter = 1,
    kSort = 2,
    kAggregate = 3,
    kJoin = 4,
};

/// A single step in a transform pipeline.
struct TransformStep
{
    TransformType type{TransformType::kFilter};
    std::string column;
    std::string expression; ///< Value to match (filter) or map expression
    bool ascending{true};   ///< For sort steps
};

/// Executes a sequence of transforms on DataRows.
class DataTransformPipeline
{
public:
    DataTransformPipeline() = default;

    // ── Pipeline construction ─────────────────────────────────────────
    void add_step(TransformStep step);
    auto remove_step(int index) -> bool;
    void clear_steps();

    // ── Execution ─────────────────────────────────────────────────────
    [[nodiscard]] auto execute(const std::vector<DataRow>& input_rows,
                               const std::vector<ColumnDef>& columns) const -> std::vector<DataRow>;

    // ── Validation ────────────────────────────────────────────────────
    [[nodiscard]] auto validate(const std::vector<ColumnDef>& columns) const -> bool;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto step_count() const -> int;
    [[nodiscard]] auto get_steps() const -> const std::vector<TransformStep>&;

private:
    std::vector<TransformStep> steps_;

    [[nodiscard]] static auto find_column(const std::vector<ColumnDef>& columns,
                                          const std::string& name) -> int;
};

} // namespace markamp::core
