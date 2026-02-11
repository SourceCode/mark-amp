#include "core/ThemeRegistry.h"
#include "core/ThemeValidator.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <string>

using namespace markamp;
using json = nlohmann::json;

// ═══════════════════════════════════════════════════════
// Helper: Build a valid theme JSON object
// ═══════════════════════════════════════════════════════

static auto make_valid_theme_json() -> json
{
    return json{{"id", "test-theme"},
                {"name", "Test Theme"},
                {"colors",
                 {{"--bg-app", "#1e1e2e"},
                  {"--bg-panel", "#252535"},
                  {"--bg-header", "#2a2a3a"},
                  {"--bg-input", "#30304a"},
                  {"--text-main", "#cdd6f4"},
                  {"--text-muted", "#a6adc8"},
                  {"--accent-primary", "#89b4fa"},
                  {"--accent-secondary", "#f5c2e7"},
                  {"--border-light", "#45475a"},
                  {"--border-dark", "#313244"}}}};
}

/// Write JSON to a temp file and return the path.
static auto write_temp_theme(const json& j, const std::string& filename) -> std::filesystem::path
{
    auto dir = std::filesystem::temp_directory_path() / "markamp_test_themes";
    std::filesystem::create_directories(dir);
    auto file_path = dir / filename;
    std::ofstream ofs(file_path);
    ofs << j.dump(2);
    ofs.close();
    return file_path;
}

/// Clean up temp directory after tests.
static void cleanup_temp()
{
    auto dir = std::filesystem::temp_directory_path() / "markamp_test_themes";
    std::error_code ec;
    std::filesystem::remove_all(dir, ec);
}

// ═══════════════════════════════════════════════════════
// ThemeValidator: Structural Checks
// ═══════════════════════════════════════════════════════

TEST_CASE("Validator accepts valid theme JSON", "[theme_import_export]")
{
    core::ThemeValidator validator;
    auto j = make_valid_theme_json();
    auto result = validator.validate_json(j);
    REQUIRE(result.is_valid);
    REQUIRE(result.errors.empty());
}

TEST_CASE("Validator rejects JSON without name field", "[theme_import_export]")
{
    core::ThemeValidator validator;
    auto j = make_valid_theme_json();
    j.erase("name");
    auto result = validator.validate_json(j);
    REQUIRE_FALSE(result.is_valid);
    REQUIRE_FALSE(result.errors.empty());
    REQUIRE_THAT(result.errors.front(), Catch::Matchers::ContainsSubstring("name"));
}

TEST_CASE("Validator rejects JSON without colors object", "[theme_import_export]")
{
    core::ThemeValidator validator;
    auto j = make_valid_theme_json();
    j.erase("colors");
    auto result = validator.validate_json(j);
    REQUIRE_FALSE(result.is_valid);
    REQUIRE_THAT(result.errors.front(), Catch::Matchers::ContainsSubstring("colors"));
}

TEST_CASE("Validator rejects JSON with non-object colors", "[theme_import_export]")
{
    core::ThemeValidator validator;
    auto j = make_valid_theme_json();
    j["colors"] = "not-an-object";
    auto result = validator.validate_json(j);
    REQUIRE_FALSE(result.is_valid);
    REQUIRE_THAT(result.errors.front(), Catch::Matchers::ContainsSubstring("colors"));
}

TEST_CASE("Validator rejects JSON missing a required color key", "[theme_import_export]")
{
    core::ThemeValidator validator;
    auto j = make_valid_theme_json();
    j["colors"].erase("--bg-app");
    auto result = validator.validate_json(j);
    REQUIRE_FALSE(result.is_valid);
    REQUIRE_THAT(result.errors.front(), Catch::Matchers::ContainsSubstring("--bg-app"));
}

TEST_CASE("Validator flags error for invalid color value", "[theme_import_export]")
{
    core::ThemeValidator validator;
    auto j = make_valid_theme_json();
    j["colors"]["--bg-app"] = "not-a-color";
    auto result = validator.validate_json(j);
    REQUIRE_FALSE(result.is_valid);
    REQUIRE_THAT(result.errors.front(), Catch::Matchers::ContainsSubstring("--bg-app"));
    REQUIRE_THAT(result.errors.front(), Catch::Matchers::ContainsSubstring("invalid color"));
}

