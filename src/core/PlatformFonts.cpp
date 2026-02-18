/// @file PlatformFonts.cpp
/// @brief V9 Phase 32 – Platform-specific font implementation.

#include "PlatformFonts.h"

#include <algorithm>

namespace markamp::core
{

auto PlatformFonts::get_default_code_font() -> FontInfo
{
    FontInfo font;
    font.is_monospace = true;
    font.style = FontStyle::kRegular;
    font.size = 13;

#if defined(__APPLE__)
    font.family = "SF Mono";
#elif defined(_WIN32) || defined(_WIN64)
    font.family = "Cascadia Mono";
#else
    font.family = "DejaVu Sans Mono";
#endif

    return font;
}

auto PlatformFonts::get_default_ui_font() -> FontInfo
{
    FontInfo font;
    font.is_monospace = false;
    font.style = FontStyle::kRegular;
    font.size = 13;

#if defined(__APPLE__)
    font.family = "SF Pro";
#elif defined(_WIN32) || defined(_WIN64)
    font.family = "Segoe UI";
#else
    font.family = "Cantarell";
#endif

    return font;
}

auto PlatformFonts::get_fallback_chain(const std::string& family) -> std::vector<std::string>
{
    // Monospace fallback chain
    if (family == "SF Mono" || family == "Cascadia Mono" || family == "DejaVu Sans Mono" ||
        family == "Consolas" || family == "Menlo" || family == "Monaco")
    {
        return {
            "SF Mono",
            "Cascadia Mono",
            "Consolas",
            "DejaVu Sans Mono",
            "Menlo",
            "Monaco",
            "Courier New",
            "monospace",
        };
    }

    // UI/sans-serif fallback chain
    return {
        "SF Pro",
        "Segoe UI",
        "Cantarell",
        "Noto Sans",
        "Helvetica Neue",
        "Arial",
        "sans-serif",
    };
}

auto PlatformFonts::get_cjk_font() -> FontInfo
{
    FontInfo font;
    font.is_monospace = false;
    font.style = FontStyle::kRegular;
    font.size = 13;

#if defined(__APPLE__)
    font.family = "Hiragino Sans";
#elif defined(_WIN32) || defined(_WIN64)
    font.family = "Yu Gothic UI";
#else
    font.family = "Noto Sans CJK";
#endif

    return font;
}

auto PlatformFonts::enumerate_system_fonts() -> std::vector<FontInfo>
{
    // Return the well-known fonts per platform.
    // In production, this would query the OS font database.
    std::vector<FontInfo> fonts;

    // Monospace fonts
    for (const auto& fam : {"SF Mono",
                            "Cascadia Mono",
                            "Consolas",
                            "DejaVu Sans Mono",
                            "Menlo",
                            "Monaco",
                            "Courier New",
                            "Fira Code",
                            "JetBrains Mono",
                            "Source Code Pro"})
    {
        FontInfo info;
        info.family = fam;
        info.is_monospace = true;
        info.size = 13;
        fonts.push_back(info);
    }

    // Sans-serif fonts
    for (const auto& fam : {"SF Pro",
                            "Segoe UI",
                            "Cantarell",
                            "Noto Sans",
                            "Helvetica Neue",
                            "Arial",
                            "Inter",
                            "Roboto",
                            "Open Sans"})
    {
        FontInfo info;
        info.family = fam;
        info.is_monospace = false;
        info.size = 13;
        fonts.push_back(info);
    }

    // CJK fonts
    for (const auto& fam : {"Hiragino Sans", "Yu Gothic UI", "Noto Sans CJK"})
    {
        FontInfo info;
        info.family = fam;
        info.is_monospace = false;
        info.size = 13;
        fonts.push_back(info);
    }

    return fonts;
}

auto PlatformFonts::is_font_available(const std::string& family) -> bool
{
    const auto fonts = enumerate_system_fonts();
    return std::any_of(
        fonts.begin(), fonts.end(), [&](const FontInfo& font) { return font.family == family; });
}

} // namespace markamp::core
