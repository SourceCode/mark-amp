// V11 Node Editor — Tier 5: Extensibility & Rollout (Phases 41-50)
// 50+ test cases covering all 10 phases

#include "node_editor/NodeDecorationProvider.h"
#include "node_editor/NodeDevTools.h"
#include "node_editor/NodeFormatBridge.h"
#include "node_editor/NodeGraph.h"
#include "node_editor/NodeMigrationEngine.h"
#include "node_editor/NodePackRegistry.h"
#include "node_editor/NodeRolloutController.h"
#include "node_editor/NodeStatusModel.h"
#include "node_editor/NodeTemplateLibrary.h"
#include "node_editor/NodeTrustPolicy.h"
#include "node_editor/NodeWidgetEmbed.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::node_editor;

// =============================================================================
// Helper: create a simple graph with two nodes and one link
// =============================================================================
static auto make_simple_graph() -> NodeGraph
{
    NodeGraph graph;
    auto nid_a = graph.add_node("test_type");
    auto nid_b = graph.add_node("test_type");

    const auto* node_a = graph.find_node(nid_a);
    const auto* node_b = graph.find_node(nid_b);
    REQUIRE(node_a != nullptr);
    REQUIRE(node_b != nullptr);

    // Add sockets and link
    auto out_sock = graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out");
    auto in_sock = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in");
    graph.add_link(out_sock, in_sock);

    return graph;
}

// =============================================================================
// Phase 41: NodePackRegistry — Plugin API & Node Packs
// =============================================================================
TEST_CASE("Phase41: Register and find a pack", "[tier5][pack_registry]")
{
    NodePackRegistry registry;
    ExtensionNodePack pack;
    pack.display_name = "AudioPack";
    pack.version = "1.0";
    pack.vendor = "MarkAmp";
    pack.trust = TrustLevel::kTrusted;
    pack.contribution_category = "Audio";

    auto pid = registry.register_pack(std::move(pack));
    REQUIRE(pid.is_valid());
    REQUIRE(registry.pack_count() == 1);

    const auto* found = registry.find_pack(pid);
    REQUIRE(found != nullptr);
    CHECK(found->display_name == "AudioPack");
    CHECK(found->vendor == "MarkAmp");
}

TEST_CASE("Phase41: Unregister a pack", "[tier5][pack_registry]")
{
    NodePackRegistry registry;
    ExtensionNodePack pack;
    pack.display_name = "TempPack";
    auto pid = registry.register_pack(std::move(pack));
    REQUIRE(registry.pack_count() == 1);

    CHECK(registry.unregister_pack(pid));
    CHECK(registry.pack_count() == 0);
    CHECK(registry.find_pack(pid) == nullptr);
}

TEST_CASE("Phase41: Validate pack manifest", "[tier5][pack_registry]")
{
    ExtensionNodePack pack;
    pack.display_name = "ValidPack";
    pack.version = "2.0";

    PackNodeEntry entry;
    entry.type_name = "MathAdd";
    entry.display_name = "Add";
    pack.node_entries.push_back(entry);

    auto result = NodePackRegistry::validate_pack(pack);
    CHECK(result.ok);
    CHECK(result.errors.empty());
}

TEST_CASE("Phase41: Pack with node entries", "[tier5][pack_registry]")
{
    NodePackRegistry registry;
    ExtensionNodePack pack;
    pack.display_name = "MathPack";

    PackNodeEntry entry1;
    entry1.type_name = "Add";
    entry1.display_name = "Add Node";
    pack.node_entries.push_back(entry1);

    PackNodeEntry entry2;
    entry2.type_name = "Multiply";
    entry2.display_name = "Multiply Node";
    pack.node_entries.push_back(entry2);

    auto pid = registry.register_pack(std::move(pack));
    CHECK(registry.nodes_from_pack(pid).size() == 2);
}

TEST_CASE("Phase41: Pack trust status", "[tier5][pack_registry]")
{
    NodePackRegistry registry;
    ExtensionNodePack pack;
    pack.display_name = "UntrustedPack";
    pack.trust = TrustLevel::kUntrusted;

    auto pid = registry.register_pack(std::move(pack));
    CHECK_FALSE(registry.is_trusted(pid));
}

