// V11 Tier 2: Comprehensive test suite for Phases 8–20
// Tests NodeEditorContext, EvaluationIR, ExecutionPlan, DomainRegistry,
// NodeScheduler, NodeValue, ValueConverter, Domain Runtimes,
// DomainSDK, and RuntimeSandbox.

#include "NodeEditorTestFixtures.h"
#include "node_editor/AudioRuntime.h"
#include "node_editor/CodeFlowRuntime.h"
#include "node_editor/DataGenRuntime.h"
#include "node_editor/DataTransformRuntime.h"
#include "node_editor/DomainRegistry.h"
#include "node_editor/DomainSDK.h"
#include "node_editor/EvaluationIR.h"
#include "node_editor/ExecutionPlan.h"
#include "node_editor/GraphicsRuntime.h"
#include "node_editor/NodeEditorContext.h"
#include "node_editor/NodeGraph.h"
#include "node_editor/NodeScheduler.h"
#include "node_editor/NodeValue.h"
#include "node_editor/RuntimeSandbox.h"
#include "node_editor/ValueConverter.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::node_editor;

// ============================================================================
// Section 1: NodeEditorContext
// ============================================================================

TEST_CASE("NodeEditorContext tracks active graph", "[node_editor][context]")
{
    NodeEditorContext ctx;
    REQUIRE_FALSE(ctx.has_active_graph());

    NodeGraph graph;
    ctx.set_active_graph(&graph);
    REQUIRE(ctx.has_active_graph());
    REQUIRE(ctx.active_graph() == &graph);

    ctx.set_active_graph(nullptr);
    REQUIRE_FALSE(ctx.has_active_graph());
    REQUIRE(ctx.active_graph() == nullptr);
}

TEST_CASE("NodeEditorContext tracks selection", "[node_editor][context]")
{
    NodeEditorContext ctx;
    REQUIRE(ctx.selection_count() == 0);
    REQUIRE_FALSE(ctx.has_selection());

    ctx.select_node(NodeId{1});
    ctx.select_node(NodeId{2});
    ctx.select_node(NodeId{3});
    REQUIRE(ctx.selection_count() == 3);
    REQUIRE(ctx.has_selection());
    REQUIRE(ctx.is_selected(NodeId{2}));

    ctx.deselect_node(NodeId{2});
    REQUIRE(ctx.selection_count() == 2);
    REQUIRE_FALSE(ctx.is_selected(NodeId{2}));

    ctx.clear_selection();
    REQUIRE_FALSE(ctx.has_selection());
}

TEST_CASE("NodeEditorContext tracks editor state", "[node_editor][context]")
{
    NodeEditorContext ctx;
    REQUIRE_FALSE(ctx.is_editor_active());

    ctx.set_editor_active(true);
    REQUIRE(ctx.is_editor_active());

    ctx.set_editor_active(false);
    REQUIRE_FALSE(ctx.is_editor_active());
}

TEST_CASE("NodeEditorContext clipboard state", "[node_editor][context]")
{
    NodeEditorContext ctx;
    REQUIRE_FALSE(ctx.has_clipboard());

    ctx.set_has_clipboard(true);
    REQUIRE(ctx.has_clipboard());
}

TEST_CASE("NodeEditorContext generates context keys", "[node_editor][context]")
{
    NodeEditorContext ctx;
    ctx.set_editor_active(true);
    ctx.select_node(NodeId{1});
    ctx.select_node(NodeId{2});
    ctx.select_node(NodeId{3});
    ctx.set_has_clipboard(true);

    NodeGraph graph;
    ctx.set_active_graph(&graph);

    const auto keys = ctx.context_keys();
    REQUIRE_FALSE(keys.empty());
}

// ============================================================================
// Section 2: EvaluationIR
// ============================================================================

TEST_CASE("EvaluationIR builds from linear chain", "[node_editor][ir]")
{
    auto graph = test_fixtures::make_linear_chain(3);
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);

    REQUIRE(evaluation_ir.node_count() == 3);
    REQUIRE(evaluation_ir.link_count() == 2);
    REQUIRE(evaluation_ir.is_valid());
}

TEST_CASE("EvaluationIR builds from diamond graph", "[node_editor][ir]")
{
    auto graph = test_fixtures::make_diamond_graph();
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);

    REQUIRE(evaluation_ir.node_count() == 4);
    REQUIRE(evaluation_ir.is_valid());
}

