#pragma once

#include "core/Block.h"
#include "core/BlockDatabase.h"

#include <nlohmann/json_fwd.hpp>

#include <expected>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace markamp::core
{

class Config;
class EventBus;

// Represents a node in the .sy document tree (JSON format).
struct SyNode
{
    std::string id;
    std::string type;               // Block type abbreviation
    std::string sub_type;           // Block sub-type abbreviation
    std::string content;            // Raw content
    std::string markdown;           // Markdown content
    std::string fcontent;           // Full content
    InlineAttributeList properties; // IAL key-value pairs

    std::vector<SyNode> children;
};

// Handles .sy document file I/O, CRUD operations, and
// file system management for the block-based document format.
class DocumentFileSystem
{
public:
    DocumentFileSystem(std::shared_ptr<Config> config,
                       std::shared_ptr<EventBus> event_bus,
                       std::shared_ptr<BlockDatabase> database);
    ~DocumentFileSystem() = default;

    DocumentFileSystem(const DocumentFileSystem&) = delete;
    auto operator=(const DocumentFileSystem&) -> DocumentFileSystem& = delete;
    DocumentFileSystem(DocumentFileSystem&&) = delete;
    auto operator=(DocumentFileSystem&&) -> DocumentFileSystem& = delete;

    // --- Tree I/O ---
    [[nodiscard]] auto load_tree(const std::string& box_id, const std::filesystem::path& doc_path)
        -> std::expected<Block, std::string>;

    auto save_tree(const Block& root_block) -> std::expected<void, std::string>;

    // --- Document CRUD ---
    [[nodiscard]] auto create_doc(const std::string& box_id,
                                  const std::filesystem::path& dir_path,
                                  const std::string& title,
                                  const std::string& initial_content = "")
        -> std::expected<Block, std::string>;

    auto rename_doc(const std::string& box_id,
                    const std::filesystem::path& doc_path,
                    const std::string& new_title) -> std::expected<void, std::string>;

    auto remove_doc(const std::string& box_id, const std::filesystem::path& doc_path)
        -> std::expected<void, std::string>;

    auto move_doc(const std::string& box_id,
                  const std::filesystem::path& from_path,
                  const std::filesystem::path& to_path) -> std::expected<void, std::string>;

    [[nodiscard]] auto duplicate_doc(const std::string& box_id,
                                     const std::filesystem::path& doc_path)
        -> std::expected<Block, std::string>;

    // --- Directory Operations ---
    auto create_dir(const std::string& box_id,
                    const std::filesystem::path& dir_path,
                    const std::string& dir_name)
        -> std::expected<std::filesystem::path, std::string>;

    auto remove_dir(const std::string& box_id, const std::filesystem::path& dir_path)
        -> std::expected<void, std::string>;

    // --- Path Resolution ---
    [[nodiscard]] auto resolve_path(const std::string& box_id,
                                    const std::filesystem::path& relative_path) const
        -> std::filesystem::path;

    [[nodiscard]] auto data_dir() const -> std::filesystem::path;

    // --- Utility ---
    [[nodiscard]] static auto atomic_write(const std::filesystem::path& target_path,
                                           const std::string& content)
        -> std::expected<void, std::string>;

    // --- Serialization ---
    [[nodiscard]] auto serialize_tree(const Block& root) const -> nlohmann::json;
    [[nodiscard]] auto deserialize_tree(const nlohmann::json& json_data,
                                        const std::string& box_id,
                                        const std::filesystem::path& doc_path) const -> Block;

    // --- Node Conversion ---
    [[nodiscard]] static auto block_to_sy_node(const Block& block) -> SyNode;
    [[nodiscard]] static auto sy_node_to_block(const SyNode& node,
                                               const std::string& root_id,
                                               const std::string& parent_id,
                                               const std::string& box_id,
                                               const std::string& path_str) -> Block;

    // --- Type Helpers ---
    [[nodiscard]] static auto block_type_to_node_type(BlockType type) -> std::string;
    [[nodiscard]] static auto node_type_to_block_type(const std::string& node_type) -> BlockType;

private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<EventBus> event_bus_;
    std::shared_ptr<BlockDatabase> database_;
};

} // namespace markamp::core
