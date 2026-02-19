#include "StickyNoteModel.h"

namespace markamp::canvas
{

void StickyNoteModel::set_text(const std::string& text)
{
    text_ = text;
}
auto StickyNoteModel::text() const -> const std::string&
{
    return text_;
}

void StickyNoteModel::set_editing(bool editing)
{
    editing_ = editing;
}
auto StickyNoteModel::is_editing() const -> bool
{
    return editing_;
}

void StickyNoteModel::set_color_preset(StickyColorPreset preset)
{
    color_preset_ = preset;
}
auto StickyNoteModel::color_preset() const -> StickyColorPreset
{
    return color_preset_;
}

auto StickyNoteModel::color_for(StickyColorPreset preset) -> std::string
{
    switch (preset)
    {
        case StickyColorPreset::kYellow:
            return "#FFF9C4";
        case StickyColorPreset::kPink:
            return "#F8BBD0";
        case StickyColorPreset::kGreen:
            return "#C8E6C9";
        case StickyColorPreset::kBlue:
            return "#BBDEFB";
        case StickyColorPreset::kOrange:
            return "#FFE0B2";
        case StickyColorPreset::kPurple:
            return "#E1BEE7";
    }
    return "#FFF9C4";
}

void StickyNoteModel::set_size_preset(StickySizePreset preset)
{
    size_preset_ = preset;
}
auto StickyNoteModel::size_preset() const -> StickySizePreset
{
    return size_preset_;
}

auto StickyNoteModel::dimensions_for(StickySizePreset preset) -> std::pair<double, double>
{
    switch (preset)
    {
        case StickySizePreset::kSmall:
            return {100.0, 100.0};
        case StickySizePreset::kMedium:
            return {150.0, 150.0};
        case StickySizePreset::kLarge:
            return {200.0, 200.0};
    }
    return {150.0, 150.0};
}

void StickyNoteModel::set_max_chars(int max_chars)
{
    max_chars_ = (max_chars < 1) ? 1 : max_chars;
}

auto StickyNoteModel::is_overflowing() const -> bool
{
    return static_cast<int>(text_.size()) > max_chars_;
}

void StickyNoteModel::set_suggestions(std::vector<ClusterSuggestion> suggestions)
{
    suggestions_ = std::move(suggestions);
}

auto StickyNoteModel::suggestions() const -> const std::vector<ClusterSuggestion>&
{
    return suggestions_;
}

} // namespace markamp::canvas