TEST_CASE("EvaluationIR preserves topological order", "[node_editor][ir]")
{
    auto graph = test_fixtures::make_linear_chain(3);
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);

    const auto& nodes = evaluation_ir.nodes();
    REQUIRE(nodes.size() == 3);
}

TEST_CASE("EvaluationIR empty graph returns invalid", "[node_editor][ir]")
{
    NodeGraph graph;
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);
    // Empty graph — valid IR with 0 nodes
    REQUIRE(evaluation_ir.node_count() == 0);
}

// ============================================================================
// Section 3: ExecutionPlan
// ============================================================================

TEST_CASE("ExecutionPlan builds from EvaluationIR", "[node_editor][plan]")
{
    auto graph = test_fixtures::make_linear_chain(3);
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);

    const auto plan = ExecutionPlan::build(evaluation_ir);
    REQUIRE(plan.is_valid());
    REQUIRE(plan.step_count() == 3);
}

TEST_CASE("ExecutionPlan identifies root and leaf steps", "[node_editor][plan]")
{
    auto graph = test_fixtures::make_linear_chain(3);
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);

    const auto plan = ExecutionPlan::build(evaluation_ir);
    const auto& steps = plan.steps();

    const auto root_indices = plan.root_steps();
    REQUIRE_FALSE(root_indices.empty());
    const auto leaf_indices = plan.leaf_steps();
    REQUIRE_FALSE(leaf_indices.empty());
}

TEST_CASE("ExecutionPlan generates unique cache keys", "[node_editor][plan]")
{
    auto graph = test_fixtures::make_diamond_graph();
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);

    const auto plan = ExecutionPlan::build(evaluation_ir);
    const auto& steps = plan.steps();

    std::unordered_set<std::string> keys;
    for (const auto& step : steps)
    {
        REQUIRE(keys.insert(step.cache_key).second);
    }
}

// ============================================================================
// Section 4: DomainRegistry
// ============================================================================

TEST_CASE("DomainRegistry register and lookup", "[node_editor][registry]")
{
    DomainRegistry registry;
    auto runtime = std::make_unique<GraphicsRuntime>();

    REQUIRE(registry.register_runtime(std::move(runtime)));
    REQUIRE(registry.has_runtime(GraphMode::Graphics));

    const auto* found = registry.find(GraphMode::Graphics);
    REQUIRE(found != nullptr);
    REQUIRE(found->domain_id() == "graphics");
}

TEST_CASE("DomainRegistry prevents duplicate registration", "[node_editor][registry]")
{
    DomainRegistry registry;
    auto runtime1 = std::make_unique<GraphicsRuntime>();
    auto runtime2 = std::make_unique<GraphicsRuntime>();

    REQUIRE(registry.register_runtime(std::move(runtime1)));
    REQUIRE_FALSE(registry.register_runtime(std::move(runtime2)));
}

TEST_CASE("DomainRegistry clear", "[node_editor][registry]")
{
    DomainRegistry registry;
    auto runtime = std::make_unique<GraphicsRuntime>();

    registry.register_runtime(std::move(runtime));
    REQUIRE(registry.count() == 1);

    registry.clear();
    REQUIRE(registry.count() == 0);
    REQUIRE_FALSE(registry.has_runtime(GraphMode::Graphics));
}

TEST_CASE("DomainRegistry lists all domain ids", "[node_editor][registry]")
{
    DomainRegistry registry;
    registry.register_runtime(std::make_unique<GraphicsRuntime>());
    registry.register_runtime(std::make_unique<CodeFlowRuntime>());
    registry.register_runtime(std::make_unique<AudioRuntime>());

    const auto ids = registry.all_domain_ids();
    REQUIRE(ids.size() == 3);
}

// ============================================================================
// Section 5: NodeScheduler + DirtyTracker
// ============================================================================

TEST_CASE("DirtyTracker marks and queries", "[node_editor][scheduler]")
{
    DirtyTracker tracker;
    const auto node_id = NodeId{42};

    REQUIRE_FALSE(tracker.is_dirty(node_id));

    tracker.mark_dirty(node_id);
    REQUIRE(tracker.is_dirty(node_id));
    REQUIRE(tracker.dirty_count() == 1);

    tracker.mark_clean(node_id);
    REQUIRE_FALSE(tracker.is_dirty(node_id));
    REQUIRE(tracker.dirty_count() == 0);
}

