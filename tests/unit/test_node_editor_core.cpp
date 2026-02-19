// V11 Node Editor — Core Unit Tests
// Covers Phases 01–03: Types, Graph Data Model, Identity System,
// Node Definition Registry, Serialization, and Command Stack.

#include "node_editor/GraphSerializer.h"
#include "node_editor/NodeCommand.h"
#include "node_editor/NodeCommandStack.h"
#include "node_editor/NodeDefinition.h"
#include "node_editor/NodeDefinitionRegistry.h"
#include "node_editor/NodeEditorTypes.h"
#include "node_editor/NodeGraph.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::node_editor;

// ============================================================================
// Section 1: Strong Typed IDs (NodeEditorTypes.h)
// ============================================================================

TEST_CASE("NodeId default is invalid", "[node_editor][types]")
{
    NodeId nid;
    REQUIRE_FALSE(nid.is_valid());
    REQUIRE(nid.value == 0);
}

TEST_CASE("NodeId explicit construction is valid", "[node_editor][types]")
{
    NodeId nid{42};
    REQUIRE(nid.is_valid());
    REQUIRE(nid.value == 42);
}

TEST_CASE("NodeId equality and ordering", "[node_editor][types]")
{
    NodeId id_a{1};
    NodeId id_b{2};
    NodeId id_a2{1};

    REQUIRE(id_a == id_a2);
    REQUIRE_FALSE(id_a == id_b);
    REQUIRE(id_a < id_b);
}

TEST_CASE("SocketId default is invalid", "[node_editor][types]")
{
    SocketId sid;
    REQUIRE_FALSE(sid.is_valid());
}

TEST_CASE("LinkId default is invalid", "[node_editor][types]")
{
    LinkId lid;
    REQUIRE_FALSE(lid.is_valid());
}

TEST_CASE("GraphId default is invalid", "[node_editor][types]")
{
    GraphId gid;
    REQUIRE_FALSE(gid.is_valid());
}

TEST_CASE("Strong ID hashing works for unordered containers", "[node_editor][types]")
{
    std::unordered_map<NodeId, std::string> node_map;
    NodeId nid{10};
    node_map[nid] = "test_node";
    REQUIRE(node_map.at(nid) == "test_node");

    std::unordered_map<SocketId, int> socket_map;
    SocketId sid{20};
    socket_map[sid] = 42;
    REQUIRE(socket_map.at(sid) == 42);

    std::unordered_map<LinkId, bool> link_map;
    LinkId lid{30};
    link_map[lid] = true;
    REQUIRE(link_map.at(lid));
}

// ============================================================================
// Section 2: NodeGraph — CRUD Operations
// ============================================================================

TEST_CASE("NodeGraph default construction", "[node_editor][graph]")
{
    NodeGraph graph;
    REQUIRE(graph.node_count() == 0);
    REQUIRE(graph.link_count() == 0);
    REQUIRE(graph.name() == "Untitled");
    REQUIRE(graph.mode() == GraphMode::Graphics);
}

TEST_CASE("NodeGraph construction with GraphId", "[node_editor][graph]")
{
    GraphId gid{100};
    NodeGraph graph(gid);
    REQUIRE(graph.id() == gid);
}

TEST_CASE("NodeGraph add and find node", "[node_editor][graph]")
{
    NodeGraph graph;
    auto nid = graph.add_node("math.add", Vec2{10.0F, 20.0F});

    REQUIRE(nid.is_valid());
    REQUIRE(graph.node_count() == 1);

    const auto* node = graph.find_node(nid);
    REQUIRE(node != nullptr);
    REQUIRE(node->type_name == "math.add");
    REQUIRE(node->position.x == 10.0F);
    REQUIRE(node->position.y == 20.0F);
}

