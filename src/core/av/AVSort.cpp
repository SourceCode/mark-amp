#include "AVSort.h"

#include "AttributeView.h"

#include <algorithm>

namespace markamp::core::av
{

SortEngine::SortEngine(const AVColumnTypeRegistry& registry, bool empty_values_last)
    : registry_(registry)
    , empty_values_last_(empty_values_last)
{
}

void SortEngine::sort_rows(std::vector<std::string>& block_ids,
                           const std::vector<AVSortSpec>& sort_specs,
                           const AttributeView& attribute_view) const
{
    if (sort_specs.empty())
    {
        return;
    }

    std::sort(block_ids.begin(),
              block_ids.end(),
              [this, &sort_specs, &attribute_view](const std::string& id_a,
                                                   const std::string& id_b) -> bool
              {
                  for (const auto& spec : sort_specs)
                  {
                      const auto* val_a = attribute_view.get_value(spec.key_id, id_a);
                      const auto* val_b = attribute_view.get_value(spec.key_id, id_b);

                      const bool a_empty = !val_a || val_a->is_empty();
                      const bool b_empty = !val_b || val_b->is_empty();

                      if (a_empty && b_empty)
                      {
                          continue;
                      }
                      if (a_empty)
                      {
                          return !empty_values_last_;
                      }
                      if (b_empty)
                      {
                          return empty_values_last_;
                      }

                      const auto* key = attribute_view.find_key(spec.key_id);
                      if (!key)
                      {
                          continue;
                      }

                      const int cmp = registry_.compare_values(*val_a, *val_b, key->type);
                      if (cmp != 0)
                      {
                          return spec.order == AVSortOrder::Ascending ? cmp < 0 : cmp > 0;
                      }
                  }
                  return false;
              });
}

} // namespace markamp::core::av