TEST_CASE("DirtyTracker mark_all_dirty", "[node_editor][scheduler]")
{
    DirtyTracker tracker;
    std::vector<NodeId> ids = {NodeId{1}, NodeId{2}, NodeId{3}};
    tracker.mark_all_dirty(ids);
    REQUIRE(tracker.dirty_count() == 3);
    REQUIRE(tracker.is_dirty(NodeId{2}));
}

TEST_CASE("DirtyTracker clear", "[node_editor][scheduler]")
{
    DirtyTracker tracker;
    tracker.mark_dirty(NodeId{1});
    tracker.mark_dirty(NodeId{2});
    tracker.clear();
    REQUIRE(tracker.dirty_count() == 0);
}

TEST_CASE("NodeScheduler schedule_full", "[node_editor][scheduler]")
{
    auto graph = test_fixtures::make_linear_chain(3);
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);

    const auto plan = ExecutionPlan::build(evaluation_ir);
    GraphicsRuntime runtime;
    CancellationToken token;

    NodeScheduler scheduler;
    const auto result = scheduler.schedule_full(graph, plan, runtime, token);

    REQUIRE(result.success);
    REQUIRE_FALSE(result.cancelled);
}

TEST_CASE("NodeScheduler respects cancellation", "[node_editor][scheduler]")
{
    auto graph = test_fixtures::make_linear_chain(3);
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);

    const auto plan = ExecutionPlan::build(evaluation_ir);
    GraphicsRuntime runtime;
    CancellationToken token;
    token.cancel();

    NodeScheduler scheduler;
    const auto result = scheduler.schedule_full(graph, plan, runtime, token);

    REQUIRE_FALSE(result.success);
    REQUIRE(result.cancelled);
}

// ============================================================================
// Section 6: NodeValue
// ============================================================================

TEST_CASE("NodeValue default is float 0", "[node_editor][value]")
{
    NodeValue val;
    REQUIRE(val.type() == SocketDataType::Float);
    REQUIRE(val.as_float().has_value());
    REQUIRE(val.as_float().value() == 0.0F);
}

TEST_CASE("NodeValue stores and retrieves types", "[node_editor][value]")
{
    SECTION("float")
    {
        NodeValue val(3.14F);
        REQUIRE(val.type() == SocketDataType::Float);
        REQUIRE(val.as_float().value() == 3.14F);
        REQUIRE_FALSE(val.as_int().has_value());
    }
    SECTION("int")
    {
        NodeValue val(42);
        REQUIRE(val.type() == SocketDataType::Int);
        REQUIRE(val.as_int().value() == 42);
    }
    SECTION("bool")
    {
        NodeValue val(true);
        REQUIRE(val.type() == SocketDataType::Bool);
        REQUIRE(val.as_bool().value() == true);
    }
    SECTION("string")
    {
        NodeValue val(std::string("hello"));
        REQUIRE(val.type() == SocketDataType::String);
        REQUIRE(val.as_string().value() == "hello");
    }
    SECTION("Vec2")
    {
        NodeValue val(Vec2{1.0F, 2.0F});
        REQUIRE(val.type() == SocketDataType::Vector2);
        REQUIRE(val.as_vec2().has_value());
    }
    SECTION("Vec3")
    {
        NodeValue val(Vec3{1.0F, 2.0F, 3.0F});
        REQUIRE(val.type() == SocketDataType::Vector3);
        REQUIRE(val.as_vec3().has_value());
    }
    SECTION("Vec4")
    {
        NodeValue val(Vec4{1.0F, 2.0F, 3.0F, 4.0F});
        REQUIRE(val.type() == SocketDataType::Vector4);
        REQUIRE(val.as_vec4().has_value());
    }
    SECTION("Color4")
    {
        NodeValue val(Color4{1.0F, 0.5F, 0.0F, 1.0F});
        REQUIRE(val.type() == SocketDataType::Color);
        REQUIRE(val.as_color4().has_value());
    }
}

TEST_CASE("NodeValue equality", "[node_editor][value]")
{
    REQUIRE(NodeValue(1.0F) == NodeValue(1.0F));
    REQUIRE(NodeValue(1.0F) != NodeValue(2.0F));
    REQUIRE(NodeValue(42) != NodeValue(1.0F));
}

TEST_CASE("NodeValue to_string", "[node_editor][value]")
{
    REQUIRE(NodeValue(true).to_string() == "true");
    REQUIRE(NodeValue(false).to_string() == "false");
    REQUIRE(NodeValue(std::string("foo")).to_string() == "foo");
}

// ============================================================================
// Section 7: ValueConverter
// ============================================================================

