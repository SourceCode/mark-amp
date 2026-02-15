/// @file test_theme_marketplace.cpp
/// @brief V4 Phase 23 – ThemeMarketplace unit tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/HttpClient.h"
#include "core/ThemeMarketplace.h"
#include "core/ThemeRegistry.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

namespace
{

struct TestFixture
{
    EventBus event_bus;
    Config config;
    ThemeRegistry theme_registry;
    HttpClient http_client;
    ThemeMarketplace marketplace{event_bus, config, theme_registry, http_client};
};

ThemeListingInfo
make_listing(const std::string& id, const std::string& name, const std::string& version = "1.0.0")
{
    ThemeListingInfo listing;
    listing.id = id;
    listing.name = name;
    listing.version = version;
    listing.author = "test_author";
    listing.description = "Test theme";
    listing.theme_content = "/* theme CSS */";
    return listing;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// Test 1: Fetch listings (empty by default)
// ---------------------------------------------------------------------------
TEST_CASE("ThemeMarketplace: fetch listings returns empty", "[theme_marketplace]")
{
    TestFixture fixture;
    auto listings = fixture.marketplace.fetch_listings();
    CHECK(listings.empty());
}

// ---------------------------------------------------------------------------
// Test 2: Search filter (fetch with search param)
// ---------------------------------------------------------------------------
TEST_CASE("ThemeMarketplace: search filter param accepted", "[theme_marketplace]")
{
    TestFixture fixture;
    auto listings = fixture.marketplace.fetch_listings("dark", "minimal");
    CHECK(listings.empty()); // Default stub returns empty
}

// ---------------------------------------------------------------------------
// Test 3: Install theme
// ---------------------------------------------------------------------------
TEST_CASE("ThemeMarketplace: install theme", "[theme_marketplace]")
{
    TestFixture fixture;

    auto listing = make_listing("dark-theme", "Dark Theme");
    auto result = fixture.marketplace.install_theme(listing);
    REQUIRE(result.has_value());

    // Verify installed
    const auto& installed = fixture.marketplace.installed_themes();
    CHECK(installed.size() == 1);
    CHECK(installed[0] == "dark-theme");
}

// ---------------------------------------------------------------------------
// Test 4: Uninstall theme
// ---------------------------------------------------------------------------
TEST_CASE("ThemeMarketplace: uninstall theme", "[theme_marketplace]")
{
    TestFixture fixture;

    auto listing = make_listing("removable", "Removable Theme");
    fixture.marketplace.install_theme(listing);
    REQUIRE(fixture.marketplace.installed_themes().size() == 1);

    auto result = fixture.marketplace.uninstall_theme("removable");
    REQUIRE(result.has_value());
    CHECK(fixture.marketplace.installed_themes().empty());
}

// ---------------------------------------------------------------------------
// Test 5: Check updates (returns empty by default)
// ---------------------------------------------------------------------------
TEST_CASE("ThemeMarketplace: check updates", "[theme_marketplace]")
{
    TestFixture fixture;
    fixture.marketplace.mark_installed("theme-v1");

    auto updates = fixture.marketplace.check_updates();
    CHECK(updates.empty());
}

// ---------------------------------------------------------------------------
// Test 6: Export theme
// ---------------------------------------------------------------------------
TEST_CASE("ThemeMarketplace: export theme", "[theme_marketplace]")
{
    TestFixture fixture;

    auto package = fixture.marketplace.export_theme("my-theme", "/* custom CSS */");
    CHECK(package.theme_content == "/* custom CSS */");
    CHECK_FALSE(package.metadata_json.empty());
    CHECK(package.metadata_json.find("my-theme") != std::string::npos);
}

// ---------------------------------------------------------------------------
// Test 7: Import theme
// ---------------------------------------------------------------------------
TEST_CASE("ThemeMarketplace: import theme", "[theme_marketplace]")
{
    TestFixture fixture;

    ThemeExportPackage package;
    package.theme_content = "/* imported theme */";
    package.metadata_json = R"({"id":"imported-theme"})";

    auto result = fixture.marketplace.import_theme(package);
    REQUIRE(result.has_value());

    const auto& installed = fixture.marketplace.installed_themes();
    CHECK(installed.size() == 1);
    CHECK(installed[0] == "imported-theme");
}

// ---------------------------------------------------------------------------
// Test 8: Rate theme (no crash)
// ---------------------------------------------------------------------------
TEST_CASE("ThemeMarketplace: rate theme", "[theme_marketplace]")
{
    TestFixture fixture;
    // Should not throw
    fixture.marketplace.rate_theme("any-theme", 5);
    CHECK(true);
}

// ---------------------------------------------------------------------------
// Test 9: Installed flag tracking
// ---------------------------------------------------------------------------
TEST_CASE("ThemeMarketplace: installed flag tracking", "[theme_marketplace]")
{
    TestFixture fixture;

    fixture.marketplace.mark_installed("theme-a");
    fixture.marketplace.mark_installed("theme-b");
    fixture.marketplace.mark_installed("theme-a"); // Duplicate

    CHECK(fixture.marketplace.installed_themes().size() == 2);

    fixture.marketplace.mark_uninstalled("theme-a");
    CHECK(fixture.marketplace.installed_themes().size() == 1);
    CHECK(fixture.marketplace.installed_themes()[0] == "theme-b");
}

// ---------------------------------------------------------------------------
// Test 10: Error handling (empty theme)
// ---------------------------------------------------------------------------
TEST_CASE("ThemeMarketplace: error handling", "[theme_marketplace]")
{
    TestFixture fixture;

    // Install with empty ID
    ThemeListingInfo empty_listing;
    auto result = fixture.marketplace.install_theme(empty_listing);
    CHECK_FALSE(result.has_value());

    // Uninstall nonexistent
    auto uninstall_result = fixture.marketplace.uninstall_theme("nonexistent");
    CHECK_FALSE(uninstall_result.has_value());

    // Import empty package
    ThemeExportPackage empty_pkg;
    auto import_result = fixture.marketplace.import_theme(empty_pkg);
    CHECK_FALSE(import_result.has_value());
}
