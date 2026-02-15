#pragma once

#include "AVColumnType.h"
#include "AVTypes.h"

#include <string>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Sort Specification
// ════════════════════════════════════════════════════════════

struct AVSortSpec
{
    std::string key_id;
    AVSortOrder order{AVSortOrder::Ascending};
};

// ════════════════════════════════════════════════════════════
// Sort Engine
// ════════════════════════════════════════════════════════════

struct AttributeView;

class SortEngine
{
public:
    explicit SortEngine(const AVColumnTypeRegistry& registry, bool empty_values_last = true);

    /// Sort block IDs according to sort specs.
    void sort_rows(std::vector<std::string>& block_ids,
                   const std::vector<AVSortSpec>& sort_specs,
                   const AttributeView& attribute_view) const;

private:
    const AVColumnTypeRegistry& registry_;
    bool empty_values_last_;
};

} // namespace markamp::core::av
