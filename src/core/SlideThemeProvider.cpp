/// @file SlideThemeProvider.cpp
/// @brief V9 Phase 43 — SlideThemeProvider implementation.

#include "SlideThemeProvider.h"

#include <algorithm>

namespace markamp::core
{

void SlideThemeProvider::register_theme(SlideTheme theme)
{
    themes_.push_back(std::move(theme));
}

auto SlideThemeProvider::remove_theme(const std::string& theme_id) -> bool
{
    auto iter = std::remove_if(themes_.begin(),
                               themes_.end(),
                               [&](const SlideTheme& theme) { return theme.theme_id == theme_id; });
    if (iter == themes_.end())
    {
        return false;
    }
    themes_.erase(iter, themes_.end());
    return true;
}

auto SlideThemeProvider::find_theme(const std::string& theme_id) const -> const SlideTheme*
{
    for (const auto& theme : themes_)
    {
        if (theme.theme_id == theme_id)
        {
            return &theme;
        }
    }
    return nullptr;
}

auto SlideThemeProvider::theme_count() const -> int
{
    return static_cast<int>(themes_.size());
}

void SlideThemeProvider::load_defaults()
{
    SlideTheme light;
    light.theme_id = "light-default";
    light.name = "Light Default";
    light.description = "Clean light theme";
    light.primary_color = "#1A73E8";
    light.background_color = "#FFFFFF";
    light.text_color = "#333333";
    light.heading_font = "Inter";
    light.body_font = "Inter";
    light.code_font = "Fira Code";
    light.is_dark = false;
    light.is_builtin = true;
    themes_.push_back(std::move(light));

    SlideTheme dark;
    dark.theme_id = "dark-default";
    dark.name = "Dark Default";
    dark.description = "Modern dark theme";
    dark.primary_color = "#8AB4F8";
    dark.background_color = "#1E1E1E";
    dark.text_color = "#E0E0E0";
    dark.heading_font = "Inter";
    dark.body_font = "Inter";
    dark.code_font = "Fira Code";
    dark.is_dark = true;
    dark.is_builtin = true;
    themes_.push_back(std::move(dark));

    SlideTheme minimal;
    minimal.theme_id = "minimal";
    minimal.name = "Minimal";
    minimal.description = "Minimalist presentation style";
    minimal.primary_color = "#000000";
    minimal.background_color = "#FAFAFA";
    minimal.text_color = "#111111";
    minimal.heading_font = "Georgia";
    minimal.body_font = "Georgia";
    minimal.code_font = "Courier New";
    minimal.is_dark = false;
    minimal.is_builtin = true;
    themes_.push_back(std::move(minimal));
}

auto SlideThemeProvider::dark_themes() const -> std::vector<const SlideTheme*>
{
    std::vector<const SlideTheme*> result;
    for (const auto& theme : themes_)
    {
        if (theme.is_dark)
        {
            result.push_back(&theme);
        }
    }
    return result;
}

auto SlideThemeProvider::light_themes() const -> std::vector<const SlideTheme*>
{
    std::vector<const SlideTheme*> result;
    for (const auto& theme : themes_)
    {
        if (!theme.is_dark)
        {
            result.push_back(&theme);
        }
    }
    return result;
}

auto SlideThemeProvider::builtin_themes() const -> std::vector<const SlideTheme*>
{
    std::vector<const SlideTheme*> result;
    for (const auto& theme : themes_)
    {
        if (theme.is_builtin)
        {
            result.push_back(&theme);
        }
    }
    return result;
}

auto SlideThemeProvider::all_themes() const -> std::vector<const SlideTheme*>
{
    std::vector<const SlideTheme*> result;
    result.reserve(themes_.size());
    for (const auto& theme : themes_)
    {
        result.push_back(&theme);
    }
    return result;
}

void SlideThemeProvider::clear_all()
{
    themes_.clear();
}

} // namespace markamp::core