TEST_CASE("Validator flags error for non-string color value", "[theme_import_export]")
{
    core::ThemeValidator validator;
    auto j = make_valid_theme_json();
    j["colors"]["--text-main"] = 12345;
    auto result = validator.validate_json(j);
    REQUIRE_FALSE(result.is_valid);
    REQUIRE_THAT(result.errors.front(), Catch::Matchers::ContainsSubstring("--text-main"));
}

// ═══════════════════════════════════════════════════════
// ThemeValidator: Contrast Warnings
// ═══════════════════════════════════════════════════════

TEST_CASE("Validator issues contrast warning for low-contrast text on bg", "[theme_import_export]")
{
    core::ThemeValidator validator;

    // Create a theme with nearly identical text and background colors
    core::Theme low_contrast;
    low_contrast.id = "low-contrast";
    low_contrast.name = "Low Contrast";
    low_contrast.colors.bg_app = *core::Color::from_string("#222222");
    low_contrast.colors.bg_panel = *core::Color::from_string("#333333");
    low_contrast.colors.bg_header = *core::Color::from_string("#444444");
    low_contrast.colors.bg_input = *core::Color::from_string("#555555");
    // Low contrast: dark text on dark background
    low_contrast.colors.text_main = *core::Color::from_string("#333333");
    low_contrast.colors.text_muted = *core::Color::from_string("#444444");
    low_contrast.colors.accent_primary = *core::Color::from_string("#89b4fa");
    low_contrast.colors.accent_secondary = *core::Color::from_string("#f5c2e7");
    low_contrast.colors.border_light = *core::Color::from_string("#45475a");
    low_contrast.colors.border_dark = *core::Color::from_string("#313244");

    auto result = validator.validate_theme(low_contrast);
    REQUIRE_FALSE(result.warnings.empty());
    // Check at least one warning mentions contrast
    bool has_contrast_warning = false;
    for (const auto& w : result.warnings)
    {
        if (w.find("contrast") != std::string::npos)
        {
            has_contrast_warning = true;
            break;
        }
    }
    REQUIRE(has_contrast_warning);
}

TEST_CASE("Validator warns when all bg colors are identical", "[theme_import_export]")
{
    core::ThemeValidator validator;
    core::Theme same_bg;
    same_bg.id = "same-bg";
    same_bg.name = "Same Bg";
    same_bg.colors.bg_app = *core::Color::from_string("#222222");
    same_bg.colors.bg_panel = *core::Color::from_string("#222222");
    same_bg.colors.bg_header = *core::Color::from_string("#222222");
    same_bg.colors.bg_input = *core::Color::from_string("#555555");
    same_bg.colors.text_main = *core::Color::from_string("#ffffff");
    same_bg.colors.text_muted = *core::Color::from_string("#cccccc");
    same_bg.colors.accent_primary = *core::Color::from_string("#89b4fa");
    same_bg.colors.accent_secondary = *core::Color::from_string("#f5c2e7");
    same_bg.colors.border_light = *core::Color::from_string("#45475a");
    same_bg.colors.border_dark = *core::Color::from_string("#313244");

    auto result = validator.validate_theme(same_bg);
    bool has_identical_warning = false;
    for (const auto& w : result.warnings)
    {
        if (w.find("identical") != std::string::npos)
        {
            has_identical_warning = true;
            break;
        }
    }
    REQUIRE(has_identical_warning);
}

