/// @file FrecencyTracker.h
/// @brief V13 Phase 31 Task 2 — Frecency algorithm for MRU ordering.
///
/// Combines frequency of access with recency decay. Files/commands opened
/// recently AND frequently rank highest. Supports serialization for persistence.
#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// A single tracked entry in the frecency system.
struct FrecencyEntry
{
    std::string key;
    int access_count{0};
    std::vector<int64_t> access_timestamps; ///< Last N timestamps (epoch seconds)

    static constexpr int kMaxTimestamps = 10;
};

/// Frecency-based ranking algorithm.
///
/// Score formula: sum(weight(age(ts)) * access_count_factor) where weight
/// decays: <4h=100, <1d=80, <3d=60, <7d=40, <30d=20, else=10.
///
/// Usage:
/// ```cpp
/// FrecencyTracker tracker;
/// tracker.record_access("file.md");
/// tracker.record_access("file.md");
/// auto ranked = tracker.ranked_keys(10);
/// ```
class FrecencyTracker
{
public:
    /// Record an access for the given key at the current time.
    void record_access(const std::string& key);

    /// Record an access for the given key at a specific timestamp (for testing).
    void record_access(const std::string& key, int64_t timestamp_epoch);

    /// Compute the frecency score for a key.
    [[nodiscard]] auto score(const std::string& key) const -> double;

    /// Compute the frecency score at a specific reference time (for testing).
    [[nodiscard]] auto score(const std::string& key, int64_t now_epoch) const -> double;

    /// Get the top N keys ranked by frecency score.
    [[nodiscard]] auto ranked_keys(int limit) const -> std::vector<std::string>;

    /// Get the top N keys ranked by frecency at a specific reference time.
    [[nodiscard]] auto ranked_keys(int limit, int64_t now_epoch) const -> std::vector<std::string>;

    /// Check if a key has been tracked.
    [[nodiscard]] auto has_key(const std::string& key) const -> bool;

    /// Get the number of tracked entries.
    [[nodiscard]] auto entry_count() const -> size_t;

    /// Get the entry for a key (for testing/inspection).
    [[nodiscard]] auto get_entry(const std::string& key) const -> const FrecencyEntry*;

    /// Clear all tracked entries.
    void clear();

    // ── Decay weight constants (public for testing) ──

    static constexpr double kWeight4Hours = 100.0;
    static constexpr double kWeight1Day = 80.0;
    static constexpr double kWeight3Days = 60.0;
    static constexpr double kWeight7Days = 40.0;
    static constexpr double kWeight30Days = 20.0;
    static constexpr double kWeightOlder = 10.0;

    // ── Time thresholds in seconds ──

    static constexpr int64_t kThreshold4Hours = 4LL * 3600;
    static constexpr int64_t kThreshold1Day = 24LL * 3600;
    static constexpr int64_t kThreshold3Days = 3LL * 24 * 3600;
    static constexpr int64_t kThreshold7Days = 7LL * 24 * 3600;
    static constexpr int64_t kThreshold30Days = 30LL * 24 * 3600;

private:
    std::unordered_map<std::string, FrecencyEntry> entries_;

    /// Get the decay weight for a given age in seconds.
    [[nodiscard]] static auto decay_weight(int64_t age_seconds) -> double;
};

} // namespace markamp::core
