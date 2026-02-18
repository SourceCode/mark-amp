/// @file test_phase41_widget_sidebar.cpp
/// @brief Comprehensive tests for Phase 41 — Widget & Sidebar Extensions.

#include "core/CommandRegistry.h"
#include "core/CustomPanelRegistry.h"
#include "core/Events.h"
#include "core/SidebarCommandProvider.h"
#include "core/SidebarWidgetManager.h"
#include "core/WidgetCommandProvider.h"
#include "core/WidgetContainer.h"
#include "core/WidgetDataProvider.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ═══════════════════════════════════════════════════════════════════
// SidebarWidgetManager Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("SidebarWidgetManager: register and find", "[phase41][sidebar]")
{
    SidebarWidgetManager mgr;
    SidebarWidget widget;
    widget.widget_id = "w1";
    widget.title = "File Explorer";
    widget.slot = SidebarSlot::kPrimary;
    mgr.register_widget(std::move(widget));

    REQUIRE(mgr.widget_count() == 1);
    const auto* found = mgr.find_widget("w1");
    REQUIRE(found != nullptr);
    REQUIRE(found->title == "File Explorer");
}

TEST_CASE("SidebarWidgetManager: unregister", "[phase41][sidebar]")
{
    SidebarWidgetManager mgr;
    SidebarWidget widget;
    widget.widget_id = "w1";
    mgr.register_widget(std::move(widget));

    REQUIRE(mgr.unregister_widget("w1") == true);
    REQUIRE(mgr.widget_count() == 0);
    REQUIRE(mgr.unregister_widget("nonexistent") == false);
}

TEST_CASE("SidebarWidgetManager: widgets_in_slot sorted by sort_order", "[phase41][sidebar]")
{
    SidebarWidgetManager mgr;
    SidebarWidget w1;
    w1.widget_id = "w1";
    w1.slot = SidebarSlot::kPrimary;
    w1.sort_order = 5;
    mgr.register_widget(std::move(w1));

    SidebarWidget w2;
    w2.widget_id = "w2";
    w2.slot = SidebarSlot::kPrimary;
    w2.sort_order = 1;
    mgr.register_widget(std::move(w2));

    auto primary = mgr.widgets_in_slot(SidebarSlot::kPrimary);
    REQUIRE(primary.size() == 2);
    REQUIRE(primary[0]->widget_id == "w2");
    REQUIRE(primary[1]->widget_id == "w1");
}

TEST_CASE("SidebarWidgetManager: move_to_slot", "[phase41][sidebar]")
{
    SidebarWidgetManager mgr;
    SidebarWidget widget;
    widget.widget_id = "w1";
    widget.slot = SidebarSlot::kPrimary;
    mgr.register_widget(std::move(widget));

    REQUIRE(mgr.move_to_slot("w1", SidebarSlot::kBottom) == true);
    REQUIRE(mgr.find_widget("w1")->slot == SidebarSlot::kBottom);
}

TEST_CASE("SidebarWidgetManager: display modes", "[phase41][sidebar]")
{
    SidebarWidgetManager mgr;
    SidebarWidget widget;
    widget.widget_id = "w1";
    mgr.register_widget(std::move(widget));

    REQUIRE(mgr.set_display_mode("w1", WidgetDisplayMode::kCollapsed) == true);
    REQUIRE(mgr.find_widget("w1")->display_mode == WidgetDisplayMode::kCollapsed);

    REQUIRE(mgr.toggle_collapsed("w1") == true);
    REQUIRE(mgr.find_widget("w1")->display_mode == WidgetDisplayMode::kExpanded);
}

TEST_CASE("SidebarWidgetManager: show/hide widget", "[phase41][sidebar]")
{
    SidebarWidgetManager mgr;
    SidebarWidget widget;
    widget.widget_id = "w1";
    mgr.register_widget(std::move(widget));

    REQUIRE(mgr.hide_widget("w1") == true);
    REQUIRE(mgr.find_widget("w1")->display_mode == WidgetDisplayMode::kHidden);
    REQUIRE(mgr.visible_widgets().empty());

    REQUIRE(mgr.show_widget("w1") == true);
    REQUIRE(mgr.visible_widgets().size() == 1);
}

