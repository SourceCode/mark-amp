#include "SyntaxHighlighter.h"

#include "Profiler.h"
#include "StringUtils.h"

#include <algorithm>
#include <cctype>

namespace markamp::core
{

// ═══════════════════════════════════════════════════════
// Construction
// ═══════════════════════════════════════════════════════

SyntaxHighlighter::SyntaxHighlighter()
{
    register_builtin_languages();
}

// ═══════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════

void SyntaxHighlighter::register_language(LanguageDefinition def)
{
    auto name = def.name;
    for (const auto& alias : def.aliases)
    {
        alias_map_[alias] = name;
    }
    alias_map_[name] = name; // Self-alias
    languages_.emplace(std::move(name), std::move(def));
}

auto SyntaxHighlighter::tokenize(std::string_view source, const std::string& language)
    -> std::vector<Token>
{
    const auto* def = resolve_language(language);
    if (def == nullptr)
    {
        // Unsupported language: return entire source as single Text token
        return {{TokenType::Text, std::string(source), 0, source.size()}};
    }
    return tokenize_with_def(source, *def);
}

auto SyntaxHighlighter::render_html(std::string_view source, const std::string& language)
    -> std::string
{
    MARKAMP_PROFILE_SCOPE("SyntaxHighlighter::render_html");
    auto tokens = tokenize(source, language);
    std::string html;
    html.reserve(source.size() * 2);

    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Whitespace || tok.type == TokenType::Text)
        {
            html += escape_html(tok.text);
        }
        else
        {
            html += "<span class=\"token-";
            html += token_class(tok.type);
            html += "\">";
            html += escape_html(tok.text);
            html += "</span>";
        }
    }
    return html;
}

auto SyntaxHighlighter::is_supported(const std::string& language) const -> bool
{
    return resolve_language(language) != nullptr;
}

auto SyntaxHighlighter::supported_languages() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(languages_.size());
    for (const auto& [name, _] : languages_)
    {
        names.push_back(name);
    }
    std::sort(names.begin(), names.end());
    return names;
}

auto SyntaxHighlighter::token_class(TokenType type) -> std::string_view
{
    switch (type)
    {
        case TokenType::Keyword:
            return "keyword";
        case TokenType::String:
            return "string";
        case TokenType::Number:
            return "number";
        case TokenType::Comment:
            return "comment";
        case TokenType::Operator:
            return "operator";
        case TokenType::Punctuation:
            return "punctuation";
        case TokenType::Function:
            return "function";
        case TokenType::Type:
            return "type";
        case TokenType::Attribute:
            return "attribute";
        case TokenType::Tag:
            return "tag";
        case TokenType::Property:
            return "property";
        case TokenType::Variable:
            return "variable";
        case TokenType::Constant:
            return "constant";
        case TokenType::Preprocessor:
            return "preprocessor";
        case TokenType::Whitespace:
            return "whitespace";
        default:
            return "text";
    }
}

// ═══════════════════════════════════════════════════════
// Private: resolution
// ═══════════════════════════════════════════════════════

