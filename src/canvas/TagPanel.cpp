#include "TagPanel.h"

namespace markamp::canvas
{

auto TagPanel::set_tags(const std::vector<TagInfo>& tags) -> void
{
    tags_ = tags;
}

auto TagPanel::clear() -> void
{
    tags_.clear();
    active_filter_.clear();
}

auto TagPanel::tag_count() const -> size_t
{
    return tags_.size();
}

auto TagPanel::tag_at(size_t index) const -> const TagInfo&
{
    return tags_.at(index);
}

auto TagPanel::active_filter() const -> const std::string&
{
    return active_filter_;
}

auto TagPanel::set_active_filter(const std::string& tag) -> void
{
    active_filter_ = tag;
    if (on_filter_by_tag_)
    {
        on_filter_by_tag_(tag);
    }
}

auto TagPanel::clear_filter() -> void
{
    active_filter_.clear();
}

auto TagPanel::set_on_filter_by_tag(OnFilterByTag callback) -> void
{
    on_filter_by_tag_ = std::move(callback);
}

auto TagPanel::is_visible() const -> bool
{
    return visible_;
}

auto TagPanel::set_visible(bool visible) -> void
{
    visible_ = visible;
}

} // namespace markamp::canvas