TEST_CASE("SidebarWidgetManager: active_slots", "[phase41][sidebar]")
{
    SidebarWidgetManager mgr;
    SidebarWidget w1;
    w1.widget_id = "w1";
    w1.slot = SidebarSlot::kTop;
    mgr.register_widget(std::move(w1));

    SidebarWidget w2;
    w2.widget_id = "w2";
    w2.slot = SidebarSlot::kBottom;
    mgr.register_widget(std::move(w2));

    auto slots = mgr.active_slots();
    REQUIRE(slots.size() == 2);
}

TEST_CASE("SidebarWidgetManager: widgets_by_extension", "[phase41][sidebar]")
{
    SidebarWidgetManager mgr;
    SidebarWidget w1;
    w1.widget_id = "w1";
    w1.extension_id = "ext.git";
    mgr.register_widget(std::move(w1));

    SidebarWidget w2;
    w2.widget_id = "w2";
    w2.extension_id = "ext.search";
    mgr.register_widget(std::move(w2));

    auto git_widgets = mgr.widgets_by_extension("ext.git");
    REQUIRE(git_widgets.size() == 1);
    REQUIRE(git_widgets[0]->widget_id == "w1");
}

TEST_CASE("SidebarWidgetManager: slot names", "[phase41][sidebar]")
{
    REQUIRE(sidebar_slot_name(SidebarSlot::kTop) == "top");
    REQUIRE(sidebar_slot_name(SidebarSlot::kPrimary) == "primary");
    REQUIRE(sidebar_slot_name(SidebarSlot::kSecondary) == "secondary");
    REQUIRE(sidebar_slot_name(SidebarSlot::kBottom) == "bottom");
}

