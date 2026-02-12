#pragma once

#include <atomic>
#include <cstdint>

namespace markamp::core
{

/// Monotonically increasing generation counter for lazy cache invalidation.
///
/// Instead of walking all caches when theme/font/wrap-width changes,
/// bump the appropriate generation counter. Caches compare their stored
/// generation with the current and lazily rebuild stale entries.
///
/// Pattern implemented: #34 Theme/font changes handled via generation counters
class GenerationCounter
{
public:
    GenerationCounter() = default;

    /// Bump the counter (e.g., on theme/font/wrap change).
    void bump() noexcept
    {
        gen_.fetch_add(1, std::memory_order_release);
    }

    /// Get the current generation.
    [[nodiscard]] auto current() const noexcept -> uint64_t
    {
        return gen_.load(std::memory_order_acquire);
    }

    /// Check if a stored generation is stale.
    [[nodiscard]] auto is_stale(uint64_t stored_gen) const noexcept -> bool
    {
        return stored_gen != current();
    }

private:
    std::atomic<uint64_t> gen_{0};
};

/// Groups theme, font, and wrap-width generation counters for
/// composite cache keys.
///
/// Pattern implemented: #34 Theme/font changes handled via generation counters
struct GenerationSet
{
    GenerationCounter theme_gen;
    GenerationCounter font_gen;
    GenerationCounter wrap_gen;

    /// Snapshot of all current generations for use as a cache key.
    struct Snapshot
    {
        uint64_t theme{0};
        uint64_t font{0};
        uint64_t wrap{0};

        auto operator==(const Snapshot& other) const noexcept -> bool = default;
        auto operator!=(const Snapshot& other) const noexcept -> bool = default;
    };

    /// Take a snapshot of current generation values.
    [[nodiscard]] auto snapshot() const noexcept -> Snapshot
    {
        return Snapshot{
            .theme = theme_gen.current(), .font = font_gen.current(), .wrap = wrap_gen.current()};
    }

    /// Check if any generation has changed since a snapshot.
    [[nodiscard]] auto is_stale(const Snapshot& snap) const noexcept -> bool
    {
        return snap != snapshot();
    }

    /// Bump all generations (e.g., full invalidation).
    void bump_all() noexcept
    {
        theme_gen.bump();
        font_gen.bump();
        wrap_gen.bump();
    }
};

} // namespace markamp::core
