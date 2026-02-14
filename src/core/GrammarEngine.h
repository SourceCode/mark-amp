#pragma once

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// A tokenized scope from a grammar rule.
struct GrammarToken
{
    int start_index{0}; ///< Start character offset within the line
    int end_index{0};   ///< End character offset within the line
    std::string scope;  ///< TextMate scope name (e.g. "keyword.control.markdown")
};

/// Represents a loaded TextMate grammar.
struct Grammar
{
    std::string scope_name; ///< Top-level scope (e.g. "source.markdown")
    std::string name;       ///< Human-readable name
    std::string path;       ///< File path to the grammar definition
};

/// Stub engine for TextMate grammar loading and tokenization.
///
/// This is a P3 placeholder. MarkAmp currently uses its own `SyntaxHighlighter`
/// for Markdown highlighting. Full TextMate grammar support would be needed for
/// extensions that contribute language support (e.g. embedded code blocks).
///
/// A complete implementation would:
/// - Parse `.tmLanguage.json` files
/// - Build regex-based tokenization rules
/// - Support scope nesting and grammar injection
///
/// For now, all methods return empty/default results.
class GrammarEngine
{
public:
    GrammarEngine() = default;

    /// Load a grammar from a `.tmLanguage.json` file.
    /// Stub: always returns false.
    auto load_grammar(const std::string& path) -> bool;

    /// Get a loaded grammar by scope name.
    /// Stub: always returns nullptr.
    [[nodiscard]] auto get_grammar(const std::string& scope_name) const -> const Grammar*;

    /// Tokenize a single line using the specified grammar.
    /// Stub: always returns empty vector.
    [[nodiscard]] auto tokenize_line(const std::string& scope_name, const std::string& line) const
        -> std::vector<GrammarToken>;

    /// List all loaded grammars.
    [[nodiscard]] auto grammars() const -> const std::vector<Grammar>&;

private:
    std::vector<Grammar> grammars_;
};

} // namespace markamp::core
