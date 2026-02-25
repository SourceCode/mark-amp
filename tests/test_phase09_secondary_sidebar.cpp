#include "ui/SidebarMode.h"
#include "ui/SidebarPanelRegistry.h"

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
