#include "core/DocumentFileSystem.h"

#include "core/BlockID.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <sstream>

namespace markamp::core
{

// -- Construction --

DocumentFileSystem::DocumentFileSystem(std::shared_ptr<Config> config,
                                       std::shared_ptr<EventBus> event_bus,
                                       std::shared_ptr<BlockDatabase> database)
    : config_(std::move(config))
    , event_bus_(std::move(event_bus))
    , database_(std::move(database))
{
}

// -- Path Resolution --

auto DocumentFileSystem::data_dir() const -> std::filesystem::path
{
    if (config_)
    {
        auto dir = config_->get_string("storage.data_dir");
        if (!dir.empty())
        {
            return dir;
        }
    }
    return std::filesystem::current_path() / "data";
}

auto DocumentFileSystem::resolve_path(const std::string& box_id,
                                      const std::filesystem::path& relative_path) const
    -> std::filesystem::path
{
    return data_dir() / box_id / relative_path;
}

// -- Atomic File Write --

auto DocumentFileSystem::atomic_write(const std::filesystem::path& target_path,
                                      const std::string& content)
    -> std::expected<void, std::string>
{
    auto tmp_path = target_path;
    tmp_path += ".tmp";

    // Ensure parent directory exists
    std::error_code err_code;
    std::filesystem::create_directories(target_path.parent_path(), err_code);
    if (err_code)
    {
        return std::unexpected("Failed to create directories: " + err_code.message());
    }

    // Write to temp file
    {
        std::ofstream ofs(tmp_path, std::ios::binary | std::ios::trunc);
        if (!ofs.is_open())
        {
            return std::unexpected("Failed to open temp file for writing: " + tmp_path.string());
        }
        ofs << content;
        ofs.flush();
        if (ofs.fail())
        {
            return std::unexpected("Failed to write to temp file: " + tmp_path.string());
        }
    }

    // Atomic rename
    std::filesystem::rename(tmp_path, target_path, err_code);
    if (err_code)
    {
        // Clean up temp file on failure
        std::filesystem::remove(tmp_path);
        return std::unexpected("Failed to rename temp file: " + err_code.message());
    }

    return {};
}

// -- JSON Serialization --

auto DocumentFileSystem::serialize_tree(const Block& root) const -> nlohmann::json
{
    nlohmann::json doc;
    doc["ID"] = root.id;
    doc["Type"] = root.type_abbr();
    doc["SubType"] = root.sub_type_abbr();
    doc["Properties"] = nlohmann::json::object();

    for (const auto& [key, value] : root.ial)
    {
        doc["Properties"][key] = value;
    }

    if (!root.content.empty())
    {
        doc["Content"] = root.content;
    }
    if (!root.markdown.empty())
    {
        doc["Markdown"] = root.markdown;
    }

    doc["Children"] = nlohmann::json::array();
    for (const auto& child_ptr : root.children)
    {
        if (child_ptr)
        {
            doc["Children"].push_back(serialize_tree(*child_ptr));
        }
    }

    return doc;
}

auto DocumentFileSystem::deserialize_tree(const nlohmann::json& json_data,
                                          const std::string& box_id,
                                          const std::filesystem::path& doc_path) const -> Block
{
    Block block;
    block.id = json_data.value("ID", "");
    block.type = BlockDatabase::abbr_to_block_type(json_data.value("Type", ""));
    block.sub_type = BlockDatabase::abbr_to_block_sub_type(json_data.value("SubType", ""));
    block.box = box_id;
    block.path = doc_path.string();

    if (json_data.contains("Content"))
    {
        block.content = json_data["Content"].get<std::string>();
    }
    if (json_data.contains("Markdown"))
    {
        block.markdown = json_data["Markdown"].get<std::string>();
    }

    if (json_data.contains("Properties"))
    {
        for (const auto& [key, value] : json_data["Properties"].items())
        {
            block.ial[key] = value.get<std::string>();
        }
    }

    // Set root_id: for document blocks it's their own ID
    if (block.type == BlockType::Document)
    {
        block.root_id = block.id;
    }

    if (json_data.contains("Children"))
    {
        for (const auto& child_json : json_data["Children"])
        {
            auto child = std::make_shared<Block>(deserialize_tree(child_json, box_id, doc_path));
            child->parent_id = block.id;
            if (child->root_id.empty())
            {
                child->root_id = block.root_id.empty() ? block.id : block.root_id;
            }
            block.children.push_back(std::move(child));
        }
    }

    return block;
}

// -- Node Conversion --

auto DocumentFileSystem::block_to_sy_node(const Block& block) -> SyNode
{
    SyNode node;
    node.id = block.id;
    node.type = block.type_abbr();
    node.sub_type = block.sub_type_abbr();
    node.content = block.content;
    node.markdown = block.markdown;
    node.fcontent = block.fcontent;
    node.properties = block.ial;

    for (const auto& child_ptr : block.children)
    {
        if (child_ptr)
        {
            node.children.push_back(block_to_sy_node(*child_ptr));
        }
    }

    return node;
}

auto DocumentFileSystem::sy_node_to_block(const SyNode& node,
                                          const std::string& root_id,
                                          const std::string& parent_id,
                                          const std::string& box_id,
                                          const std::string& path_str) -> Block
{
    Block block;
    block.id = node.id;
    block.root_id = root_id;
    block.parent_id = parent_id;
    block.box = box_id;
    block.path = path_str;
    block.type = BlockDatabase::abbr_to_block_type(node.type);
    block.sub_type = BlockDatabase::abbr_to_block_sub_type(node.sub_type);
    block.content = node.content;
    block.markdown = node.markdown;
    block.fcontent = node.fcontent;
    block.ial = node.properties;

    for (const auto& child_node : node.children)
    {
        auto child = std::make_shared<Block>(
            sy_node_to_block(child_node, root_id, block.id, box_id, path_str));
        block.children.push_back(std::move(child));
    }

    return block;
}

// -- Type Helpers --

auto DocumentFileSystem::block_type_to_node_type(BlockType type) -> std::string
{
    Block tmp;
    tmp.type = type;
    return tmp.type_abbr();
}

auto DocumentFileSystem::node_type_to_block_type(const std::string& node_type) -> BlockType
{
    return BlockDatabase::abbr_to_block_type(node_type);
}

// -- Tree I/O --

auto DocumentFileSystem::load_tree(const std::string& box_id, const std::filesystem::path& doc_path)
    -> std::expected<Block, std::string>
{
    auto full_path = resolve_path(box_id, doc_path);
    if (!full_path.has_extension() || full_path.extension() != ".sy")
    {
        full_path.replace_extension(".sy");
    }

    if (!std::filesystem::exists(full_path))
    {
        return std::unexpected("Document file not found: " + full_path.string());
    }

    // Read file content
    std::ifstream ifs(full_path, std::ios::binary);
    if (!ifs.is_open())
    {
        return std::unexpected("Failed to open document file: " + full_path.string());
    }

    std::string file_content((std::istreambuf_iterator<char>(ifs)),
                             std::istreambuf_iterator<char>());

    // Parse JSON
    nlohmann::json json_data;
    try
    {
        json_data = nlohmann::json::parse(file_content);
    }
    catch (const nlohmann::json::parse_error& parse_err)
    {
        return std::unexpected("Failed to parse document JSON: " + std::string(parse_err.what()));
    }

    auto root = deserialize_tree(json_data, box_id, doc_path);

    if (event_bus_)
    {
        events::DocumentLoadedEvent evt;
        evt.box_id = box_id;
        evt.path = doc_path.string();
        event_bus_->publish(evt);
    }

    return root;
}

auto DocumentFileSystem::save_tree(const Block& root_block) -> std::expected<void, std::string>
{
    auto doc_path = std::filesystem::path(root_block.path);
    auto full_path = resolve_path(root_block.box, doc_path);
    if (!full_path.has_extension() || full_path.extension() != ".sy")
    {
        full_path.replace_extension(".sy");
    }

    auto json_data = serialize_tree(root_block);
    auto content = json_data.dump(2); // Pretty-print with 2-space indent

    auto write_result = atomic_write(full_path, content);
    if (!write_result)
    {
        return std::unexpected(write_result.error());
    }

    if (event_bus_)
    {
        events::DocumentSavedEvent evt;
        evt.box_id = root_block.box;
        evt.path = doc_path.string();
        evt.block_id = root_block.id;
        event_bus_->publish(evt);
    }

    return {};
}

// -- Document CRUD --

auto DocumentFileSystem::create_doc(const std::string& box_id,
                                    const std::filesystem::path& dir_path,
                                    const std::string& title,
                                    const std::string& initial_content)
    -> std::expected<Block, std::string>
{
    Block doc_block;
    doc_block.id = BlockIDGenerator::generate();
    doc_block.root_id = doc_block.id;
    doc_block.box = box_id;
    doc_block.type = BlockType::Document;
    doc_block.name = title;
    doc_block.content = title;
    doc_block.created_str = BlockIDGenerator::current_timestamp_string();
    doc_block.updated_str = doc_block.created_str;
    doc_block.set_ial("id", doc_block.id);
    doc_block.set_ial("title", title);
    doc_block.set_ial("updated", doc_block.updated_str);

    auto doc_file_path = dir_path / (doc_block.id + ".sy");
    doc_block.path = doc_file_path.string();
    doc_block.hpath = "/" + title;

    // Create initial paragraph if content is provided
    if (!initial_content.empty())
    {
        auto para = std::make_shared<Block>();
        para->id = BlockIDGenerator::generate();
        para->root_id = doc_block.id;
        para->parent_id = doc_block.id;
        para->box = box_id;
        para->path = doc_block.path;
        para->type = BlockType::Paragraph;
        para->content = initial_content;
        para->markdown = initial_content;
        para->created_str = doc_block.created_str;
        para->updated_str = doc_block.updated_str;
        doc_block.children.push_back(std::move(para));
    }

    // Save to disk
    auto save_result = save_tree(doc_block);
    if (!save_result)
    {
        return std::unexpected(save_result.error());
    }

    // Persist to database
    if (database_)
    {
        database_->upsert_block(doc_block);
        for (const auto& child : doc_block.children)
        {
            if (child)
            {
                database_->upsert_block(*child);
            }
        }
    }

    if (event_bus_)
    {
        events::DocumentCreatedEvent evt;
        evt.box_id = box_id;
        evt.block_id = doc_block.id;
        evt.title = title;
        event_bus_->publish(evt);
    }

    return doc_block;
}

auto DocumentFileSystem::rename_doc(const std::string& box_id,
                                    const std::filesystem::path& doc_path,
                                    const std::string& new_title)
    -> std::expected<void, std::string>
{
    auto tree = load_tree(box_id, doc_path);
    if (!tree)
    {
        return std::unexpected(tree.error());
    }

    auto& root = tree.value();
    root.name = new_title;
    root.content = new_title;
    root.set_ial("title", new_title);
    root.updated_str = BlockIDGenerator::current_timestamp_string();
    root.set_ial("updated", root.updated_str);

    auto save_result = save_tree(root);
    if (!save_result)
    {
        return std::unexpected(save_result.error());
    }

    if (database_)
    {
        database_->upsert_block(root);
    }

    if (event_bus_)
    {
        events::DocumentRenamedEvent evt;
        evt.box_id = box_id;
        evt.block_id = root.id;
        evt.title = new_title;
        event_bus_->publish(evt);
    }

    return {};
}

auto DocumentFileSystem::remove_doc(const std::string& box_id,
                                    const std::filesystem::path& doc_path)
    -> std::expected<void, std::string>
{
    auto full_path = resolve_path(box_id, doc_path);
    if (!full_path.has_extension() || full_path.extension() != ".sy")
    {
        full_path.replace_extension(".sy");
    }

    // Load tree first to get block ID for cleanup
    std::string block_id;
    auto tree = load_tree(box_id, doc_path);
    if (tree)
    {
        block_id = tree.value().id;
    }

    // Remove file
    std::error_code err_code;
    if (std::filesystem::exists(full_path))
    {
        std::filesystem::remove(full_path, err_code);
        if (err_code)
        {
            return std::unexpected("Failed to remove document file: " + err_code.message());
        }
    }

    // Clean up database
    if (database_ && !block_id.empty())
    {
        database_->delete_blocks_by_root(block_id);
    }

    if (event_bus_)
    {
        events::DocumentRemovedEvent evt;
        evt.box_id = box_id;
        evt.block_id = block_id;
        evt.path = doc_path.string();
        event_bus_->publish(evt);
    }

    return {};
}

auto DocumentFileSystem::move_doc(const std::string& box_id,
                                  const std::filesystem::path& from_path,
                                  const std::filesystem::path& to_path)
    -> std::expected<void, std::string>
{
    auto full_from = resolve_path(box_id, from_path);
    auto full_to = resolve_path(box_id, to_path);

    if (!full_from.has_extension() || full_from.extension() != ".sy")
    {
        full_from.replace_extension(".sy");
    }
    if (!full_to.has_extension() || full_to.extension() != ".sy")
    {
        full_to.replace_extension(".sy");
    }

    if (!std::filesystem::exists(full_from))
    {
        return std::unexpected("Source document not found: " + full_from.string());
    }

    // Ensure target directory exists
    std::error_code err_code;
    std::filesystem::create_directories(full_to.parent_path(), err_code);
    if (err_code)
    {
        return std::unexpected("Failed to create target directory: " + err_code.message());
    }

    std::filesystem::rename(full_from, full_to, err_code);
    if (err_code)
    {
        return std::unexpected("Failed to move document: " + err_code.message());
    }

    if (event_bus_)
    {
        events::DocumentMovedEvent evt;
        evt.box_id = box_id;
        evt.from_path = from_path.string();
        evt.to_path = to_path.string();
        event_bus_->publish(evt);
    }

    return {};
}

auto DocumentFileSystem::duplicate_doc(const std::string& box_id,
                                       const std::filesystem::path& doc_path)
    -> std::expected<Block, std::string>
{
    auto tree = load_tree(box_id, doc_path);
    if (!tree)
    {
        return std::unexpected(tree.error());
    }

    auto& original = tree.value();
    auto dir_path = doc_path.parent_path();
    auto title = original.name + " (copy)";

    return create_doc(box_id, dir_path, title, original.content);
}

// -- Directory Operations --

auto DocumentFileSystem::create_dir(const std::string& box_id,
                                    const std::filesystem::path& dir_path,
                                    const std::string& dir_name)
    -> std::expected<std::filesystem::path, std::string>
{
    auto full_path = resolve_path(box_id, dir_path / dir_name);

    std::error_code err_code;
    std::filesystem::create_directories(full_path, err_code);
    if (err_code)
    {
        return std::unexpected("Failed to create directory: " + err_code.message());
    }

    return full_path;
}

auto DocumentFileSystem::remove_dir(const std::string& box_id,
                                    const std::filesystem::path& dir_path)
    -> std::expected<void, std::string>
{
    auto full_path = resolve_path(box_id, dir_path);

    if (!std::filesystem::exists(full_path))
    {
        return {};
    }

    std::error_code err_code;
    std::filesystem::remove_all(full_path, err_code);
    if (err_code)
    {
        return std::unexpected("Failed to remove directory: " + err_code.message());
    }

    return {};
}

} // namespace markamp::core
