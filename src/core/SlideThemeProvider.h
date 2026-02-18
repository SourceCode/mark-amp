/// @file SlideThemeProvider.h
/// @brief V9 Phase 43 — Slide themes with color schemes, fonts, and layouts.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// A slide theme definition.
struct SlideTheme
{
    std::string theme_id;
    std::string name;
    std::string description;
    std::string primary_color;
    std::string secondary_color;
    std::string background_color;
    std::string text_color;
    std::string accent_color;
    std::string heading_font;
    std::string body_font;
    std::string code_font;
    bool is_dark{false};
    bool is_builtin{false};
};

/// Registry for slide presentation themes.
///
/// Provides built-in themes and supports custom user themes.
class SlideThemeProvider
{
public:
    SlideThemeProvider() = default;

    // ── Theme management ──────────────────────────────────────────────
    void register_theme(SlideTheme theme);
    auto remove_theme(const std::string& theme_id) -> bool;
    [[nodiscard]] auto find_theme(const std::string& theme_id) const -> const SlideTheme*;
    [[nodiscard]] auto theme_count() const -> int;

    // ── Defaults ──────────────────────────────────────────────────────
    void load_defaults();
    [[nodiscard]] auto dark_themes() const -> std::vector<const SlideTheme*>;
    [[nodiscard]] auto light_themes() const -> std::vector<const SlideTheme*>;
    [[nodiscard]] auto builtin_themes() const -> std::vector<const SlideTheme*>;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto all_themes() const -> std::vector<const SlideTheme*>;
    void clear_all();

private:
    std::vector<SlideTheme> themes_;
};

} // namespace markamp::core