TEST_CASE("NodeGraph add multiple nodes", "[node_editor][graph]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("input.value", Vec2{0, 0});
    auto nid_b = graph.add_node("math.multiply", Vec2{200, 100});
    auto nid_c = graph.add_node("output.result", Vec2{400, 50});

    REQUIRE(graph.node_count() == 3);
    REQUIRE(nid_a != nid_b);
    REQUIRE(nid_b != nid_c);
}

TEST_CASE("NodeGraph remove node", "[node_editor][graph]")
{
    NodeGraph graph;
    auto nid = graph.add_node("test", Vec2{0, 0});
    REQUIRE(graph.node_count() == 1);

    REQUIRE(graph.remove_node(nid));
    REQUIRE(graph.node_count() == 0);
    REQUIRE(graph.find_node(nid) == nullptr);
}

TEST_CASE("NodeGraph remove nonexistent node returns false", "[node_editor][graph]")
{
    NodeGraph graph;
    REQUIRE_FALSE(graph.remove_node(NodeId{999}));
}

TEST_CASE("NodeGraph all_node_ids", "[node_editor][graph]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{0, 0});

    auto ids = graph.all_node_ids();
    REQUIRE(ids.size() == 2);
    // Both IDs present (order not guaranteed)
    bool found_a = std::find(ids.begin(), ids.end(), nid_a) != ids.end();
    bool found_b = std::find(ids.begin(), ids.end(), nid_b) != ids.end();
    REQUIRE(found_a);
    REQUIRE(found_b);
}

// ============================================================================
// Section 3: Socket Operations
// ============================================================================

TEST_CASE("NodeGraph add socket to node", "[node_editor][graph][socket]")
{
    NodeGraph graph;
    auto nid = graph.add_node("test", Vec2{0, 0});
    auto sid = graph.add_socket(nid, SocketDirection::Input, SocketDataType::Float, "Value", "0.0");

    REQUIRE(sid.is_valid());
    const auto* socket = graph.find_socket(sid);
    REQUIRE(socket != nullptr);
    REQUIRE(socket->owner_node == nid);
    REQUIRE(socket->direction == SocketDirection::Input);
    REQUIRE(socket->data_type == SocketDataType::Float);
    REQUIRE(socket->label == "Value");
    REQUIRE(socket->default_value == "0.0");
    REQUIRE_FALSE(socket->is_connected());
}

TEST_CASE("NodeGraph socket on invalid node returns invalid id", "[node_editor][graph][socket]")
{
    NodeGraph graph;
    auto sid =
        graph.add_socket(NodeId{999}, SocketDirection::Input, SocketDataType::Float, "x", "");
    REQUIRE_FALSE(sid.is_valid());
}

TEST_CASE("NodeGraph sockets are linked to their owner node", "[node_editor][graph][socket]")
{
    NodeGraph graph;
    auto nid = graph.add_node("test", Vec2{0, 0});
    auto in_sid = graph.add_socket(nid, SocketDirection::Input, SocketDataType::Int, "in", "");
    auto out_sid = graph.add_socket(nid, SocketDirection::Output, SocketDataType::Int, "out", "");

    const auto* node = graph.find_node(nid);
    REQUIRE(node != nullptr);
    REQUIRE(node->inputs.size() == 1);
    REQUIRE(node->inputs[0] == in_sid);
    REQUIRE(node->outputs.size() == 1);
    REQUIRE(node->outputs[0] == out_sid);
}

// ============================================================================
// Section 4: Link Operations
// ============================================================================

TEST_CASE("NodeGraph add link between compatible sockets", "[node_editor][graph][link]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("source", Vec2{0, 0});
    auto nid_b = graph.add_node("target", Vec2{200, 0});
    auto out_sid =
        graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_sid = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");

    auto lid = graph.add_link(out_sid, in_sid);
    REQUIRE(lid.is_valid());
    REQUIRE(graph.link_count() == 1);

    const auto* link = graph.find_link(lid);
    REQUIRE(link != nullptr);
    REQUIRE(link->source == out_sid);
    REQUIRE(link->target == in_sid);
    REQUIRE(link->valid);
}

