/// @file CustomFontRegistry.h
/// @brief V9 Phase 38 — Per-vault custom font management.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// FontEntry — a registered custom font
// ============================================================================

enum class FontWeight : uint16_t
{
    kThin = 100,
    kExtraLight = 200,
    kLight = 300,
    kNormal = 400,
    kMedium = 500,
    kSemiBold = 600,
    kBold = 700,
    kExtraBold = 800,
    kBlack = 900
};

enum class FontStyle : uint8_t
{
    kNormal,
    kItalic,
    kOblique
};

enum class FontFormat : uint8_t
{
    kTTF,
    kOTF,
    kWOFF,
    kWOFF2,
    kUnknown
};

struct FontEntry
{
    std::string name;      ///< Display name
    std::string file_path; ///< Path to font file
    std::string family;    ///< CSS font-family name
    FontStyle style{FontStyle::kNormal};
    FontWeight weight{FontWeight::kNormal};
    FontFormat format{FontFormat::kUnknown};
    bool is_builtin{false};

    [[nodiscard]] auto weight_value() const -> int
    {
        return static_cast<int>(weight);
    }
    [[nodiscard]] auto style_string() const -> std::string;
    [[nodiscard]] auto format_string() const -> std::string;
};

// ============================================================================
// CustomFontRegistry
// ============================================================================

/// Manages custom fonts per vault and generates @font-face CSS declarations.
///
/// Usage:
/// ```cpp
/// CustomFontRegistry registry;
/// FontEntry font;
/// font.name = "JetBrains Mono";
/// font.family = "JetBrains Mono";
/// font.file_path = "/path/to/font.ttf";
/// font.format = FontFormat::kTTF;
/// registry.register_font(std::move(font));
/// auto css = registry.generate_font_face_css();
/// ```
class CustomFontRegistry
{
public:
    CustomFontRegistry() = default;

    // ── CRUD ──

    /// Register a custom font.
    void register_font(FontEntry font);

    /// Unregister a font by name. Returns true if found.
    auto unregister_font(const std::string& name) -> bool;

    /// Find a font by name.
    [[nodiscard]] auto find_font(const std::string& name) const -> const FontEntry*;

    /// List all registered fonts.
    [[nodiscard]] auto list_fonts() const -> const std::vector<FontEntry>&;

    /// Get fonts filtered by family.
    [[nodiscard]] auto fonts_by_family(const std::string& family) const
        -> std::vector<const FontEntry*>;

    /// Total font count.
    [[nodiscard]] auto font_count() const -> std::size_t;

    /// Get all unique family names.
    [[nodiscard]] auto families() const -> std::vector<std::string>;

    // ── CSS Generation ──

    /// Generate @font-face CSS for all registered fonts.
    [[nodiscard]] auto generate_font_face_css() const -> std::string;

    /// Generate @font-face for a single font.
    [[nodiscard]] static auto generate_single_font_face(const FontEntry& font) -> std::string;

    // ── Validation ──

    /// Validate a font entry (check format support, path non-empty).
    [[nodiscard]] static auto validate_font(const FontEntry& font) -> std::pair<bool, std::string>;

    /// Detect font format from file extension.
    [[nodiscard]] static auto detect_format(const std::string& file_path) -> FontFormat;

    /// Format enum to CSS format string.
    [[nodiscard]] static auto format_to_css(FontFormat fmt) -> std::string;

private:
    std::vector<FontEntry> fonts_;
};

} // namespace markamp::core
