#include "ScrollPositionStore.h"

namespace markamp::ui
{

void ScrollPositionStore::save(const std::string& file_path, const ScrollPosition& position)
{
    positions_[file_path] = position;
    evict_if_needed();
}

auto ScrollPositionStore::restore(const std::string& file_path) const -> ScrollPosition
{
    auto it = positions_.find(file_path);
    return it != positions_.end() ? it->second : ScrollPosition{};
}

auto ScrollPositionStore::has(const std::string& file_path) const -> bool
{
    return positions_.contains(file_path);
}

void ScrollPositionStore::remove(const std::string& file_path)
{
    positions_.erase(file_path);
}

void ScrollPositionStore::clear()
{
    positions_.clear();
}

auto ScrollPositionStore::count() const -> int
{
    return static_cast<int>(positions_.size());
}

void ScrollPositionStore::set_max_entries(int max)
{
    max_entries_ = max > 0 ? max : 1;
}
auto ScrollPositionStore::max_entries() const -> int
{
    return max_entries_;
}

void ScrollPositionStore::evict_if_needed()
{
    if (static_cast<int>(positions_.size()) > max_entries_)
    {
        // Simple eviction: remove first element (approximates LRU for unordered_map)
        positions_.erase(positions_.begin());
    }
}

} // namespace markamp::ui
