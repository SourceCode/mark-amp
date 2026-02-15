#include "AVQueryEngine.h"

#include "AttributeView.h"

#include <algorithm>

namespace markamp::core::av
{

AVQueryEngine::AVQueryEngine(const AVColumnTypeRegistry& registry)
    : registry_(registry)
    , filter_engine_(registry_)
    , sort_engine_(registry_)
{
}

auto AVQueryEngine::execute(const AttributeView& attribute_view,
                            const AVFilterGroup& filters,
                            const std::vector<AVSortSpec>& sort_specs,
                            int page,
                            int page_size) const -> AVQueryResult
{
    AVQueryResult result;
    result.total_unfiltered = attribute_view.row_count();

    // Step 1: Filter
    auto filtered = filter_engine_.filter_rows(filters, attribute_view);
    result.total_filtered = filtered.size();

    // Step 2: Sort
    sort_engine_.sort_rows(filtered, sort_specs, attribute_view);

    // Step 3: Paginate
    result.page_size = std::max(1, page_size);
    result.total_pages = std::max(
        1,
        static_cast<int>((result.total_filtered + static_cast<size_t>(result.page_size) - 1) /
                         static_cast<size_t>(result.page_size)));
    result.page = std::clamp(page, 1, result.total_pages);

    const auto start_idx = static_cast<size_t>((result.page - 1) * result.page_size);
    const auto end_idx =
        std::min(start_idx + static_cast<size_t>(result.page_size), filtered.size());

    if (start_idx < filtered.size())
    {
        result.block_ids.assign(filtered.begin() + static_cast<ptrdiff_t>(start_idx),
                                filtered.begin() + static_cast<ptrdiff_t>(end_idx));
    }

    return result;
}

auto AVQueryEngine::filter_only(const AttributeView& attribute_view,
                                const AVFilterGroup& filters) const -> std::vector<std::string>
{
    return filter_engine_.filter_rows(filters, attribute_view);
}

void AVQueryEngine::sort_only(std::vector<std::string>& block_ids,
                              const std::vector<AVSortSpec>& sort_specs,
                              const AttributeView& attribute_view) const
{
    sort_engine_.sort_rows(block_ids, sort_specs, attribute_view);
}

} // namespace markamp::core::av