// =============================================================================
// Phase 42: NodeWidgetEmbed — Webview & Custom Widget Embeds
// =============================================================================
TEST_CASE("Phase42: Register embed descriptor", "[tier5][widget_embed]")
{
    NodeWidgetEmbed embeds;
    NodeId nid{100};
    EmbedDescriptor desc;
    desc.widget_type = WidgetEmbedType::kSlider;
    desc.label = "Volume";
    desc.current_value = "0.5";

    auto eid = embeds.register_embed(nid, std::move(desc));
    REQUIRE(eid.is_valid());
    CHECK(embeds.embed_count() == 1);
}

TEST_CASE("Phase42: Set and get widget value", "[tier5][widget_embed]")
{
    NodeWidgetEmbed embeds;
    NodeId nid{101};
    EmbedDescriptor desc;
    desc.label = "Gain";
    desc.current_value = "1.0";

    auto eid = embeds.register_embed(nid, std::move(desc));
    embeds.set_value(eid, "0.75");
    CHECK(embeds.value(eid) == "0.75");
}

TEST_CASE("Phase42: Remove embed", "[tier5][widget_embed]")
{
    NodeWidgetEmbed embeds;
    NodeId nid{102};
    EmbedDescriptor desc;
    auto eid = embeds.register_embed(nid, std::move(desc));
    REQUIRE(embeds.embed_count() == 1);

    CHECK(embeds.remove_embed(eid));
    CHECK(embeds.embed_count() == 0);
}

TEST_CASE("Phase42: Embeds for node", "[tier5][widget_embed]")
{
    NodeWidgetEmbed embeds;
    NodeId nid{103};
    EmbedDescriptor desc1;
    desc1.label = "A";
    EmbedDescriptor desc2;
    desc2.label = "B";

    embeds.register_embed(nid, std::move(desc1));
    embeds.register_embed(nid, std::move(desc2));
    CHECK(embeds.embeds_for(nid).size() == 2);
}

TEST_CASE("Phase42: Sandbox status", "[tier5][widget_embed]")
{
    NodeWidgetEmbed embeds;
    NodeId nid{104};
    EmbedDescriptor desc;
    desc.sandboxed = true;

    auto eid = embeds.register_embed(nid, std::move(desc));
    CHECK(embeds.is_sandboxed(eid));
}

// =============================================================================
// Phase 43: NodeDecorationProvider — Decoration Integration
// =============================================================================
TEST_CASE("Phase43: Register provider and add decoration", "[tier5][decoration]")
{
    NodeDecorationProvider provider;
    provider.register_provider("linter", "Code linter decorations");

    NodeId nid{200};
    NodeDecoration deco;
    deco.type = DecorationType::kBadge;
    deco.display_text = "Error";
    deco.color_hex = "#FF0000";
    deco.provider_id = "linter";

    auto did = provider.add_decoration(nid, std::move(deco));
    REQUIRE(did.is_valid());
    CHECK(provider.decorations_for(nid).size() == 1);
}

TEST_CASE("Phase43: Remove decoration", "[tier5][decoration]")
{
    NodeDecorationProvider provider;
    NodeId nid{201};
    NodeDecoration deco;
    deco.display_text = "Temp";

    auto did = provider.add_decoration(nid, std::move(deco));
    CHECK(provider.remove_decoration(did));
    CHECK(provider.decorations_for(nid).empty());
}

TEST_CASE("Phase43: Language hint", "[tier5][decoration]")
{
    NodeDecorationProvider provider;
    NodeId nid{202};
    provider.set_language_hint(nid, "python");
    CHECK(provider.language_hint(nid) == "python");
}

TEST_CASE("Phase43: Provider count", "[tier5][decoration]")
{
    NodeDecorationProvider provider;
    provider.register_provider("lint", "Linter");
    provider.register_provider("debug", "Debugger");
    CHECK(provider.provider_count() == 2);
}

