#include "DesignTokenValue.h"

namespace markamp::ui
{

std::optional<int> DesignTokenValue::as_int() const
{
    if (std::holds_alternative<int>(value_))
    {
        return std::get<int>(value_);
    }
    return std::nullopt;
}

std::optional<float> DesignTokenValue::as_float() const
{
    if (std::holds_alternative<float>(value_))
    {
        return std::get<float>(value_);
    }
    return std::nullopt;
}

std::optional<wxColour> DesignTokenValue::as_colour() const
{
    if (std::holds_alternative<wxColour>(value_))
    {
        return std::get<wxColour>(value_);
    }
    return std::nullopt;
}

std::optional<wxFont> DesignTokenValue::as_font() const
{
    if (std::holds_alternative<wxFont>(value_))
    {
        return std::get<wxFont>(value_);
    }
    return std::nullopt;
}

std::optional<ShadowSpec> DesignTokenValue::as_shadow() const
{
    if (std::holds_alternative<ShadowSpec>(value_))
    {
        return std::get<ShadowSpec>(value_);
    }
    return std::nullopt;
}

DesignTokenKind DesignTokenValue::kind() const
{
    if (std::holds_alternative<int>(value_))
        return DesignTokenKind::kInt;
    if (std::holds_alternative<float>(value_))
        return DesignTokenKind::kFloat;
    if (std::holds_alternative<wxColour>(value_))
        return DesignTokenKind::kColour;
    if (std::holds_alternative<wxFont>(value_))
        return DesignTokenKind::kFont;
    if (std::holds_alternative<ShadowSpec>(value_))
        return DesignTokenKind::kShadow;
    return DesignTokenKind::kInt; // Fallback
}

} // namespace markamp::ui
