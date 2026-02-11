#pragma once

#include "Color.h"

#include <nlohmann/json_fwd.hpp>

#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// The 10 color tokens from the reference implementation.
struct ThemeColors
{
    Color bg_app;           // --bg-app
    Color bg_panel;         // --bg-panel
    Color bg_header;        // --bg-header
    Color bg_input;         // --bg-input
    Color text_main;        // --text-main
    Color text_muted;       // --text-muted
    Color accent_primary;   // --accent-primary
    Color accent_secondary; // --accent-secondary
    Color border_light;     // --border-light
    Color border_dark;      // --border-dark

    auto operator<=>(const ThemeColors&) const = default;
    auto operator==(const ThemeColors&) const -> bool = default;
};

/// Full theme data with ID, name, and color tokens.
struct Theme
{
    std::string id;
    std::string name;
    ThemeColors colors;

    // Validation
    [[nodiscard]] auto is_valid() const -> bool;
    [[nodiscard]] auto validation_errors() const -> std::vector<std::string>;

    // Derived colors (computed from the base 10)
    [[nodiscard]] auto scrollbar_track() const -> Color;
    [[nodiscard]] auto scrollbar_thumb() const -> Color;
    [[nodiscard]] auto scrollbar_hover() const -> Color;
    [[nodiscard]] auto selection_bg() const -> Color;
    [[nodiscard]] auto hover_bg() const -> Color;
    [[nodiscard]] auto error_color() const -> Color;
    [[nodiscard]] auto success_color() const -> Color;
    [[nodiscard]] auto is_dark() const -> bool;

    auto operator==(const Theme&) const -> bool = default;
};

/// Lightweight theme metadata for listing.
struct ThemeInfo
{
    std::string id;
    std::string name;
    bool is_builtin{false};
    std::optional<std::string> file_path;
};

// JSON serialization (ADL for nlohmann/json)
void to_json(nlohmann::json& j, const Color& c);
void from_json(const nlohmann::json& j, Color& c);
void to_json(nlohmann::json& j, const ThemeColors& tc);
void from_json(const nlohmann::json& j, ThemeColors& tc);
void to_json(nlohmann::json& j, const Theme& t);
void from_json(const nlohmann::json& j, Theme& t);

} // namespace markamp::core