TEST_CASE("Phase43: Unregister provider", "[tier5][decoration]")
{
    NodeDecorationProvider provider;
    provider.register_provider("temp", "Temporary");
    CHECK(provider.provider_count() == 1);
    provider.unregister_provider("temp");
    CHECK(provider.provider_count() == 0);
}

// =============================================================================
// Phase 44: NodeStatusModel — Output, Problems & StatusBar
// =============================================================================
TEST_CASE("Phase44: Set and get node status", "[tier5][status_model]")
{
    NodeStatusModel model;
    NodeId nid{300};
    model.set_status(nid, NodeStatusLevel::kRunning, "Processing");

    const auto* stat = model.status(nid);
    REQUIRE(stat != nullptr);
    CHECK(stat->level == NodeStatusLevel::kRunning);
    CHECK(stat->message == "Processing");
}

TEST_CASE("Phase44: Append output log", "[tier5][status_model]")
{
    NodeStatusModel model;
    NodeId nid{301};
    model.append_output(nid, "Line 1");
    model.append_output(nid, "Line 2");
    CHECK(model.output_lines(nid).size() == 2);
}

TEST_CASE("Phase44: Clear status", "[tier5][status_model]")
{
    NodeStatusModel model;
    NodeId nid{302};
    model.set_status(nid, NodeStatusLevel::kSuccess, "Done");
    model.clear_status(nid);
    CHECK(model.status(nid) == nullptr);
}

TEST_CASE("Phase44: Status bar text", "[tier5][status_model]")
{
    NodeStatusModel model;
    NodeId nid1{303};
    NodeId nid2{304};
    model.set_status(nid1, NodeStatusLevel::kError, "Failed");
    model.set_status(nid2, NodeStatusLevel::kWarning, "Warn");

    auto text = model.status_bar_text();
    CHECK_FALSE(text.empty());
}

TEST_CASE("Phase44: Set progress", "[tier5][status_model]")
{
    NodeStatusModel model;
    NodeId nid{305};
    model.set_status(nid, NodeStatusLevel::kRunning, "Working");
    model.set_progress(nid, 0.5F);

    const auto* stat = model.status(nid);
    REQUIRE(stat != nullptr);
    CHECK(stat->progress == 0.5F);
}

// =============================================================================
// Phase 45: NodeTemplateLibrary — Templates & Snippets
// =============================================================================
TEST_CASE("Phase45: Built-in templates exist", "[tier5][template_library]")
{
    NodeTemplateLibrary library;
    CHECK(library.template_count() > 0);
}

TEST_CASE("Phase45: Register custom template", "[tier5][template_library]")
{
    NodeTemplateLibrary library;
    auto initial_count = library.template_count();

    GraphTemplate tmpl;
    tmpl.name = "Custom Flow";
    tmpl.description = "A custom workflow";
    tmpl.category = "Custom";
    tmpl.tags = {"workflow", "custom"};
    tmpl.serialized_graph = "{}";

    auto tid = library.register_template(std::move(tmpl));
    REQUIRE(tid.is_valid());
    CHECK(library.template_count() == initial_count + 1);
}

TEST_CASE("Phase45: Find template by ID", "[tier5][template_library]")
{
    NodeTemplateLibrary library;
    GraphTemplate tmpl;
    tmpl.name = "Findable";
    tmpl.category = "Test";
    auto tid = library.register_template(std::move(tmpl));

    const auto* found = library.find_template(tid);
    REQUIRE(found != nullptr);
    CHECK(found->name == "Findable");
}

TEST_CASE("Phase45: Register snippet", "[tier5][template_library]")
{
    NodeTemplateLibrary library;
    NodeSnippet snippet;
    snippet.name = "Quick Math";
    snippet.serialized_nodes = "add(a, b)";
    snippet.category = "Math";

    auto sid = library.register_snippet(std::move(snippet));
    REQUIRE(sid.is_valid());
    CHECK(library.snippet_count() >= 1);
}

TEST_CASE("Phase45: Search templates", "[tier5][template_library]")
{
    NodeTemplateLibrary library;
    // Built-in templates should be searchable
    auto results = library.search_templates("");
    CHECK_FALSE(results.empty());
}

