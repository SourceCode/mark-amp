#include "core/ThemeEngine.h"
#include "ui/FileTypeIconRegistry.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp;
using namespace markamp::ui;

TEST_CASE("FileTypeIconRegistry mapping", "[ui][tab_bar]")
{
    FileTypeIconRegistry registry;

    SECTION("Exact extension mapping")
    {
        REQUIRE(registry.GetIconId("main.cpp") == FileTypeIconId::kCpp);
        REQUIRE(registry.GetIconId("header.h") == FileTypeIconId::kHeader);
        REQUIRE(registry.GetIconId("script.py") == FileTypeIconId::kPython);
        REQUIRE(registry.GetIconId("app.ts") == FileTypeIconId::kTypescript);
        REQUIRE(registry.GetIconId("index.js") == FileTypeIconId::kJavascript);
        REQUIRE(registry.GetIconId("README.md") == FileTypeIconId::kMarkdown);
        REQUIRE(registry.GetIconId("data.json") == FileTypeIconId::kJson);
        REQUIRE(registry.GetIconId("config.yaml") == FileTypeIconId::kYaml);
        REQUIRE(registry.GetIconId("style.css") == FileTypeIconId::kCss);
        REQUIRE(registry.GetIconId("page.html") == FileTypeIconId::kHtml);
        REQUIRE(registry.GetIconId("Cargo.toml") == FileTypeIconId::kToml);
        REQUIRE(registry.GetIconId("CMakeLists.txt") == FileTypeIconId::kCMake);
    }

    SECTION("Case insensitive extension mapping")
    {
        REQUIRE(registry.GetIconId("SOURCE.CPP") == FileTypeIconId::kCpp);
        REQUIRE(registry.GetIconId("Data.JSON") == FileTypeIconId::kJson);
    }

    SECTION("Fallback to unknown generic file")
    {
        REQUIRE(registry.GetIconId("unknown.xyz") == FileTypeIconId::kUnknown);
        REQUIRE(registry.GetIconId("noextension") == FileTypeIconId::kUnknown);
    }
}
