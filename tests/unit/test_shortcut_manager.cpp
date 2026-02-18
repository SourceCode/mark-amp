/// test_shortcut_manager.cpp — Unit tests
#include "core/ShortcutManager.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("Shortcut: default construction", "[shortcut_manager]")
{
    Shortcut sc;
    REQUIRE(sc.id.empty());
}

TEST_CASE("ShortcutManager: compiles", "[shortcut_manager]")
{
    static_assert(sizeof(ShortcutManager) > 0);
}