TEST_CASE("NodeGraph link rejects same-node connection", "[node_editor][graph][link]")
{
    NodeGraph graph;
    auto nid = graph.add_node("test", Vec2{0, 0});
    auto out_sid = graph.add_socket(nid, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_sid = graph.add_socket(nid, SocketDirection::Input, SocketDataType::Float, "in", "");

    auto lid = graph.add_link(out_sid, in_sid);
    REQUIRE_FALSE(lid.is_valid());
}

TEST_CASE("NodeGraph link rejects wrong direction", "[node_editor][graph][link]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});
    auto in_a = graph.add_socket(nid_a, SocketDirection::Input, SocketDataType::Float, "in", "");
    auto in_b = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");

    // Input -> Input should fail
    auto lid = graph.add_link(in_a, in_b);
    REQUIRE_FALSE(lid.is_valid());
}

TEST_CASE("NodeGraph link rejects incompatible types", "[node_editor][graph][link]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});
    auto out_str =
        graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::String, "out", "");
    auto in_geom =
        graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Geometry, "in", "");

    auto lid = graph.add_link(out_str, in_geom);
    REQUIRE_FALSE(lid.is_valid());
}

TEST_CASE("NodeGraph link rejects duplicates", "[node_editor][graph][link]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});
    auto out_sid =
        graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_sid = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");

    auto lid1 = graph.add_link(out_sid, in_sid);
    REQUIRE(lid1.is_valid());

    auto lid2 = graph.add_link(out_sid, in_sid);
    REQUIRE_FALSE(lid2.is_valid());
}

TEST_CASE("NodeGraph remove link", "[node_editor][graph][link]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});
    auto out_sid =
        graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_sid = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");

    auto lid = graph.add_link(out_sid, in_sid);
    REQUIRE(graph.link_count() == 1);

    REQUIRE(graph.remove_link(lid));
    REQUIRE(graph.link_count() == 0);
    REQUIRE(graph.find_link(lid) == nullptr);
}

TEST_CASE("NodeGraph removing node removes attached links", "[node_editor][graph][link]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});
    auto out_sid =
        graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_sid = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");

    graph.add_link(out_sid, in_sid);
    REQUIRE(graph.link_count() == 1);

    graph.remove_node(nid_a);
    REQUIRE(graph.link_count() == 0);
}

// ============================================================================
// Section 5: Type Compatibility
// ============================================================================

TEST_CASE("Type compatibility — same types via add_link", "[node_editor][graph][types]")
{
    // Test type compatibility indirectly through the public add_link API
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});

    // Float -> Float should succeed
    auto out_f = graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_f = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");
    REQUIRE(graph.add_link(out_f, in_f).is_valid());
}

TEST_CASE("Type compatibility — numeric coercion via add_link", "[node_editor][graph][types]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});

    // Float -> Int should succeed (numeric coercion)
    auto out_f = graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_i = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Int, "in", "");
    REQUIRE(graph.add_link(out_f, in_i).is_valid());
}

TEST_CASE("Type compatibility — vector promotion via add_link", "[node_editor][graph][types]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});

    // Float -> Vector3 should succeed (promotion)
    auto out_f = graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_v3 = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Vector3, "in", "");
    REQUIRE(graph.add_link(out_f, in_v3).is_valid());
}

TEST_CASE("Type compatibility — color and vector4 via add_link", "[node_editor][graph][types]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});

    // Color -> Vector4 should succeed
    auto out_c = graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Color, "out", "");
    auto in_v4 = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Vector4, "in", "");
    REQUIRE(graph.add_link(out_c, in_v4).is_valid());
}

TEST_CASE("Type compatibility — bool widening via add_link", "[node_editor][graph][types]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});

    // Bool -> Int should succeed
    auto out_b = graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Bool, "out", "");
    auto in_i = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Int, "in", "");
    REQUIRE(graph.add_link(out_b, in_i).is_valid());
}

