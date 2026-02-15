#pragma once

#include "AVColumnType.h"
#include "AVFilter.h"
#include "AVSort.h"

#include <string>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Query Result
// ════════════════════════════════════════════════════════════

struct AVQueryResult
{
    std::vector<std::string> block_ids;
    size_t total_unfiltered{0};
    size_t total_filtered{0};
    int page{1};
    int page_size{50};
    int total_pages{1};
};

// ════════════════════════════════════════════════════════════
// Query Engine: filter → sort → paginate pipeline
// ════════════════════════════════════════════════════════════

struct AttributeView;

class AVQueryEngine
{
public:
    explicit AVQueryEngine(const AVColumnTypeRegistry& registry);

    /// Run the full pipeline: filter → sort → paginate.
    [[nodiscard]] auto execute(const AttributeView& attribute_view,
                               const AVFilterGroup& filters,
                               const std::vector<AVSortSpec>& sort_specs,
                               int page = 1,
                               int page_size = 50) const -> AVQueryResult;

    /// Filter only (no sort/paginate).
    [[nodiscard]] auto filter_only(const AttributeView& attribute_view,
                                   const AVFilterGroup& filters) const -> std::vector<std::string>;

    /// Sort only (no filter/paginate).
    void sort_only(std::vector<std::string>& block_ids,
                   const std::vector<AVSortSpec>& sort_specs,
                   const AttributeView& attribute_view) const;

private:
    const AVColumnTypeRegistry& registry_;
    FilterEngine filter_engine_;
    SortEngine sort_engine_;
};

} // namespace markamp::core::av
