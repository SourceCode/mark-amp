#pragma once

/// @file RingBuffer.h
/// @brief Fixed-capacity circular buffer for bounded data collection.
///
/// Phase 16: Replaces unbounded std::vector for timing samples.
/// O(1) write, no dynamic allocation, overwrites oldest on wrap.

#include <array>
#include <cstddef>
#include <cstdint>

namespace markamp::core
{

/// Fixed-capacity circular buffer that overwrites oldest entries on wrap.
///
/// @tparam T    Element type.
/// @tparam Cap  Maximum number of elements (must be > 0).
template <typename T, std::size_t Cap>
class RingBuffer
{
    static_assert(Cap > 0, "RingBuffer capacity must be greater than zero");

public:
    /// Push a value. Overwrites the oldest entry if at capacity.
    void push(const T& value) noexcept
    {
        data_[write_pos_ % Cap] = value;
        ++write_pos_;
        if (count_ < Cap)
        {
            ++count_;
        }
    }

    /// Number of elements currently stored (up to Cap).
    [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
    {
        return count_;
    }

    /// True if the buffer has wrapped at least once.
    [[nodiscard]] constexpr auto has_wrapped() const noexcept -> bool
    {
        return write_pos_ > Cap;
    }

    /// Maximum capacity.
    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t
    {
        return Cap;
    }

    /// True if no elements stored.
    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return count_ == 0;
    }

    /// Access element by logical index (0 = oldest).
    /// Requires idx < size().
    [[nodiscard]] auto at(std::size_t idx) const -> const T&
    {
        if (count_ < Cap)
        {
            // Not yet wrapped: element 0 is at data_[0]
            return data_[idx];
        }
        // Wrapped: oldest is at write_pos_ % Cap
        return data_[(write_pos_ + idx) % Cap];
    }

    /// Access most recently pushed element.
    /// Requires !empty().
    [[nodiscard]] auto back() const -> const T&
    {
        return data_[(write_pos_ - 1) % Cap];
    }

    /// Total number of items ever pushed (including overwrites).
    [[nodiscard]] constexpr auto total_pushes() const noexcept -> uint64_t
    {
        return write_pos_;
    }

    /// Reset to empty state.
    void clear() noexcept
    {
        write_pos_ = 0;
        count_ = 0;
    }

private:
    std::array<T, Cap> data_{};
    uint64_t write_pos_{0};
    std::size_t count_{0};
};

} // namespace markamp::core