TEST_CASE("Type compatibility — incompatible types via add_link", "[node_editor][graph][types]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});

    // String -> Float should fail
    auto out_s =
        graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::String, "out", "");
    auto in_f = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");
    REQUIRE_FALSE(graph.add_link(out_s, in_f).is_valid());
}

// ============================================================================
// Section 6: Topological Sort & Cycle Detection
// ============================================================================

TEST_CASE("NodeGraph topological sort — linear chain", "[node_editor][graph][topo]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});
    auto nid_c = graph.add_node("c", Vec2{400, 0});

    auto out_a = graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_b = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");
    auto out_b = graph.add_socket(nid_b, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_c = graph.add_socket(nid_c, SocketDirection::Input, SocketDataType::Float, "in", "");

    graph.add_link(out_a, in_b);
    graph.add_link(out_b, in_c);

    auto sorted = graph.topological_sort();
    REQUIRE(sorted.size() == 3);

    // A must come before B, B before C
    auto pos_a = std::find(sorted.begin(), sorted.end(), nid_a) - sorted.begin();
    auto pos_b = std::find(sorted.begin(), sorted.end(), nid_b) - sorted.begin();
    auto pos_c = std::find(sorted.begin(), sorted.end(), nid_c) - sorted.begin();
    REQUIRE(pos_a < pos_b);
    REQUIRE(pos_b < pos_c);
}

TEST_CASE("NodeGraph has_cycles — acyclic graph", "[node_editor][graph][topo]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});
    auto out_a = graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_b = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");
    graph.add_link(out_a, in_b);

    REQUIRE_FALSE(graph.has_cycles());
}

TEST_CASE("NodeGraph empty graph has no cycles", "[node_editor][graph][topo]")
{
    NodeGraph graph;
    REQUIRE_FALSE(graph.has_cycles());
}

// ============================================================================
// Section 7: Graph Validation
// ============================================================================

TEST_CASE("NodeGraph validate — valid empty graph", "[node_editor][graph][validate]")
{
    NodeGraph graph;
    auto result = graph.validate();
    REQUIRE(result.valid);
    REQUIRE(result.errors.empty());
}

TEST_CASE("NodeGraph validate — valid connected graph", "[node_editor][graph][validate]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});
    auto out_a = graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_b = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "1.0");
    graph.add_link(out_a, in_b);

    auto result = graph.validate();
    REQUIRE(result.valid);
    REQUIRE(result.errors.empty());
}

TEST_CASE("NodeGraph validate — warns on unconnected input without default",
          "[node_editor][graph][validate]")
{
    NodeGraph graph;
    auto nid = graph.add_node("test", Vec2{0, 0});
    graph.add_socket(nid, SocketDirection::Input, SocketDataType::Float, "value", "");

    auto result = graph.validate();
    REQUIRE(result.valid); // Warnings don't invalidate
    REQUIRE_FALSE(result.warnings.empty());
}

TEST_CASE("NodeGraph clear resets graph", "[node_editor][graph]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});
    auto out_a = graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_b = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");
    graph.add_link(out_a, in_b);

    graph.clear();
    REQUIRE(graph.node_count() == 0);
    REQUIRE(graph.link_count() == 0);
}

// ============================================================================
// Section 8: NodeDefinitionRegistry
// ============================================================================

TEST_CASE("NodeDefinitionRegistry register and find", "[node_editor][registry]")
{
    NodeDefinitionRegistry registry;

    NodeDefinition def;
    def.type_name = "math.add";
    def.display_name = "Add";
    def.category = "math";
    def.description = "Adds two numbers";
    def.inputs = {{"A", SocketDirection::Input, SocketDataType::Float, "0", "First operand"},
                  {"B", SocketDirection::Input, SocketDataType::Float, "0", "Second operand"}};
    def.outputs = {
        {"Result", SocketDirection::Output, SocketDataType::Float, "", "Sum of A and B"}};

    REQUIRE(registry.register_definition(def));
    REQUIRE(registry.count() == 1);
    REQUIRE(registry.contains("math.add"));

    const auto* found = registry.find("math.add");
    REQUIRE(found != nullptr);
    REQUIRE(found->display_name == "Add");
    REQUIRE(found->inputs.size() == 2);
    REQUIRE(found->outputs.size() == 1);
}

