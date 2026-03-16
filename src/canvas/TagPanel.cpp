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

auto TagPanel::remove_tag(const std::string& tag_name) -> bool
{
    auto iter = std::find_if(tags_.begin(),
                             tags_.end(),
                             [&tag_name](const TagInfo& tag_info)
                             { return tag_info.name == tag_name; });
    if (iter == tags_.end())
    {
        return false;
    }
    tags_.erase(iter);
    // Clear filter if the removed tag was the active filter.
    if (active_filter_ == tag_name)
    {
        active_filter_.clear();
    }
    return true;
}

auto TagPanel::render_tags() const -> std::vector<TagChipLayout>
{
    std::vector<TagChipLayout> chips;
    chips.reserve(tags_.size());

    double cursor_x = 0.0;
    double cursor_y = 0.0;
    constexpr double kChipHeight = 28.0;
    constexpr double kChipPadding = 8.0;
    constexpr double kChipGap = 6.0;
    constexpr double kPanelWidth = 280.0;

    for (const auto& tag : tags_)
    {
        // Estimate chip width based on tag name length.
        const double chip_width = static_cast<double>(tag.name.size()) * 8.0 + kChipPadding * 2.0 + 20.0;

        // Wrap to next row if needed.
        if (cursor_x + chip_width > kPanelWidth && cursor_x > 0.0)
        {
            cursor_x = 0.0;
            cursor_y += kChipHeight + kChipGap;
        }

        TagChipLayout chip;
        chip.tag = tag;
        chip.rect = {cursor_x, cursor_y, cursor_x + chip_width, cursor_y + kChipHeight};
        chip.is_active = (tag.name == active_filter_);
        chips.push_back(std::move(chip));

        cursor_x += chip_width + kChipGap;
    }

    return chips;
}

} // namespace markamp::canvas