TEST_CASE("Validator produces no contrast warnings for good contrast", "[theme_import_export]")
{
    core::ThemeValidator validator;
    core::Theme good_theme;
    good_theme.id = "good";
    good_theme.name = "Good Theme";
    good_theme.colors.bg_app = *core::Color::from_string("#1e1e2e");
    good_theme.colors.bg_panel = *core::Color::from_string("#252535");
    good_theme.colors.bg_header = *core::Color::from_string("#2a2a3a");
    good_theme.colors.bg_input = *core::Color::from_string("#30304a");
    good_theme.colors.text_main = *core::Color::from_string("#cdd6f4");
    good_theme.colors.text_muted = *core::Color::from_string("#a6adc8");
    good_theme.colors.accent_primary = *core::Color::from_string("#89b4fa");
    good_theme.colors.accent_secondary = *core::Color::from_string("#f5c2e7");
    good_theme.colors.border_light = *core::Color::from_string("#45475a");
    good_theme.colors.border_dark = *core::Color::from_string("#313244");

    auto result = validator.validate_theme(good_theme);
    bool has_contrast_warning = false;
    for (const auto& w : result.warnings)
    {
        if (w.find("contrast") != std::string::npos)
        {
            has_contrast_warning = true;
            break;
        }
    }
    REQUIRE_FALSE(has_contrast_warning);
}

// ═══════════════════════════════════════════════════════
// ThemeRegistry: Filename Sanitization
// ═══════════════════════════════════════════════════════

TEST_CASE("sanitize_filename lowercases and replaces spaces", "[theme_import_export]")
{
    REQUIRE(core::ThemeRegistry::sanitize_filename("Dark Mode") == "dark_mode");
}

TEST_CASE("sanitize_filename replaces hyphens", "[theme_import_export]")
{
    REQUIRE(core::ThemeRegistry::sanitize_filename("My-Theme") == "my_theme");
}

TEST_CASE("sanitize_filename strips special characters", "[theme_import_export]")
{
    REQUIRE(core::ThemeRegistry::sanitize_filename("Theme! (v2)") == "theme_v2");
}

TEST_CASE("sanitize_filename handles consecutive separators", "[theme_import_export]")
{
    REQUIRE(core::ThemeRegistry::sanitize_filename("A   B---C") == "a_b_c");
}

TEST_CASE("sanitize_filename handles leading/trailing separators", "[theme_import_export]")
{
    auto result = core::ThemeRegistry::sanitize_filename("  test  ");
    REQUIRE(result == "test");
}

// ═══════════════════════════════════════════════════════
// ThemeRegistry: Import/Export Round-Trip
// ═══════════════════════════════════════════════════════

TEST_CASE("Registry imports a valid theme from file", "[theme_import_export]")
{
    auto j = make_valid_theme_json();
    auto path = write_temp_theme(j, "valid_import.json");

    core::ThemeRegistry registry;
    registry.initialize();
    size_t original_count = registry.theme_count();

    auto result = registry.import_theme(path);
    REQUIRE(result.has_value());
    REQUIRE(result->id == "test-theme");
    REQUIRE(result->name == "Test Theme");
    REQUIRE(registry.theme_count() == original_count + 1);

    cleanup_temp();
}

TEST_CASE("Registry rejects import of invalid theme JSON", "[theme_import_export]")
{
    json invalid_json = {{"not_valid", true}};
    auto path = write_temp_theme(invalid_json, "invalid_import.json");

    core::ThemeRegistry registry;
    registry.initialize();

    auto result = registry.import_theme(path);
    REQUIRE_FALSE(result.has_value());

    cleanup_temp();
}

TEST_CASE("Registry export produces valid JSON with 2-space indent", "[theme_import_export]")
{
    core::ThemeRegistry registry;
    registry.initialize();

    // Export a built-in theme
    auto themes = registry.list_themes();
    REQUIRE_FALSE(themes.empty());

    auto export_dir = std::filesystem::temp_directory_path() / "markamp_test_themes";
    std::filesystem::create_directories(export_dir);
    auto export_path = export_dir / "exported.theme.json";

    auto result = registry.export_theme(themes.front().id, export_path);
    REQUIRE(result.has_value());
    REQUIRE(std::filesystem::exists(export_path));

    // Verify it's valid JSON with expected fields
    std::ifstream ifs(export_path);
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    auto parsed = json::parse(content);
    REQUIRE(parsed.contains("id"));
    REQUIRE(parsed.contains("name"));
    REQUIRE(parsed.contains("colors"));

    // Verify 2-space indent (first indent should be "  ")
    REQUIRE(content.find("  \"") != std::string::npos);

    cleanup_temp();
}

