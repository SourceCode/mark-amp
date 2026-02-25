#include "core/EventBus.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"
#include "ui/ComponentSizeResolver.h"
#include "ui/DesignSystemContext.h"
#include "ui/DesignTokenRegistry.h"
#include "ui/ElevationSystem.h"
#include "ui/LayoutMetrics.h"
#include "ui/SecondarySidebarTabStrip.h"
#include "ui/SidebarMode.h"
#include "ui/SidebarPanelRegistry.h"
#include "ui/SpacingGrid.h"
#include "ui/TypographyScale.h" // Added for TypographyScale

#include <catch2/catch_test_macros.hpp>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/panel.h>

using namespace markamp;

TEST_CASE("Secondary Sidebar Registry Independence", "[ui][sidebar][phase09]")
{
    // Initialize wxWidgets if not already initialized
    if (wxTheApp == nullptr)
    {
        wxApp::SetInstance(new wxApp());
        int argc = 0;
        char** argv = nullptr;
        wxEntryStart(argc, argv);
    }

    auto* parent = new wxFrame(nullptr, wxID_ANY, "Test Frame");

    ui::SidebarPanelRegistry primary_registry;
    ui::SidebarPanelRegistry secondary_registry;

    // Register simple wxPanel factory in Primary
    primary_registry.Register(ui::kSidebarModeExplorer,
                              "EXPLORER",
                              "\xF0\x9F\x93\x81",
                              [&](wxWindow* parent_win) -> wxPanel*
                              {
                                  auto* p = new wxPanel(parent_win);
                                  p->Hide();
                                  return p;
                              });

    // Register simple wxPanel factory in Secondary
    secondary_registry.Register(ui::kSidebarModeExplorer,
                                "EXPLORER",
                                "\xF0\x9F\x93\x81",
                                [&](wxWindow* parent_win) -> wxPanel*
                                {
                                    auto* p = new wxPanel(parent_win);
                                    p->Hide();
                                    return p;
                                });

    auto* primary_panel = primary_registry.GetOrCreate(ui::kSidebarModeExplorer, parent);
    auto* secondary_panel = secondary_registry.GetOrCreate(ui::kSidebarModeExplorer, parent);

    REQUIRE(primary_panel != nullptr);
    REQUIRE(secondary_panel != nullptr);
    REQUIRE(primary_panel != secondary_panel); // Ensure panels are distinct instances

    parent->Destroy();
}

TEST_CASE("Secondary Sidebar Tab Strip Logic", "[ui][sidebar][phase09]")
{
    if (wxTheApp == nullptr)
    {
        wxApp::SetInstance(new wxApp());
        int argc = 0;
        char** argv = nullptr;
        wxEntryStart(argc, argv);
    }

    auto* parent = new wxFrame(nullptr, wxID_ANY, "Test Frame");
    core::EventBus event_bus;
    core::ThemeRegistry theme_registry;
    core::ThemeEngine theme_engine(event_bus, theme_registry);
    ui::DesignTokenRegistry token_registry(theme_engine, event_bus);
    ui::TypographyScale typography;
    ui::SpacingGrid spacing;
    ui::ElevationSystem elevation;

    ui::DesignSystemContext ds{theme_engine,
                               token_registry,
                               typography,
                               spacing,
                               ui::ComponentSizeResolver::get(),
                               elevation,
                               ui::LayoutMetrics::get()};

    auto* tab_strip = new ui::SecondarySidebarTabStrip(parent, theme_engine, ds, event_bus);

    REQUIRE(tab_strip->GetActiveMode() == "");
    REQUIRE(tab_strip->HasTab(ui::kSidebarModeExplorer) == false);

    tab_strip->AddTab(ui::kSidebarModeExplorer, "E", "Explorer");
    REQUIRE(tab_strip->HasTab(ui::kSidebarModeExplorer) == true);

    // Simulate LayoutManager setting active mode
    tab_strip->SetActiveMode(ui::kSidebarModeExplorer);
    REQUIRE(tab_strip->GetActiveMode() == ui::kSidebarModeExplorer);

    tab_strip->AddTab(ui::kSidebarModeSearch, "S", "Search");
    REQUIRE(tab_strip->HasTab(ui::kSidebarModeSearch) == true);
    // Active mode should not change automatically when adding subsequent tabs
    REQUIRE(tab_strip->GetActiveMode() == ui::kSidebarModeExplorer);

    tab_strip->SetActiveMode(ui::kSidebarModeSearch);
    REQUIRE(tab_strip->GetActiveMode() == ui::kSidebarModeSearch);

    tab_strip->RemoveTab(ui::kSidebarModeSearch);
    REQUIRE(tab_strip->HasTab(ui::kSidebarModeSearch) == false);
    // Should fallback to the remaining tab
    REQUIRE(tab_strip->GetActiveMode() == ui::kSidebarModeExplorer);

    tab_strip->RemoveTab(ui::kSidebarModeExplorer);
    REQUIRE(tab_strip->HasTab(ui::kSidebarModeExplorer) == false);
    // Should fallback to empty state
    REQUIRE(tab_strip->GetActiveMode() == "");

    parent->Destroy();
}
