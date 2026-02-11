#include "ThemeValidator.h"

#include "Theme.h"

#include <nlohmann/json.hpp>

namespace markamp::core
{

auto ThemeValidator::validate_json(const nlohmann::json& json) const -> ValidationResult
{
    ValidationResult result;
    result.is_valid = true;

    check_required_fields(json, result);

    if (json.contains("colors") && json["colors"].is_object())
    {
        check_color_values(json["colors"], result);
    }

    // Input validation: string length limits
    if (json.contains("name") && json["name"].is_string())
    {
        auto name = json["name"].get<std::string>();
        if (name.size() > kMaxNameLength)
        {
            result.errors.emplace_back("Theme name exceeds maximum length of 100 characters");
            result.is_valid = false;
        }
        if (contains_null_bytes(name) || contains_control_chars(name))
        {
            result.errors.emplace_back("Theme name contains invalid characters (null bytes or "
                                       "control characters)");
            result.is_valid = false;
        }
    }

    if (json.contains("id") && json["id"].is_string())
    {
        auto theme_id = json["id"].get<std::string>();
        if (theme_id.size() > kMaxIdLength)
        {
            result.errors.emplace_back("Theme ID exceeds maximum length of 64 characters");
            result.is_valid = false;
        }
        if (contains_null_bytes(theme_id) || contains_control_chars(theme_id))
        {
            result.errors.emplace_back(
                "Theme ID contains invalid characters (null bytes or control characters)");
            result.is_valid = false;
        }
    }

    return result;
}

auto ThemeValidator::validate_theme(const Theme& theme) const -> ValidationResult
{
    ValidationResult result;
    result.is_valid = true;

    if (theme.id.empty())
    {
        result.errors.emplace_back("Theme ID is empty");
        result.is_valid = false;
    }
    if (theme.name.empty())
    {
        result.errors.emplace_back("Invalid theme format: Missing 'name' field.");
        result.is_valid = false;
    }

    // String length limits
    if (theme.name.size() > kMaxNameLength)
    {
        result.errors.emplace_back("Theme name exceeds maximum length of 100 characters");
        result.is_valid = false;
    }
    if (theme.id.size() > kMaxIdLength)
    {
        result.errors.emplace_back("Theme ID exceeds maximum length of 64 characters");
        result.is_valid = false;
    }

    // Null byte / control char checks
    if (contains_null_bytes(theme.name) || contains_control_chars(theme.name))
    {
        result.errors.emplace_back(
            "Theme name contains invalid characters (null bytes or control characters)");
        result.is_valid = false;
    }
    if (contains_null_bytes(theme.id) || contains_control_chars(theme.id))
    {
        result.errors.emplace_back(
            "Theme ID contains invalid characters (null bytes or control characters)");
        result.is_valid = false;
    }

    check_contrast_ratios(theme, result);

    return result;
}

void ThemeValidator::check_required_fields(const nlohmann::json& json,
                                           ValidationResult& result) const
{
    // Check name
    if (!json.contains("name") || !json["name"].is_string() ||
        json["name"].get<std::string>().empty())
    {
        result.errors.emplace_back("Invalid theme format: Missing 'name' field.");
        result.is_valid = false;
    }

    // Check colors object
    if (!json.contains("colors"))
    {
        result.errors.emplace_back("Invalid theme format: Missing 'colors' object.");
        result.is_valid = false;
        return; // Can't check color keys without colors object
    }

    if (!json["colors"].is_object())
    {
        result.errors.emplace_back("Invalid theme format: 'colors' is not an object.");
        result.is_valid = false;
        return;
    }

    // Check all 10 required color keys
    const auto& colors = json["colors"];
    for (const char* key : kRequiredColorKeys)
    {
        if (!colors.contains(key))
        {
            result.errors.emplace_back(std::string("Invalid theme format: Missing color '") + key +
                                       "'.");
            result.is_valid = false;
        }
    }
}

void ThemeValidator::check_color_values(const nlohmann::json& colors,
                                        ValidationResult& result) const
{
    for (const char* key : kRequiredColorKeys)
    {
        if (!colors.contains(key))
        {
            continue; // Already reported in check_required_fields
        }

        const auto& val = colors[key];
        if (!val.is_string())
        {
            result.errors.emplace_back(std::string("Invalid theme format: '") + key +
                                       "' has invalid color value.");
            result.is_valid = false;
            continue;
        }

        auto color_str = val.get<std::string>();
        auto parsed = Color::from_string(color_str);
        if (!parsed)
        {
            result.errors.emplace_back(std::string("Invalid theme format: '") + key +
                                       "' has invalid color value.");
            result.is_valid = false;
        }
    }
}

void ThemeValidator::check_contrast_ratios(const Theme& theme, ValidationResult& result) const
{
    // WCAG AA: text on background should have >= 4.5:1 contrast ratio
    float main_contrast = theme.colors.text_main.contrast_ratio(theme.colors.bg_app);
    if (main_contrast < kMinContrastRatio)
    {
        result.warnings.emplace_back("Low contrast: text_main vs bg_app ratio is " +
                                     std::to_string(main_contrast).substr(0, 4) +
                                     ":1 (WCAG AA requires >= 4.5:1)");
    }

    float muted_contrast = theme.colors.text_muted.contrast_ratio(theme.colors.bg_panel);
    if (muted_contrast < kMinContrastRatio)
    {
        result.warnings.emplace_back("Low contrast: text_muted vs bg_panel ratio is " +
                                     std::to_string(muted_contrast).substr(0, 4) +
                                     ":1 (WCAG AA requires >= 4.5:1)");
    }

    // Sanity check: bg_app and bg_panel shouldn't be identical
    if (theme.colors.bg_app == theme.colors.bg_panel &&
        theme.colors.bg_app == theme.colors.bg_header)
    {
        result.warnings.emplace_back("bg_app, bg_panel, and bg_header are identical — UI elements "
                                     "won't be visually distinct");
    }
}

auto ThemeValidator::contains_null_bytes(const std::string& str) -> bool
{
    return str.find('\0') != std::string::npos;
}

auto ThemeValidator::contains_control_chars(const std::string& str) -> bool
{
    for (const auto chr : str)
    {
        auto uch = static_cast<unsigned char>(chr);
        // Allow tab (0x09), newline (0x0A), carriage return (0x0D)
        if (uch < 0x20 && uch != 0x09 && uch != 0x0A && uch != 0x0D)
        {
            return true;
        }
        // DEL character
        if (uch == 0x7F)
        {
            return true;
        }
    }
    return false;
}

} // namespace markamp::core