TEST_CASE("NodeDefinitionRegistry rejects duplicate registration", "[node_editor][registry]")
{
    NodeDefinitionRegistry registry;

    NodeDefinition def;
    def.type_name = "test.node";
    REQUIRE(registry.register_definition(def));
    REQUIRE_FALSE(registry.register_definition(def));
    REQUIRE(registry.count() == 1);
}

TEST_CASE("NodeDefinitionRegistry find nonexistent returns nullptr", "[node_editor][registry]")
{
    NodeDefinitionRegistry registry;
    REQUIRE(registry.find("nonexistent") == nullptr);
}

TEST_CASE("NodeDefinitionRegistry by_category", "[node_editor][registry]")
{
    NodeDefinitionRegistry registry;

    NodeDefinition math_add;
    math_add.type_name = "math.add";
    math_add.category = "math";
    registry.register_definition(math_add);

    NodeDefinition math_mul;
    math_mul.type_name = "math.multiply";
    math_mul.category = "math";
    registry.register_definition(math_mul);

    NodeDefinition io_input;
    io_input.type_name = "io.input";
    io_input.category = "io";
    registry.register_definition(io_input);

    auto math_nodes = registry.by_category("math");
    REQUIRE(math_nodes.size() == 2);

    auto io_nodes = registry.by_category("io");
    REQUIRE(io_nodes.size() == 1);

    auto unknown = registry.by_category("unknown");
    REQUIRE(unknown.empty());
}

TEST_CASE("NodeDefinitionRegistry all returns all definitions", "[node_editor][registry]")
{
    NodeDefinitionRegistry registry;

    NodeDefinition def_a;
    def_a.type_name = "a";
    registry.register_definition(def_a);

    NodeDefinition def_b;
    def_b.type_name = "b";
    registry.register_definition(def_b);

    auto all = registry.all();
    REQUIRE(all.size() == 2);
}

TEST_CASE("NodeDefinitionRegistry clear", "[node_editor][registry]")
{
    NodeDefinitionRegistry registry;

    NodeDefinition def;
    def.type_name = "test";
    registry.register_definition(def);
    REQUIRE(registry.count() == 1);

    registry.clear();
    REQUIRE(registry.count() == 0);
    REQUIRE_FALSE(registry.contains("test"));
}

// ============================================================================
// Section 9: GraphSerializer — Round-trip Serialization
// ============================================================================

TEST_CASE("GraphSerializer round-trip — empty graph", "[node_editor][serializer]")
{
    NodeGraph original;
    original.set_name("Test Graph");
    original.set_mode(GraphMode::CodeFlow);

    auto json_str = GraphSerializer::serialize(original);
    REQUIRE_FALSE(json_str.empty());

    auto deserialized = GraphSerializer::deserialize(json_str);
    REQUIRE(deserialized.name() == "Test Graph");
    REQUIRE(deserialized.mode() == GraphMode::CodeFlow);
    REQUIRE(deserialized.node_count() == 0);
}

TEST_CASE("GraphSerializer round-trip — graph with nodes and links", "[node_editor][serializer]")
{
    NodeGraph original;
    original.set_name("Math Graph");

    auto nid_a = original.add_node("math.add", Vec2{100.0F, 200.0F});
    auto nid_b = original.add_node("output.display", Vec2{400.0F, 200.0F});
    auto out_a =
        original.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "result", "");
    auto in_b =
        original.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "value", "0");
    original.add_link(out_a, in_b);

    auto json_str = GraphSerializer::serialize(original);
    auto deserialized = GraphSerializer::deserialize(json_str);

    REQUIRE(deserialized.name() == "Math Graph");
    REQUIRE(deserialized.node_count() == 2);
    REQUIRE(deserialized.link_count() == 1);

    // Verify structural integrity after deserialization
    auto validation = deserialized.validate();
    REQUIRE(validation.valid);
}

