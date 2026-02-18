/// @file PlatformFonts.h
/// @brief V9 Phase 32 – Platform-specific font defaults and enumeration.

#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// Font style.
enum class FontStyle : uint8_t
{
    kRegular,
    kBold,
    kItalic,
    kBoldItalic
};

/// Describes a font.
struct FontInfo
{
    std::string family;
    FontStyle style{FontStyle::kRegular};
    int size{12};
    bool is_monospace{false};
};

/// Cross-platform font service for defaults, fallbacks, and enumeration.
class PlatformFonts
{
public:
    /// Get the default code/monospace font for the current platform.
    /// macOS: SF Mono, Windows: Cascadia Mono, Linux: DejaVu Sans Mono.
    [[nodiscard]] static auto get_default_code_font() -> FontInfo;

    /// Get the default UI/sans-serif font for the current platform.
    /// macOS: SF Pro, Windows: Segoe UI, Linux: system sans-serif.
    [[nodiscard]] static auto get_default_ui_font() -> FontInfo;

    /// Get the fallback chain for a given font family.
    [[nodiscard]] static auto get_fallback_chain(const std::string& family)
        -> std::vector<std::string>;

    /// Get the appropriate CJK font for the current platform.
    [[nodiscard]] static auto get_cjk_font() -> FontInfo;

    /// Enumerate system fonts (returns common known fonts per platform).
    [[nodiscard]] static auto enumerate_system_fonts() -> std::vector<FontInfo>;

    /// Check if a font family is likely available (checks against known fonts).
    [[nodiscard]] static auto is_font_available(const std::string& family) -> bool;
};

} // namespace markamp::core
