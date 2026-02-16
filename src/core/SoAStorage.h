/// SoAStorage.h — Phase 23: Struct-of-Arrays container
///
/// Stores each field in a separate contiguous vector for cache-friendly
/// iteration over individual columns. O(1) swap-and-pop removal.
///
/// Pattern implemented: #6 Data-oriented layout for cache lines

#pragma once

#include <cassert>
#include <cstddef>
#include <tuple>
#include <vector>

namespace markamp::core
{

/// Struct-of-Arrays container for data-oriented storage.
///
/// Usage:
///   SoAStore<int, std::string, float> store;
///   store.add(42, "hello", 3.14f);
///   auto id = store.get<0>(0);     // => 42
///   store.remove(0);               // swap-and-pop
template <typename... Fields>
class SoAStore
{
public:
    SoAStore() = default;

    /// Append a row with one value per field.
    void add(Fields... values)
    {
        add_impl(std::index_sequence_for<Fields...>{}, std::move(values)...);
        ++size_;
    }

    /// Access field N at the given index.
    template <std::size_t N>
    [[nodiscard]] auto get(std::size_t index) const -> const auto&
    {
        assert(index < size_);
        return std::get<N>(columns_)[index];
    }

    /// Mutable access to field N at the given index.
    template <std::size_t N>
    [[nodiscard]] auto get_mut(std::size_t index) -> auto&
    {
        assert(index < size_);
        return std::get<N>(columns_)[index];
    }

    /// Remove item at index using swap-and-pop (O(1), does NOT preserve order).
    void remove(std::size_t index)
    {
        assert(index < size_);
        if (index < size_ - 1)
        {
            swap_impl(std::index_sequence_for<Fields...>{}, index, size_ - 1);
        }
        pop_impl(std::index_sequence_for<Fields...>{});
        --size_;
    }

    /// Get a const reference to a specific column vector.
    template <std::size_t N>
    [[nodiscard]] auto column() const
        -> const std::vector<std::tuple_element_t<N, std::tuple<Fields...>>>&
    {
        return std::get<N>(columns_);
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return size_;
    }

    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return size_ == 0;
    }

    void clear()
    {
        clear_impl(std::index_sequence_for<Fields...>{});
        size_ = 0;
    }

    /// Reserve capacity in all columns.
    void reserve(std::size_t new_capacity)
    {
        reserve_impl(std::index_sequence_for<Fields...>{}, new_capacity);
    }

private:
    std::tuple<std::vector<Fields>...> columns_;
    std::size_t size_{0};

    template <std::size_t... I>
    void add_impl(std::index_sequence<I...> /*seq*/, Fields... values)
    {
        (std::get<I>(columns_).push_back(std::move(values)), ...);
    }

    template <std::size_t... I>
    void swap_impl(std::index_sequence<I...> /*seq*/, std::size_t idx_a, std::size_t idx_b)
    {
        (std::swap(std::get<I>(columns_)[idx_a], std::get<I>(columns_)[idx_b]), ...);
    }

    template <std::size_t... I>
    void pop_impl(std::index_sequence<I...> /*seq*/)
    {
        (std::get<I>(columns_).pop_back(), ...);
    }

    template <std::size_t... I>
    void clear_impl(std::index_sequence<I...> /*seq*/)
    {
        (std::get<I>(columns_).clear(), ...);
    }

    template <std::size_t... I>
    void reserve_impl(std::index_sequence<I...> /*seq*/, std::size_t new_capacity)
    {
        (std::get<I>(columns_).reserve(new_capacity), ...);
    }
};

} // namespace markamp::core
