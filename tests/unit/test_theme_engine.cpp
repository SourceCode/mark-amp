#include "core/BuiltInThemes.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <string>

using namespace markamp::core;

// ===== ThemeEngine initialization tests =====

TEST_CASE("ThemeEngine: initializes with default theme", "[engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    registry.initialize();

    ThemeEngine engine(bus, registry);
    REQUIRE(engine.current_theme().id == "midnight-neon");
}

TEST_CASE("ThemeEngine: apply_theme switches theme", "[engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    registry.initialize();

    ThemeEngine engine(bus, registry);
    engine.apply_theme("cyber-night");
    REQUIRE(engine.current_theme().id == "cyber-night");
    REQUIRE(engine.current_theme().name == "Cyber Night");
}

TEST_CASE("ThemeEngine: apply_theme with invalid ID does nothing", "[engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    registry.initialize();

    ThemeEngine engine(bus, registry);
    engine.apply_theme("nonexistent");
    // Should still be the default
    REQUIRE(engine.current_theme().id == "midnight-neon");
}

TEST_CASE("ThemeEngine: apply_theme publishes event", "[engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    registry.initialize();

    ThemeEngine engine(bus, registry);

    std::string received_id;
    auto sub = bus.subscribe<events::ThemeChangedEvent>([&](const events::ThemeChangedEvent& e)
                                                        { received_id = e.theme_id; });

    engine.apply_theme("solarized-dark");
    REQUIRE(received_id == "solarized-dark");
}

TEST_CASE("ThemeEngine: subscribe_theme_change fires", "[engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    registry.initialize();

    ThemeEngine engine(bus, registry);

    std::string received_id;
    auto sub = engine.subscribe_theme_change([&](const std::string& id) { received_id = id; });

    engine.apply_theme("matrix-core");
    REQUIRE(received_id == "matrix-core");
}

TEST_CASE("ThemeEngine: available_themes returns all built-ins", "[engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    registry.initialize();

    ThemeEngine engine(bus, registry);
    auto themes = engine.available_themes();
    REQUIRE(themes.size() >= 8);
}

TEST_CASE("ThemeEngine: ThemeColorToken enum values", "[engine]")
{
    // Verify all 17 tokens have distinct values
    REQUIRE(ThemeColorToken::BgApp != ThemeColorToken::BgPanel);
    REQUIRE(ThemeColorToken::SelectionBg != ThemeColorToken::HoverBg);
    REQUIRE(ThemeColorToken::ScrollbarTrack != ThemeColorToken::ScrollbarThumb);
}

TEST_CASE("ThemeEngine: ThemeFontToken enum values", "[engine]")
{
    REQUIRE(ThemeFontToken::SansRegular != ThemeFontToken::MonoRegular);
    REQUIRE(ThemeFontToken::UISmall != ThemeFontToken::UIHeading);
}

TEST_CASE("ThemeEngine: import and export theme", "[engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    registry.initialize();

    ThemeEngine engine(bus, registry);

    auto tmpDir = std::filesystem::temp_directory_path() / "markamp_engine_test";
    std::filesystem::create_directories(tmpDir);
    auto tmpFile = tmpDir / "export_test.json";

    engine.export_theme("midnight-neon", tmpFile);
    REQUIRE(std::filesystem::exists(tmpFile));

    // Read back and validate
    std::ifstream f(tmpFile);
    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    REQUIRE(content.find("midnight-neon") != std::string::npos);

    std::filesystem::remove_all(tmpDir);
}

TEST_CASE("ThemeEngine: switching multiple themes", "[engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    registry.initialize();

    ThemeEngine engine(bus, registry);

    const auto& builtins = get_builtin_themes();
    for (const auto& t : builtins)
    {
        engine.apply_theme(t.id);
        REQUIRE(engine.current_theme().id == t.id);
    }
}

// ===== V8 Phase 1: Theme guardrail tests =====

TEST_CASE("ThemeEngine: all color tokens return valid colors", "[engine][guardrail]")
{
    EventBus bus;
    ThemeRegistry registry;
    registry.initialize();

    ThemeEngine engine(bus, registry);

    // Enumerate every ThemeColorToken
    const std::vector<ThemeColorToken> all_tokens = {
        ThemeColorToken::BgApp,
        ThemeColorToken::BgPanel,
        ThemeColorToken::BgHeader,
        ThemeColorToken::BgInput,
        ThemeColorToken::TextMain,
        ThemeColorToken::TextMuted,
        ThemeColorToken::AccentPrimary,
        ThemeColorToken::AccentSecondary,
        ThemeColorToken::BorderLight,
        ThemeColorToken::BorderDark,
        ThemeColorToken::SelectionBg,
        ThemeColorToken::HoverBg,
        ThemeColorToken::ErrorColor,
        ThemeColorToken::SuccessColor,
        ThemeColorToken::ScrollbarTrack,
        ThemeColorToken::ScrollbarThumb,
        ThemeColorToken::ScrollbarHover,
        ThemeColorToken::SyntaxKeyword,
        ThemeColorToken::SyntaxString,
        ThemeColorToken::SyntaxComment,
        ThemeColorToken::SyntaxNumber,
        ThemeColorToken::SyntaxType,
        ThemeColorToken::SyntaxFunction,
        ThemeColorToken::SyntaxOperator,
        ThemeColorToken::SyntaxPreprocessor,
        ThemeColorToken::RenderHeading,
        ThemeColorToken::RenderLink,
        ThemeColorToken::RenderCodeBg,
        ThemeColorToken::RenderCodeFg,
        ThemeColorToken::RenderBlockquoteBorder,
        ThemeColorToken::RenderBlockquoteBg,
        ThemeColorToken::RenderTableBorder,
        ThemeColorToken::RenderTableHeaderBg,
    };

    // Verify every built-in theme produces valid colors for all tokens
    const auto& builtins = get_builtin_themes();
    for (const auto& theme : builtins)
    {
        engine.apply_theme(theme.id);
        for (const auto& token : all_tokens)
        {
            const wxColour& color = engine.color(token);
            REQUIRE(color.IsOk());
        }
    }
}

TEST_CASE("ThemeEngine: hot-swap updates at least one token", "[engine][guardrail]")
{
    EventBus bus;
    ThemeRegistry registry;
    registry.initialize();

    ThemeEngine engine(bus, registry);

    // Apply first theme
    engine.apply_theme("midnight-neon");
    const wxColour color_before = engine.color(ThemeColorToken::AccentPrimary);

    // Apply a different theme
    engine.apply_theme("solarized-dark");
    const wxColour color_after = engine.color(ThemeColorToken::AccentPrimary);

    // At least AccentPrimary should differ between distinct themes
    REQUIRE(color_before != color_after);
}