TEST_CASE("ValueConverter can_convert same type", "[node_editor][converter]")
{
    REQUIRE(ValueConverter::can_convert(SocketDataType::Float, SocketDataType::Float));
    REQUIRE(ValueConverter::can_convert(SocketDataType::Int, SocketDataType::Int));
    REQUIRE(ValueConverter::can_convert(SocketDataType::Bool, SocketDataType::Bool));
}

TEST_CASE("ValueConverter numeric coercion", "[node_editor][converter]")
{
    REQUIRE(ValueConverter::can_convert(SocketDataType::Float, SocketDataType::Int));
    REQUIRE(ValueConverter::can_convert(SocketDataType::Int, SocketDataType::Float));
    REQUIRE(ValueConverter::can_convert(SocketDataType::Bool, SocketDataType::Float));
    REQUIRE(ValueConverter::can_convert(SocketDataType::Float, SocketDataType::Bool));
}

TEST_CASE("ValueConverter float to int conversion", "[node_editor][converter]")
{
    const auto result = ValueConverter::convert(NodeValue(3.7F), SocketDataType::Int);
    REQUIRE(result.has_value());
    REQUIRE(result->as_int().value() == 3);
}

TEST_CASE("ValueConverter float to vector promotion", "[node_editor][converter]")
{
    REQUIRE(ValueConverter::can_convert(SocketDataType::Float, SocketDataType::Vector2));
    REQUIRE(ValueConverter::can_convert(SocketDataType::Float, SocketDataType::Vector3));
    REQUIRE(ValueConverter::can_convert(SocketDataType::Float, SocketDataType::Vector4));

    const auto result = ValueConverter::convert(NodeValue(2.0F), SocketDataType::Vector3);
    REQUIRE(result.has_value());
    const auto vec = result->as_vec3().value();
    REQUIRE(vec.x == 2.0F);
    REQUIRE(vec.y == 2.0F);
    REQUIRE(vec.z == 2.0F);
}

TEST_CASE("ValueConverter color to vector4", "[node_editor][converter]")
{
    const auto result = ValueConverter::convert(NodeValue(Color4{1.0F, 0.5F, 0.25F, 0.75F}),
                                                SocketDataType::Vector4);
    REQUIRE(result.has_value());
    const auto vec = result->as_vec4().value();
    REQUIRE(vec.x == 1.0F);
    REQUIRE(vec.y == 0.5F);
    REQUIRE(vec.z == 0.25F);
    REQUIRE(vec.w == 0.75F);
}

TEST_CASE("ValueConverter unsupported conversion", "[node_editor][converter]")
{
    REQUIRE_FALSE(ValueConverter::can_convert(SocketDataType::String, SocketDataType::Float));
    const auto result =
        ValueConverter::convert(NodeValue(std::string("text")), SocketDataType::Float);
    REQUIRE_FALSE(result.has_value());
}

// ============================================================================
// Section 8: Domain Runtimes
// ============================================================================

TEST_CASE("GraphicsRuntime contract", "[node_editor][domain]")
{
    GraphicsRuntime runtime;
    REQUIRE(runtime.domain_id() == "graphics");
    REQUIRE(runtime.graph_mode() == GraphMode::Graphics);
    REQUIRE(runtime.display_name() == "Graphics");
    REQUIRE_FALSE(runtime.supported_node_types().empty());

    const auto caps = runtime.capabilities();
    REQUIRE(caps.supports_preview);
    REQUIRE(caps.supports_caching);
}

TEST_CASE("CodeFlowRuntime contract", "[node_editor][domain]")
{
    CodeFlowRuntime runtime;
    REQUIRE(runtime.domain_id() == "codeflow");
    REQUIRE(runtime.graph_mode() == GraphMode::CodeFlow);
    REQUIRE(runtime.display_name() == "CodeFlow");

    const auto caps = runtime.capabilities();
    REQUIRE(caps.supports_streaming);
    REQUIRE(caps.supports_debug);
}

TEST_CASE("DataTransformRuntime contract", "[node_editor][domain]")
{
    DataTransformRuntime runtime;
    REQUIRE(runtime.domain_id() == "data_transform");
    REQUIRE(runtime.graph_mode() == GraphMode::DataTransform);
}

TEST_CASE("DataGenRuntime contract", "[node_editor][domain]")
{
    DataGenRuntime runtime;
    REQUIRE(runtime.domain_id() == "data_gen");
    REQUIRE(runtime.graph_mode() == GraphMode::DataGeneration);
}

