/// @file test_vault_style.cpp
/// @brief V4 Phase 22 – VaultStyleService unit tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"
#include "core/VaultService.h"
#include "core/VaultStyleService.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::core;

namespace
{

struct TestFixture
{
    EventBus event_bus;
    Config config;
    ThemeRegistry theme_registry;
    ThemeEngine theme_engine{event_bus, theme_registry};
    VaultService vault_service{event_bus, config};
    VaultStyleService service{event_bus, config, vault_service, theme_engine};

    // Create a temp vault directory
    std::filesystem::path vault_path =
        std::filesystem::temp_directory_path() / "markamp_test_vault_style";

    TestFixture()
    {
        std::filesystem::create_directories(vault_path / ".markamp");
    }

    ~TestFixture()
    {
        std::error_code ec;
        std::filesystem::remove_all(vault_path, ec);
    }
};

} // anonymous namespace

// ---------------------------------------------------------------------------
// Test 1: Load existing CSS
// ---------------------------------------------------------------------------
TEST_CASE("VaultStyle: load existing CSS", "[vault_style]")
{
    TestFixture fixture;

    // Write a CSS file
    auto css_path = VaultStyleService::vault_css_path(fixture.vault_path);
    {
        std::ofstream file(css_path);
        file << ":root { --primary-color: #ff0000; }";
    }

    auto result = fixture.service.load_vault_style(fixture.vault_path);
    REQUIRE(result.has_value());
    CHECK(result->is_valid);
    CHECK_FALSE(result->css_content.empty());
}

// ---------------------------------------------------------------------------
// Test 2: No CSS file (not an error)
// ---------------------------------------------------------------------------
TEST_CASE("VaultStyle: no CSS file", "[vault_style]")
{
    TestFixture fixture;

    // Create a vault dir without styles.css
    auto empty_vault = std::filesystem::temp_directory_path() / "markamp_empty_vault_style";
    std::filesystem::create_directories(empty_vault);

    auto result = fixture.service.load_vault_style(empty_vault);
    REQUIRE(result.has_value());
    CHECK(result->is_valid);
    CHECK(result->css_content.empty());

    std::error_code ec;
    std::filesystem::remove_all(empty_vault, ec);
}

// ---------------------------------------------------------------------------
// Test 3: Merge with theme CSS
// ---------------------------------------------------------------------------
TEST_CASE("VaultStyle: merge with theme CSS", "[vault_style]")
{
    TestFixture fixture;

    // Write vault CSS
    auto css_path = VaultStyleService::vault_css_path(fixture.vault_path);
    {
        std::ofstream file(css_path);
        file << ".custom { color: red; }";
    }

    fixture.service.load_vault_style(fixture.vault_path);

    auto merged = fixture.service.get_merged_css("body { background: #fff; }");
    CHECK(merged.find("body { background: #fff; }") != std::string::npos);
    CHECK(merged.find(".custom { color: red; }") != std::string::npos);
}

// ---------------------------------------------------------------------------
// Test 4: CSS validation valid
// ---------------------------------------------------------------------------
TEST_CASE("VaultStyle: CSS validation valid", "[vault_style]")
{
    TestFixture fixture;

    auto [valid, error] =
        fixture.service.validate_css(":root { --var: #000; }\n.class { color: blue; }");
    CHECK(valid);
    CHECK(error.empty());
}

// ---------------------------------------------------------------------------
// Test 5: CSS validation invalid (unbalanced braces)
// ---------------------------------------------------------------------------
TEST_CASE("VaultStyle: CSS validation invalid", "[vault_style]")
{
    TestFixture fixture;

    auto [valid, error] =
        fixture.service.validate_css(":root { --var: #000; \n.class { color: blue; }");
    CHECK_FALSE(valid);
    CHECK_FALSE(error.empty());
}

// ---------------------------------------------------------------------------
// Test 6: Extract CSS variables
// ---------------------------------------------------------------------------
TEST_CASE("VaultStyle: extract variables", "[vault_style]")
{
    TestFixture fixture;

    auto vars = fixture.service.extract_variables(":root {\n"
                                                  "    --primary-color: #007acc;\n"
                                                  "    --bg-color: #1e1e1e;\n"
                                                  "    --font-size: 14px;\n"
                                                  "}");

    REQUIRE(vars.size() == 3);
    CHECK(vars[0].name == "--primary-color");
    CHECK(vars[0].value == "#007acc");
    CHECK(vars[1].name == "--bg-color");
    CHECK(vars[1].value == "#1e1e1e");
    CHECK(vars[2].name == "--font-size");
    CHECK(vars[2].value == "14px");
}

// ---------------------------------------------------------------------------
// Test 7: Save CSS
// ---------------------------------------------------------------------------
TEST_CASE("VaultStyle: save CSS", "[vault_style]")
{
    TestFixture fixture;

    auto result = fixture.service.save_vault_style(fixture.vault_path, ".new { color: green; }");
    REQUIRE(result.has_value());

    // Verify file written
    auto css_path = VaultStyleService::vault_css_path(fixture.vault_path);
    REQUIRE(std::filesystem::exists(css_path));

    std::ifstream file(css_path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    CHECK(content == ".new { color: green; }");
}

// ---------------------------------------------------------------------------
// Test 8: Generate default CSS
// ---------------------------------------------------------------------------
TEST_CASE("VaultStyle: generate default CSS", "[vault_style]")
{
    auto default_css = VaultStyleService::generate_default_css();
    CHECK_FALSE(default_css.empty());
    CHECK(default_css.find("--primary-color") != std::string::npos);
    CHECK(default_css.find(":root") != std::string::npos);
}

// ---------------------------------------------------------------------------
// Test 9: Variable override detection
// ---------------------------------------------------------------------------
TEST_CASE("VaultStyle: variable override", "[vault_style]")
{
    TestFixture fixture;

    auto vars = fixture.service.extract_variables(":root { --editor-background: #282c34; }");
    REQUIRE(vars.size() == 1);
    CHECK(vars[0].name == "--editor-background");
    CHECK(vars[0].value == "#282c34");
}

// ---------------------------------------------------------------------------
// Test 10: Vault CSS path
// ---------------------------------------------------------------------------
TEST_CASE("VaultStyle: vault CSS path", "[vault_style]")
{
    auto path = VaultStyleService::vault_css_path("/home/user/vault");
    CHECK(path.string().find(".markamp") != std::string::npos);
    CHECK(path.string().find("styles.css") != std::string::npos);
}
