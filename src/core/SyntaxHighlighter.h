#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ═══════════════════════════════════════════════════════
// Token classification
// ═══════════════════════════════════════════════════════

enum class TokenType
{
    Text,         // Normal text / identifiers
    Keyword,      // Language keywords (if, else, return, class, etc.)
    String,       // String literals
    Number,       // Numeric literals
    Comment,      // Comments (line and block)
    Operator,     // Operators (+, -, *, etc.)
    Punctuation,  // Braces, brackets, semicolons
    Function,     // Function names (identifier followed by '(')
    Type,         // Type names
    Attribute,    // Attributes/decorators
    Tag,          // HTML/XML tags
    Property,     // Object properties
    Variable,     // Variables
    Constant,     // Constants (true, false, null, ALL_CAPS)
    Preprocessor, // Preprocessor directives (#include, #define)
    Whitespace    // Whitespace (preserved but not colored)
};

struct Token
{
    TokenType type{TokenType::Text};
    std::string text;
    size_t start{0};
    size_t length{0};
};

// ═══════════════════════════════════════════════════════
// Structure-of-Arrays (SoA) token layout
// ═══════════════════════════════════════════════════════

/// Cache-friendly token storage using parallel arrays.
/// Iteration over token types or positions without loading the text
/// is significantly faster due to data locality.
///
/// Pattern implemented: #10 Cache-friendly data layout
struct TokenArraySoA
{
    std::vector<TokenType> types;
    std::vector<size_t> starts;
    std::vector<size_t> lengths;
    std::vector<uint16_t> scope_ids; // compact identifier for scope/language

    void push_back(TokenType type, size_t start, size_t length, uint16_t scope_id = 0)
    {
        types.push_back(type);
        starts.push_back(start);
        lengths.push_back(length);
        scope_ids.push_back(scope_id);
    }

    void reserve(size_t count)
    {
        types.reserve(count);
        starts.reserve(count);
        lengths.reserve(count);
        scope_ids.reserve(count);
    }

    [[nodiscard]] auto size() const noexcept -> size_t
    {
        return types.size();
    }
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return types.empty();
    }

    void clear()
    {
        types.clear();
        starts.clear();
        lengths.clear();
        scope_ids.clear();
    }
};

// ═══════════════════════════════════════════════════════
// Language definition
// ═══════════════════════════════════════════════════════

struct LanguageDefinition
{
    std::string name;
    std::vector<std::string> aliases; // e.g., "js" -> "javascript"
    std::vector<std::string> keywords;
    std::vector<std::string> types;
    std::vector<std::string> constants;
    std::string line_comment;             // e.g., "//"
    std::string block_comment_start;      // e.g., "/*"
    std::string block_comment_end;        // e.g., "*/"
    std::string string_delimiters{"\"'"}; // Which chars start strings
    bool has_backtick_strings{false};     // Template literals
    bool has_preprocessor{false};
    std::string preprocessor_prefix; // e.g., "#"
    bool has_raw_strings{false};     // R"(...)" style
};

// ═══════════════════════════════════════════════════════
// Syntax highlighter
// ═══════════════════════════════════════════════════════

class SyntaxHighlighter
{
public:
    SyntaxHighlighter();

    /// Register a language definition.
    void register_language(LanguageDefinition def);

    /// Tokenize source code in the given language.
    [[nodiscard]] auto tokenize(std::string_view source, const std::string& language)
        -> std::vector<Token>;

    /// Tokenize into SoA layout for cache-friendly iteration (Pattern #10).
    [[nodiscard]] auto tokenize_soa(std::string_view source, const std::string& language)
        -> TokenArraySoA;

    /// Render source code as HTML with <span class="token-*"> tags.
    [[nodiscard]] auto render_html(std::string_view source, const std::string& language)
        -> std::string;

    /// Check if a language is supported.
    [[nodiscard]] auto is_supported(const std::string& language) const -> bool;

    /// List all supported language names.
    [[nodiscard]] auto supported_languages() const -> std::vector<std::string>;

    /// Convert TokenType to CSS class suffix (e.g. TokenType::Keyword -> "keyword").
    [[nodiscard]] static auto token_class(TokenType type) -> std::string_view;

private:
    std::unordered_map<std::string, LanguageDefinition> languages_;
    std::unordered_map<std::string, std::string> alias_map_; // alias -> canonical name

    [[nodiscard]] auto resolve_language(const std::string& name_or_alias) const
        -> const LanguageDefinition*;

    [[nodiscard]] auto tokenize_with_def(std::string_view source, const LanguageDefinition& def)
        -> std::vector<Token>;

    void register_builtin_languages();

    // Tokenizer helpers
    [[nodiscard]] static auto is_operator_char(char ch) -> bool;
    [[nodiscard]] static auto is_punctuation_char(char ch) -> bool;
    [[nodiscard]] static auto
    starts_with(std::string_view text, size_t pos, std::string_view prefix) -> bool;

    [[nodiscard]] static auto escape_html(std::string_view text) -> std::string;

    // Classify an identifier against the language definition
    [[nodiscard]] static auto classify_identifier(const std::string& id,
                                                  std::string_view source,
                                                  size_t end_pos,
                                                  const LanguageDefinition& def) -> TokenType;
};

} // namespace markamp::core