TEST_CASE("GraphSerializer schema_version extraction", "[node_editor][serializer]")
{
    NodeGraph graph;
    auto json_str = GraphSerializer::serialize(graph);
    int ver = GraphSerializer::schema_version(json_str);
    REQUIRE(ver == NodeGraph::kSchemaVersion);
}

TEST_CASE("GraphSerializer deserialize invalid JSON returns empty graph",
          "[node_editor][serializer]")
{
    auto graph = GraphSerializer::deserialize("not valid json }{");
    REQUIRE(graph.node_count() == 0);
}

// ============================================================================
// Section 10: NodeCommandStack — Undo/Redo
// ============================================================================

TEST_CASE("NodeCommandStack execute and undo AddNodeCommand", "[node_editor][command]")
{
    NodeGraph graph;
    NodeCommandStack stack;

    auto cmd = std::make_unique<AddNodeCommand>("test.node", Vec2{50, 50});
    stack.execute(std::move(cmd), graph);

    REQUIRE(graph.node_count() == 1);
    REQUIRE(stack.can_undo());
    REQUIRE_FALSE(stack.can_redo());

    stack.undo(graph);
    REQUIRE(graph.node_count() == 0);
    REQUIRE(stack.can_redo());
}

TEST_CASE("NodeCommandStack redo after undo", "[node_editor][command]")
{
    NodeGraph graph;
    NodeCommandStack stack;

    auto cmd = std::make_unique<AddNodeCommand>("test.node", Vec2{50, 50});
    stack.execute(std::move(cmd), graph);
    stack.undo(graph);

    REQUIRE(graph.node_count() == 0);

    stack.redo(graph);
    REQUIRE(graph.node_count() == 1);
}

TEST_CASE("NodeCommandStack new command clears redo stack", "[node_editor][command]")
{
    NodeGraph graph;
    NodeCommandStack stack;

    stack.execute(std::make_unique<AddNodeCommand>("a", Vec2{0, 0}), graph);
    stack.execute(std::make_unique<AddNodeCommand>("b", Vec2{100, 0}), graph);
    REQUIRE(graph.node_count() == 2);

    stack.undo(graph); // Remove "b"
    REQUIRE(stack.can_redo());

    // Execute new command — should clear redo
    stack.execute(std::make_unique<AddNodeCommand>("c", Vec2{200, 0}), graph);
    REQUIRE_FALSE(stack.can_redo());
    REQUIRE(graph.node_count() == 2);
}

TEST_CASE("NodeCommandStack MoveNodeCommand", "[node_editor][command]")
{
    NodeGraph graph;
    NodeCommandStack stack;

    stack.execute(std::make_unique<AddNodeCommand>("test", Vec2{10, 20}), graph);
    auto node_ids = graph.all_node_ids();
    REQUIRE(node_ids.size() == 1);

    auto nid = node_ids[0];
    const auto* node = graph.find_node(nid);
    REQUIRE(node->position.x == 10.0F);
    REQUIRE(node->position.y == 20.0F);

    stack.execute(std::make_unique<MoveNodeCommand>(nid, Vec2{100, 200}), graph);
    node = graph.find_node(nid);
    REQUIRE(node->position.x == 100.0F);
    REQUIRE(node->position.y == 200.0F);

    stack.undo(graph);
    node = graph.find_node(nid);
    REQUIRE(node->position.x == 10.0F);
    REQUIRE(node->position.y == 20.0F);
}

