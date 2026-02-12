#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

namespace markamp::core
{

/// A stable identifier for a line in the document.
///
/// Unlike line numbers, StableLineIds survive insertions and deletions
/// above the line. Caches keyed by StableLineId are resilient across edits.
///
/// Pattern implemented: #26 Stable IDs for UI elements and caches
struct StableLineId
{
    uint64_t id{0};

    auto operator==(const StableLineId& other) const noexcept -> bool = default;
    auto operator!=(const StableLineId& other) const noexcept -> bool = default;
    auto operator<(const StableLineId& other) const noexcept -> bool
    {
        return id < other.id;
    }
};

/// Thread-safe allocator for monotonically increasing StableLineIds.
///
/// Pattern implemented: #26 Stable IDs for UI elements and caches
class StableIdAllocator
{
public:
    StableIdAllocator() = default;

    /// Issue a fresh, unique StableLineId.
    [[nodiscard]] auto allocate() noexcept -> StableLineId
    {
        return StableLineId{next_id_.fetch_add(1, std::memory_order_relaxed)};
    }

    /// Issue N fresh StableLineIds.
    [[nodiscard]] auto allocate_batch(std::size_t count) -> std::vector<StableLineId>
    {
        std::vector<StableLineId> ids;
        ids.reserve(count);
        auto base = next_id_.fetch_add(count, std::memory_order_relaxed);
        for (std::size_t idx = 0; idx < count; ++idx)
        {
            ids.push_back(StableLineId{base + idx});
        }
        return ids;
    }

    /// Current counter value (for diagnostics).
    [[nodiscard]] auto current_counter() const noexcept -> uint64_t
    {
        return next_id_.load(std::memory_order_relaxed);
    }

private:
    std::atomic<uint64_t> next_id_{1}; // Start at 1; 0 is "no ID"
};

/// Maps line numbers to StableLineIds.
///
/// Updated incrementally on insert/delete — shifts IDs, assigns new IDs
/// for inserted lines. Caches keyed by (StableLineId, version_gen) are
/// resilient to insertions/deletions.
///
/// Pattern implemented: #26 Stable IDs for UI elements and caches
class LineIdMap
{
public:
    LineIdMap() = default;

    /// Build the initial mapping for `line_count` lines.
    void initialize(std::size_t line_count)
    {
        ids_ = allocator_.allocate_batch(line_count);
    }

    /// Get the StableLineId for a line number.
    [[nodiscard]] auto get(std::size_t line) const noexcept -> StableLineId
    {
        if (line < ids_.size())
        {
            return ids_[line];
        }
        return StableLineId{0};
    }

    /// Handle insertion: insert `count` new IDs starting at `line`.
    void on_insert(std::size_t line, std::size_t count)
    {
        auto new_ids = allocator_.allocate_batch(count);
        auto insert_pos =
            (line <= ids_.size()) ? ids_.begin() + static_cast<std::ptrdiff_t>(line) : ids_.end();
        ids_.insert(insert_pos, new_ids.begin(), new_ids.end());
    }

    /// Handle deletion: remove `count` IDs starting at `line`.
    void on_erase(std::size_t line, std::size_t count)
    {
        if (line >= ids_.size())
        {
            return;
        }
        auto erase_end = (line + count <= ids_.size())
                             ? ids_.begin() + static_cast<std::ptrdiff_t>(line + count)
                             : ids_.end();
        ids_.erase(ids_.begin() + static_cast<std::ptrdiff_t>(line), erase_end);
    }

    /// Total number of mapped lines.
    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return ids_.size();
    }

    /// Access the underlying allocator (for testing).
    [[nodiscard]] auto allocator() const noexcept -> const StableIdAllocator&
    {
        return allocator_;
    }

private:
    StableIdAllocator allocator_;
    std::vector<StableLineId> ids_;
};

} // namespace markamp::core

// Hash specialization for StableLineId (for use in unordered_map / LRUCache)
template <>
struct std::hash<markamp::core::StableLineId>
{
    auto operator()(const markamp::core::StableLineId& id) const noexcept -> std::size_t
    {
        return std::hash<uint64_t>{}(id.id);
    }
};
