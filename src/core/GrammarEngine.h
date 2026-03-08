/// @file GrammarEngine.h
/// @brief V16 Phase 12 — TextMate grammar loading, scope stack, and tokenization.
///
/// Parses `.tmLanguage.json` grammar files and produces scope-annotated tokens.
/// Supports repository rules, nested begin/end patterns, and injection grammars.
#pragma once

#include "SyntaxHighlighter.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// A tokenized scope from a grammar rule.
struct GrammarToken
{
    int start_index{0}; ///< Start character offset within the line
    int end_index{0};   ///< End character offset within the line
    std::string scope;  ///< TextMate scope name (e.g. "keyword.control.cpp")
};

/// Represents a loaded TextMate grammar.
struct Grammar
{
    std::string scope_name; ///< Top-level scope (e.g. "source.cpp")
    std::string name;       ///< Human-readable name (e.g. "C++")
    std::string path;       ///< File path to the grammar definition
};

/// A single rule in a TextMate grammar (match or begin/end pair).
struct GrammarRule
{
    /// Pattern type: simple match, or begin/end region.
    enum class PatternType
    {
        Match,
        BeginEnd
    };

    PatternType pattern_type{PatternType::Match};

    /// For Match rules: the regex pattern to match.
    std::string match_pattern;

    /// For BeginEnd rules: begin and end patterns.
    std::string begin_pattern;
    std::string end_pattern;

    /// Scope assigned to the entire match or region.
    std::string scope_name;

    /// Captured group scopes (group index → scope name).
    std::unordered_map<int, std::string> captures;

    /// For BeginEnd: captures applied to the begin match.
    std::unordered_map<int, std::string> begin_captures;

    /// For BeginEnd: captures applied to the end match.
    std::unordered_map<int, std::string> end_captures;

    /// Nested patterns (for BeginEnd rules).
    std::vector<GrammarRule> nested_patterns;

    /// Reference to a repository rule by name (e.g. "#comments").
    std::string include_ref;

    /// Content name (scope applied to the content between begin/end).
    std::string content_name;
};

/// Internal parsed representation of a grammar definition.
struct GrammarDefinition
{
    Grammar metadata;

    /// Top-level patterns.
    std::vector<GrammarRule> patterns;

    /// Named rule repository for reuse.
    std::unordered_map<std::string, GrammarRule> repository;

    /// File extensions associated with this grammar.
    std::vector<std::string> file_extensions;
};

/// TextMate grammar engine for loading and tokenization.
///
/// Usage:
/// ```cpp
/// GrammarEngine engine;
/// engine.load_grammar("resources/grammars/cpp.tmLanguage.json");
/// auto tokens = engine.tokenize_line("source.cpp", "int main() {");
/// ```
class GrammarEngine
{
public:
    GrammarEngine() = default;

    /// Load a grammar from a `.tmLanguage.json` file.
    /// Returns true if parsed successfully.
    auto load_grammar(const std::string& path) -> bool;

    /// Load a grammar from a JSON string (for testing).
    auto load_grammar_from_string(const std::string& json_content, const std::string& scope_name)
        -> bool;

    /// Get a loaded grammar by scope name.
    [[nodiscard]] auto get_grammar(const std::string& scope_name) const -> const Grammar*;

    /// Tokenize a single line using the specified grammar.
    [[nodiscard]] auto tokenize_line(const std::string& scope_name, const std::string& line) const
        -> std::vector<GrammarToken>;

    /// Tokenize a single line with state carried from the previous line.
    /// Returns updated scope stack for the next line.
    [[nodiscard]] auto tokenize_line_stateful(const std::string& scope_name,
                                              const std::string& line,
                                              ScopeStack& state) const -> std::vector<GrammarToken>;

    /// Map a TextMate scope string to a TokenType.
    [[nodiscard]] static auto scope_to_token_type(const std::string& scope) -> TokenType;

    /// List all loaded grammars.
    [[nodiscard]] auto grammars() const -> const std::vector<Grammar>&;

    /// Get the grammar definition for a scope (for introspection).
    [[nodiscard]] auto get_definition(const std::string& scope_name) const
        -> const GrammarDefinition*;

    /// Resolve a file extension to a grammar scope name.
    [[nodiscard]] auto scope_for_extension(const std::string& extension) const
        -> std::optional<std::string>;

    /// Number of loaded grammars.
    [[nodiscard]] auto grammar_count() const -> size_t;

private:
    std::vector<Grammar> grammars_;
    std::unordered_map<std::string, GrammarDefinition> definitions_;
    std::unordered_map<std::string, std::string> extension_map_;

    /// Parse a grammar definition from JSON content.
    auto parse_grammar_json(const std::string& json_content, const std::string& file_path)
        -> std::optional<GrammarDefinition>;

    /// Parse a patterns array from JSON.
    auto parse_patterns(const std::string& json_content, const std::string& patterns_key)
        -> std::vector<GrammarRule>;

    /// Apply a set of rules to a line, producing tokens.
    void apply_rules(const std::string& line,
                     const std::vector<GrammarRule>& rules,
                     const GrammarDefinition& grammar_def,
                     std::vector<GrammarToken>& tokens,
                     int depth = 0) const;

    /// Resolve #include references to repository rules.
    auto resolve_include(const std::string& ref, const GrammarDefinition& grammar_def) const
        -> const GrammarRule*;

    /// Maximum rule nesting depth to prevent infinite recursion.
    static constexpr int kMaxRuleDepth = 32;
};

} // namespace markamp::core