TEST_CASE("Import then export round-trip preserves theme data", "[theme_import_export]")
{
    auto j = make_valid_theme_json();
    auto import_path = write_temp_theme(j, "roundtrip_input.json");

    core::ThemeRegistry registry;
    registry.initialize();

    auto import_result = registry.import_theme(import_path);
    REQUIRE(import_result.has_value());

    auto export_dir = std::filesystem::temp_directory_path() / "markamp_test_themes";
    auto export_path = export_dir / "roundtrip_output.json";
    auto export_result = registry.export_theme("test-theme", export_path);
    REQUIRE(export_result.has_value());

    // Read back and verify fields match
    std::ifstream ifs(export_path);
    auto exported = json::parse(ifs);
    REQUIRE(exported["id"] == "test-theme");
    REQUIRE(exported["name"] == "Test Theme");
    REQUIRE(exported["colors"]["--bg-app"].is_string());

    cleanup_temp();
}

// ═══════════════════════════════════════════════════════
// ThemeRegistry: ID Auto-generation
// ═══════════════════════════════════════════════════════

TEST_CASE("Import auto-generates ID from name when ID is missing", "[theme_import_export]")
{
    auto j = make_valid_theme_json();
    j.erase("id");
    j["name"] = "My Custom Theme";
    auto path = write_temp_theme(j, "no_id.json");

    core::ThemeRegistry registry;
    registry.initialize();

    auto result = registry.import_theme(path);
    REQUIRE(result.has_value());
    REQUIRE(result->id == "my-custom-theme");

    cleanup_temp();
}

TEST_CASE("Import auto-generates ID from name when ID is empty", "[theme_import_export]")
{
    auto j = make_valid_theme_json();
    j["id"] = "";
    j["name"] = "Another Theme";
    auto path = write_temp_theme(j, "empty_id.json");

    core::ThemeRegistry registry;
    registry.initialize();

    auto result = registry.import_theme(path);
    REQUIRE(result.has_value());
    REQUIRE(result->id == "another-theme");

    cleanup_temp();
}

// ═══════════════════════════════════════════════════════
// ThemeRegistry: Duplicate Handling
// ═══════════════════════════════════════════════════════

TEST_CASE("Import with duplicate custom theme ID overwrites existing", "[theme_import_export]")
{
    core::ThemeRegistry registry;
    registry.initialize();

    // Import theme once
    auto j = make_valid_theme_json();
    j["name"] = "Original Name";
    auto path1 = write_temp_theme(j, "dup1.json");
    auto result1 = registry.import_theme(path1);
    REQUIRE(result1.has_value());

    size_t count_after_first = registry.theme_count();

    // Import again with same ID but different name
    j["name"] = "Updated Name";
    auto path2 = write_temp_theme(j, "dup2.json");
    auto result2 = registry.import_theme(path2);
    REQUIRE(result2.has_value());

    // Count should be the same (overwrite, not add)
    REQUIRE(registry.theme_count() == count_after_first);
    // The theme should have the updated name
    auto theme = registry.get_theme("test-theme");
    REQUIRE(theme.has_value());
    REQUIRE(theme->name == "Updated Name");

    cleanup_temp();
}

TEST_CASE("Import with built-in theme ID generates custom ID", "[theme_import_export]")
{
    core::ThemeRegistry registry;
    registry.initialize();

    // Get first built-in theme ID
    auto themes = registry.list_themes();
    REQUIRE_FALSE(themes.empty());
    auto builtin_id = themes.front().id;

    // Import a theme with the same ID as a built-in
    auto j = make_valid_theme_json();
    j["id"] = builtin_id;
    j["name"] = "My Clone";
    auto path = write_temp_theme(j, "builtin_clone.json");

    size_t original_count = registry.theme_count();
    auto result = registry.import_theme(path);
    REQUIRE(result.has_value());

    // Should have added a new theme (not overwritten the built-in)
    REQUIRE(registry.theme_count() == original_count + 1);
    // The ID should differ from the built-in
    REQUIRE(result->id != builtin_id);
    // The name should have " (Custom)" suffix
    REQUIRE_THAT(result->name, Catch::Matchers::ContainsSubstring("(Custom)"));

    cleanup_temp();
}