TEST_CASE("SidebarWidgetManager: clear_all", "[phase41][sidebar]")
{
    SidebarWidgetManager mgr;
    SidebarWidget w1;
    w1.widget_id = "w1";
    mgr.register_widget(std::move(w1));
    mgr.clear_all();
    REQUIRE(mgr.widget_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// WidgetContainer Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("WidgetContainer: add and find", "[phase41][container]")
{
    WidgetContainer container("main");
    ContainedWidget widget;
    widget.widget_id = "cw1";
    widget.title = "Outline";
    container.add_widget(std::move(widget));

    REQUIRE(container.widget_count() == 1);
    REQUIRE(container.container_id() == "main");
    REQUIRE(container.find_widget("cw1")->title == "Outline");
}

TEST_CASE("WidgetContainer: remove widget", "[phase41][container]")
{
    WidgetContainer container("main");
    ContainedWidget widget;
    widget.widget_id = "cw1";
    container.add_widget(std::move(widget));

    REQUIRE(container.remove_widget("cw1") == true);
    REQUIRE(container.widget_count() == 0);
}

TEST_CASE("WidgetContainer: swap_widgets", "[phase41][container]")
{
    WidgetContainer container("main");
    ContainedWidget w1;
    w1.widget_id = "cw1";
    container.add_widget(std::move(w1));

    ContainedWidget w2;
    w2.widget_id = "cw2";
    container.add_widget(std::move(w2));

    int pos1 = container.find_widget("cw1")->position;
    int pos2 = container.find_widget("cw2")->position;
    REQUIRE(container.swap_widgets("cw1", "cw2") == true);
    REQUIRE(container.find_widget("cw1")->position == pos2);
    REQUIRE(container.find_widget("cw2")->position == pos1);
}

TEST_CASE("WidgetContainer: set_widget_state", "[phase41][container]")
{
    WidgetContainer container("main");
    ContainedWidget widget;
    widget.widget_id = "cw1";
    container.add_widget(std::move(widget));

    REQUIRE(container.set_widget_state("cw1", WidgetState::kLoading) == true);
    REQUIRE(container.find_widget("cw1")->state == WidgetState::kLoading);
}

TEST_CASE("WidgetContainer: set_widget_visible", "[phase41][container]")
{
    WidgetContainer container("main");
    ContainedWidget widget;
    widget.widget_id = "cw1";
    container.add_widget(std::move(widget));

    REQUIRE(container.set_widget_visible("cw1", false) == true);
    REQUIRE(container.visible_widgets().empty());
}

TEST_CASE("WidgetContainer: set_widget_height", "[phase41][container]")
{
    WidgetContainer container("main");
    ContainedWidget widget;
    widget.widget_id = "cw1";
    widget.height = 200;
    container.add_widget(std::move(widget));

    REQUIRE(container.set_widget_height("cw1", 300) == true);
    REQUIRE(container.find_widget("cw1")->height == 300);
}

TEST_CASE("WidgetContainer: total_height", "[phase41][container]")
{
    WidgetContainer container("main");
    ContainedWidget w1;
    w1.widget_id = "cw1";
    w1.height = 100;
    container.add_widget(std::move(w1));

    ContainedWidget w2;
    w2.widget_id = "cw2";
    w2.height = 150;
    container.add_widget(std::move(w2));

    REQUIRE(container.total_height() == 250);
}

TEST_CASE("WidgetContainer: ordered_widgets", "[phase41][container]")
{
    WidgetContainer container("main");
    ContainedWidget w1;
    w1.widget_id = "cw1";
    container.add_widget(std::move(w1));

    ContainedWidget w2;
    w2.widget_id = "cw2";
    container.add_widget(std::move(w2));

    auto ordered = container.ordered_widgets();
    REQUIRE(ordered.size() == 2);
    REQUIRE(ordered[0]->position <= ordered[1]->position);
}

// ═══════════════════════════════════════════════════════════════════
// WidgetDataProviderManager Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("WidgetDataProviderManager: register and find", "[phase41][data-provider]")
{
    WidgetDataProviderManager mgr;
    DataProviderRegistration reg;
    reg.provider_id = "dp1";
    reg.widget_id = "w1";
    reg.data_type = WidgetDataType::kList;
    mgr.register_provider(std::move(reg));

    REQUIRE(mgr.provider_count() == 1);
    REQUIRE(mgr.find_provider("dp1") != nullptr);
    REQUIRE(mgr.find_provider("dp1")->widget_id == "w1");
}

TEST_CASE("WidgetDataProviderManager: unregister", "[phase41][data-provider]")
{
    WidgetDataProviderManager mgr;
    DataProviderRegistration reg;
    reg.provider_id = "dp1";
    mgr.register_provider(std::move(reg));

    REQUIRE(mgr.unregister_provider("dp1") == true);
    REQUIRE(mgr.provider_count() == 0);
}

TEST_CASE("WidgetDataProviderManager: refresh fetches data", "[phase41][data-provider]")
{
    WidgetDataProviderManager mgr;
    DataProviderRegistration reg;
    reg.provider_id = "dp1";
    reg.fetch_fn = []()
    {
        std::vector<WidgetDataItem> items;
        WidgetDataItem item;
        item.item_id = "i1";
        item.label = "Todo";
        items.push_back(std::move(item));
        WidgetDataItem item2;
        item2.item_id = "i2";
        item2.label = "Notes";
        items.push_back(std::move(item2));
        return items;
    };
    mgr.register_provider(std::move(reg));

    auto data = mgr.refresh("dp1");
    REQUIRE(data.size() == 2);
    REQUIRE(data[0].label == "Todo");
}

TEST_CASE("WidgetDataProviderManager: cached_data", "[phase41][data-provider]")
{
    WidgetDataProviderManager mgr;
    DataProviderRegistration reg;
    reg.provider_id = "dp1";
    reg.fetch_fn = []()
    {
        std::vector<WidgetDataItem> items;
        WidgetDataItem item;
        item.item_id = "i1";
        item.label = "Cached";
        items.push_back(std::move(item));
        return items;
    };
    mgr.register_provider(std::move(reg));

    mgr.refresh("dp1");
    const auto& cached = mgr.cached_data("dp1");
    REQUIRE(cached.size() == 1);
    REQUIRE(cached[0].label == "Cached");
}

TEST_CASE("WidgetDataProviderManager: providers_for_widget", "[phase41][data-provider]")
{
    WidgetDataProviderManager mgr;
    DataProviderRegistration reg1;
    reg1.provider_id = "dp1";
    reg1.widget_id = "w1";
    mgr.register_provider(std::move(reg1));

    DataProviderRegistration reg2;
    reg2.provider_id = "dp2";
    reg2.widget_id = "w2";
    mgr.register_provider(std::move(reg2));

    auto w1_providers = mgr.providers_for_widget("w1");
    REQUIRE(w1_providers.size() == 1);
    REQUIRE(w1_providers[0]->provider_id == "dp1");
}

// ═══════════════════════════════════════════════════════════════════
// CustomPanelRegistry Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("CustomPanelRegistry: register and find", "[phase41][panels]")
{
    CustomPanelRegistry reg;
    CustomPanel panel;
    panel.panel_id = "p1";
    panel.title = "Git Changes";
    panel.extension_id = "ext.git";
    panel.location = PanelLocation::kLeftSidebar;
    reg.register_panel(std::move(panel));

    REQUIRE(reg.panel_count() == 1);
    REQUIRE(reg.find_panel("p1")->title == "Git Changes");
}

TEST_CASE("CustomPanelRegistry: unregister", "[phase41][panels]")
{
    CustomPanelRegistry reg;
    CustomPanel panel;
    panel.panel_id = "p1";
    reg.register_panel(std::move(panel));

    REQUIRE(reg.unregister_panel("p1") == true);
    REQUIRE(reg.panel_count() == 0);
}

TEST_CASE("CustomPanelRegistry: panels_at_location", "[phase41][panels]")
{
    CustomPanelRegistry reg;
    CustomPanel p1;
    p1.panel_id = "p1";
    p1.location = PanelLocation::kLeftSidebar;
    reg.register_panel(std::move(p1));

    CustomPanel p2;
    p2.panel_id = "p2";
    p2.location = PanelLocation::kRightSidebar;
    reg.register_panel(std::move(p2));

    auto left = reg.panels_at_location(PanelLocation::kLeftSidebar);
    REQUIRE(left.size() == 1);
    REQUIRE(left[0]->panel_id == "p1");
}

TEST_CASE("CustomPanelRegistry: move_panel", "[phase41][panels]")
{
    CustomPanelRegistry reg;
    CustomPanel panel;
    panel.panel_id = "p1";
    panel.location = PanelLocation::kLeftSidebar;
    reg.register_panel(std::move(panel));

    REQUIRE(reg.move_panel("p1", PanelLocation::kFloating) == true);
    REQUIRE(reg.find_panel("p1")->location == PanelLocation::kFloating);
}

TEST_CASE("CustomPanelRegistry: show/hide", "[phase41][panels]")
{
    CustomPanelRegistry reg;
    CustomPanel panel;
    panel.panel_id = "p1";
    reg.register_panel(std::move(panel));

    REQUIRE(reg.hide_panel("p1") == true);
    REQUIRE(reg.visible_panels().empty());

    REQUIRE(reg.show_panel("p1") == true);
    REQUIRE(reg.visible_panels().size() == 1);
}

TEST_CASE("CustomPanelRegistry: pin/unpin", "[phase41][panels]")
{
    CustomPanelRegistry reg;
    CustomPanel panel;
    panel.panel_id = "p1";
    reg.register_panel(std::move(panel));

    REQUIRE(reg.pin_panel("p1") == true);
    REQUIRE(reg.pinned_panels().size() == 1);
    REQUIRE(reg.unpin_panel("p1") == true);
    REQUIRE(reg.pinned_panels().empty());
}

TEST_CASE("CustomPanelRegistry: panels_by_extension", "[phase41][panels]")
{
    CustomPanelRegistry reg;
    CustomPanel p1;
    p1.panel_id = "p1";
    p1.extension_id = "ext.git";
    reg.register_panel(std::move(p1));

    CustomPanel p2;
    p2.panel_id = "p2";
    p2.extension_id = "ext.docker";
    reg.register_panel(std::move(p2));

    auto git = reg.panels_by_extension("ext.git");
    REQUIRE(git.size() == 1);
    REQUIRE(git[0]->panel_id == "p1");
}

// ═══════════════════════════════════════════════════════════════════
// WidgetCommandProvider Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("WidgetCommandProvider: command count", "[phase41][widget-commands]")
{
    REQUIRE(WidgetCommandProvider::command_count() == 8);
}

TEST_CASE("WidgetCommandProvider: command IDs", "[phase41][widget-commands]")
{
    auto ids = WidgetCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "widgets.showAll");
    REQUIRE(ids[7] == "widgets.configureWidget");
}

TEST_CASE("WidgetCommandProvider: register to registry", "[phase41][widget-commands]")
{
    CommandRegistry registry;
    WidgetCommandProvider provider;
    provider.register_commands(registry);

    const auto* cmd = registry.get_command("widgets.showAll");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->category == "Widgets");
}

