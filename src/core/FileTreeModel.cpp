/// @file FileTreeModel.cpp
/// @brief Phase 20 – File tree model implementation.

#include "core/FileTreeModel.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <filesystem>

namespace markamp::core
{

namespace fs = std::filesystem;

// ============================================================================
// Construction
// ============================================================================

FileTreeModel::FileTreeModel(EventBus& event_bus)
    : event_bus_(event_bus)
{
    root_.type = FileNodeType::Folder;
}

// ============================================================================
// Loading
// ============================================================================

auto FileTreeModel::load_directory(const std::string& path) -> bool
{
    std::error_code error_code;
    if (!fs::is_directory(path, error_code))
    {
        return false;
    }

    root_path_ = path;
    root_ = FileNode{};
    root_.id = path;
    root_.name = fs::path(path).filename().string();
    root_.type = FileNodeType::Folder;
    root_.is_open = true;

    build_tree(root_, path);
    apply_sort(root_);
    expanded_.insert(path);

    notify_refresh();
    return true;
}

auto FileTreeModel::refresh() -> bool
{
    if (root_path_.empty())
    {
        return false;
    }

    // Preserve expanded state.
    auto saved_expanded = expanded_;

    root_ = FileNode{};
    root_.id = root_path_;
    root_.name = fs::path(root_path_).filename().string();
    root_.type = FileNodeType::Folder;

    build_tree(root_, root_path_);
    apply_sort(root_);

    expanded_ = std::move(saved_expanded);

    // Publish event.
    events::FileTreeRefreshedEvent evt;
    evt.root_path = root_path_;
    evt.node_count = static_cast<int>(node_count());
    event_bus_.publish(evt);

    notify_refresh();
    return true;
}

auto FileTreeModel::root_path() const -> const std::string&
{
    return root_path_;
}

// ============================================================================
// Tree Access
// ============================================================================

auto FileTreeModel::root() -> FileNode&
{
    return root_;
}

auto FileTreeModel::root() const -> const FileNode&
{
    return root_;
}

auto FileTreeModel::find_node(const std::string& path) -> FileNode*
{
    return root_.find_by_id(path);
}

auto FileTreeModel::find_node(const std::string& path) const -> const FileNode*
{
    return root_.find_by_id(path);
}

// ============================================================================
// Expand/Collapse
// ============================================================================

void FileTreeModel::expand_node(const std::string& path)
{
    expanded_.insert(path);
    auto* node = root_.find_by_id(path);
    if (node != nullptr)
    {
        node->is_open = true;
    }
}

void FileTreeModel::collapse_node(const std::string& path)
{
    expanded_.erase(path);
    auto* node = root_.find_by_id(path);
    if (node != nullptr)
    {
        node->is_open = false;
    }
}

void FileTreeModel::toggle_node(const std::string& path)
{
    if (is_expanded(path))
    {
        collapse_node(path);
    }
    else
    {
        expand_node(path);
    }
}

auto FileTreeModel::is_expanded(const std::string& path) const -> bool
{
    return expanded_.contains(path);
}

auto FileTreeModel::expanded_paths() const -> std::vector<std::string>
{
    return {expanded_.begin(), expanded_.end()};
}

void FileTreeModel::expand_all()
{
    std::function<void(const FileNode&)> walk = [&](const FileNode& node)
    {
        if (node.is_folder())
        {
            expanded_.insert(node.id);
            for (const auto& child : node.children)
            {
                walk(child);
            }
        }
    };
    walk(root_);
}

void FileTreeModel::collapse_all()
{
    expanded_.clear();
    expanded_.insert(root_path_); // Keep root expanded.
}

// ============================================================================
// Sorting
// ============================================================================

void FileTreeModel::set_sort_order(FileTreeSortOrder order)
{
    sort_order_ = order;
    apply_sort(root_);
}

auto FileTreeModel::sort_order() const -> FileTreeSortOrder
{
    return sort_order_;
}

// ============================================================================
// Filtering
// ============================================================================

void FileTreeModel::set_filter(const FileTreeFilter& new_filter)
{
    filter_ = new_filter;

    // Re-apply filter visibility on all nodes.
    std::function<void(FileNode&)> apply = [&](FileNode& node)
    {
        node.filter_visible = passes_filter(node);
        for (auto& child : node.children)
        {
            apply(child);
        }
        // A folder is visible if any child is visible.
        if (node.is_folder())
        {
            bool any_visible =
                std::any_of(node.children.begin(),
                            node.children.end(),
                            [](const FileNode& child_node) { return child_node.filter_visible; });
            if (any_visible)
            {
                node.filter_visible = true;
            }
        }
    };
    apply(root_);
}

auto FileTreeModel::filter() const -> const FileTreeFilter&
{
    return filter_;
}

// ============================================================================
// Flattening
// ============================================================================

auto FileTreeModel::flatten() const -> std::vector<const FileNode*>
{
    std::vector<const FileNode*> result;
    flatten_recursive(root_, result);
    return result;
}

// ============================================================================
// Statistics
// ============================================================================

auto FileTreeModel::node_count() const -> std::size_t
{
    return count_nodes(root_);
}

auto FileTreeModel::visible_node_count() const -> std::size_t
{
    return flatten().size();
}

// ============================================================================
// Callbacks
// ============================================================================

auto FileTreeModel::on_refresh(RefreshCallback callback) -> std::size_t
{
    auto callback_id = next_callback_id_++;
    refresh_callbacks_.emplace_back(callback_id, std::move(callback));
    return callback_id;
}

void FileTreeModel::remove_refresh_callback(std::size_t callback_id)
{
    refresh_callbacks_.erase(std::remove_if(refresh_callbacks_.begin(),
                                            refresh_callbacks_.end(),
                                            [callback_id](const auto& pair)
                                            { return pair.first == callback_id; }),
                             refresh_callbacks_.end());
}

void FileTreeModel::clear()
{
    root_ = FileNode{};
    root_.type = FileNodeType::Folder;
    root_path_.clear();
    expanded_.clear();
}

// ============================================================================
// Private Helpers
// ============================================================================

void FileTreeModel::build_tree(FileNode& node, const std::string& dir_path)
{
    std::error_code error_code;

    for (const auto& entry : fs::directory_iterator(dir_path, error_code))
    {
        FileNode child;
        child.id = entry.path().string();
        child.name = entry.path().filename().string();

        if (entry.is_directory(error_code))
        {
            child.type = FileNodeType::Folder;
            build_tree(child, child.id);
        }
        else
        {
            child.type = FileNodeType::File;
        }

        child.filter_visible = passes_filter(child);
        node.children.push_back(std::move(child));
    }
}

void FileTreeModel::apply_sort(FileNode& node)
{
    auto comparator = [this](const FileNode& node_a, const FileNode& node_b) -> bool
    {
        // Folders always come first in type-grouped mode.
        if (sort_order_ == FileTreeSortOrder::kTypeGrouped)
        {
            if (node_a.is_folder() != node_b.is_folder())
            {
                return node_a.is_folder();
            }
        }

        switch (sort_order_)
        {
            case FileTreeSortOrder::kNameAsc:
            case FileTreeSortOrder::kTypeGrouped:
                return node_a.name < node_b.name;
            case FileTreeSortOrder::kNameDesc:
                return node_a.name > node_b.name;
            default:
                return node_a.name < node_b.name;
        }
    };

    std::sort(node.children.begin(), node.children.end(), comparator);

    for (auto& child : node.children)
    {
        if (child.is_folder())
        {
            apply_sort(child);
        }
    }
}

auto FileTreeModel::passes_filter(const FileNode& node) const -> bool
{
    // Hidden files check.
    if (!filter_.show_hidden && !node.name.empty() && node.name[0] == '.')
    {
        return false;
    }

    // Dotfiles check (more specific: .git, .markamp, etc.).
    if (!filter_.show_dotfiles && node.is_folder() && !node.name.empty() && node.name[0] == '.')
    {
        return false;
    }

    // Extension filter (files only).
    if (node.is_file() && !filter_.include_extensions.empty())
    {
        auto ext = node.extension();
        if (!ext.empty() && ext[0] == '.')
        {
            ext = ext.substr(1);
        }
        bool matched = std::any_of(filter_.include_extensions.begin(),
                                   filter_.include_extensions.end(),
                                   [&ext](const std::string& incl) { return ext == incl; });
        if (!matched)
        {
            return false;
        }
    }

    // Exclude patterns (simple name matching).
    for (const auto& pattern : filter_.exclude_patterns)
    {
        if (node.name == pattern)
        {
            return false;
        }
    }

    return true;
}

auto FileTreeModel::count_nodes(const FileNode& node) -> std::size_t
{
    std::size_t count = 1;
    for (const auto& child : node.children)
    {
        count += count_nodes(child);
    }
    return count;
}

void FileTreeModel::flatten_recursive(const FileNode& node,
                                      std::vector<const FileNode*>& result) const
{
    if (!node.filter_visible)
    {
        return;
    }

    result.push_back(&node);

    if (node.is_folder() && is_expanded(node.id))
    {
        for (const auto& child : node.children)
        {
            flatten_recursive(child, result);
        }
    }
}

void FileTreeModel::notify_refresh()
{
    for (const auto& [callback_id, callback] : refresh_callbacks_)
    {
        callback();
    }
}

} // namespace markamp::core
