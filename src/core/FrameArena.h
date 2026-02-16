#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <memory_resource>
#include <new>
#include <utility>
#include <vector>

namespace markamp::core
{

/// Monotonic arena allocator for per-frame / per-batch allocations.
///
/// Allocations during a frame are fast (bump pointer). At frame end,
/// call reset() to reclaim all memory in O(1). Uses pmr so standard
/// containers (vector, string) can use the arena seamlessly.
///
/// Pattern implemented: #9 Arena allocators + object pools
class FrameArena
{
public:
    static constexpr std::size_t kDefaultBufferSize = 64 * 1024; // 64 KB

    explicit FrameArena(std::size_t buffer_size = kDefaultBufferSize)
        : heap_buffer_(buffer_size)
        , resource_(heap_buffer_.data(), heap_buffer_.size(), std::pmr::null_memory_resource())
    {
    }

    /// Get a polymorphic allocator backed by this arena.
    [[nodiscard]] auto allocator() noexcept -> std::pmr::polymorphic_allocator<std::byte>
    {
        return &resource_;
    }

    /// Convenience alias for pmr containers on the hot path.
    template <typename T>
    using Vector = std::pmr::vector<T>;

    template <typename T>
    using String = std::pmr::string;

    /// Create a vector backed by this arena.
    template <typename T>
    [[nodiscard]] auto make_vector() -> Vector<T>
    {
        return Vector<T>{allocator()};
    }

    /// Reset the arena — reclaims all memory in O(1).
    /// All pointers/containers previously obtained from this arena become invalid.
    void reset() noexcept
    {
        resource_.release();
        // Re-initialize: destroy + placement-new (operator= is deleted)
        resource_.~monotonic_buffer_resource();
        // R20 Fix 29: Note — monotonic_buffer_resource ctor is not declared noexcept
        // on all platforms. In practice this ctor never allocates (just assigns pointer
        // and size), so throwing is not observed. If the ctor did throw, this noexcept
        // function would call std::terminate, which is the safest outcome.
        ::new (&resource_) std::pmr::monotonic_buffer_resource(
            heap_buffer_.data(), heap_buffer_.size(), std::pmr::null_memory_resource());
    }

private:
    std::vector<std::byte> heap_buffer_;
    std::pmr::monotonic_buffer_resource resource_;
};

/// Thread-local arena pool — each thread gets its own arena.
///
/// Phase 14: Provides per-thread arena instances to avoid contention.
/// `get_arena()` returns the calling thread's arena. `reset_current()`
/// resets the current thread's arena (call at frame boundary).
///
/// Unlike `FrameArena` (which uses null_memory_resource and fails on
/// overflow), the pool arenas use `new_delete_resource()` as upstream
/// so arena exhaustion gracefully falls back to heap allocation.
///
/// Pattern implemented: #9 Arena allocators + object pools
class FrameArenaPool
{
public:
    static constexpr std::size_t kDefaultPoolArenaSize = 256UL * 1024; // 256 KB

    /// A FrameArena variant with heap fallback on exhaustion.
    class PoolArena
    {
    public:
        explicit PoolArena(std::size_t buffer_size)
            : heap_buffer_(buffer_size)
            , resource_(heap_buffer_.data(), heap_buffer_.size(), std::pmr::new_delete_resource())
        {
        }

        [[nodiscard]] auto allocator() noexcept -> std::pmr::polymorphic_allocator<std::byte>
        {
            return &resource_;
        }

        template <typename T>
        [[nodiscard]] auto make_vector() -> std::pmr::vector<T>
        {
            return std::pmr::vector<T>{allocator()};
        }

        void reset() noexcept
        {
            resource_.release();
            resource_.~monotonic_buffer_resource();
            ::new (&resource_) std::pmr::monotonic_buffer_resource(
                heap_buffer_.data(), heap_buffer_.size(), std::pmr::new_delete_resource());
        }

    private:
        std::vector<std::byte> heap_buffer_;
        std::pmr::monotonic_buffer_resource resource_;
    };

