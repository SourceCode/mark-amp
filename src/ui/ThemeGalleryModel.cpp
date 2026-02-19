#include "ThemeGalleryModel.h"

namespace markamp::ui
{

void ThemeGalleryModel::set_themes(std::vector<ThemeEntry> themes)
{
    themes_ = std::move(themes);
}

auto ThemeGalleryModel::all_themes() const -> const std::vector<ThemeEntry>&
{
    return themes_;
}

auto ThemeGalleryModel::by_style(ThemeStyle style) const -> std::vector<ThemeEntry>
{
    std::vector<ThemeEntry> result;
    for (const auto& theme : themes_)
    {
        if (theme.style == style)
        {
            result.push_back(theme);
        }
    }
    return result;
}

void ThemeGalleryModel::set_preview(const std::string& theme_id)
{
    preview_id_ = theme_id;
}
void ThemeGalleryModel::clear_preview()
{
    preview_id_.clear();
}
auto ThemeGalleryModel::preview_id() const -> const std::string&
{
    return preview_id_;
}
auto ThemeGalleryModel::is_previewing() const -> bool
{
    return !preview_id_.empty();
}

void ThemeGalleryModel::apply(const std::string& theme_id)
{
    previous_id_ = active_id_;
    active_id_ = theme_id;
    preview_id_.clear();
}

void ThemeGalleryModel::rollback()
{
    if (!previous_id_.empty())
    {
        active_id_ = previous_id_;
        previous_id_.clear();
    }
}

auto ThemeGalleryModel::active_id() const -> const std::string&
{
    return active_id_;
}
auto ThemeGalleryModel::previous_id() const -> const std::string&
{
    return previous_id_;
}
auto ThemeGalleryModel::can_rollback() const -> bool
{
    return !previous_id_.empty();
}

void ThemeGalleryModel::set_tokens(std::vector<ThemeToken> tokens)
{
    tokens_ = std::move(tokens);
}

auto ThemeGalleryModel::tokens() const -> const std::vector<ThemeToken>&
{
    return tokens_;
}

auto ThemeGalleryModel::tokens_by_group(const std::string& group) const -> std::vector<ThemeToken>
{
    std::vector<ThemeToken> result;
    for (const auto& token : tokens_)
    {
        if (token.group == group)
        {
            result.push_back(token);
        }
    }
    return result;
}

auto ThemeGalleryModel::contrast_warnings() const -> std::vector<ThemeToken>
{
    std::vector<ThemeToken> result;
    for (const auto& token : tokens_)
    {
        if (token.has_contrast_warning)
        {
            result.push_back(token);
        }
    }
    return result;
}

} // namespace markamp::ui
