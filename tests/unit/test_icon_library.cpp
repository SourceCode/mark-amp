#include "canvas/IconLibrary.h"
#include "canvas/IconObject.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::canvas;

TEST_CASE("IconLibrary: load builtins", "[icon_library]")
{
    IconLibrary library;
    library.load_builtins();

    REQUIRE(library.icon_count() >= 10);
    REQUIRE(library.get_icon("arrow-right") != nullptr);
    REQUIRE(library.get_icon("check") != nullptr);
    REQUIRE(library.get_icon("star") != nullptr);
}

TEST_CASE("IconLibrary: search icons", "[icon_library]")
{
    IconLibrary library;
    library.load_builtins();

    auto results = library.search("arrow");
    REQUIRE(results.size() >= 4); // arrow-right, arrow-left, arrow-up, arrow-down
    for (const auto* entry : results)
    {
        REQUIRE(entry->name.find("Arrow") != std::string::npos);
    }
}

TEST_CASE("IconLibrary: categories", "[icon_library]")
{
    IconLibrary library;
    library.load_builtins();

    auto cats = library.categories();
    REQUIRE(cats.size() >= 2); // "Arrows" and "Symbols"

    bool has_arrows = false;
    bool has_symbols = false;
    for (const auto& cat : cats)
    {
        if (cat == "Arrows")
        {
            has_arrows = true;
        }
        if (cat == "Symbols")
        {
            has_symbols = true;
        }
    }
    REQUIRE(has_arrows);
    REQUIRE(has_symbols);
}

TEST_CASE("IconLibrary: import pack", "[icon_library]")
{
    const auto temp_dir = std::filesystem::temp_directory_path() / "markamp_icon_test";
    std::filesystem::create_directories(temp_dir);

    // Create fake SVG files.
    for (int idx = 0; idx < 3; ++idx)
    {
        std::ofstream file(temp_dir / ("icon_" + std::to_string(idx) + ".svg"));
        file << "<svg><circle r='10'/></svg>";
    }

    IconLibrary library;
    const int imported = library.import_pack(temp_dir, "Custom");
    REQUIRE(imported == 3);
    REQUIRE(library.icon_count() == 3);

    auto custom_icons = library.icons_in_category("Custom");
    REQUIRE(custom_icons.size() == 3);

    std::filesystem::remove_all(temp_dir);
}

TEST_CASE("IconObject: JSON round-trip", "[icon_library]")
{
    IconObject icon;
    icon.set_icon_id("star");
    icon.set_icon_size(64.0);
    icon.set_icon_color({255, 200, 0, 255});

    const auto json = icon.to_json();
    REQUIRE(json.find("\"icon_id\":\"star\"") != std::string::npos);
    REQUIRE(json.find("\"size\":64") != std::string::npos);
    REQUIRE(json.find("[255,200,0,255]") != std::string::npos);
}

TEST_CASE("IconLibrary: emoji list", "[icon_library]")
{
    IconLibrary library;
    library.load_builtins();

    const auto& emojis = library.all_emojis();
    REQUIRE(emojis.size() >= 10);
}
