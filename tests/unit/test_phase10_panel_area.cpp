#include "core/Events.h"
#include "ui/PanelAreaModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp;
using namespace markamp::ui;

TEST_CASE("PanelAreaModel Registration", "[ui][panel]")
{
    core::EventBus event_bus;
    PanelAreaModel model(event_bus);

    PanelTabConfig config1{"output", "Output", "icon-output"};
    PanelTabConfig config2{"problems", "Problems", "icon-problems"};

    SECTION("Registration and retrieval")
    {
        model.register_panel(config1);
        model.register_panel(config2);

        REQUIRE(model.has_panel("output"));
        REQUIRE(model.has_panel("problems"));
        REQUIRE_FALSE(model.has_panel("walkthrough"));

        auto panels = model.panels();
        REQUIRE(panels.size() == 2);
    }

    SECTION("Active panel management")
    {
        model.register_panel(config1);
        model.register_panel(config2);

        // Active panel should default to nothing or the first one depending on logic
        // But let's set it
        model.set_active("problems");
        REQUIRE(model.active_panel() == "problems");

        model.set_active("output");
        REQUIRE(model.active_panel() == "output");
    }

    SECTION("Unregistration handles active state")
    {
        model.register_panel(config1);
        model.register_panel(config2);
        model.set_active("output");

        model.unregister_panel("output");
        REQUIRE_FALSE(model.has_panel("output"));
        REQUIRE(model.active_panel() != "output"); // should fall back to another or empty
    }

    SECTION("Visibility toggle")
    {
        model.register_panel(config1);
        model.set_visible("output", false);

        auto panels = model.panels();
        REQUIRE(panels.empty()); // Invisible panels should not be returned by panels()

        model.set_visible("output", true);
        REQUIRE(model.panels().size() == 1);
    }

    SECTION("Badge state")
    {
        model.register_panel(config1);
        model.set_badge("output", core::events::BadgeState::kInfo, 5);

        auto panels = model.panels();
        REQUIRE(panels.size() == 1);
        REQUIRE(panels[0].badge == core::events::BadgeState::kInfo);
        REQUIRE(panels[0].badge_count == 5);
    }
}