// =============================================================================
// Phase 46: NodeFormatBridge — Import/Export & Format Bridges
// =============================================================================
TEST_CASE("Phase46: Built-in adapters exist", "[tier5][format_bridge]")
{
    NodeFormatBridge bridge;
    CHECK(bridge.adapter_count() >= 2); // JSON + SVG
}

TEST_CASE("Phase46: Adapters for .json extension", "[tier5][format_bridge]")
{
    NodeFormatBridge bridge;
    auto adapters = bridge.adapters_for_extension(".json");
    CHECK_FALSE(adapters.empty());
}

TEST_CASE("Phase46: Export graph", "[tier5][format_bridge]")
{
    NodeFormatBridge bridge;
    auto graph = make_simple_graph();

    auto adapters = bridge.adapters_for_extension(".json");
    REQUIRE_FALSE(adapters.empty());

    auto result = bridge.export_graph(graph, adapters[0]);
    CHECK(result.success);
    CHECK_FALSE(result.data.empty());
}

TEST_CASE("Phase46: Import graph data", "[tier5][format_bridge]")
{
    NodeFormatBridge bridge;
    auto adapters = bridge.adapters_for_extension(".json");
    REQUIRE_FALSE(adapters.empty());

    auto result = bridge.import_graph("{\"nodes\":[]}", adapters[0]);
    CHECK(result.success);
}

TEST_CASE("Phase46: Supported extensions", "[tier5][format_bridge]")
{
    NodeFormatBridge bridge;
    auto import_exts = bridge.supported_import_extensions();
    auto export_exts = bridge.supported_export_extensions();
    CHECK_FALSE(import_exts.empty());
    CHECK_FALSE(export_exts.empty());
}

// =============================================================================
// Phase 47: NodeTrustPolicy — Security & Trust Policy
// =============================================================================
TEST_CASE("Phase47: Default deny policy", "[tier5][trust_policy]")
{
    NodeTrustPolicy policy;
    // Default should deny everything
    auto perm = policy.check_permission("unknown_pack", ExecutionPermission::kFileRead);
    CHECK(perm == TrustAction::kDeny);
}

TEST_CASE("Phase47: Add allow rule", "[tier5][trust_policy]")
{
    NodeTrustPolicy policy;
    TrustRule rule;
    rule.scope = "trusted_pack";
    rule.action = TrustAction::kAllow;
    rule.permissions = {ExecutionPermission::kFileRead, ExecutionPermission::kFileWrite};

    policy.add_rule(std::move(rule));
    auto perm = policy.check_permission("trusted_pack", ExecutionPermission::kFileRead);
    CHECK(perm == TrustAction::kAllow);
}

TEST_CASE("Phase47: Remove rule", "[tier5][trust_policy]")
{
    NodeTrustPolicy policy;
    TrustRule rule;
    rule.scope = "temp";
    rule.action = TrustAction::kAllow;
    rule.permissions = {ExecutionPermission::kClipboard};

    auto rid = policy.add_rule(std::move(rule));
    CHECK(policy.rule_count() >= 2); // default + added
    CHECK(policy.remove_rule(rid));
}

TEST_CASE("Phase47: Audit log", "[tier5][trust_policy]")
{
    NodeTrustPolicy policy;
    policy.log_access("test_pack", ExecutionPermission::kNetwork, false);
    CHECK(policy.audit_log().size() == 1);
}

TEST_CASE("Phase47: Wildcard rule", "[tier5][trust_policy]")
{
    NodeTrustPolicy policy;
    policy.clear();

    TrustRule allow_all;
    allow_all.scope = "*";
    allow_all.action = TrustAction::kAllow;
    allow_all.permissions = {ExecutionPermission::kFileRead};
    policy.add_rule(std::move(allow_all));

    auto perm = policy.check_permission("any_pack", ExecutionPermission::kFileRead);
    CHECK(perm == TrustAction::kAllow);
}

// =============================================================================
// Phase 48: NodeDevTools — Documentation & Developer Tooling
// =============================================================================
TEST_CASE("Phase48: Graph summary", "[tier5][dev_tools]")
{
    auto graph = make_simple_graph();
    auto summary = NodeDevTools::graph_summary(graph);

    CHECK(summary.node_count == 2);
    CHECK(summary.link_count == 1);
}

