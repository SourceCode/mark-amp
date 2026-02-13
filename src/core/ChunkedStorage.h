#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <list>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Fixed-size chunked buffer for large text documents.
///
/// Stores text in fixed-size chunks for cache-line-friendly sequential
/// access. Avoids the single-allocation problem with very large strings.
///
/// Pattern implemented: #39 Memory locality via chunked storage
template <std::size_t ChunkSize = 65536> // 64KB default
class ChunkedBuffer
{
public:
    ChunkedBuffer() = default;

    /// Append text to the buffer.
    void append(std::string_view text)
    {
        while (!text.empty())
        {
            if (chunks_.empty() || chunk_used_.back() == ChunkSize)
            {
                chunks_.emplace_back(ChunkSize, '\0');
                chunk_used_.push_back(0);
            }

            auto& chunk = chunks_.back();
            auto space = ChunkSize - chunk_used_.back();
            auto to_copy = std::min(space, text.size());

            std::memcpy(chunk.data() + chunk_used_.back(), text.data(), to_copy);
            chunk_used_.back() += to_copy;
            total_size_ += to_copy;
            text.remove_prefix(to_copy);
        }
    }

    /// Read `length` bytes starting at `offset`.
    [[nodiscard]] auto read(std::size_t offset, std::size_t length) const -> std::string
    {
        if (offset >= total_size_)
        {
            return {};
        }
        length = std::min(length, total_size_ - offset);

        std::string result;
        result.reserve(length);

        auto chunk_idx = offset / ChunkSize;
        auto chunk_offset = offset % ChunkSize;

        while (length > 0 && chunk_idx < chunks_.size())
        {
            // R19 Fix 25: Guard against underflow if chunk_offset exceeds chunk_used_
            if (chunk_offset >= chunk_used_[chunk_idx])
            {
                chunk_offset = 0;
                ++chunk_idx;
                continue;
            }
            auto available = chunk_used_[chunk_idx] - chunk_offset;
            auto to_copy = std::min(available, length);

            result.append(chunks_[chunk_idx].data() + chunk_offset, to_copy);
            length -= to_copy;
            chunk_offset = 0;
            ++chunk_idx;
        }

        return result;
    }

    /// Total stored bytes.
    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return total_size_;
    }

    /// Number of chunks allocated.
    [[nodiscard]] auto chunk_count() const noexcept -> std::size_t
    {
        return chunks_.size();
    }

    /// Whether the buffer is empty.
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return total_size_ == 0;
    }

    /// Clear all content.
    void clear()
    {
        chunks_.clear();
        chunk_used_.clear();
        total_size_ = 0;
    }

    /// Capacity (total allocated bytes).
    [[nodiscard]] auto capacity() const noexcept -> std::size_t
    {
        return chunks_.size() * ChunkSize;
    }

private:
    std::vector<std::string> chunks_;
    std::vector<std::size_t> chunk_used_;
    std::size_t total_size_{0};
};

/// LRU cache with a byte-size cap (instead of entry count).
///
/// Evicts by byte size rather than entry count, suitable for
/// bitmap caches where entries vary in size.
///
/// Pattern implemented: #39 Memory locality — capped render caches
template <typename Key, typename Value>
class ByteCappedLRU
{
public:
    using SizeFunc = std::size_t (*)(const Value&);

    explicit ByteCappedLRU(std::size_t max_bytes, SizeFunc size_fn)
        : max_bytes_(max_bytes)
        , size_fn_(size_fn)
    {
    }

    /// Insert or update a value. Evicts oldest entries if byte cap is exceeded.
    void put(const Key& key, Value value)
    {
        // Remove existing entry if present
        auto iter = map_.find(key);
        if (iter != map_.end())
        {
            current_bytes_ -= size_fn_(iter->second->second);
            order_.erase(iter->second);
            map_.erase(iter);
        }

        auto value_size = size_fn_(value);

        // Evict until we have room
        while (current_bytes_ + value_size > max_bytes_ && !order_.empty())
        {
            evict_oldest();
        }

        order_.emplace_front(key, std::move(value));
        map_[key] = order_.begin();
        current_bytes_ += value_size;
    }

    /// Get a value by key. Returns nullptr if not found.
    [[nodiscard]] auto get(const Key& key) -> Value*
    {
        auto iter = map_.find(key);
        if (iter == map_.end())
        {
            return nullptr;
        }
        // Move to front (most recently used)
        order_.splice(order_.begin(), order_, iter->second);
        return &iter->second->second;
    }

    /// Current total byte usage.
    [[nodiscard]] auto current_bytes() const noexcept -> std::size_t
    {
        return current_bytes_;
    }

    /// Maximum byte budget.
    [[nodiscard]] auto max_bytes() const noexcept -> std::size_t
    {
        return max_bytes_;
    }

    /// Number of entries.
    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return map_.size();
    }

    /// Clear all entries.
    void clear()
    {
        order_.clear();
        map_.clear();
        current_bytes_ = 0;
    }

private:
    using ListType = std::list<std::pair<Key, Value>>;

    void evict_oldest()
    {
        if (order_.empty())
            return;
        auto& oldest = order_.back();
        // R19 Fix 26: Guard against underflow in current_bytes_ subtraction
        auto entry_size = size_fn_(oldest.second);
        if (entry_size <= current_bytes_)
        {
            current_bytes_ -= entry_size;
        }
        else
        {
            current_bytes_ = 0;
        }
        map_.erase(oldest.first);
        order_.pop_back();
    }

    std::size_t max_bytes_;
    SizeFunc size_fn_;
    std::size_t current_bytes_{0};
    ListType order_;
    std::unordered_map<Key, typename ListType::iterator> map_;
};

} // namespace markamp::core
