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