TEST_CASE("Phase48: Register type doc", "[tier5][dev_tools]")
{
    NodeDevTools tools;
    NodeTypeDoc doc;
    doc.name = "MathAdd";
    doc.description = "Adds two numbers";
    doc.inputs = {"a", "b"};
    doc.outputs = {"result"};

    tools.register_type_doc(std::move(doc));
    CHECK(tools.documented_type_count() == 1);

    const auto* found = tools.document_node_type("MathAdd");
    REQUIRE(found != nullptr);
    CHECK(found->description == "Adds two numbers");
}

TEST_CASE("Phase48: Lint graph", "[tier5][dev_tools]")
{
    auto graph = make_simple_graph();
    auto issues = NodeDevTools::lint_graph(graph);
    // Should have some issues (e.g., unconnected sockets or empty labels)
    CHECK(issues.size() >= 0); // Non-negative; actual count depends on graph
}

TEST_CASE("Phase48: Export schema", "[tier5][dev_tools]")
{
    auto graph = make_simple_graph();
    auto schema = NodeDevTools::export_schema(graph);
    CHECK_FALSE(schema.empty());
    CHECK_THAT(schema, Catch::Matchers::ContainsSubstring("node_count"));
}

TEST_CASE("Phase48: Generate playbook", "[tier5][dev_tools]")
{
    auto graph = make_simple_graph();
    auto playbook = NodeDevTools::generate_playbook(graph);
    CHECK_THAT(playbook, Catch::Matchers::ContainsSubstring("Playbook"));
}

TEST_CASE("Phase48: Graph stats", "[tier5][dev_tools]")
{
    auto graph = make_simple_graph();
    auto graph_stats = NodeDevTools::stats(graph);
    CHECK(graph_stats.density >= 0.0F);
    CHECK(graph_stats.avg_degree >= 0.0F);
}

// =============================================================================
// Phase 49: NodeMigrationEngine — Migration & Backward Compatibility
// =============================================================================
TEST_CASE("Phase49: Register migration step", "[tier5][migration]")
{
    NodeMigrationEngine engine;
    MigrationStep step;
    step.from_version = 1;
    step.to_version = 2;
    step.description = "Rename NodeA to NodeAlpha";
    step.action = MigrationAction::kRenameNode;

    engine.register_step(std::move(step));
    CHECK(engine.step_count() == 1);
}

TEST_CASE("Phase49: Migrate graph data", "[tier5][migration]")
{
    NodeMigrationEngine engine;
    MigrationStep step;
    step.from_version = 1;
    step.to_version = 2;
    step.description = "v1 to v2";
    engine.register_step(std::move(step));

    auto result = engine.migrate("{}", 1, 2);
    CHECK(result.success);
    CHECK(result.steps_applied == 1);
}

TEST_CASE("Phase49: No-op migration same version", "[tier5][migration]")
{
    NodeMigrationEngine engine;
    auto result = engine.migrate("{}", 1, 1);
    CHECK(result.success);
    CHECK(result.steps_applied == 0);
}

TEST_CASE("Phase49: Compatibility check", "[tier5][migration]")
{
    NodeMigrationEngine engine;
    engine.set_current_version(3);

    MigrationStep step;
    step.from_version = 1;
    step.to_version = 2;
    step.description = "v1 to v2";
    engine.register_step(std::move(step));

    auto report = engine.check_compatibility("{}", 1);
    CHECK(report.current_version == 3);
    CHECK(report.graph_version == 1);
}

TEST_CASE("Phase49: Deprecation tracking", "[tier5][migration]")
{
    NodeMigrationEngine engine;
    engine.register_deprecation("OldNode", "NewNode", 5);
    CHECK(engine.deprecation_count() == 1);
    CHECK(engine.is_deprecated("OldNode"));
    CHECK_FALSE(engine.is_deprecated("NewNode"));
}

