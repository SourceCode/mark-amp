#pragma once

#include "ElevationSystem.h"

#include <wx/colour.h>
#include <wx/font.h>

#include <optional>
#include <variant>

namespace markamp::ui
{

enum class DesignTokenKind
{
    kInt,
    kFloat,
    kColour,
    kFont,
    kShadow
};

class DesignTokenValue
{
public:
    DesignTokenValue() = default;
    explicit DesignTokenValue(int v)
        : value_(v)
    {
    }
    explicit DesignTokenValue(float v)
        : value_(v)
    {
    }
    explicit DesignTokenValue(wxColour v)
        : value_(std::move(v))
    {
    }
    explicit DesignTokenValue(wxFont v)
        : value_(std::move(v))
    {
    }
    explicit DesignTokenValue(ShadowSpec v)
        : value_(std::move(v))
    {
    }

    std::optional<int> as_int() const;
    std::optional<float> as_float() const;
    std::optional<wxColour> as_colour() const;
    std::optional<wxFont> as_font() const;
    std::optional<ShadowSpec> as_shadow() const;

    DesignTokenKind kind() const;

private:
    std::variant<std::monostate, int, float, wxColour, wxFont, ShadowSpec> value_;
};

} // namespace markamp::ui
