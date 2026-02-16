/// test_constexpr_tables.cpp — Compile-time table validation
///
/// Phase 11: Verifies that key data structures are constexpr-evaluated
/// and that lookup operations work correctly on compile-time arrays.

#include "core/StartupPhase.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <string_view>

// ═══════════════════════════════════════════════════════
// Helper: constexpr sorted array lookup via binary search
// ═══════════════════════════════════════════════════════

namespace
{

/// A constexpr key-value pair for compile-time lookup tables.
template <typename Key, typename Value>
struct ConstexprEntry
{
    Key key;
    Value value;
};

/// Constexpr binary search on a sorted array of ConstexprEntry.
template <typename Key, typename Value, std::size_t N>
constexpr auto constexpr_lookup(const std::array<ConstexprEntry<Key, Value>, N>& table,
                                const Key& key) -> const Value*
{
    std::size_t low = 0;
    std::size_t high = N;

    while (low < high)
    {
        std::size_t mid = low + (high - low) / 2;
        if (table[mid].key < key)
        {
            low = mid + 1;
        }
        else if (key < table[mid].key)
        {
            high = mid;
        }
        else
        {
            return &table[mid].value;
        }
    }
    return nullptr;
}

// ═══════════════════════════════════════════════════════
// Example constexpr lookup table: MIME types by extension
// ═══════════════════════════════════════════════════════

constexpr std::array<ConstexprEntry<std::string_view, std::string_view>, 8> kMimeTypes = {{
    {".cpp", "text/x-c++src"},
    {".css", "text/css"},
    {".h", "text/x-c++hdr"},
    {".html", "text/html"},
    {".js", "application/javascript"},
    {".json", "application/json"},
    {".md", "text/markdown"},
    {".py", "text/x-python"},
}};

// Verify the table is sorted at compile time
static_assert(
    []
    {
        for (std::size_t idx = 1; idx < kMimeTypes.size(); ++idx)
        {
            if (kMimeTypes[idx - 1].key >= kMimeTypes[idx].key)
            {
                return false;
            }
        }
        return true;
    }(),
    "kMimeTypes must be sorted by key");

// ═══════════════════════════════════════════════════════
// Example constexpr lookup table: Config default values
// ═══════════════════════════════════════════════════════

constexpr std::array<ConstexprEntry<std::string_view, int>, 5> kConfigDefaults = {{
    {"editor.column_guide", 80},
    {"editor.font_size", 14},
    {"editor.indent_size", 4},
    {"editor.tab_size", 4},
    {"window.sidebar_width", 250},
}};

static_assert(
    []
    {
        for (std::size_t idx = 1; idx < kConfigDefaults.size(); ++idx)
        {
            if (kConfigDefaults[idx - 1].key >= kConfigDefaults[idx].key)
            {
                return false;
            }
        }
        return true;
    }(),
    "kConfigDefaults must be sorted by key");

} // namespace

// ═══════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("constexpr MIME table: lookup existing entry", "[constexpr]")
{
    const auto* result = constexpr_lookup(kMimeTypes, std::string_view{".md"});
    REQUIRE(result != nullptr);
    REQUIRE(*result == "text/markdown");
}

TEST_CASE("constexpr MIME table: lookup missing entry returns nullptr", "[constexpr]")
{
    const auto* result = constexpr_lookup(kMimeTypes, std::string_view{".xyz"});
    REQUIRE(result == nullptr);
}

TEST_CASE("constexpr MIME table: lookup first and last entries", "[constexpr]")
{
    SECTION("first entry")
    {
        const auto* result = constexpr_lookup(kMimeTypes, std::string_view{".cpp"});
        REQUIRE(result != nullptr);
        REQUIRE(*result == "text/x-c++src");
    }

    SECTION("last entry")
    {
        const auto* result = constexpr_lookup(kMimeTypes, std::string_view{".py"});
        REQUIRE(result != nullptr);
        REQUIRE(*result == "text/x-python");
    }
}

TEST_CASE("constexpr config defaults: lookup existing", "[constexpr]")
{
    const auto* result = constexpr_lookup(kConfigDefaults, std::string_view{"editor.font_size"});
    REQUIRE(result != nullptr);
    REQUIRE(*result == 14);
}

TEST_CASE("constexpr config defaults: all entries accessible", "[constexpr]")
{
    for (const auto& entry : kConfigDefaults)
    {
        const auto* result = constexpr_lookup(kConfigDefaults, entry.key);
        REQUIRE(result != nullptr);
        REQUIRE(*result == entry.value);
    }
}

TEST_CASE("StartupPhase name lookup is constexpr", "[constexpr]")
{
    // Verify that phase_name() is constexpr and returns non-empty strings
    constexpr auto name = markamp::core::phase_name(markamp::core::StartupPhase::LoggerInit);
    static_assert(name == "LoggerInit");

    constexpr auto config_name = markamp::core::phase_name(markamp::core::StartupPhase::ConfigLoad);
    static_assert(config_name == "ConfigLoad");

    // Runtime check: all phases have non-empty names
    constexpr auto count = static_cast<uint8_t>(markamp::core::StartupPhase::Count);
    for (uint8_t idx = 0; idx < count; ++idx)
    {
        auto phase = static_cast<markamp::core::StartupPhase>(idx);
        auto phase_name_str = markamp::core::phase_name(phase);
        REQUIRE_FALSE(phase_name_str.empty());
    }
}

TEST_CASE("constexpr table: compile-time evaluation verified", "[constexpr]")
{
    // These static_asserts prove compile-time evaluation
    static_assert(kMimeTypes.size() == 8);
    static_assert(kConfigDefaults.size() == 5);
    static_assert(kMimeTypes[0].key == ".cpp");
    static_assert(kConfigDefaults[0].value == 80);

    // Binary search works at compile time
    static_assert(constexpr_lookup(kMimeTypes, std::string_view{".md"}) != nullptr);
    static_assert(*constexpr_lookup(kMimeTypes, std::string_view{".md"}) == "text/markdown");
    static_assert(constexpr_lookup(kMimeTypes, std::string_view{".xyz"}) == nullptr);

    REQUIRE(true); // If we reach here, all static_asserts passed
}
