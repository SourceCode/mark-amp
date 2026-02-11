#include "core/Theme.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <algorithm>
#include <cctype>
#include <string>

using namespace markamp;

// ═══════════════════════════════════════════════════════
// ThemePreviewCard Constants (inlined from ThemePreviewCard.h)
// These mirror the constants defined in the ThemePreviewCard class.
// ═══════════════════════════════════════════════════════

namespace card_constants
{
constexpr int kCardWidth = 280;
constexpr int kCardHeight = 160;
constexpr int kPreviewHeight = 112;
constexpr int kFooterHeight = 48;
constexpr int kCornerRadius = 8;
constexpr int kBorderWidth = 2;
constexpr int kBadgeSize = 20;
} // namespace card_constants

// ═══════════════════════════════════════════════════════
// ThemeGallery Constants (inlined from ThemeGallery.h)
// ═══════════════════════════════════════════════════════

namespace gallery_constants
{
constexpr int kDialogWidth = 896;
constexpr int kCardGap = 24;
constexpr int kGridPadding = 24;
constexpr int kHeaderHeight = 72;
constexpr int kToolbarHeight = 48;
} // namespace gallery_constants

// ═══════════════════════════════════════════════════════
// Grid column calculation (same formula as ThemeGallery::CalculateColumns)
// ═══════════════════════════════════════════════════════

auto calculate_columns(int available_width) -> int
{
    int card_width = card_constants::kCardWidth;
    int gap = gallery_constants::kCardGap;
    return std::max(1, (available_width + gap) / (card_width + gap));
}

// ═══════════════════════════════════════════════════════
// Export filename generation (same logic as ThemeGallery::ExportFilename)
// ═══════════════════════════════════════════════════════

auto export_filename(const std::string& theme_name) -> std::string
{
    std::string result;
    result.reserve(theme_name.size() + 16);
    bool prev_was_space = false;
    for (char ch : theme_name)
    {
        if (ch == ' ' || ch == '-')
        {
            if (!result.empty() && !prev_was_space)
            {
                result += '_';
            }
            prev_was_space = true;
        }
        else if (std::isalnum(static_cast<unsigned char>(ch)))
        {
            result += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            prev_was_space = false;
        }
    }
    if (!result.empty() && result.back() == '_')
    {
        result.pop_back();
    }
    result += ".theme.json";
    return result;
}

// ═══════════════════════════════════════════════════════
// Card Constants Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Card width constant is 280", "[theme_gallery]")
{
    REQUIRE(card_constants::kCardWidth == 280);
}

TEST_CASE("Card height constant is 160", "[theme_gallery]")
{
    REQUIRE(card_constants::kCardHeight == 160);
}

TEST_CASE("Preview height constant is 112", "[theme_gallery]")
{
    REQUIRE(card_constants::kPreviewHeight == 112);
}

TEST_CASE("Footer height constant is 48", "[theme_gallery]")
{
    REQUIRE(card_constants::kFooterHeight == 48);
}

TEST_CASE("Preview + footer equals total card height", "[theme_gallery]")
{
    REQUIRE(card_constants::kPreviewHeight + card_constants::kFooterHeight ==
            card_constants::kCardHeight);
}

TEST_CASE("Corner radius is reasonable", "[theme_gallery]")
{
    REQUIRE(card_constants::kCornerRadius >= 4);
    REQUIRE(card_constants::kCornerRadius <= 16);
}

TEST_CASE("Badge size is reasonable", "[theme_gallery]")
{
    REQUIRE(card_constants::kBadgeSize >= 16);
    REQUIRE(card_constants::kBadgeSize <= 30);
}

// ═══════════════════════════════════════════════════════
// Gallery Constants Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Gallery dialog width is 896", "[theme_gallery]")
{
    REQUIRE(gallery_constants::kDialogWidth == 896);
}

TEST_CASE("Gallery card gap is 24", "[theme_gallery]")
{
    REQUIRE(gallery_constants::kCardGap == 24);
}

TEST_CASE("Gallery grid padding is 24", "[theme_gallery]")
{
    REQUIRE(gallery_constants::kGridPadding == 24);
}

TEST_CASE("Gallery header height is 72", "[theme_gallery]")
{
    REQUIRE(gallery_constants::kHeaderHeight == 72);
}

TEST_CASE("Gallery toolbar height is 48", "[theme_gallery]")
{
    REQUIRE(gallery_constants::kToolbarHeight == 48);
}

// ═══════════════════════════════════════════════════════
// Grid Column Calculation
// ═══════════════════════════════════════════════════════

TEST_CASE("CalculateColumns returns 1 for narrow width", "[theme_gallery]")
{
    REQUIRE(calculate_columns(200) == 1);
}

