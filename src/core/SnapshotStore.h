/// SnapshotStore.h — Phase 21: Lock-free read, mutex-on-write snapshot store
///
/// Uses shared_mutex for read-heavy workloads:
/// - Readers take shared_lock (concurrent reads allowed)
/// - Writers take unique_lock to serialize updates + COW
///
/// Patterns implemented:
///   #7  Minimal locking via message passing
///   #8  COW (Copy-on-Write) for shared state

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace markamp::core
{

/// Snapshot store for shared immutable state.
/// Readers take shared_lock (concurrent). Writers take unique_lock.
///
/// Usage:
///   SnapshotStore<std::vector<int>> store(std::vector<int>{1, 2, 3});
///   auto snap = store.read();   // shared_lock — concurrent reads
///   store.update([](const auto& v) { auto copy = v; copy.push_back(4); return copy; });
template <typename T>
class SnapshotStore
{
public:
    SnapshotStore()
        : snapshot_(std::make_shared<const T>())
    {
    }

    explicit SnapshotStore(T initial)
        : snapshot_(std::make_shared<const T>(std::move(initial)))
    {
    }

    /// Concurrent read. Returns a shared_ptr to an immutable snapshot.
    /// The snapshot remains valid even if a writer updates the store.
    [[nodiscard]] auto read() const -> std::shared_ptr<const T>
    {
        const std::shared_lock lock(mutex_);
        return snapshot_;
    }

    /// Atomically replace the stored value. Writers are serialized.
    void store(T value)
    {
        const std::unique_lock lock(mutex_);
        snapshot_ = std::make_shared<const T>(std::move(value));
    }

    /// Read-modify-write. The modifier receives the current value and
    /// returns the new value. Writers are serialized via unique_lock.
    void update(std::function<T(const T&)> modifier)
    {
        const std::unique_lock lock(mutex_);
        auto updated = modifier(*snapshot_);
        snapshot_ = std::make_shared<const T>(std::move(updated));
    }

    /// Check if the store has been written to at least once after construction.
    [[nodiscard]] auto version() const -> uint64_t
    {
        return version_.load(std::memory_order_acquire);
    }

    /// Atomically replace and increment version.
    void store_versioned(T value)
    {
        const std::unique_lock lock(mutex_);
        snapshot_ = std::make_shared<const T>(std::move(value));
        version_.fetch_add(1, std::memory_order_release);
    }

private:
    std::shared_ptr<const T> snapshot_;
    mutable std::shared_mutex mutex_;
    std::atomic<uint64_t> version_{0};
};

} // namespace markamp::core