    /// Construct with a configurable per-thread arena buffer size.
    explicit FrameArenaPool(std::size_t arena_size = kDefaultPoolArenaSize)
        : arena_size_(arena_size)
    {
    }

    // Non-copyable, non-movable
    FrameArenaPool(const FrameArenaPool&) = delete;
    auto operator=(const FrameArenaPool&) -> FrameArenaPool& = delete;
    FrameArenaPool(FrameArenaPool&&) = delete;
    auto operator=(FrameArenaPool&&) -> FrameArenaPool& = delete;

    ~FrameArenaPool() = default;

    /// Get the current thread's arena. Creates on first call per thread.
    [[nodiscard]] auto get_arena() -> PoolArena&
    {
        thread_local PoolArena arena{arena_size_};
        return arena;
    }

    /// Reset the current thread's arena. Call at frame boundary.
    void reset_current()
    {
        get_arena().reset();
    }

    /// Get the configured arena size.
    [[nodiscard]] auto arena_size() const noexcept -> std::size_t
    {
        return arena_size_;
    }

private:
    std::size_t arena_size_;
};

/// Fixed-size object pool with free-list recycling.
///
/// Ideal for small, frequently allocated/deallocated objects like
/// piece-table nodes, tokens, or event payloads. Allocations are
/// O(1) amortized. No individual deallocation overhead — objects
/// are returned to the free list instantly.
///
/// Pattern implemented: #9 Arena allocators + object pools
template <typename T, std::size_t BlockSize = 256>
class ObjectPool
{
public:
    ObjectPool() = default;

    ~ObjectPool()
    {
        // All memory is freed when blocks_ is destroyed.
        // No need to call destructors on free-list nodes.
    }

    // Non-copyable, non-movable
    ObjectPool(const ObjectPool&) = delete;
    auto operator=(const ObjectPool&) -> ObjectPool& = delete;
    ObjectPool(ObjectPool&&) = delete;
    auto operator=(ObjectPool&&) -> ObjectPool& = delete;

    /// Allocate and construct a T with the given arguments.
    template <typename... Args>
    [[nodiscard]] auto create(Args&&... args) -> T*
    {
        if (free_list_ == nullptr)
        {
            grow();
        }
        auto* node = free_list_;
        free_list_ = node->next;
        ++active_count_;
        return std::construct_at(reinterpret_cast<T*>(node), std::forward<Args>(args)...);
    }

    /// Destroy a T and return the memory to the pool.
    void destroy(T* ptr) noexcept
    {
        if (ptr == nullptr)
        {
            return;
        }
        std::destroy_at(ptr);
        auto* node = reinterpret_cast<FreeNode*>(ptr);
        node->next = free_list_;
        free_list_ = node;
        --active_count_;
    }

    /// Number of objects currently allocated (not on free list).
    [[nodiscard]] auto active_count() const noexcept -> std::size_t
    {
        return active_count_;
    }

    /// Total capacity (allocated + free).
    [[nodiscard]] auto total_capacity() const noexcept -> std::size_t
    {
        return blocks_.size() * BlockSize;
    }

private:
    union FreeNode
    {
        // Ensure the node is large enough and aligned for T
        alignas(T) std::byte storage[sizeof(T)];
        FreeNode* next;
    };
    // R19 Fix 39: Verify FreeNode alignment/size at compile time
    static_assert(alignof(FreeNode) >= alignof(T),
                  "FreeNode alignment must satisfy T's alignment requirement");
    static_assert(sizeof(FreeNode) >= sizeof(T), "FreeNode must be at least as large as T");

    std::vector<std::unique_ptr<FreeNode[]>> blocks_;
    FreeNode* free_list_{nullptr};
    std::size_t active_count_{0};

    void grow()
    {
        auto block = std::make_unique<FreeNode[]>(BlockSize);
        for (std::size_t i = 0; i < BlockSize - 1; ++i)
        {
            block[i].next = &block[i + 1];
        }
        block[BlockSize - 1].next = free_list_;
        free_list_ = &block[0];
        blocks_.push_back(std::move(block));
    }
};

} // namespace markamp::core
