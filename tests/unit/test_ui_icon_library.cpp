#include "ui/IconLibrary.h"
#include "ui/IconRegistry.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("UI IconLibrary: register core icons", "[ui_icon_library]")
{
    IconRegistry registry;
    CHECK(registry.size() == 0);

    RegisterCoreIcons(registry);

    // Check an activity bar icon
    CHECK(registry.has_icon("activity-explorer"));
    CHECK(registry.has_icon("activity-settings"));

    // Check a toolbar icon
    CHECK(registry.has_icon("toolbar-save"));

    // Check a file type icon
    CHECK(registry.has_icon("filetype-default"));
    CHECK(registry.has_icon("filetype-cpp"));

    // Check overall size
    CHECK(registry.size() > 20);
}
