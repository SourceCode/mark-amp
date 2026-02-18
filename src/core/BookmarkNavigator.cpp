/// @file BookmarkNavigator.cpp
/// @brief V9 Phase 37 — BookmarkNavigator implementation.

#include "BookmarkNavigator.h"

#include <algorithm>
#include <unordered_map>

namespace markamp::core
{

void BookmarkNavigator::load(std::vector<BookmarkEntry> entries)
{
    all_entries_ = std::move(entries);
    current_index_ = 0;
    visited_.clear();
    rebuild_filtered();
}

auto BookmarkNavigator::next() -> const BookmarkEntry*
{
    if (filtered_entries_.empty())
    {
        return nullptr;
    }
    current_index_ = (current_index_ + 1) % filtered_entries_.size();
    return &filtered_entries_[current_index_];
}

auto BookmarkNavigator::previous() -> const BookmarkEntry*
{
    if (filtered_entries_.empty())
    {
        return nullptr;
    }
    if (current_index_ == 0)
    {
        current_index_ = filtered_entries_.size() - 1;
    }
    else
    {
        --current_index_;
    }
    return &filtered_entries_[current_index_];
}

auto BookmarkNavigator::go_to(std::size_t index) -> const BookmarkEntry*
{
    if (index >= filtered_entries_.size())
    {
        return nullptr;
    }
    current_index_ = index;
    return &filtered_entries_[current_index_];
}

auto BookmarkNavigator::current() const -> const BookmarkEntry*
{
    if (filtered_entries_.empty())
    {
        return nullptr;
    }
    return &filtered_entries_[current_index_];
}

auto BookmarkNavigator::current_index() const -> std::size_t
{
    return current_index_;
}

auto BookmarkNavigator::count() const -> std::size_t
{
    return filtered_entries_.size();
}

void BookmarkNavigator::filter_by_label(const std::string& label)
{
    filter_label_ = label;
    current_index_ = 0;
    visited_.clear();
    rebuild_filtered();
}

auto BookmarkNavigator::filter_label() const -> std::string
{
    return filter_label_;
}

void BookmarkNavigator::set_sort_order(BookmarkSortOrder order)
{
    sort_order_ = order;
    apply_sort();
    current_index_ = 0;
}

auto BookmarkNavigator::sort_order() const -> BookmarkSortOrder
{
    return sort_order_;
}

void BookmarkNavigator::mark_visited()
{
    if (!filtered_entries_.empty())
    {
        visited_.insert(current_index_);
    }
}

auto BookmarkNavigator::is_visited(std::size_t index) const -> bool
{
    return visited_.contains(index);
}

auto BookmarkNavigator::visited_indices() const -> std::vector<std::size_t>
{
    std::vector<std::size_t> result(visited_.begin(), visited_.end());
    std::sort(result.begin(), result.end());
    return result;
}

auto BookmarkNavigator::visited_count() const -> std::size_t
{
    return visited_.size();
}

void BookmarkNavigator::clear_visited()
{
    visited_.clear();
}

auto BookmarkNavigator::has_next() const -> bool
{
    return !filtered_entries_.empty();
}

auto BookmarkNavigator::has_previous() const -> bool
{
    return !filtered_entries_.empty();
}

auto BookmarkNavigator::summary() const -> BookmarkSummary
{
    BookmarkSummary result;
    result.total_bookmarks = static_cast<int>(all_entries_.size());

    std::unordered_map<std::string, int> label_map;
    std::unordered_map<std::string, int> doc_map;

    for (const auto& entry : all_entries_)
    {
        label_map[entry.label]++;
        doc_map[entry.root_id]++;
    }

    result.total_labels = static_cast<int>(label_map.size());
    result.total_documents = static_cast<int>(doc_map.size());

    result.per_label_counts.reserve(label_map.size());
    for (const auto& [label, cnt] : label_map)
    {
        result.per_label_counts.emplace_back(label, cnt);
    }
    std::sort(result.per_label_counts.begin(), result.per_label_counts.end());

    result.per_document_counts.reserve(doc_map.size());
    for (const auto& [doc, cnt] : doc_map)
    {
        result.per_document_counts.emplace_back(doc, cnt);
    }
    std::sort(result.per_document_counts.begin(), result.per_document_counts.end());

    return result;
}

void BookmarkNavigator::rebuild_filtered()
{
    filtered_entries_.clear();
    for (const auto& entry : all_entries_)
    {
        if (filter_label_.empty() || entry.label == filter_label_)
        {
            filtered_entries_.push_back(entry);
        }
    }
    apply_sort();
}

void BookmarkNavigator::apply_sort()
{
    switch (sort_order_)
    {
        case BookmarkSortOrder::kLabel:
            std::sort(filtered_entries_.begin(),
                      filtered_entries_.end(),
                      [](const BookmarkEntry& lhs, const BookmarkEntry& rhs)
                      { return lhs.label < rhs.label; });
            break;
        case BookmarkSortOrder::kDocument:
            std::sort(filtered_entries_.begin(),
                      filtered_entries_.end(),
                      [](const BookmarkEntry& lhs, const BookmarkEntry& rhs)
                      { return lhs.root_id < rhs.root_id; });
            break;
        case BookmarkSortOrder::kBlockType:
            std::sort(filtered_entries_.begin(),
                      filtered_entries_.end(),
                      [](const BookmarkEntry& lhs, const BookmarkEntry& rhs)
                      { return lhs.block_type < rhs.block_type; });
            break;
        case BookmarkSortOrder::kCreationTime:
        default:
            // Already in insertion order (creation time).
            break;
    }
}

} // namespace markamp::core
