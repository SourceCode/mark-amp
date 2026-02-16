/// ConstexprMap.h — Phase 26: Compile-time sorted lookup table
///
/// A constexpr-friendly map implemented as a sorted array with binary search.
/// O(log N) lookup at both compile-time and runtime. Zero heap allocation.
///
/// Pattern implemented: #9 Constexpr lookup tables

#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

namespace markamp::core
{

/// A single key-value entry for the constexpr map.
template <typename Key, typename Value>
struct MapEntry
{
    Key key;
    Value value;

    constexpr auto operator<(const MapEntry& other) const -> bool
    {
        return key < other.key;
    }
};

/// Compile-time sorted map backed by a fixed-size std::array.
///
/// Usage:
///   constexpr auto kMimeTypes = make_constexpr_map<std::string_view, std::string_view>({
///       {"css", "text/css"},
///       {"html", "text/html"},
///       {"js", "application/javascript"},
///   });
///   auto found = kMimeTypes.find("html"); // O(log N) binary search
template <typename Key, typename Value, std::size_t N>
class ConstexprMap
{
public:
    using Entry = MapEntry<Key, Value>;

    /// Construct from sorted array. Caller must ensure sorted order.
    constexpr explicit ConstexprMap(std::array<Entry, N> sorted_data)
        : data_(sorted_data)
    {
    }

    /// O(log N) binary search lookup. Returns nullptr if not found.
    [[nodiscard]] constexpr auto find(const Key& key) const -> const Value*
    {
        std::size_t lo = 0;
        std::size_t hi = N;

        while (lo < hi)
        {
            auto mid = lo + (hi - lo) / 2;
            if (data_[mid].key < key)
            {
                lo = mid + 1;
            }
            else if (key < data_[mid].key)
            {
                hi = mid;
            }
            else
            {
                return &data_[mid].value;
            }
        }
        return nullptr;
    }

    /// O(log N) lookup with default value if not found.
    [[nodiscard]] constexpr auto get(const Key& key, const Value& default_value) const -> Value
    {
        const auto* found = find(key);
        return found != nullptr ? *found : default_value;
    }

    /// Check if a key exists.
    [[nodiscard]] constexpr auto contains(const Key& key) const -> bool
    {
        return find(key) != nullptr;
    }

    /// Number of entries.
    [[nodiscard]] constexpr auto size() const -> std::size_t
    {
        return N;
    }

    /// Direct access to the sorted data array.
    [[nodiscard]] constexpr auto data() const -> const std::array<Entry, N>&
    {
        return data_;
    }

private:
    std::array<Entry, N> data_;
};

/// Helper: sort at compile time and construct a ConstexprMap.
///
/// Usage:
///   constexpr auto kMap = make_constexpr_map<std::string_view, int>({
///       {"b", 2}, {"a", 1}, {"c", 3}
///   });
///   // Internal storage is sorted: {"a",1}, {"b",2}, {"c",3}
template <typename Key, typename Value, std::size_t N>
constexpr auto make_constexpr_map(MapEntry<Key, Value> (&&entries)[N])
    -> ConstexprMap<Key, Value, N>
{
    std::array<MapEntry<Key, Value>, N> arr{};
    for (std::size_t idx = 0; idx < N; ++idx)
    {
        arr[idx] = entries[idx];
    }
    // Compile-time insertion sort (std::sort not constexpr in all compilers)
    for (std::size_t i_idx = 1; i_idx < N; ++i_idx)
    {
        auto temp = arr[i_idx];
        std::size_t j_idx = i_idx;
        while (j_idx > 0 && temp < arr[j_idx - 1])
        {
            arr[j_idx] = arr[j_idx - 1];
            --j_idx;
        }
        arr[j_idx] = temp;
    }
    return ConstexprMap<Key, Value, N>(arr);
}

// ═══════════════════════════════════════════════════════
// Built-in MIME type table (constexpr)
// ═══════════════════════════════════════════════════════

inline constexpr auto kMimeTypes = make_constexpr_map<std::string_view, std::string_view>({
    {"css", "text/css"},
    {"csv", "text/csv"},
    {"gif", "image/gif"},
    {"htm", "text/html"},
    {"html", "text/html"},
    {"jpeg", "image/jpeg"},
    {"jpg", "image/jpeg"},
    {"js", "application/javascript"},
    {"json", "application/json"},
    {"md", "text/markdown"},
    {"pdf", "application/pdf"},
    {"png", "image/png"},
    {"svg", "image/svg+xml"},
    {"toml", "application/toml"},
    {"ts", "application/typescript"},
    {"txt", "text/plain"},
    {"xml", "application/xml"},
    {"yaml", "application/yaml"},
    {"yml", "application/yaml"},
});

// ═══════════════════════════════════════════════════════
// Keyword classification table (constexpr)
// ═══════════════════════════════════════════════════════

inline constexpr auto kCppKeywords = make_constexpr_map<std::string_view, bool>({
    {"alignas", true},   {"alignof", true},       {"auto", true},
    {"bool", true},      {"break", true},         {"case", true},
    {"catch", true},     {"class", true},         {"concept", true},
    {"const", true},     {"consteval", true},     {"constexpr", true},
    {"constinit", true}, {"continue", true},      {"co_await", true},
    {"co_return", true}, {"co_yield", true},      {"decltype", true},
    {"default", true},   {"delete", true},        {"do", true},
    {"double", true},    {"else", true},          {"enum", true},
    {"explicit", true},  {"export", true},        {"extern", true},
    {"false", true},     {"float", true},         {"for", true},
    {"friend", true},    {"goto", true},          {"if", true},
    {"inline", true},    {"int", true},           {"long", true},
    {"mutable", true},   {"namespace", true},     {"new", true},
    {"noexcept", true},  {"nullptr", true},       {"operator", true},
    {"private", true},   {"protected", true},     {"public", true},
    {"register", true},  {"requires", true},      {"return", true},
    {"short", true},     {"signed", true},        {"sizeof", true},
    {"static", true},    {"static_assert", true}, {"static_cast", true},
    {"struct", true},    {"switch", true},        {"template", true},
    {"this", true},      {"throw", true},         {"true", true},
    {"try", true},       {"typedef", true},       {"typeid", true},
    {"typename", true},  {"union", true},         {"unsigned", true},
    {"using", true},     {"virtual", true},       {"void", true},
    {"volatile", true},  {"while", true},
});

} // namespace markamp::core
