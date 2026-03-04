#pragma once

/// @file OutlineIconProvider.h
/// @brief Phase 33 – Icon provider for outline heading levels and symbol types.

#include "../core/ISymbolProvider.h"

#include <cstdint>
#include <string>

namespace markamp::ui
{

/// Heading level icon types for the outline panel.
enum class OutlineIconType : std::uint8_t
{
    kH1 = 0,
    kH2,
    kH3,
    kH4,
    kH5,
    kH6,
    kFunction,
    kClass,
    kVariable,
    kNamespace,
    kInterface,
    kEnum,
    kConstant,
    kProperty,
    kUnknown
};

/// Returns a human-readable icon character for an outline heading level.
/// Uses numeric indicators in stylized form: "H1" through "H6".
[[nodiscard]] inline auto outline_icon_char(OutlineIconType icon_type) -> std::string
{
    switch (icon_type)
    {
        case OutlineIconType::kH1:
            return "H1";
        case OutlineIconType::kH2:
            return "H2";
        case OutlineIconType::kH3:
            return "H3";
        case OutlineIconType::kH4:
            return "H4";
        case OutlineIconType::kH5:
            return "H5";
        case OutlineIconType::kH6:
            return "H6";
        case OutlineIconType::kFunction:
            return "f()";
        case OutlineIconType::kClass:
            return "C";
        case OutlineIconType::kVariable:
            return "x";
        case OutlineIconType::kNamespace:
            return "N";
        case OutlineIconType::kInterface:
            return "I";
        case OutlineIconType::kEnum:
            return "E";
        case OutlineIconType::kConstant:
            return "K";
        case OutlineIconType::kProperty:
            return "P";
        case OutlineIconType::kUnknown:
            return "?";
    }
    return "?";
}

/// Map a heading level (1-6) to an OutlineIconType.
[[nodiscard]] inline auto icon_for_heading_level(int level) -> OutlineIconType
{
    switch (level)
    {
        case 1:
            return OutlineIconType::kH1;
        case 2:
            return OutlineIconType::kH2;
        case 3:
            return OutlineIconType::kH3;
        case 4:
            return OutlineIconType::kH4;
        case 5:
            return OutlineIconType::kH5;
        case 6:
            return OutlineIconType::kH6;
        default:
            return OutlineIconType::kUnknown;
    }
}

/// Map a SymbolKind to an OutlineIconType.
[[nodiscard]] inline auto icon_for_symbol_kind(core::SymbolKind kind) -> OutlineIconType
{
    switch (kind)
    {
        case core::SymbolKind::kFunction:
            return OutlineIconType::kFunction;
        case core::SymbolKind::kClass:
            return OutlineIconType::kClass;
        case core::SymbolKind::kVariable:
            return OutlineIconType::kVariable;
        case core::SymbolKind::kNamespace:
            return OutlineIconType::kNamespace;
        case core::SymbolKind::kInterface:
            return OutlineIconType::kInterface;
        case core::SymbolKind::kEnum:
            return OutlineIconType::kEnum;
        case core::SymbolKind::kConstant:
            return OutlineIconType::kConstant;
        case core::SymbolKind::kProperty:
            return OutlineIconType::kProperty;
        case core::SymbolKind::kHeading:
            return OutlineIconType::kH1; // default to H1
        default:
            return OutlineIconType::kUnknown;
    }
    return OutlineIconType::kUnknown;
}

/// Returns a label suitable for display (icon char + text).
[[nodiscard]] inline auto outline_label(OutlineIconType icon_type, const std::string& text)
    -> std::string
{
    return outline_icon_char(icon_type) + " " + text;
}

} // namespace markamp::ui
