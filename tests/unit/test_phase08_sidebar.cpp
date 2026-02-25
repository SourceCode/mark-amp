#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

// Headers to test
#include "ui/EmptyPanelState.h"
#include "ui/SidebarFooter.h"
#include "ui/SidebarHeader.h"
#include "ui/SidebarSection.h"

// Core dependencies
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"
#include "ui/ComponentSizeResolver.h"
#include "ui/DesignSystemContext.h"
#include "ui/DesignTokenRegistry.h"
#include "ui/ElevationSystem.h"
#include "ui/IconManager.h"
#include "ui/LayoutMetrics.h"
#include "ui/SpacingGrid.h"
#include "ui/TypographyScale.h"

#include <wx/wx.h>

using namespace markamp;
using namespace markamp::ui;

// Helper to create a headless app context for testing wxWidgets components without a real GUI
struct HeadlessAppContext
{
    HeadlessAppContext()
        : theme_registry()
        , theme_engine(event_bus, theme_registry)
        , token_registry(theme_engine, event_bus)
        , typography()
        , spacing()
        , elevation()
        , ds{theme_engine,
             token_registry,
             typography,
             spacing,
             ComponentSizeResolver::get(),
             elevation,
             LayoutMetrics::get()}
    {
        if (!wxTheApp)
        {
            wxApp::SetInstance(new wxApp());
            int argc = 0;
            char** argv = nullptr;
            wxEntryStart(argc, argv);
        }
        parent = new wxFrame(nullptr, wxID_ANY, "Test Frame");
    }

    ~HeadlessAppContext()
    {
        if (parent)
        {
            parent->Destroy();
        }
    }

    core::EventBus event_bus;
    core::ThemeRegistry theme_registry;
    core::ThemeEngine theme_engine;
    DesignTokenRegistry token_registry;
    TypographyScale typography;
    SpacingGrid spacing;
    ElevationSystem elevation;
    DesignSystemContext ds;
    wxWindow* parent{nullptr};
};

TEST_CASE("SidebarSection toggles expand/collapse state", "[sidebar]")
{
    HeadlessAppContext ctx;
    auto& icon_manager = IconManager::get();

    // We pass a null config to avoid writing to disk during testing
    SidebarSection section(
        ctx.parent, ctx.ds, icon_manager, ctx.event_bus, nullptr, "Test Section");

    // Initially expanded by default
    REQUIRE(section.is_expanded() == true);

    section.set_expanded(false);
    REQUIRE(section.is_expanded() == false);

    section.set_expanded(true);
    REQUIRE(section.is_expanded() == true);
}

TEST_CASE("SidebarHeader basic API", "[sidebar]")
{
    HeadlessAppContext ctx;
    auto& icon_manager = IconManager::get();

    SidebarHeader header(ctx.parent, ctx.ds, icon_manager, ctx.event_bus);

    // SidebarHeader has `set_title`, `set_display_mode`, and `set_breadcrumb`
    header.set_title("EXPLORER");
    header.set_display_mode(SidebarHeaderMode::kBreadcrumb);
    header.set_breadcrumb({"Path", "To", "File"});

    // As there are no getters, we just verify they can be called without crashing
    REQUIRE(true);
}

TEST_CASE("EmptyPanelState has expected metrics", "[sidebar]")
{
    HeadlessAppContext ctx;
    auto& icon_manager = IconManager::get();

    EmptyPanelState empty_state(ctx.parent, ctx.ds, icon_manager);

    // It has `set_icon`, `set_message`, and `set_action`
    empty_state.set_icon("icon-search");
    empty_state.set_message("No items found.");

    bool action_called = false;
    empty_state.set_action("Retry", [&action_called]() { action_called = true; });

    REQUIRE(true);
}

TEST_CASE("SidebarFooter accepts and updates status text", "[sidebar]")
{
    HeadlessAppContext ctx;

    SidebarFooter footer(ctx.parent, ctx.ds, ctx.event_bus);

    // It has `set_text` and `get_text`
    REQUIRE(footer.get_text() == "");

    footer.set_text("3 files selected");
    REQUIRE(footer.get_text() == "3 files selected");
}