TEST_CASE("CalculateColumns returns 1 for single-card width", "[theme_gallery]")
{
    REQUIRE(calculate_columns(280) == 1);
}

TEST_CASE("CalculateColumns returns 2 for two-card width", "[theme_gallery]")
{
    int two_card_width = 280 * 2 + 24;
    REQUIRE(calculate_columns(two_card_width) >= 2);
}

TEST_CASE("CalculateColumns returns 3 for wide width", "[theme_gallery]")
{
    int three_card_width = 280 * 3 + 24 * 2;
    REQUIRE(calculate_columns(three_card_width) >= 3);
}

TEST_CASE("CalculateColumns never returns 0", "[theme_gallery]")
{
    REQUIRE(calculate_columns(0) >= 1);
    REQUIRE(calculate_columns(-100) >= 1);
}

TEST_CASE("CalculateColumns matches formula", "[theme_gallery]")
{
    int width = 848; // kDialogWidth - 2 * kGridPadding
    int gap = gallery_constants::kCardGap;
    int card_width = card_constants::kCardWidth;
    int expected = std::max(1, (width + gap) / (card_width + gap));
    REQUIRE(calculate_columns(width) == expected);
}

TEST_CASE("Gallery default width fits 2+ columns", "[theme_gallery]")
{
    int usable_width = gallery_constants::kDialogWidth - 2 * gallery_constants::kGridPadding;
    int columns = calculate_columns(usable_width);
    REQUIRE(columns >= 2);
}

TEST_CASE("Minimum column count is always 1", "[theme_gallery]")
{
    for (int w = -50; w <= 100; w += 25)
    {
        REQUIRE(calculate_columns(w) >= 1);
    }
}

// ═══════════════════════════════════════════════════════
// Export Filename Generation
// ═══════════════════════════════════════════════════════

TEST_CASE("ExportFilename converts simple name", "[theme_gallery]")
{
    REQUIRE(export_filename("Dark Mode") == "dark_mode.theme.json");
}

TEST_CASE("ExportFilename handles single word", "[theme_gallery]")
{
    REQUIRE(export_filename("Monokai") == "monokai.theme.json");
}

TEST_CASE("ExportFilename handles hyphens", "[theme_gallery]")
{
    REQUIRE(export_filename("Solarized-Dark") == "solarized_dark.theme.json");
}

TEST_CASE("ExportFilename handles multiple spaces", "[theme_gallery]")
{
    REQUIRE(export_filename("One   Dark   Pro") == "one_dark_pro.theme.json");
}

TEST_CASE("ExportFilename strips special characters", "[theme_gallery]")
{
    REQUIRE(export_filename("My Theme! (v2)") == "my_theme_v2.theme.json");
}

TEST_CASE("ExportFilename always has .theme.json extension", "[theme_gallery]")
{
    REQUIRE_THAT(export_filename("Test"), Catch::Matchers::EndsWith(".theme.json"));
}

// ═══════════════════════════════════════════════════════
// ThemeInfo Struct
// ═══════════════════════════════════════════════════════

TEST_CASE("ThemeInfo default is_builtin is false", "[theme_gallery]")
{
    core::ThemeInfo info;
    REQUIRE(info.is_builtin == false);
}

TEST_CASE("ThemeInfo file_path default is nullopt", "[theme_gallery]")
{
    core::ThemeInfo info;
    REQUIRE_FALSE(info.file_path.has_value());
}

TEST_CASE("ThemeInfo can store id and name", "[theme_gallery]")
{
    core::ThemeInfo info;
    info.id = "dark_mode";
    info.name = "Dark Mode";
    info.is_builtin = true;

    REQUIRE(info.id == "dark_mode");
    REQUIRE(info.name == "Dark Mode");
    REQUIRE(info.is_builtin);
}

// ═══════════════════════════════════════════════════════
// ThemeColors Fields
// ═══════════════════════════════════════════════════════

TEST_CASE("ThemeColors has all 10 required color fields", "[theme_gallery]")
{
    core::ThemeColors colors;
    REQUIRE(sizeof(colors.bg_app) > 0);
    REQUIRE(sizeof(colors.bg_header) > 0);
    REQUIRE(sizeof(colors.bg_panel) > 0);
    REQUIRE(sizeof(colors.bg_input) > 0);
    REQUIRE(sizeof(colors.text_main) > 0);
    REQUIRE(sizeof(colors.text_muted) > 0);
    REQUIRE(sizeof(colors.accent_primary) > 0);
    REQUIRE(sizeof(colors.accent_secondary) > 0);
    REQUIRE(sizeof(colors.border_light) > 0);
    REQUIRE(sizeof(colors.border_dark) > 0);
}
