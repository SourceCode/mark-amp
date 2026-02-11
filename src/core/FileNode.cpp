#include "FileNode.h"

namespace markamp::core
{

auto FileNode::is_folder() const -> bool
{
    return type == FileNodeType::Folder;
}

auto FileNode::is_file() const -> bool
{
    return type == FileNodeType::File;
}

auto FileNode::find_by_id(const std::string& target_id) -> FileNode*
{
    if (id == target_id)
    {
        return this;
    }
    for (auto& child : children)
    {
        if (auto* found = child.find_by_id(target_id))
        {
            return found;
        }
    }
    return nullptr;
}

auto FileNode::find_by_id(const std::string& target_id) const -> const FileNode*
{
    if (id == target_id)
    {
        return this;
    }
    for (const auto& child : children)
    {
        if (const auto* found = child.find_by_id(target_id))
        {
            return found;
        }
    }
    return nullptr;
}

auto FileNode::file_count() const -> std::size_t
{
    if (is_file())
    {
        return 1;
    }
    std::size_t count = 0;
    for (const auto& child : children)
    {
        count += child.file_count();
    }
    return count;
}

auto FileNode::folder_count() const -> std::size_t
{
    if (is_file())
    {
        return 0;
    }
    std::size_t count = 1; // Count self
    for (const auto& child : children)
    {
        count += child.folder_count();
    }
    return count;
}

auto FileNode::extension() const -> std::string
{
    if (is_folder())
    {
        return "";
    }
    auto dot_pos = name.rfind('.');
    if (dot_pos == std::string::npos || dot_pos == 0)
    {
        return "";
    }
    return name.substr(dot_pos);
}

} // namespace markamp::core
