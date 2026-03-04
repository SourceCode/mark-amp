/// @file ISymbolProvider.h
/// @brief V13 Phase 31 Task 4 — Symbol provider interface and SymbolInfo types.
///
/// Defines the interface for providing document symbols to the command palette's
/// @ mode. Concrete implementations include MarkdownSymbolProvider.
#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Kind of symbol for icon/rendering selection.
enum class SymbolKind : uint8_t
{
    kFunction = 0,
    kClass = 1,
    kVariable = 2,
    kHeading = 3,
    kNamespace = 4,
    kInterface = 5,
    kProperty = 6,
    kEnum = 7,
    kConstant = 8,
    kMethod = 9
};

/// Information about a single symbol in a document.
struct SymbolInfo
{
    std::string name; ///< Symbol name (e.g., "MyFunction", "## Section Title")
    SymbolKind kind{SymbolKind::kFunction}; ///< Symbol kind for icon/rendering
    int line{0};                            ///< 1-based line number in the document
    std::string container_name; ///< Containing scope (e.g., namespace, class, parent heading)
    int level{0};               ///< Nesting/heading level (1-6 for headings, 0 otherwise)
};

/// Returns a short label string for a SymbolKind.
[[nodiscard]] inline auto symbol_kind_label(SymbolKind kind) -> const char*
{
    switch (kind)
    {
        case SymbolKind::kFunction:
            return "function";
        case SymbolKind::kClass:
            return "class";
        case SymbolKind::kVariable:
            return "variable";
        case SymbolKind::kHeading:
            return "heading";
        case SymbolKind::kNamespace:
            return "namespace";
        case SymbolKind::kInterface:
            return "interface";
        case SymbolKind::kProperty:
            return "property";
        case SymbolKind::kEnum:
            return "enum";
        case SymbolKind::kConstant:
            return "constant";
        case SymbolKind::kMethod:
            return "method";
    }
    return "unknown";
}

/// Returns a single-character icon representation for a SymbolKind.
[[nodiscard]] inline auto symbol_kind_icon(SymbolKind kind) -> const char*
{
    switch (kind)
    {
        case SymbolKind::kFunction:
            return "ƒ";
        case SymbolKind::kClass:
            return "C";
        case SymbolKind::kVariable:
            return "V";
        case SymbolKind::kHeading:
            return "H";
        case SymbolKind::kNamespace:
            return "N";
        case SymbolKind::kInterface:
            return "I";
        case SymbolKind::kProperty:
            return "P";
        case SymbolKind::kEnum:
            return "E";
        case SymbolKind::kConstant:
            return "K";
        case SymbolKind::kMethod:
            return "M";
    }
    return "?";
}

/// Abstract interface for providing symbols for a document.
///
/// Concrete implementations provide symbols from different document types
/// (Markdown headings, code functions/classes, etc.).
class ISymbolProvider
{
public:
    virtual ~ISymbolProvider() = default;

    /// Get all symbols for a given document.
    /// @param document_id  Identifier for the document (usually a file path).
    /// @return Vector of SymbolInfo sorted by document position (line).
    [[nodiscard]] virtual auto get_symbols(const std::string& document_id) const
        -> std::vector<SymbolInfo> = 0;

    /// Check if this provider supports the given document type.
    [[nodiscard]] virtual auto supports(const std::string& document_id) const -> bool = 0;
};

} // namespace markamp::core
