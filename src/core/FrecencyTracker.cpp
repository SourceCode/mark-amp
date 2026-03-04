/// @file FrecencyTracker.cpp
/// @brief V13 Phase 31 Task 2 — Frecency algorithm implementation.

#include "core/FrecencyTracker.h"

#include <algorithm>

namespace markamp::core
{

void FrecencyTracker::record_access(const std::string& key)
{
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto epoch = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    record_access(key, epoch);
}

void FrecencyTracker::record_access(const std::string& key, int64_t timestamp_epoch)
{
    auto& entry = entries_[key];
    entry.key = key;
    entry.access_count++;

    entry.access_timestamps.push_back(timestamp_epoch);

    // Keep only the last N timestamps
    if (static_cast<int>(entry.access_timestamps.size()) > FrecencyEntry::kMaxTimestamps)
    {
        entry.access_timestamps.erase(entry.access_timestamps.begin());
    }
}

auto FrecencyTracker::score(const std::string& key) const -> double
{
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto epoch = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    return score(key, epoch);
}

auto FrecencyTracker::score(const std::string& key, int64_t now_epoch) const -> double
{
    auto it = entries_.find(key);
    if (it == entries_.end())
    {
        return 0.0;
    }

    const auto& entry = it->second;
    double total = 0.0;

    for (const auto ts : entry.access_timestamps)
    {
        const int64_t age = now_epoch - ts;
        total += decay_weight(age >= 0 ? age : 0);
    }

    // Boost by access count factor (diminishing returns)
    const double count_factor = 1.0 + std::log2(1.0 + entry.access_count);
    return total * count_factor;
}

auto FrecencyTracker::ranked_keys(int limit) const -> std::vector<std::string>
{
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto epoch = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    return ranked_keys(limit, epoch);
}

auto FrecencyTracker::ranked_keys(int limit, int64_t now_epoch) const -> std::vector<std::string>
{
    struct ScoredKey
    {
        std::string key;
        double frecency_score;
    };

    std::vector<ScoredKey> scored;
    scored.reserve(entries_.size());

    for (const auto& [key, entry] : entries_)
    {
        scored.push_back({key, score(key, now_epoch)});
    }

    std::sort(scored.begin(),
              scored.end(),
              [](const ScoredKey& a, const ScoredKey& b)
              { return a.frecency_score > b.frecency_score; });

    std::vector<std::string> result;
    const int count = std::min(limit, static_cast<int>(scored.size()));
    result.reserve(static_cast<size_t>(count));

    for (int i = 0; i < count; ++i)
    {
        result.push_back(std::move(scored[static_cast<size_t>(i)].key));
    }

    return result;
}

auto FrecencyTracker::has_key(const std::string& key) const -> bool
{
    return entries_.count(key) > 0;
}

auto FrecencyTracker::entry_count() const -> size_t
{
    return entries_.size();
}

auto FrecencyTracker::get_entry(const std::string& key) const -> const FrecencyEntry*
{
    auto it = entries_.find(key);
    return it != entries_.end() ? &it->second : nullptr;
}

void FrecencyTracker::clear()
{
    entries_.clear();
}

auto FrecencyTracker::decay_weight(int64_t age_seconds) -> double
{
    if (age_seconds < kThreshold4Hours)
        return kWeight4Hours;
    if (age_seconds < kThreshold1Day)
        return kWeight1Day;
    if (age_seconds < kThreshold3Days)
        return kWeight3Days;
    if (age_seconds < kThreshold7Days)
        return kWeight7Days;
    if (age_seconds < kThreshold30Days)
        return kWeight30Days;
    return kWeightOlder;
}

} // namespace markamp::core