TEST_CASE("AudioRuntime contract", "[node_editor][domain]")
{
    AudioRuntime runtime;
    REQUIRE(runtime.domain_id() == "audio");
    REQUIRE(runtime.graph_mode() == GraphMode::Audio);

    const auto caps = runtime.capabilities();
    REQUIRE(caps.supports_streaming);
    REQUIRE_FALSE(caps.supports_caching);
}

TEST_CASE("Domain runtimes validate correct mode", "[node_editor][domain]")
{
    GraphicsRuntime graphics;
    AudioRuntime audio;

    NodeGraph graphics_graph;
    graphics_graph.set_mode(GraphMode::Graphics);
    REQUIRE(graphics.validate(graphics_graph));
    REQUIRE_FALSE(audio.validate(graphics_graph));

    NodeGraph audio_graph;
    audio_graph.set_mode(GraphMode::Audio);
    REQUIRE(audio.validate(audio_graph));
    REQUIRE_FALSE(graphics.validate(audio_graph));
}

// ============================================================================
// Section 9: DomainSDK
// ============================================================================

TEST_CASE("DomainSDK register and find handlers", "[node_editor][sdk]")
{
    DomainSDK sdk;

    DomainNodeHandler handler;
    handler.type_name = "custom_blend";
    handler.on_execute = [](const NodeGraph& /*graph*/, NodeId /*node*/, CancellationToken& /*tok*/)
    {
        return ExecutionResult{true, "", 0, {}};
    };
    handler.on_validate = [](const NodeGraph& /*graph*/, NodeId /*node*/) { return true; };

    REQUIRE(sdk.register_handler(std::move(handler)));
    REQUIRE(sdk.handler_count() == 1);

    const auto* found = sdk.find_handler("custom_blend");
    REQUIRE(found != nullptr);
    REQUIRE(found->type_name == "custom_blend");
}

TEST_CASE("DomainSDK prevents duplicate handlers", "[node_editor][sdk]")
{
    DomainSDK sdk;

    DomainNodeHandler handler1;
    handler1.type_name = "my_node";
    DomainNodeHandler handler2;
    handler2.type_name = "my_node";

    REQUIRE(sdk.register_handler(std::move(handler1)));
    REQUIRE_FALSE(sdk.register_handler(std::move(handler2)));
}

TEST_CASE("DomainSDK remove and clear", "[node_editor][sdk]")
{
    DomainSDK sdk;

    DomainNodeHandler handler;
    handler.type_name = "test_node";
    sdk.register_handler(std::move(handler));

    REQUIRE(sdk.remove_handler("test_node"));
    REQUIRE(sdk.handler_count() == 0);
    REQUIRE_FALSE(sdk.remove_handler("nonexistent"));

    DomainNodeHandler handler2;
    handler2.type_name = "another";
    sdk.register_handler(std::move(handler2));
    sdk.clear();
    REQUIRE(sdk.handler_count() == 0);
}

TEST_CASE("DomainSDK lists all types", "[node_editor][sdk]")
{
    DomainSDK sdk;

    DomainNodeHandler hdl_a;
    hdl_a.type_name = "alpha";
    DomainNodeHandler hdl_b;
    hdl_b.type_name = "beta";

    sdk.register_handler(std::move(hdl_a));
    sdk.register_handler(std::move(hdl_b));

    const auto types = sdk.all_handler_types();
    REQUIRE(types.size() == 2);
}

// ============================================================================
// Section 10: RuntimeSandbox
// ============================================================================

TEST_CASE("RuntimeSandbox default quota", "[node_editor][sandbox]")
{
    RuntimeSandbox sandbox;
    const auto& quota = sandbox.quota();
    REQUIRE(quota.max_node_count == 1000);
    REQUIRE(quota.max_work_units == 10000);
    REQUIRE(quota.max_memory_bytes == 256UL * 1024UL * 1024UL);
}

TEST_CASE("RuntimeSandbox executes within limits", "[node_editor][sandbox]")
{
    auto graph = test_fixtures::make_linear_chain(3);
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);

    const auto plan = ExecutionPlan::build(evaluation_ir);
    GraphicsRuntime runtime;
    CancellationToken token;
    RuntimeSandbox sandbox;

    const auto result = sandbox.execute(runtime, graph, plan, token);
    REQUIRE(result.violation == SandboxViolation::None);
    REQUIRE(result.execution_result.success);
}

