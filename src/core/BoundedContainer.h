/// BoundedContainer.h — Phase 19: Bounded-capacity containers
///
/// Drop-in replacements for std containers that enforce maximum capacity,
/// preventing unbounded memory growth from queues, logs, or histories.
///
/// Pattern implemented: #24 Bounded container caps

#pragma once

#include <cstddef>
#include <deque>
#include <string>

namespace markamp::core
{

/// Eviction policy when a bounded container reaches capacity.
enum class EvictionPolicy : uint8_t
{
    EvictOldest, // Drop the oldest element to make room (default)
    RejectNew    // Reject the new element, keeping existing ones
};

/// Bounded-capacity deque. Drops oldest (or rejects new) when at capacity.
///
/// Use this wherever an unbounded std::deque or std::vector could grow
/// without limit: event queues, notification lists, log buffers, etc.
template <typename T, std::size_t MaxSize>
class BoundedDeque
{
public:
    static_assert(MaxSize > 0, "BoundedDeque MaxSize must be > 0");

    explicit BoundedDeque(EvictionPolicy policy = EvictionPolicy::EvictOldest)
        : policy_(policy)
    {
    }

    /// Push an element to the back. Evicts oldest or rejects based on policy.
    /// Returns true if the element was added.
    auto push_back(T value) -> bool
    {
        if (data_.size() >= MaxSize)
        {
            if (policy_ == EvictionPolicy::RejectNew)
            {
                return false;
            }
            data_.pop_front(); // EvictOldest
        }
        data_.push_back(std::move(value));
        return true;
    }

    /// Push to front (e.g. for MRU lists).
    auto push_front(T value) -> bool
    {
        if (data_.size() >= MaxSize)
        {
            if (policy_ == EvictionPolicy::RejectNew)
            {
                return false;
            }
            data_.pop_back(); // EvictOldest from the other end
        }
        data_.push_front(std::move(value));
        return true;
    }

    void pop_front()
    {
        if (!data_.empty())
        {
            data_.pop_front();
        }
    }

    void pop_back()
    {
        if (!data_.empty())
        {
            data_.pop_back();
        }
    }

    [[nodiscard]] auto front() const -> const T&
    {
        return data_.front();
    }

    [[nodiscard]] auto back() const -> const T&
    {
        return data_.back();
    }

    [[nodiscard]] auto operator[](std::size_t index) const -> const T&
    {
        return data_[index];
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return data_.size();
    }

    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return data_.empty();
    }

    [[nodiscard]] static constexpr auto capacity() noexcept -> std::size_t
    {
        return MaxSize;
    }

    [[nodiscard]] auto full() const noexcept -> bool
    {
        return data_.size() >= MaxSize;
    }

    void clear() noexcept
    {
        data_.clear();
    }

    [[nodiscard]] auto begin() const noexcept
    {
        return data_.begin();
    }

    [[nodiscard]] auto end() const noexcept
    {
        return data_.end();
    }

    [[nodiscard]] auto begin() noexcept
    {
        return data_.begin();
    }

    [[nodiscard]] auto end() noexcept
    {
        return data_.end();
    }

private:
    std::deque<T> data_;
    EvictionPolicy policy_;
};

/// Bounded-capacity string. Truncates on append when at byte limit.
///
/// Use for output channel content, log buffers, etc.
class BoundedString
{
public:
    explicit BoundedString(std::size_t max_bytes)
        : max_bytes_(max_bytes)
    {
    }

    /// Append text, truncating from the front if the result exceeds capacity.
    void append(std::string_view text)
    {
        data_.append(text);
        if (data_.size() > max_bytes_)
        {
            // Remove from the front to keep the most recent content
            auto excess = data_.size() - max_bytes_;
            data_.erase(0, excess);
        }
    }

    /// Set the content, truncating if it exceeds capacity.
    void set(std::string_view text)
    {
        if (text.size() > max_bytes_)
        {
            data_.assign(text.substr(text.size() - max_bytes_));
        }
        else
        {
            data_.assign(text);
        }
    }

    [[nodiscard]] auto str() const noexcept -> const std::string&
    {
        return data_;
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return data_.size();
    }

    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return data_.empty();
    }

    [[nodiscard]] auto max_bytes() const noexcept -> std::size_t
    {
        return max_bytes_;
    }

    void clear() noexcept
    {
        data_.clear();
    }

private:
    std::string data_;
    std::size_t max_bytes_;
};

} // namespace markamp::core
