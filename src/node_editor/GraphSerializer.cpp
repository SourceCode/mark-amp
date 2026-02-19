// V11 Phase 06: Node Graph Serialization & Version Migrations
// JSON serialization implementation using nlohmann/json.

#include "node_editor/GraphSerializer.h"

#include <nlohmann/json.hpp>

namespace markamp::node_editor
{

auto GraphSerializer::serialize(const NodeGraph& graph) -> std::string
{
    nlohmann::json root;
    root["schema_version"] = NodeGraph::kSchemaVersion;
    root["graph_id"] = graph.id().value;
    root["name"] = graph.name();
    root["mode"] = static_cast<int>(graph.mode());

    // Serialize nodes
    auto& nodes_arr = root["nodes"];
    nodes_arr = nlohmann::json::array();

    // We need to iterate all nodes — gather IDs first
    auto node_ids = graph.all_node_ids();
    for (const auto& nid : node_ids)
    {
        const auto* node = graph.find_node(nid);
        if (node == nullptr)
        {
            continue;
        }

        nlohmann::json node_obj;
        node_obj["id"] = node->id.value;
        node_obj["type_name"] = node->type_name;
        node_obj["label"] = node->label;
        node_obj["position"] = {{"x", node->position.x}, {"y", node->position.y}};
        node_obj["dimensions"] = {{"w", node->dimensions.x}, {"h", node->dimensions.y}};
        node_obj["flags"] = static_cast<uint16_t>(node->flags);

        // Serialize input socket IDs
        node_obj["inputs"] = nlohmann::json::array();
        for (const auto& sid : node->inputs)
        {
            node_obj["inputs"].push_back(sid.value);
        }
        node_obj["outputs"] = nlohmann::json::array();
        for (const auto& sid : node->outputs)
        {
            node_obj["outputs"].push_back(sid.value);
        }

        nodes_arr.push_back(std::move(node_obj));
    }

    // Serialize sockets (flat list — reconstructed via owner_node on deserialize)
    auto& sockets_arr = root["sockets"];
    sockets_arr = nlohmann::json::array();

    // Gather all socket IDs from all nodes
    for (const auto& nid : node_ids)
    {
        const auto* node = graph.find_node(nid);
        if (node == nullptr)
        {
            continue;
        }
        auto all_sids = node->inputs;
        all_sids.insert(all_sids.end(), node->outputs.begin(), node->outputs.end());
        for (const auto& sid : all_sids)
        {
            const auto* socket = graph.find_socket(sid);
            if (socket == nullptr)
            {
                continue;
            }

            nlohmann::json sock_obj;
            sock_obj["id"] = socket->id.value;
            sock_obj["owner_node"] = socket->owner_node.value;
            sock_obj["direction"] = static_cast<int>(socket->direction);
            sock_obj["data_type"] = static_cast<int>(socket->data_type);
            sock_obj["label"] = socket->label;
            sock_obj["default_value"] = socket->default_value;
            sockets_arr.push_back(std::move(sock_obj));
        }
    }

    // Serialize links
    auto& links_arr = root["links"];
    links_arr = nlohmann::json::array();

    auto link_ids = graph.all_link_ids();
    for (const auto& lid : link_ids)
    {
        const auto* link = graph.find_link(lid);
        if (link == nullptr)
        {
            continue;
        }

        nlohmann::json link_obj;
        link_obj["id"] = link->id.value;
        link_obj["source"] = link->source.value;
        link_obj["target"] = link->target.value;
        link_obj["valid"] = link->valid;
        links_arr.push_back(std::move(link_obj));
    }

    return root.dump(2);
}

auto GraphSerializer::deserialize(const std::string& json_str) -> NodeGraph
{
    NodeGraph graph;

    try
    {
        auto root = nlohmann::json::parse(json_str);

        // Schema version check
        const int version = root.value("schema_version", 0);
        if (version > NodeGraph::kSchemaVersion)
        {
            // Future version — return empty graph
            return graph;
        }

        graph.set_id(GraphId{root.value("graph_id", static_cast<uint64_t>(0))});
        graph.set_name(root.value("name", std::string{"Untitled"}));
        graph.set_mode(static_cast<GraphMode>(root.value("mode", 0)));

        // Build a mapping of old IDs to track what we reconstruct
        // Since we're rebuilding, we add nodes and sockets in order and track mappings
        std::unordered_map<uint64_t, NodeId> node_id_map;
        std::unordered_map<uint64_t, SocketId> socket_id_map;

        // First pass: add all nodes
        if (root.contains("nodes"))
        {
            for (const auto& node_json : root["nodes"])
            {
                auto old_id = node_json.value("id", static_cast<uint64_t>(0));
                auto type_name = node_json.value("type_name", std::string{});
                float pos_x = 0;
                float pos_y = 0;
                if (node_json.contains("position"))
                {
                    pos_x = node_json["position"].value("x", 0.0F);
                    pos_y = node_json["position"].value("y", 0.0F);
                }

                auto new_nid = graph.add_node(type_name, Vec2{pos_x, pos_y});
                node_id_map[old_id] = new_nid;

                // Set additional properties
                auto* node = graph.find_node_mut(new_nid);
                if (node != nullptr)
                {
                    node->label = node_json.value("label", type_name);
                    if (node_json.contains("dimensions"))
                    {
                        node->dimensions.x = node_json["dimensions"].value("w", 180.0F);
                        node->dimensions.y = node_json["dimensions"].value("h", 0.0F);
                    }
                    node->flags =
                        static_cast<NodeFlags>(node_json.value("flags", static_cast<uint16_t>(0)));
                }
            }
        }

        // Second pass: add sockets
        if (root.contains("sockets"))
        {
            for (const auto& sock_json : root["sockets"])
            {
                auto old_sid = sock_json.value("id", static_cast<uint64_t>(0));
                auto old_owner = sock_json.value("owner_node", static_cast<uint64_t>(0));

                auto owner_it = node_id_map.find(old_owner);
                if (owner_it == node_id_map.end())
                {
                    continue; // Owner node not found
                }

                auto direction = static_cast<SocketDirection>(sock_json.value("direction", 0));
                auto data_type = static_cast<SocketDataType>(sock_json.value("data_type", 0));
                auto label = sock_json.value("label", std::string{});
                auto default_value = sock_json.value("default_value", std::string{});

                auto new_sid =
                    graph.add_socket(owner_it->second, direction, data_type, label, default_value);
                socket_id_map[old_sid] = new_sid;
            }
        }

        // Third pass: add links
        if (root.contains("links"))
        {
            for (const auto& link_json : root["links"])
            {
                auto old_source = link_json.value("source", static_cast<uint64_t>(0));
                auto old_target = link_json.value("target", static_cast<uint64_t>(0));

                auto src_it = socket_id_map.find(old_source);
                auto tgt_it = socket_id_map.find(old_target);
                if (src_it == socket_id_map.end() || tgt_it == socket_id_map.end())
                {
                    continue;
                }

                graph.add_link(src_it->second, tgt_it->second);
            }
        }
    }
    catch (const nlohmann::json::exception& /*ex*/)
    {
        // Parse failure — return empty graph
        graph.clear();
    }

    return graph;
}

auto GraphSerializer::schema_version(const std::string& json_str) -> int
{
    try
    {
        auto root = nlohmann::json::parse(json_str);
        return root.value("schema_version", 0);
    }
    catch (const nlohmann::json::exception& /*ex*/)
    {
        return 0;
    }
}

} // namespace markamp::node_editor
