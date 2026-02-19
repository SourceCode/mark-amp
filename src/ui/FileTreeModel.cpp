#include "FileTreeModel.h"

#include <algorithm>
#include <cctype>

namespace markamp::ui
{

void FileTreeModel::set_items(std::vector<TreeItemModel> items)
{
    items_ = std::move(items);
    focus_index_ = items_.empty() ? -1 : 0;
    anchor_index_ = focus_index_;
}

auto FileTreeModel::items() const -> const std::vector<TreeItemModel>&
{
    return items_;
}

auto FileTreeModel::item_count() const -> int
{
    return static_cast<int>(items_.size());
}

void FileTreeModel::select(int index)
{
    if (index < 0 || index >= item_count())
    {
        return;
    }
    clear_selection();
    items_[static_cast<std::size_t>(index)].is_selected = true;
    focus_index_ = index;
    anchor_index_ = index;
}

void FileTreeModel::toggle_select(int index)
{
    if (index < 0 || index >= item_count())
    {
        return;
    }
    auto& item = items_[static_cast<std::size_t>(index)];
    item.is_selected = !item.is_selected;
    focus_index_ = index;
}

void FileTreeModel::range_select(int index)
{
    if (index < 0 || index >= item_count())
    {
        return;
    }
    if (anchor_index_ < 0)
    {
        anchor_index_ = 0;
    }

    const int start = std::min(anchor_index_, index);
    const int end = std::max(anchor_index_, index);

    clear_selection();
    for (int idx = start; idx <= end; ++idx)
    {
        items_[static_cast<std::size_t>(idx)].is_selected = true;
    }
    focus_index_ = index;
}

auto FileTreeModel::selected_indices() const -> std::vector<int>
{
    std::vector<int> result;
    for (int idx = 0; idx < item_count(); ++idx)
    {
        if (items_[static_cast<std::size_t>(idx)].is_selected)
        {
            result.push_back(idx);
        }
    }
    return result;
}

auto FileTreeModel::selection_count() const -> int
{
    return static_cast<int>(selected_indices().size());
}

void FileTreeModel::clear_selection()
{
    for (auto& item : items_)
    {
        item.is_selected = false;
    }
}

auto FileTreeModel::focus_index() const -> int
{
    return focus_index_;
}

void FileTreeModel::focus_next()
{
    if (items_.empty())
    {
        return;
    }
    focus_index_ = std::min(focus_index_ + 1, item_count() - 1);
}

void FileTreeModel::focus_previous()
{
    if (items_.empty())
    {
        return;
    }
    focus_index_ = std::max(focus_index_ - 1, 0);
}

void FileTreeModel::expand_focused()
{
    if (focus_index_ < 0 || focus_index_ >= item_count())
    {
        return;
    }
    auto& item = items_[static_cast<std::size_t>(focus_index_)];
    if (item.is_directory)
    {
        item.is_expanded = true;
    }
}

void FileTreeModel::collapse_focused()
{
    if (focus_index_ < 0 || focus_index_ >= item_count())
    {
        return;
    }
    auto& item = items_[static_cast<std::size_t>(focus_index_)];
    if (item.is_directory && item.is_expanded)
    {
        item.is_expanded = false;
    }
}

auto FileTreeModel::validate_name(const std::string& proposed_name,
                                  const std::vector<std::string>& sibling_names) -> RenameValidation
{
    if (proposed_name.empty())
    {
        return RenameValidation::kEmpty;
    }

    // Check for invalid characters
    static constexpr const char* kInvalidChars = R"(/\:*?"<>|)";
    for (char character : proposed_name)
    {
        for (const char* invalid = kInvalidChars; *invalid != '\0'; ++invalid)
        {
            if (character == *invalid)
            {
                return RenameValidation::kInvalidChars;
            }
        }
    }

    // Check for conflicts
    for (const auto& sibling : sibling_names)
    {
        if (sibling == proposed_name)
        {
            return RenameValidation::kConflict;
        }
    }

    return RenameValidation::kValid;
}

auto FileTreeModel::filter(const std::string& query) const -> std::vector<int>
{
    std::vector<int> result;
    if (query.empty())
    {
        return result;
    }

    // Case-insensitive substring match
    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char character)
                   { return static_cast<char>(std::tolower(character)); });

    for (int idx = 0; idx < item_count(); ++idx)
    {
        std::string lower_name = items_[static_cast<std::size_t>(idx)].name;
        std::transform(lower_name.begin(),
                       lower_name.end(),
                       lower_name.begin(),
                       [](unsigned char character)
                       { return static_cast<char>(std::tolower(character)); });

        if (lower_name.find(lower_query) != std::string::npos)
        {
            result.push_back(idx);
        }
    }
    return result;
}

auto FileTreeModel::selected_ids() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& item : items_)
    {
        if (item.is_selected)
        {
            result.push_back(item.node_id);
        }
    }
    return result;
}

} // namespace markamp::ui
