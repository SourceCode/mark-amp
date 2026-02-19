#include "LargeBoardModel.h"

#include <algorithm>

namespace markamp::canvas
{

void LargeBoardModel::set_total_objects(int count)
{
    total_objects_ = std::max(0, count);
}
auto LargeBoardModel::total_objects() const -> int
{
    return total_objects_;
}

void LargeBoardModel::set_visible_count(int count)
{
    visible_count_ = std::clamp(count, 0, total_objects_);
}
auto LargeBoardModel::visible_count() const -> int
{
    return visible_count_;
}
auto LargeBoardModel::culled_count() const -> int
{
    return total_objects_ - visible_count_;
}

auto LargeBoardModel::lod_for_zoom(double zoom) const -> LodLevel
{
    if (zoom >= 0.5)
    {
        return LodLevel::kFull;
    }
    if (zoom >= 0.15)
    {
        return LodLevel::kSimplified;
    }
    return LodLevel::kBoundingBox;
}

void LargeBoardModel::record_cache_hit()
{
    ++cache_hits_;
}
void LargeBoardModel::record_cache_miss()
{
    ++cache_misses_;
}
void LargeBoardModel::record_cache_eviction()
{
    ++cache_evictions_;
}
auto LargeBoardModel::cache_hits() const -> int
{
    return cache_hits_;
}
auto LargeBoardModel::cache_misses() const -> int
{
    return cache_misses_;
}
auto LargeBoardModel::cache_evictions() const -> int
{
    return cache_evictions_;
}

auto LargeBoardModel::cache_hit_rate() const -> double
{
    const int total = cache_hits_ + cache_misses_;
    if (total == 0)
    {
        return 0.0;
    }
    return static_cast<double>(cache_hits_) / static_cast<double>(total);
}

void LargeBoardModel::set_telemetry(RenderTelemetry telemetry)
{
    telemetry_ = telemetry;
}
auto LargeBoardModel::telemetry() const -> const RenderTelemetry&
{
    return telemetry_;
}

} // namespace markamp::canvas
