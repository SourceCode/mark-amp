/// @file CustomFontRegistry.cpp
/// @brief V9 Phase 38 — CustomFontRegistry implementation.

#include "CustomFontRegistry.h"

#include <algorithm>
#include <set>
#include <sstream>

namespace markamp::core
{

// ── FontEntry helpers ─────────────────────────────────────────────────────────

auto FontEntry::style_string() const -> std::string
{
    switch (style)
    {
        case FontStyle::kNormal:
            return "normal";
        case FontStyle::kItalic:
            return "italic";
        case FontStyle::kOblique:
            return "oblique";
    }
    return "normal";
}

auto FontEntry::format_string() const -> std::string
{
    return CustomFontRegistry::format_to_css(format);
}

// ── CRUD ──────────────────────────────────────────────────────────────────────

void CustomFontRegistry::register_font(FontEntry font)
{
    for (auto& existing : fonts_)
    {
        if (existing.name == font.name)
        {
            existing = std::move(font);
            return;
        }
    }
    fonts_.push_back(std::move(font));
}

auto CustomFontRegistry::unregister_font(const std::string& name) -> bool
{
    auto iter = std::find_if(
        fonts_.begin(), fonts_.end(), [&name](const FontEntry& font) { return font.name == name; });
    if (iter == fonts_.end())
    {
        return false;
    }
    fonts_.erase(iter);
    return true;
}

auto CustomFontRegistry::find_font(const std::string& name) const -> const FontEntry*
{
    for (const auto& font : fonts_)
    {
        if (font.name == name)
        {
            return &font;
        }
    }
    return nullptr;
}

auto CustomFontRegistry::list_fonts() const -> const std::vector<FontEntry>&
{
    return fonts_;
}

auto CustomFontRegistry::fonts_by_family(const std::string& family) const
    -> std::vector<const FontEntry*>
{
    std::vector<const FontEntry*> result;
    for (const auto& font : fonts_)
    {
        if (font.family == family)
        {
            result.push_back(&font);
        }
    }
    return result;
}

auto CustomFontRegistry::font_count() const -> std::size_t
{
    return fonts_.size();
}

auto CustomFontRegistry::families() const -> std::vector<std::string>
{
    std::set<std::string> fams;
    for (const auto& font : fonts_)
    {
        if (!font.family.empty())
        {
            fams.insert(font.family);
        }
    }
    return {fams.begin(), fams.end()};
}

// ── CSS Generation ────────────────────────────────────────────────────────────

auto CustomFontRegistry::generate_font_face_css() const -> std::string
{
    std::ostringstream oss;
    for (const auto& font : fonts_)
    {
        oss << generate_single_font_face(font) << "\n";
    }
    return oss.str();
}

auto CustomFontRegistry::generate_single_font_face(const FontEntry& font) -> std::string
{
    std::ostringstream oss;
    oss << "@font-face {\n";
    oss << "  font-family: \"" << font.family << "\";\n";
    oss << "  src: url(\"" << font.file_path << "\")";

    const std::string fmt = format_to_css(font.format);
    if (!fmt.empty())
    {
        oss << " format(\"" << fmt << "\")";
    }
    oss << ";\n";
    oss << "  font-weight: " << font.weight_value() << ";\n";
    oss << "  font-style: " << font.style_string() << ";\n";
    oss << "}\n";
    return oss.str();
}

// ── Validation ────────────────────────────────────────────────────────────────

auto CustomFontRegistry::validate_font(const FontEntry& font) -> std::pair<bool, std::string>
{
    if (font.name.empty())
    {
        return {false, "Font name is required"};
    }
    if (font.family.empty())
    {
        return {false, "Font family is required"};
    }
    if (font.file_path.empty())
    {
        return {false, "Font file path is required"};
    }
    if (font.format == FontFormat::kUnknown)
    {
        return {false, "Font format could not be detected"};
    }
    return {true, ""};
}

auto CustomFontRegistry::detect_format(const std::string& file_path) -> FontFormat
{
    auto dot_pos = file_path.rfind('.');
    if (dot_pos == std::string::npos)
    {
        return FontFormat::kUnknown;
    }

    std::string ext = file_path.substr(dot_pos + 1);
    // Convert to lowercase
    std::transform(ext.begin(),
                   ext.end(),
                   ext.begin(),
                   [](unsigned char character)
                   { return static_cast<char>(std::tolower(character)); });

    if (ext == "ttf")
    {
        return FontFormat::kTTF;
    }
    if (ext == "otf")
    {
        return FontFormat::kOTF;
    }
    if (ext == "woff")
    {
        return FontFormat::kWOFF;
    }
    if (ext == "woff2")
    {
        return FontFormat::kWOFF2;
    }
    return FontFormat::kUnknown;
}

auto CustomFontRegistry::format_to_css(FontFormat fmt) -> std::string
{
    switch (fmt)
    {
        case FontFormat::kTTF:
            return "truetype";
        case FontFormat::kOTF:
            return "opentype";
        case FontFormat::kWOFF:
            return "woff";
        case FontFormat::kWOFF2:
            return "woff2";
        case FontFormat::kUnknown:
            return "";
    }
    return "";
}

} // namespace markamp::core
