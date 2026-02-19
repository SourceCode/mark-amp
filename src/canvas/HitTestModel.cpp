#include "HitTestModel.h"

#include <algorithm>

namespace markamp::canvas
{

void HitTestModel::set_candidates(std::vector<HitCandidate> candidates)
{
    candidates_ = std::move(candidates);
}
auto HitTestModel::candidates() const -> const std::vector<HitCandidate>&
{
    return candidates_;
}

auto HitTestModel::resolve() const -> HitCandidate
{
    if (candidates_.empty())
    {
        return {"", HitTarget::kCanvas, 0.0};
    }

    // Sort by priority (lower enum = higher priority), then by distance
    auto sorted = candidates_;
    std::sort(sorted.begin(),
              sorted.end(),
              [](const HitCandidate& lhs, const HitCandidate& rhs)
              {
                  if (lhs.target_type != rhs.target_type)
                  {
                      return static_cast<uint8_t>(lhs.target_type) <
                             static_cast<uint8_t>(rhs.target_type);
                  }
                  return lhs.distance < rhs.distance;
              });
    return sorted.front();
}

void HitTestModel::set_base_tolerance(double pixels)
{
    base_tolerance_ = std::max(1.0, pixels);
}

auto HitTestModel::tolerance_at_zoom(double zoom) const -> double
{
    if (zoom <= 0.0)
    {
        return base_tolerance_;
    }
    // Scale tolerance inversely with zoom so handles remain usable when zoomed out
    return base_tolerance_ / zoom;
}

void HitTestModel::set_prefetch_candidates(std::vector<std::string> ids)
{
    prefetch_ = std::move(ids);
}
auto HitTestModel::prefetch_candidates() const -> const std::vector<std::string>&
{
    return prefetch_;
}

void HitTestModel::set_latency_ms(double latency_ms)
{
    latency_ms_ = std::max(0.0, latency_ms);
}
auto HitTestModel::latency_ms() const -> double
{
    return latency_ms_;
}
auto HitTestModel::is_within_budget(double budget_ms) const -> bool
{
    return latency_ms_ <= budget_ms;
}

} // namespace markamp::canvas
