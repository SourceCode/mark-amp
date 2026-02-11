#include "Theme.h"

#include <nlohmann/json.hpp>

namespace markamp::core
{

// --- Theme validation ---

auto Theme::is_valid() const -> bool
{
    return validation_errors().empty();
}

auto Theme::validation_errors() const -> std::vector<std::string>
{
    std::vector<std::string> errors;

    if (id.empty())
    {
        errors.emplace_back("Theme ID is empty");
    }
    if (name.empty())
    {
        errors.emplace_back("Theme name is empty");
    }

    // Check that key colors are not fully transparent (likely a parsing error)
    if (colors.bg_app.a == 0)
    {
        errors.emplace_back("bg_app is fully transparent");
    }
    if (colors.text_main.a == 0)
    {
        errors.emplace_back("text_main is fully transparent");
    }
    if (colors.accent_primary.a == 0)
    {
        errors.emplace_back("accent_primary is fully transparent");
    }

    return errors;
}

// --- Derived colors ---

auto Theme::scrollbar_track() const -> Color
{
    return colors.bg_panel;
}
auto Theme::scrollbar_thumb() const -> Color
{
    return colors.accent_secondary;
}
auto Theme::scrollbar_hover() const -> Color
{
    return colors.accent_primary;
}

auto Theme::selection_bg() const -> Color
{
    return colors.accent_primary.with_alpha(0.2f);
}

auto Theme::hover_bg() const -> Color
{
    return colors.accent_primary.with_alpha(0.1f);
}

auto Theme::error_color() const -> Color
{
    // A red that contrasts well with the background
    if (is_dark())
    {
        return Color::from_rgb(255, 85, 85); // bright red on dark
    }
    return Color::from_rgb(200, 0, 0); // dark red on light
}

auto Theme::success_color() const -> Color
{
    if (is_dark())
    {
        return Color::from_rgb(85, 255, 85); // bright green on dark
    }
    return Color::from_rgb(0, 150, 0); // dark green on light
}

auto Theme::is_dark() const -> bool
{
    return colors.bg_app.luminance() < 0.2f;
}

// --- JSON serialization ---

void to_json(nlohmann::json& j, const Color& c)
{
    j = c.to_theme_string();
}

void from_json(const nlohmann::json& j, Color& c)
{
    auto str = j.get<std::string>();
    auto result = Color::from_string(str);
    if (result)
    {
        c = *result;
    }
    else
    {
        throw nlohmann::json::other_error::create(601, "Invalid color: " + result.error(), &j);
    }
}

void to_json(nlohmann::json& j, const ThemeColors& tc)
{
    j = nlohmann::json{
        {"--bg-app", tc.bg_app},
        {"--bg-panel", tc.bg_panel},
        {"--bg-header", tc.bg_header},
        {"--bg-input", tc.bg_input},
        {"--text-main", tc.text_main},
        {"--text-muted", tc.text_muted},
        {"--accent-primary", tc.accent_primary},
        {"--accent-secondary", tc.accent_secondary},
        {"--border-light", tc.border_light},
        {"--border-dark", tc.border_dark},
    };
}

void from_json(const nlohmann::json& j, ThemeColors& tc)
{
    j.at("--bg-app").get_to(tc.bg_app);
    j.at("--bg-panel").get_to(tc.bg_panel);
    j.at("--bg-header").get_to(tc.bg_header);
    j.at("--bg-input").get_to(tc.bg_input);
    j.at("--text-main").get_to(tc.text_main);
    j.at("--text-muted").get_to(tc.text_muted);
    j.at("--accent-primary").get_to(tc.accent_primary);
    j.at("--accent-secondary").get_to(tc.accent_secondary);
    j.at("--border-light").get_to(tc.border_light);
    j.at("--border-dark").get_to(tc.border_dark);
}

void to_json(nlohmann::json& j, const Theme& t)
{
    j = nlohmann::json{
        {"id", t.id},
        {"name", t.name},
        {"colors", t.colors},
    };
}

void from_json(const nlohmann::json& j, Theme& t)
{
    j.at("id").get_to(t.id);
    j.at("name").get_to(t.name);
    j.at("colors").get_to(t.colors);
}

} // namespace markamp::core