// ═══════════════════════════════════════════════════════
// ThemeRegistry: has_theme / is_builtin
// ═══════════════════════════════════════════════════════

TEST_CASE("has_theme returns true for existing theme", "[theme_import_export]")
{
    core::ThemeRegistry registry;
    registry.initialize();
    auto themes = registry.list_themes();
    REQUIRE_FALSE(themes.empty());
    REQUIRE(registry.has_theme(themes.front().id));
}

TEST_CASE("has_theme returns false for missing theme", "[theme_import_export]")
{
    core::ThemeRegistry registry;
    registry.initialize();
    REQUIRE_FALSE(registry.has_theme("nonexistent-theme-12345"));
}

TEST_CASE("is_builtin returns true for built-in themes", "[theme_import_export]")
{
    core::ThemeRegistry registry;
    registry.initialize();
    auto themes = registry.list_themes();
    // At least the first theme should be built-in
    bool found_builtin = false;
    for (const auto& t : themes)
    {
        if (t.is_builtin)
        {
            REQUIRE(registry.is_builtin(t.id));
            found_builtin = true;
            break;
        }
    }
    REQUIRE(found_builtin);
}

TEST_CASE("is_builtin returns false for imported theme", "[theme_import_export]")
{
    core::ThemeRegistry registry;
    registry.initialize();

    auto j = make_valid_theme_json();
    auto path = write_temp_theme(j, "custom_check.json");
    auto result = registry.import_theme(path);
    REQUIRE(result.has_value());
    REQUIRE_FALSE(registry.is_builtin(result->id));

    cleanup_temp();
}

// ═══════════════════════════════════════════════════════
// ThemeRegistry: Delete
// ═══════════════════════════════════════════════════════

TEST_CASE("delete_theme rejects built-in themes", "[theme_import_export]")
{
    core::ThemeRegistry registry;
    registry.initialize();
    auto themes = registry.list_themes();
    REQUIRE_FALSE(themes.empty());

    // Find a built-in theme
    std::string builtin_id;
    for (const auto& t : themes)
    {
        if (t.is_builtin)
        {
            builtin_id = t.id;
            break;
        }
    }
    REQUIRE_FALSE(builtin_id.empty());

    auto result = registry.delete_theme(builtin_id);
    REQUIRE_FALSE(result.has_value());
    REQUIRE_THAT(result.error(), Catch::Matchers::ContainsSubstring("built-in"));
}

TEST_CASE("delete_theme removes imported theme", "[theme_import_export]")
{
    core::ThemeRegistry registry;
    registry.initialize();

    auto j = make_valid_theme_json();
    auto path = write_temp_theme(j, "to_delete.json");
    auto import_result = registry.import_theme(path);
    REQUIRE(import_result.has_value());

    size_t count_before = registry.theme_count();
    auto delete_result = registry.delete_theme("test-theme");
    REQUIRE(delete_result.has_value());
    REQUIRE(registry.theme_count() == count_before - 1);
    REQUIRE_FALSE(registry.has_theme("test-theme"));

    cleanup_temp();
}

TEST_CASE("delete_theme returns error for nonexistent theme", "[theme_import_export]")
{
    core::ThemeRegistry registry;
    registry.initialize();
    auto result = registry.delete_theme("nonexistent-id-12345");
    REQUIRE_FALSE(result.has_value());
    REQUIRE_THAT(result.error(), Catch::Matchers::ContainsSubstring("not found"));
}

// ═══════════════════════════════════════════════════════
// ThemeRegistry: Export Errors
// ═══════════════════════════════════════════════════════

TEST_CASE("export_theme returns error for nonexistent theme", "[theme_import_export]")
{
    core::ThemeRegistry registry;
    registry.initialize();
    auto export_path = std::filesystem::temp_directory_path() / "markamp_test_themes" / "no.json";
    auto result = registry.export_theme("nonexistent-id-12345", export_path);
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("import_theme returns error for nonexistent file", "[theme_import_export]")
{
    core::ThemeRegistry registry;
    registry.initialize();
    auto result = registry.import_theme("/tmp/markamp_nonexistent_file.json");
    REQUIRE_FALSE(result.has_value());
}
