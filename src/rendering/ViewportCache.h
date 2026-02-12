#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <list>
#include <optional>
#include <unordered_map>

namespace markamp::rendering
{

// ═══════════════════════════════════════════════════════
// LRU Cache
// ═══════════════════════════════════════════════════════

/// Fixed-capacity LRU cache for rendered line bitmaps, layout data, etc.
///
/// Patterns implemented:
///   #13 Viewport virtualization (cache visible + prefetch tiles)
///   #12 Lazy layout and measurement caching
template <typename Key, typename Value, std::size_t MaxEntries = 256>
class LRUCache
{
public:
    LRUCache() = default;

    /// Get a value. Returns nullopt if not present.
    [[nodiscard]] auto get(const Key& key) -> std::optional<std::reference_wrapper<Value>>
    {
        auto map_iter = map_.find(key);
        if (map_iter == map_.end())
        {
            return std::nullopt;
        }
        // Move to front (most recently used)
        order_.splice(order_.begin(), order_, map_iter->second);
        return map_iter->second->second;
    }

    /// Insert or update a value.
    void put(const Key& key, Value value)
    {
        auto map_iter = map_.find(key);
        if (map_iter != map_.end())
        {
            // Update existing entry
            map_iter->second->second = std::move(value);
            order_.splice(order_.begin(), order_, map_iter->second);
            return;
        }

        // Evict LRU entry if at capacity
        if (map_.size() >= MaxEntries)
        {
            auto& lru = order_.back();
            map_.erase(lru.first);
            order_.pop_back();
        }

        // Insert new entry at front
        order_.emplace_front(key, std::move(value));
        map_[key] = order_.begin();
    }

    /// Check if a key exists.
    [[nodiscard]] auto contains(const Key& key) const -> bool
    {
        return map_.find(key) != map_.end();
    }

    /// Number of cached entries.
    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return map_.size();
    }

    /// Clear the cache.
    void clear()
    {
        map_.clear();
        order_.clear();
    }

private:
    using ListType = std::list<std::pair<Key, Value>>;
    ListType order_;
    std::unordered_map<Key, typename ListType::iterator> map_;
};

// ═══════════════════════════════════════════════════════
// Viewport state
// ═══════════════════════════════════════════════════════

/// Tracks the visible line range with a prefetch margin.
///
/// Pattern implemented: #13 Viewport virtualization
struct ViewportState
{
    std::size_t first_visible_line{0};
    std::size_t visible_line_count{0};
    std::size_t prefetch_margin{10}; // lines to prefetch above/below

    /// First line in the render range (with prefetch).
    [[nodiscard]] auto render_start(std::size_t total_lines) const noexcept -> std::size_t
    {
        (void)total_lines;
        return first_visible_line > prefetch_margin ? first_visible_line - prefetch_margin : 0;
    }

    /// One past the last line in the render range (with prefetch).
    [[nodiscard]] auto render_end(std::size_t total_lines) const noexcept -> std::size_t
    {
        auto raw_end = first_visible_line + visible_line_count + prefetch_margin;
        return raw_end < total_lines ? raw_end : total_lines;
    }
};

// ═══════════════════════════════════════════════════════
// Lazy computation cache
// ═══════════════════════════════════════════════════════

/// Single-value lazy cache that defers computation until first access.
///
/// Pattern implemented: #12 Lazy layout and measurement
template <typename T>
class LazyCache
{
public:
    LazyCache() = default;

    /// Mark the cached value as stale.
    void invalidate() noexcept
    {
        valid_ = false;
    }

    /// Get the value, computing it if stale.
    template <typename Factory>
    [[nodiscard]] auto get(Factory&& factory) -> const T&
    {
        if (!valid_)
        {
            value_ = factory();
            valid_ = true;
        }
        return value_;
    }

    /// Check if the cache has a valid value.
    [[nodiscard]] auto is_valid() const noexcept -> bool
    {
        return valid_;
    }

private:
    T value_{};
    bool valid_{false};
};

// ═══════════════════════════════════════════════════════
// Line measurement cache
// ═══════════════════════════════════════════════════════

/// Per-line layout measurement.
///
/// Pattern implemented: #12 Lazy layout and measurement
struct LineMeasurement
{
    int32_t height_px{0};
    int32_t baseline_px{0};
    std::size_t wrapped_line_count{1}; // for word-wrapped lines
    uint64_t content_hash{0};          // to detect stale measurements
};

/// Cache of per-line layout measurements indexed by line number.
///
/// Patterns implemented:
///   #12 Lazy layout and measurement
///   #16 Fast scrolling with cached line heights
class LineLayoutCache
{
public:
    LineLayoutCache() = default;

    /// Get the measurement for a given line. Returns nullopt if not cached.
    [[nodiscard]] auto get(std::size_t line) const
        -> std::optional<std::reference_wrapper<const LineMeasurement>>
    {
        if (line < measurements_.size() && measurements_[line].height_px > 0)
        {
            return std::cref(measurements_[line]);
        }
        return std::nullopt;
    }

    /// Set the measurement for a given line.
    void set(std::size_t line, LineMeasurement measurement)
    {
        if (line >= measurements_.size())
        {
            measurements_.resize(line + 1);
        }
        measurements_[line] = measurement;
    }

    /// Invalidate a range of lines (e.g., after an edit).
    void invalidate_range(std::size_t start_line, std::size_t end_line)
    {
        for (auto line = start_line; line < end_line && line < measurements_.size(); ++line)
        {
            measurements_[line] = LineMeasurement{};
        }
    }

    /// Invalidate all cached measurements.
    void invalidate_all()
    {
        measurements_.clear();
    }

    /// Total cached line count.
    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return measurements_.size();
    }

private:
    std::vector<LineMeasurement> measurements_;
};

} // namespace markamp::rendering
