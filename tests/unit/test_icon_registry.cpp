#include "ui/IconRegistry.h"

#include <catch2/catch_test_macros.hpp>

#include <fstream>

using namespace markamp::ui;

TEST_CASE("IconRegistry: basic operations", "[icon_registry]")
{
    IconRegistry registry;

    CHECK(registry.size() == 0);

    std::string valid_svg = R"(
        <svg viewBox="0 0 24 24" fill="none">
            <path d="M 0 0 L 24 24" fill="red" />
        </svg>
    )";

    std::string invalid_svg = "<invalid>";

    // Test register_icon
    bool success1 = registry.register_icon("test1", valid_svg);
    CHECK(success1);
    CHECK(registry.size() == 1);
    CHECK(registry.has_icon("test1"));

    bool success2 = registry.register_icon("test2", invalid_svg);
    CHECK(!success2); // Fails to parse invalid SVG visually
    CHECK(registry.size() == 1);
    CHECK(!registry.has_icon("test2"));

    // Test get_icon
    auto doc = registry.get_icon("test1");
    REQUIRE(doc.has_value());
    CHECK(doc->viewBox().width == 24.0f);

    // Test load_icon
    std::filesystem::path temp_file = std::filesystem::temp_directory_path() / "test_icon.svg";
    {
        std::ofstream out(temp_file);
        out << valid_svg;
    }

    bool load_success = registry.load_icon("test3", temp_file);
    CHECK(load_success);
    CHECK(registry.size() == 2);
    CHECK(registry.has_icon("test3"));

    std::filesystem::remove(temp_file);

    registry.clear();
    CHECK(registry.size() == 0);
}