auto SyntaxHighlighter::resolve_language(const std::string& name_or_alias) const
    -> const LanguageDefinition*
{
    // Try direct lookup
    auto alias_it = alias_map_.find(name_or_alias);
    if (alias_it == alias_map_.end())
    {
        // Try lowercase
        std::string lower = name_or_alias;
        std::transform(lower.begin(),
                       lower.end(),
                       lower.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        alias_it = alias_map_.find(lower);
        if (alias_it == alias_map_.end())
        {
            return nullptr;
        }
    }

    auto lang_it = languages_.find(alias_it->second);
    if (lang_it == languages_.end())
    {
        return nullptr;
    }
    return &lang_it->second;
}

// ═══════════════════════════════════════════════════════
// Private: tokenizer
// ═══════════════════════════════════════════════════════

auto SyntaxHighlighter::tokenize_with_def(std::string_view source, const LanguageDefinition& def)
    -> std::vector<Token>
{
    std::vector<Token> tokens;
    tokens.reserve(source.size() / 4); // Rough estimate

    size_t pos = 0;
    const size_t len = source.size();

    while (pos < len)
    {
        // 1. Block comment
        if (!def.block_comment_start.empty() && starts_with(source, pos, def.block_comment_start))
        {
            size_t start = pos;
            pos += def.block_comment_start.size();
            while (pos < len && !starts_with(source, pos, def.block_comment_end))
            {
                ++pos;
            }
            if (pos < len)
            {
                pos += def.block_comment_end.size();
            }
            tokens.push_back({TokenType::Comment,
                              std::string(source.substr(start, pos - start)),
                              start,
                              pos - start});
            continue;
        }

        // 2. Line comment
        if (!def.line_comment.empty() && starts_with(source, pos, def.line_comment))
        {
            size_t start = pos;
            while (pos < len && source[pos] != '\n')
            {
                ++pos;
            }
            tokens.push_back({TokenType::Comment,
                              std::string(source.substr(start, pos - start)),
                              start,
                              pos - start});
            continue;
        }

        // 2b. Python-style line comment (#)
        // Handled by the line_comment check above for bash/python

        // 3. String literal
        if (def.string_delimiters.find(source[pos]) != std::string::npos)
        {
            char delim = source[pos];
            size_t start = pos;
            ++pos;

            // Check for triple-quoted strings (Python)
            bool triple = false;
            if (pos + 1 < len && source[pos] == delim && source[pos + 1] == delim)
            {
                triple = true;
                pos += 2;
            }

            while (pos < len)
            {
                if (source[pos] == '\\' && pos + 1 < len)
                {
                    pos += 2; // Skip escape
                    continue;
                }
                if (triple)
                {
                    if (pos + 2 < len && source[pos] == delim && source[pos + 1] == delim &&
                        source[pos + 2] == delim)
                    {
                        pos += 3;
                        break;
                    }
                }
                else if (source[pos] == delim)
                {
                    ++pos;
                    break;
                }
                else if (!triple && source[pos] == '\n')
                {
                    break; // Unterminated single-line string
                }
                ++pos;
            }
            tokens.push_back({TokenType::String,
                              std::string(source.substr(start, pos - start)),
                              start,
                              pos - start});
            continue;
        }

        // 3b. Backtick template literals
        if (def.has_backtick_strings && source[pos] == '`')
        {
            size_t start = pos;
            ++pos;
            while (pos < len)
            {
                if (source[pos] == '\\' && pos + 1 < len)
                {
                    pos += 2;
                    continue;
                }
                if (source[pos] == '`')
                {
                    ++pos;
                    break;
                }
                ++pos;
            }
            tokens.push_back({TokenType::String,
                              std::string(source.substr(start, pos - start)),
                              start,
                              pos - start});
            continue;
        }

        // 4. Preprocessor directive (at start of line)
        if (def.has_preprocessor && source[pos] == def.preprocessor_prefix[0])
        {
            // Check if at line start (pos == 0 or previous char is '\n')
            if (pos == 0 || source[pos - 1] == '\n')
            {
                size_t start = pos;
                while (pos < len && source[pos] != '\n')
                {
                    ++pos;
                }
                tokens.push_back({TokenType::Preprocessor,
                                  std::string(source.substr(start, pos - start)),
                                  start,
                                  pos - start});
                continue;
            }
        }

        // 5. Number literal
        if (std::isdigit(static_cast<unsigned char>(source[pos])) ||
            (source[pos] == '.' && pos + 1 < len &&
             std::isdigit(static_cast<unsigned char>(source[pos + 1]))))
        {
            size_t start = pos;
            // Hex prefix
            if (source[pos] == '0' && pos + 1 < len &&
                (source[pos + 1] == 'x' || source[pos + 1] == 'X'))
            {
                pos += 2;
                while (pos < len && std::isxdigit(static_cast<unsigned char>(source[pos])))
                {
                    ++pos;
                }
            }
            else
            {
                while (pos < len && (std::isdigit(static_cast<unsigned char>(source[pos])) ||
                                     source[pos] == '.' || source[pos] == 'e' ||
                                     source[pos] == 'E' || source[pos] == '_'))
                {
                    ++pos;
                }
            }
            // Suffix chars (f, l, u, etc.)
            while (pos < len && std::isalpha(static_cast<unsigned char>(source[pos])))
            {
                ++pos;
            }
            tokens.push_back({TokenType::Number,
                              std::string(source.substr(start, pos - start)),
                              start,
                              pos - start});
            continue;
        }

        // 6. Identifier (letter or underscore start)
        if (std::isalpha(static_cast<unsigned char>(source[pos])) || source[pos] == '_' ||
            source[pos] == '@') // @ for decorators
        {
            // Check for decorator/attribute
            if (source[pos] == '@')
            {
                size_t start = pos;
                ++pos;
                while (pos < len && (std::isalnum(static_cast<unsigned char>(source[pos])) ||
                                     source[pos] == '_' || source[pos] == '.'))
                {
                    ++pos;
                }
                tokens.push_back({TokenType::Attribute,
                                  std::string(source.substr(start, pos - start)),
                                  start,
                                  pos - start});
                continue;
            }

            size_t start = pos;
            while (pos < len &&
                   (std::isalnum(static_cast<unsigned char>(source[pos])) || source[pos] == '_'))
            {
                ++pos;
            }
            std::string id(source.substr(start, pos - start));
            auto type = classify_identifier(id, source, pos, def);
            tokens.push_back({type, std::move(id), start, pos - start});
            continue;
        }

        // 7. Operator
        if (is_operator_char(source[pos]))
        {
            size_t start = pos;
            // Consume multi-char operators
            ++pos;
            while (pos < len && is_operator_char(source[pos]))
            {
                ++pos;
            }
            tokens.push_back({TokenType::Operator,
                              std::string(source.substr(start, pos - start)),
                              start,
                              pos - start});
            continue;
        }

        // 8. Punctuation
        if (is_punctuation_char(source[pos]))
        {
            tokens.push_back({TokenType::Punctuation, std::string(1, source[pos]), pos, 1});
            ++pos;
            continue;
        }

        // 9. Whitespace
        if (std::isspace(static_cast<unsigned char>(source[pos])))
        {
            size_t start = pos;
            while (pos < len && std::isspace(static_cast<unsigned char>(source[pos])))
            {
                ++pos;
            }
            tokens.push_back({TokenType::Whitespace,
                              std::string(source.substr(start, pos - start)),
                              start,
                              pos - start});
            continue;
        }

        // 10. Anything else -> Text
        tokens.push_back({TokenType::Text, std::string(1, source[pos]), pos, 1});
        ++pos;
    }

    return tokens;
}

auto SyntaxHighlighter::classify_identifier(const std::string& id,
                                            std::string_view source,
                                            size_t end_pos,
                                            const LanguageDefinition& def) -> TokenType
{
    // Check keywords
    for (const auto& kw : def.keywords)
    {
        if (id == kw)
            return TokenType::Keyword;
    }

    // Check types
    for (const auto& tp : def.types)
    {
        if (id == tp)
            return TokenType::Type;
    }

    // Check constants
    for (const auto& ct : def.constants)
    {
        if (id == ct)
            return TokenType::Constant;
    }

    // Check if ALL_CAPS -> constant
    if (id.size() > 1)
    {
        bool all_upper = true;
        for (char ch : id)
        {
            if (ch != '_' && !std::isupper(static_cast<unsigned char>(ch)) &&
                !std::isdigit(static_cast<unsigned char>(ch)))
            {
                all_upper = false;
                break;
            }
        }
        if (all_upper)
            return TokenType::Constant;
    }

    // Check if followed by '(' -> function
    size_t check = end_pos;
    while (check < source.size() && source[check] == ' ')
    {
        ++check;
    }
    if (check < source.size() && source[check] == '(')
    {
        return TokenType::Function;
    }

    return TokenType::Text;
}

// ═══════════════════════════════════════════════════════
// Helpers
// ═══════════════════════════════════════════════════════

auto SyntaxHighlighter::is_operator_char(char ch) -> bool
{
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '=' ||
           ch == '!' || ch == '<' || ch == '>' || ch == '&' || ch == '|' || ch == '^' ||
           ch == '~' || ch == '?';
}

auto SyntaxHighlighter::is_punctuation_char(char ch) -> bool
{
    return ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == '[' || ch == ']' ||
           ch == ';' || ch == ',' || ch == ':' || ch == '.';
}

auto SyntaxHighlighter::starts_with(std::string_view text, size_t pos, std::string_view prefix)
    -> bool
{
    if (pos + prefix.size() > text.size())
        return false;
    return text.substr(pos, prefix.size()) == prefix;
}

auto SyntaxHighlighter::escape_html(std::string_view text) -> std::string
{
    return core::escape_html(text);
}

// ═══════════════════════════════════════════════════════
// Built-in language definitions (15 Tier 1 languages)
// ═══════════════════════════════════════════════════════

void SyntaxHighlighter::register_builtin_languages()
{
    // --- JavaScript ---
    register_language({
        .name = "javascript",
        .aliases = {"js", "jsx", "mjs"},
        .keywords = {"async",    "await",      "break",   "case",   "catch",    "class",  "const",
                     "continue", "debugger",   "default", "delete", "do",       "else",   "export",
                     "extends",  "finally",    "for",     "from",   "function", "if",     "import",
                     "in",       "instanceof", "let",     "new",    "of",       "return", "static",
                     "super",    "switch",     "this",    "throw",  "try",      "typeof", "var",
                     "void",     "while",      "with",    "yield"},
        .types = {"Array",
                  "Boolean",
                  "Date",
                  "Error",
                  "Function",
                  "Map",
                  "Number",
                  "Object",
                  "Promise",
                  "RegExp",
                  "Set",
                  "String",
                  "Symbol",
                  "WeakMap",
                  "WeakSet"},
        .constants = {"true",
                      "false",
                      "null",
                      "undefined",
                      "NaN",
                      "Infinity",
                      "console",
                      "window",
                      "document",
                      "globalThis"},
        .line_comment = "//",
        .block_comment_start = "/*",
        .block_comment_end = "*/",
        .string_delimiters = "\"'",
        .has_backtick_strings = true,
    });

    // --- TypeScript ---
    register_language({
        .name = "typescript",
        .aliases = {"ts", "tsx"},
        .keywords = {"abstract",   "as",       "async",  "await",      "break",     "case",
                     "catch",      "class",    "const",  "continue",   "debugger",  "declare",
                     "default",    "delete",   "do",     "else",       "enum",      "export",
                     "extends",    "finally",  "for",    "from",       "function",  "if",
                     "implements", "import",   "in",     "instanceof", "interface", "is",
                     "keyof",      "let",      "module", "namespace",  "new",       "of",
                     "override",   "readonly", "return", "static",     "super",     "switch",
                     "this",       "throw",    "try",    "type",       "typeof",    "var",
                     "void",       "while",    "with",   "yield"},
        .types = {"any",     "boolean",  "never",    "number", "object", "string",  "symbol",
                  "unknown", "void",     "Array",    "Map",    "Set",    "Promise", "Record",
                  "Partial", "Required", "Readonly", "Pick",   "Omit",   "Exclude", "Extract"},
        .constants = {"true", "false", "null", "undefined", "NaN", "Infinity"},
        .line_comment = "//",
        .block_comment_start = "/*",
        .block_comment_end = "*/",
        .string_delimiters = "\"'",
        .has_backtick_strings = true,
    });

    // --- Python ---
    register_language({
        .name = "python",
        .aliases = {"py", "python3"},
        .keywords = {"and",      "as",       "assert", "async", "await",  "break",  "class",
                     "continue", "def",      "del",    "elif",  "else",   "except", "finally",
                     "for",      "from",     "global", "if",    "import", "in",     "is",
                     "lambda",   "nonlocal", "not",    "or",    "pass",   "raise",  "return",
                     "try",      "while",    "with",   "yield"},
        .types = {"int",
                  "float",
                  "str",
                  "bool",
                  "list",
                  "dict",
                  "tuple",
                  "set",
                  "bytes",
                  "bytearray",
                  "complex",
                  "frozenset",
                  "type"},
        .constants = {"True", "False", "None", "self", "cls"},
        .line_comment = "#",
        .block_comment_start = {},
        .block_comment_end = {},
        .string_delimiters = "\"'",
        .has_backtick_strings = false,
    });

    // --- C ---
    register_language({
        .name = "c",
        .aliases = {"h"},
        .keywords = {"auto",     "break",    "case",     "const",  "continue", "default", "do",
                     "else",     "enum",     "extern",   "for",    "goto",     "if",      "inline",
                     "register", "restrict", "return",   "sizeof", "static",   "struct",  "switch",
                     "typedef",  "union",    "volatile", "while"},
        .types = {"char",     "double",   "float",    "int",      "long",    "short",   "signed",
                  "unsigned", "void",     "size_t",   "int8_t",   "int16_t", "int32_t", "int64_t",
                  "uint8_t",  "uint16_t", "uint32_t", "uint64_t", "bool",    "FILE",    "NULL"},
        .constants = {"true", "false", "NULL", "EOF", "stdin", "stdout", "stderr"},
        .line_comment = "//",
        .block_comment_start = "/*",
        .block_comment_end = "*/",
        .string_delimiters = "\"'",
        .has_preprocessor = true,
        .preprocessor_prefix = "#",
    });

    // --- C++ ---
    register_language({
        .name = "cpp",
        .aliases = {"c++", "cxx", "cc", "hpp"},
        .keywords = {"alignas",   "alignof",       "auto",
                     "break",     "case",          "catch",
                     "class",     "const",         "constexpr",
                     "consteval", "constinit",     "continue",
                     "co_await",  "co_return",     "co_yield",
                     "decltype",  "default",       "delete",
                     "do",        "dynamic_cast",  "else",
                     "enum",      "explicit",      "export",
                     "extern",    "final",         "for",
                     "friend",    "goto",          "if",
                     "inline",    "mutable",       "namespace",
                     "new",       "noexcept",      "operator",
                     "override",  "private",       "protected",
                     "public",    "register",      "reinterpret_cast",
                     "requires",  "return",        "sizeof",
                     "static",    "static_assert", "static_cast",
                     "struct",    "switch",        "template",
                     "this",      "throw",         "try",
                     "typedef",   "typeid",        "typename",
                     "union",     "using",         "virtual",
                     "volatile",  "while"},
        .types = {"bool",          "char",       "char8_t",    "char16_t", "char32_t",
                  "double",        "float",      "int",        "long",     "short",
                  "signed",        "unsigned",   "void",       "wchar_t",  "size_t",
                  "string",        "vector",     "map",        "set",      "unordered_map",
                  "unordered_set", "shared_ptr", "unique_ptr", "optional", "variant",
                  "tuple",         "pair",       "array",      "span",     "string_view",
                  "expected"},
        .constants = {"true", "false", "nullptr", "NULL"},
        .line_comment = "//",
        .block_comment_start = "/*",
        .block_comment_end = "*/",
        .string_delimiters = "\"'",
        .has_preprocessor = true,
        .preprocessor_prefix = "#",
    });

    // --- Rust ---
    register_language({
        .name = "rust",
        .aliases = {"rs"},
        .keywords = {"as",     "async", "await",  "break",  "const", "continue", "crate", "dyn",
                     "else",   "enum",  "extern", "fn",     "for",   "if",       "impl",  "in",
                     "let",    "loop",  "match",  "mod",    "move",  "mut",      "pub",   "ref",
                     "return", "self",  "static", "struct", "super", "trait",    "type",  "unsafe",
                     "use",    "where", "while",  "yield"},
        .types = {"bool", "char",   "f32",    "f64",     "i8",      "i16",      "i32",
                  "i64",  "i128",   "isize",  "str",     "u8",      "u16",      "u32",
                  "u64",  "u128",   "usize",  "String",  "Vec",     "Box",      "Rc",
                  "Arc",  "Option", "Result", "HashMap", "HashSet", "BTreeMap", "BTreeSet"},
        .constants = {"true", "false", "None", "Some", "Ok", "Err", "Self"},
        .line_comment = "//",
        .block_comment_start = "/*",
        .block_comment_end = "*/",
        .string_delimiters = "\"'",
    });

    // --- Go ---
    register_language({
        .name = "go",
        .aliases = {"golang"},
        .keywords = {"break",  "case",        "chan", "const",   "continue", "default", "defer",
                     "else",   "fallthrough", "for",  "func",    "go",       "goto",    "if",
                     "import", "interface",   "map",  "package", "range",    "return",  "select",
                     "struct", "switch",      "type", "var"},
        .types = {"bool", "byte",  "complex64", "complex128", "error",  "float32", "float64",
                  "int",  "int8",  "int16",     "int32",      "int64",  "rune",    "string",
                  "uint", "uint8", "uint16",    "uint32",     "uint64", "uintptr"},
        .constants = {"true", "false", "nil", "iota"},
        .line_comment = "//",
        .block_comment_start = "/*",
        .block_comment_end = "*/",
        .string_delimiters = "\"'",
        .has_backtick_strings = true,
    });

    // --- Java ---
    register_language({
        .name = "java",
        .aliases = {},
        .keywords = {"abstract",   "assert",       "break",  "case",   "catch",      "class",
                     "continue",   "default",      "do",     "else",   "enum",       "extends",
                     "final",      "finally",      "for",    "if",     "implements", "import",
                     "instanceof", "interface",    "native", "new",    "package",    "private",
                     "protected",  "public",       "return", "static", "strictfp",   "super",
                     "switch",     "synchronized", "this",   "throw",  "throws",     "transient",
                     "try",        "volatile",     "while"},
        .types = {"boolean", "byte", "char",   "double",  "float",     "int",     "long",
                  "short",   "void", "String", "Integer", "Long",      "Double",  "Boolean",
                  "Object",  "List", "Map",    "Set",     "ArrayList", "HashMap", "Optional"},
        .constants = {"true", "false", "null"},
        .line_comment = "//",
        .block_comment_start = "/*",
        .block_comment_end = "*/",
        .string_delimiters = "\"'",
    });

    // --- C# ---
    register_language({
        .name = "csharp",
        .aliases = {"cs", "c#"},
        .keywords = {"abstract",  "as",       "async",     "await",     "base",       "break",
                     "case",      "catch",    "checked",   "class",     "const",      "continue",
                     "default",   "delegate", "do",        "else",      "enum",       "event",
                     "explicit",  "extern",   "finally",   "fixed",     "for",        "foreach",
                     "goto",      "if",       "implicit",  "in",        "interface",  "internal",
                     "is",        "lock",     "namespace", "new",       "operator",   "out",
                     "override",  "params",   "private",   "protected", "public",     "readonly",
                     "ref",       "return",   "sealed",    "sizeof",    "stackalloc", "static",
                     "struct",    "switch",   "this",      "throw",     "try",        "typeof",
                     "unchecked", "unsafe",   "using",     "virtual",   "void",       "volatile",
                     "while",     "yield"},
        .types = {"bool",   "byte",   "char",    "decimal", "double",     "float", "int",
                  "long",   "object", "sbyte",   "short",   "string",     "uint",  "ulong",
                  "ushort", "var",    "dynamic", "List",    "Dictionary", "Task"},
        .constants = {"true", "false", "null"},
        .line_comment = "//",
        .block_comment_start = "/*",
        .block_comment_end = "*/",
        .string_delimiters = "\"'",
    });

    // --- HTML ---
    register_language({
        .name = "html",
        .aliases = {"htm"},
        .keywords = {},
        .types = {},
        .constants = {},
        .line_comment = {},
        .block_comment_start = "<!--",
        .block_comment_end = "-->",
        .string_delimiters = "\"'",
    });

    // --- CSS ---
    register_language({
        .name = "css",
        .aliases = {"scss", "less"},
        .keywords = {"!important",
                     "@media",
                     "@import",
                     "@keyframes",
                     "@font-face",
                     "@charset",
                     "@supports",
                     "@layer",
                     "@container"},
        .types = {},
        .constants = {"inherit",
                      "initial",
                      "unset",
                      "revert",
                      "none",
                      "auto",
                      "transparent",
                      "currentColor"},
        .line_comment = "//", // SCSS
        .block_comment_start = "/*",
        .block_comment_end = "*/",
        .string_delimiters = "\"'",
    });

    // --- JSON ---
    register_language({
        .name = "json",
        .aliases = {"jsonc"},
        .keywords = {},
        .types = {},
        .constants = {"true", "false", "null"},
        .line_comment = "//", // JSONC
        .block_comment_start = {},
        .block_comment_end = {},
        .string_delimiters = "\"",
    });

    // --- YAML ---
    register_language({
        .name = "yaml",
        .aliases = {"yml"},
        .keywords = {},
        .types = {},
        .constants = {"true", "false", "null", "yes", "no", "on", "off"},
        .line_comment = "#",
        .block_comment_start = {},
        .block_comment_end = {},
        .string_delimiters = "\"'",
    });

    // --- SQL ---
    register_language({
        .name = "sql",
        .aliases = {"mysql", "postgresql", "sqlite"},
        .keywords = {"SELECT",
                     "FROM",
                     "WHERE",
                     "INSERT",
                     "INTO",
                     "VALUES",
                     "UPDATE",
                     "SET",
                     "DELETE",
                     "CREATE",
                     "TABLE",
                     "ALTER",
                     "DROP",
                     "INDEX",
                     "JOIN",
                     "INNER",
                     "LEFT",
                     "RIGHT",
                     "OUTER",
                     "ON",
                     "AND",
                     "OR",
                     "NOT",
                     "IN",
                     "BETWEEN",
                     "LIKE",
                     "IS",
                     "NULL",
                     "AS",
                     "ORDER",
                     "BY",
                     "GROUP",
                     "HAVING",
                     "LIMIT",
                     "OFFSET",
                     "UNION",
                     "ALL",
                     "DISTINCT",
                     "EXISTS",
                     "CASE",
                     "WHEN",
                     "THEN",
                     "ELSE",
                     "END",
                     "BEGIN",
                     "COMMIT",
                     "ROLLBACK",
                     "PRIMARY",
                     "KEY",
                     "FOREIGN",
                     "REFERENCES",
                     "CONSTRAINT",
                     "DEFAULT",
                     "CHECK",
                     "UNIQUE",
                     "CASCADE",
                     // lowercase versions
                     "select",
                     "from",
                     "where",
                     "insert",
                     "into",
                     "values",
                     "update",
                     "set",
                     "delete",
                     "create",
                     "table",
                     "alter",
                     "drop",
                     "index",
                     "join",
                     "inner",
                     "left",
                     "right",
                     "outer",
                     "on",
                     "and",
                     "or",
                     "not",
                     "in",
                     "between",
                     "like",
                     "is",
                     "null",
                     "as",
                     "order",
                     "by",
                     "group",
                     "having",
                     "limit",
                     "offset",
                     "union",
                     "all",
                     "distinct",
                     "exists",
                     "case",
                     "when",
                     "then",
                     "else",
                     "end",
                     "begin",
                     "commit",
                     "rollback",
                     "primary",
                     "key",
                     "foreign",
                     "references",
                     "constraint",
                     "default",
                     "check",
                     "unique",
                     "cascade"},
        .types = {"INT",     "INTEGER",   "BIGINT",  "SMALLINT", "FLOAT",   "DOUBLE",   "DECIMAL",
                  "NUMERIC", "VARCHAR",   "CHAR",    "TEXT",     "BOOLEAN", "DATE",     "TIMESTAMP",
                  "BLOB",    "SERIAL",    "int",     "integer",  "bigint",  "smallint", "float",
                  "double",  "decimal",   "numeric", "varchar",  "char",    "text",     "boolean",
                  "date",    "timestamp", "blob",    "serial"},
        .constants = {"TRUE", "FALSE", "NULL", "true", "false"},
        .line_comment = "--",
        .block_comment_start = "/*",
        .block_comment_end = "*/",
        .string_delimiters = "\"'",
    });

    // --- Bash/Shell ---
    register_language({
        .name = "bash",
        .aliases = {"sh", "shell", "zsh"},
        .keywords = {"if",     "then",   "else",  "elif",    "fi",       "for",    "while", "do",
                     "done",   "case",   "esac",  "in",      "function", "return", "exit",  "local",
                     "export", "source", "alias", "unalias", "set",      "unset",  "shift", "trap",
                     "eval",   "exec",   "read",  "echo",    "printf",   "cd",     "pwd",   "ls",
                     "mkdir",  "rm",     "cp",    "mv",      "cat",      "grep",   "sed",   "awk",
                     "find",   "xargs",  "sudo"},
        .types = {},
        .constants = {"true", "false"},
        .line_comment = "#",
        .block_comment_start = {},
        .block_comment_end = {},
        .string_delimiters = "\"'",
    });
}

} // namespace markamp::core