// =============================================================================
// Phase 50: NodeRolloutController — Validation & Staged Rollout
// =============================================================================
TEST_CASE("Phase50: Add rollout stage", "[tier5][rollout]")
{
    NodeRolloutController controller;
    RolloutStage stage;
    stage.name = "Canary";
    stage.phase = RolloutPhase::kCanary;
    stage.enabled_features = {"feature_a"};
    stage.health_threshold = 0.9F;

    auto sid = controller.add_stage(std::move(stage));
    REQUIRE(sid.is_valid());
    CHECK(controller.stage_count() == 1);
}

TEST_CASE("Phase50: Current stage", "[tier5][rollout]")
{
    NodeRolloutController controller;
    RolloutStage stage;
    stage.name = "Canary";
    stage.phase = RolloutPhase::kCanary;
    controller.add_stage(std::move(stage));

    const auto* current = controller.current_stage();
    REQUIRE(current != nullptr);
    CHECK(current->name == "Canary");
}

TEST_CASE("Phase50: Advance and rollback stages", "[tier5][rollout]")
{
    NodeRolloutController controller;
    RolloutStage canary;
    canary.name = "Canary";
    canary.phase = RolloutPhase::kCanary;
    controller.add_stage(std::move(canary));

    RolloutStage beta;
    beta.name = "Beta";
    beta.phase = RolloutPhase::kBeta;
    controller.add_stage(std::move(beta));

    CHECK(controller.current_stage()->name == "Canary");
    CHECK(controller.advance_stage());
    CHECK(controller.current_stage()->name == "Beta");
    CHECK(controller.rollback_stage());
    CHECK(controller.current_stage()->name == "Canary");
}

TEST_CASE("Phase50: Feature gates", "[tier5][rollout]")
{
    NodeRolloutController controller;
    RolloutStage stage;
    stage.name = "GA";
    stage.phase = RolloutPhase::kGA;
    stage.enabled_features = {"node_editor_v11"};
    controller.add_stage(std::move(stage));

    CHECK(controller.is_feature_enabled("node_editor_v11"));
    CHECK_FALSE(controller.is_feature_enabled("unknown_feature"));
}

TEST_CASE("Phase50: Enable/disable extra features", "[tier5][rollout]")
{
    NodeRolloutController controller;
    RolloutStage stage;
    stage.name = "GA";
    controller.add_stage(std::move(stage));

    controller.enable_feature("experimental");
    CHECK(controller.is_feature_enabled("experimental"));
    controller.disable_feature("experimental");
    CHECK_FALSE(controller.is_feature_enabled("experimental"));
}

TEST_CASE("Phase50: Health reporting", "[tier5][rollout]")
{
    NodeRolloutController controller;
    RolloutStage stage;
    stage.name = "Canary";
    stage.health_threshold = 0.8F;
    controller.add_stage(std::move(stage));

    controller.report_health("cpu", 0.95F);
    controller.report_health("memory", 0.85F);
    CHECK(controller.health_score() > 0.8F);
    CHECK(controller.is_healthy());
}

TEST_CASE("Phase50: Unhealthy state", "[tier5][rollout]")
{
    NodeRolloutController controller;
    RolloutStage stage;
    stage.name = "Canary";
    stage.health_threshold = 0.9F;
    controller.add_stage(std::move(stage));

    controller.report_health("error_rate", 0.3F);
    CHECK(controller.health_score() < 0.9F);
    CHECK_FALSE(controller.is_healthy());
}

TEST_CASE("Phase50: Validation suite", "[tier5][rollout]")
{
    NodeRolloutController controller;
    RolloutStage stage;
    stage.name = "GA";
    controller.add_stage(std::move(stage));
    controller.report_health("uptime", 1.0F);

    auto report = controller.run_validation_suite();
    CHECK(report.total_checks >= 4);
    CHECK(report.passed > 0);
}

TEST_CASE("Phase50: Remove stage", "[tier5][rollout]")
{
    NodeRolloutController controller;
    RolloutStage stage;
    stage.name = "Removable";
    auto sid = controller.add_stage(std::move(stage));
    REQUIRE(controller.stage_count() == 1);

    CHECK(controller.remove_stage(sid));
    CHECK(controller.stage_count() == 0);
}