// ═══════════════════════════════════════════════════════════════════
// SidebarCommandProvider Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("SidebarCommandProvider: command count", "[phase41][sidebar-commands]")
{
    REQUIRE(SidebarCommandProvider::command_count() == 8);
}

TEST_CASE("SidebarCommandProvider: command IDs", "[phase41][sidebar-commands]")
{
    auto ids = SidebarCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "sidebar.toggleLeft");
    REQUIRE(ids[7] == "sidebar.resetPanels");
}

TEST_CASE("SidebarCommandProvider: register to registry", "[phase41][sidebar-commands]")
{
    CommandRegistry registry;
    SidebarCommandProvider provider;
    provider.register_commands(registry);

    const auto* cmd = registry.get_command("sidebar.toggleLeft");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->category == "Sidebar");
}

// ═══════════════════════════════════════════════════════════════════
// Phase 41 Events Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Phase 41 Events: SidebarWidgetChangedEvent", "[phase41][events]")
{
    SidebarWidgetChangedEvent event;
    event.widget_id = "w1";
    event.action = "registered";
    event.slot = "primary";
    REQUIRE(event.action == "registered");
}

TEST_CASE("Phase 41 Events: WidgetDisplayModeChangedEvent", "[phase41][events]")
{
    WidgetDisplayModeChangedEvent event;
    event.widget_id = "w1";
    event.new_mode = "collapsed";
    REQUIRE(event.new_mode == "collapsed");
}

TEST_CASE("Phase 41 Events: WidgetDataRefreshedEvent", "[phase41][events]")
{
    WidgetDataRefreshedEvent event;
    event.provider_id = "dp1";
    event.widget_id = "w1";
    event.item_count = 5;
    REQUIRE(event.item_count == 5);
}

TEST_CASE("Phase 41 Events: CustomPanelRegisteredEvent", "[phase41][events]")
{
    CustomPanelRegisteredEvent event;
    event.panel_id = "p1";
    event.extension_id = "ext.git";
    event.location = "left_sidebar";
    REQUIRE(event.location == "left_sidebar");
}

TEST_CASE("Phase 41 Events: WidgetLayoutChangedEvent", "[phase41][events]")
{
    WidgetLayoutChangedEvent event;
    event.container_id = "main";
    event.widget_count = 3;
    REQUIRE(event.widget_count == 3);
}

TEST_CASE("Phase 41 Events: SidebarPanelVisibilityEvent", "[phase41][events]")
{
    SidebarPanelVisibilityEvent event;
    event.panel_id = "p1";
    event.visible = true;
    REQUIRE(event.visible == true);
}