TEST_CASE("NodeCommandStack depth enforcement", "[node_editor][command]")
{
    NodeGraph graph;
    NodeCommandStack stack(3); // Max depth of 3

    stack.execute(std::make_unique<AddNodeCommand>("a", Vec2{0, 0}), graph);
    stack.execute(std::make_unique<AddNodeCommand>("b", Vec2{0, 0}), graph);
    stack.execute(std::make_unique<AddNodeCommand>("c", Vec2{0, 0}), graph);
    stack.execute(std::make_unique<AddNodeCommand>("d", Vec2{0, 0}), graph);

    REQUIRE(graph.node_count() == 4);

    // Undo 3 times (max depth)
    stack.undo(graph); // Remove d
    stack.undo(graph); // Remove c
    stack.undo(graph); // Remove b

    // 'a' was pushed out of the stack by depth enforcement
    REQUIRE_FALSE(stack.can_undo());
    REQUIRE(graph.node_count() == 1); // 'a' remains
}

TEST_CASE("NodeCommandStack clear", "[node_editor][command]")
{
    NodeGraph graph;
    NodeCommandStack stack;

    stack.execute(std::make_unique<AddNodeCommand>("a", Vec2{0, 0}), graph);
    stack.execute(std::make_unique<AddNodeCommand>("b", Vec2{0, 0}), graph);

    stack.clear();
    REQUIRE_FALSE(stack.can_undo());
    REQUIRE_FALSE(stack.can_redo());
}

TEST_CASE("NodeCommandStack AddLinkCommand", "[node_editor][command]")
{
    NodeGraph graph;
    NodeCommandStack stack;

    auto nid_a = graph.add_node("source", Vec2{0, 0});
    auto nid_b = graph.add_node("target", Vec2{200, 0});
    auto out_sid =
        graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_sid = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");

    stack.execute(std::make_unique<AddLinkCommand>(out_sid, in_sid), graph);
    REQUIRE(graph.link_count() == 1);

    stack.undo(graph);
    REQUIRE(graph.link_count() == 0);

    stack.redo(graph);
    REQUIRE(graph.link_count() == 1);
}

// ============================================================================
// Section 11: Graph Name and Mode Setters
// ============================================================================

TEST_CASE("NodeGraph set_name and set_mode", "[node_editor][graph]")
{
    NodeGraph graph;

    graph.set_name("My Graph");
    REQUIRE(graph.name() == "My Graph");

    graph.set_mode(GraphMode::DataTransform);
    REQUIRE(graph.mode() == GraphMode::DataTransform);
}

TEST_CASE("NodeGraph set_id", "[node_editor][graph]")
{
    NodeGraph graph;
    GraphId gid{42};
    graph.set_id(gid);
    REQUIRE(graph.id() == gid);
}

// ============================================================================
// Section 12: Links for Socket
// ============================================================================

TEST_CASE("NodeGraph links_for_socket", "[node_editor][graph][link]")
{
    NodeGraph graph;
    auto nid_a = graph.add_node("a", Vec2{0, 0});
    auto nid_b = graph.add_node("b", Vec2{200, 0});
    auto nid_c = graph.add_node("c", Vec2{200, 100});
    auto out_a = graph.add_socket(nid_a, SocketDirection::Output, SocketDataType::Float, "out", "");
    auto in_b = graph.add_socket(nid_b, SocketDirection::Input, SocketDataType::Float, "in", "");
    auto in_c = graph.add_socket(nid_c, SocketDirection::Input, SocketDataType::Float, "in", "");

    auto lid1 = graph.add_link(out_a, in_b);
    auto lid2 = graph.add_link(out_a, in_c);

    auto links = graph.links_for_socket(out_a);
    REQUIRE(links.size() == 2);

    bool found1 = std::find(links.begin(), links.end(), lid1) != links.end();
    bool found2 = std::find(links.begin(), links.end(), lid2) != links.end();
    REQUIRE(found1);
    REQUIRE(found2);
}

TEST_CASE("NodeGraph links_for_socket invalid socket returns empty", "[node_editor][graph][link]")
{
    NodeGraph graph;
    auto links = graph.links_for_socket(SocketId{999});
    REQUIRE(links.empty());
}