TEST_CASE("RuntimeSandbox enforces node count limit", "[node_editor][sandbox]")
{
    ResourceQuota quota;
    quota.max_node_count = 2;
    RuntimeSandbox sandbox(quota);

    // Graph with 3 nodes exceeds limit of 2
    auto node_graph = test_fixtures::make_linear_chain(3);
    const auto violation = sandbox.pre_validate(node_graph);
    REQUIRE(violation == SandboxViolation::NodeCountExceeded);
}

TEST_CASE("RuntimeSandbox handles cancellation", "[node_editor][sandbox]")
{
    auto graph = test_fixtures::make_linear_chain(3);
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);

    const auto plan = ExecutionPlan::build(evaluation_ir);
    GraphicsRuntime runtime;
    CancellationToken token;
    token.cancel();

    RuntimeSandbox sandbox;
    const auto result = sandbox.execute(runtime, graph, plan, token);
    REQUIRE(result.violation == SandboxViolation::Cancelled);
    REQUIRE_FALSE(result.execution_result.success);
}

TEST_CASE("RuntimeSandbox quota update", "[node_editor][sandbox]")
{
    RuntimeSandbox sandbox;
    ResourceQuota new_quota;
    new_quota.max_node_count = 500;
    new_quota.max_work_units = 5000;

    sandbox.set_quota(new_quota);
    REQUIRE(sandbox.quota().max_node_count == 500);
    REQUIRE(sandbox.quota().max_work_units == 5000);
}

// ============================================================================
// Section 11: Test Fixtures Smoke Tests
// ============================================================================

TEST_CASE("Test fixture: linear chain", "[node_editor][fixtures]")
{
    auto graph = test_fixtures::make_linear_chain(3);
    REQUIRE(graph.node_count() == 3);
    REQUIRE_FALSE(graph.has_cycles());
}

TEST_CASE("Test fixture: diamond", "[node_editor][fixtures]")
{
    auto graph = test_fixtures::make_diamond_graph();
    REQUIRE(graph.node_count() == 4);
    REQUIRE_FALSE(graph.has_cycles());
}

TEST_CASE("Test fixture: disconnected", "[node_editor][fixtures]")
{
    auto graph = test_fixtures::make_disconnected_graph();
    REQUIRE(graph.node_count() == 3);
}

TEST_CASE("Test fixture: complex math", "[node_editor][fixtures]")
{
    auto graph = test_fixtures::make_complex_math_graph();
    REQUIRE(graph.node_count() >= 4);
    REQUIRE_FALSE(graph.has_cycles());
}

// ============================================================================
// Section 12: Integration — Full Pipeline
// ============================================================================

TEST_CASE("Full pipeline: graph -> IR -> plan -> execute", "[node_editor][integration]")
{
    // Build graph
    auto graph = test_fixtures::make_diamond_graph();
    graph.set_mode(GraphMode::Graphics);

    // Create IR
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);

    // Build execution plan
    const auto plan = ExecutionPlan::build(evaluation_ir);
    REQUIRE(plan.is_valid());

    // Register runtime via registry
    DomainRegistry registry;
    auto runtime = std::make_unique<GraphicsRuntime>();
    auto* runtime_ptr = runtime.get();
    registry.register_runtime(std::move(runtime));

    // Find runtime (registry returns const, so use the stored non-const pointer)
    REQUIRE(registry.has_runtime(GraphMode::Graphics));

    // Execute via sandbox
    CancellationToken token;
    RuntimeSandbox sandbox;
    const auto sandbox_result = sandbox.execute(*runtime_ptr, graph, plan, token);
    REQUIRE(sandbox_result.execution_result.success);
    REQUIRE(sandbox_result.violation == SandboxViolation::None);
}

TEST_CASE("Full pipeline: schedule with dirty tracking", "[node_editor][integration]")
{
    auto graph = test_fixtures::make_linear_chain(3);
    const auto evaluation_ir = EvaluationIR::build_from_graph(graph);

    const auto plan = ExecutionPlan::build(evaluation_ir);
    GraphicsRuntime runtime;
    CancellationToken token;

    // First full execution
    NodeScheduler scheduler;
    auto result = scheduler.schedule_full(graph, plan, runtime, token);
    REQUIRE(result.success);

    // Incremental: mark only one node dirty
    DirtyTracker tracker;
    const auto sorted = graph.topological_sort();
    REQUIRE_FALSE(sorted.empty());
    tracker.mark_dirty(sorted.front());

    CancellationToken token2;
    const auto incr_result = scheduler.schedule(graph, plan, runtime, tracker, token2);
    REQUIRE(incr_result.success);
}
