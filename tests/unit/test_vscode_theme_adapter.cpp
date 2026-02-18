/// test_vscode_theme_adapter.cpp — Unit tests
#include "core/VsCodeThemeAdapter.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("ThemeCompatibility: enum values", "[vscode_theme_adapter]")
{
    REQUIRE(static_cast<uint8_t>(ThemeCompatibility::kFull) != static_cast<uint8_t>(ThemeCompatibility::kPartial));
}

TEST_CASE("VsCodeTokenRule: default values", "[vscode_theme_adapter]")
{
    VsCodeTokenRule rule;
    REQUIRE(rule.scope.empty());
}

TEST_CASE("VsCodeThemeAdapter: compiles", "[vscode_theme_adapter]")
{
    static_assert(sizeof(VsCodeThemeAdapter) > 0);
}
