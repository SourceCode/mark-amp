#pragma once

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

enum class FileNodeType
{
    File,
    Folder
};

struct FileNode
{
    std::string id;
    std::string name;
    FileNodeType type{FileNodeType::File};
    std::optional<std::string> content; // Only for files
    std::vector<FileNode> children;     // Only for folders
    bool is_open{false};                // Folder toggle state

    // Helpers
    [[nodiscard]] auto is_folder() const -> bool;
    [[nodiscard]] auto is_file() const -> bool;
    [[nodiscard]] auto find_by_id(const std::string& target_id) -> FileNode*;
    [[nodiscard]] auto find_by_id(const std::string& target_id) const -> const FileNode*;
    [[nodiscard]] auto file_count() const -> std::size_t;
    [[nodiscard]] auto folder_count() const -> std::size_t;
    [[nodiscard]] auto extension() const -> std::string;
};

} // namespace markamp::core
