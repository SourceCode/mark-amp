// ============================================================================
// File: src/core/TemplateTypes.h
// Phase 35: Template Engine — Types and configuration
// ============================================================================
#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace markamp::core
{

// Template variable value types.
using TemplateValue = std::variant<std::string,
                                   int64_t,
                                   double,
                                   bool,
                                   std::vector<std::string>,
                                   std::unordered_map<std::string, std::string>>;

// A single template variable with name and value.
struct TemplateVariable
{
    std::string name;
    TemplateValue value;

    [[nodiscard]] auto as_string() const -> std::string;
    [[nodiscard]] auto as_int() const -> int64_t;
    [[nodiscard]] auto as_double() const -> double;
    [[nodiscard]] auto as_bool() const -> bool;
    [[nodiscard]] auto is_truthy() const -> bool;
};

// Template context — map of variable names to values.
using TemplateContext = std::unordered_map<std::string, TemplateVariable>;

// Result of template rendering.
struct TemplateResult
{
    std::string output;                // Rendered template output
    int64_t elapsed_us{0};             // Rendering time in microseconds
    std::vector<std::string> warnings; // Non-fatal warnings (undefined vars, etc.)
    bool success{true};
    std::string error_message;

    [[nodiscard]] auto has_warnings() const -> bool
    {
        return !warnings.empty();
    }
};

// Options controlling template rendering behavior.
struct TemplateOptions
{
    bool strict_mode{false};          // Error on undefined variables
    bool auto_escape_html{false};     // HTML-escape output by default
    int max_recursion_depth{10};      // Max nesting depth for includes/blocks
    int max_iterations{1000};         // Max loop iterations (prevent infinite loops)
    std::string left_delim{"{{"};     // Left template delimiter
    std::string right_delim{"}}"};    // Right template delimiter
    std::string comment_left{"{/*"};  // Comment start
    std::string comment_right{"*/}"}; // Comment end
    bool trim_whitespace{false};      // Trim whitespace around tags
};

// Supported template node types (AST).
enum class TemplateNodeType : uint8_t
{
    Text,     // Raw text passthrough
    Variable, // {{ .varname }} or {{ .varname | func }}
    If,       // {{ if .cond }} ... {{ else }} ... {{ end }}
    Range,    // {{ range .list }} ... {{ end }}
    With,     // {{ with .var }} ... {{ end }}
    Block,    // {{ block "name" }} ... {{ end }}
    Define,   // {{ define "name" }} ... {{ end }}
    Template, // {{ template "name" . }}
    Comment,  // {/* comment */}
    Pipe,     // Pipeline: value | func1 | func2
};

// A node in the template AST.
struct TemplateNode
{
    TemplateNodeType type{TemplateNodeType::Text};
    std::string content;                   // Raw text or expression
    std::string name;                      // Block/define name
    std::vector<std::string> args;         // Function arguments
    std::vector<TemplateNode> children;    // Child nodes (body)
    std::vector<TemplateNode> else_branch; // Else branch (for if)
    int line{0};                           // Source line number
};

// Template function signature.
// Takes a list of string arguments, returns a string result.
using TemplateFuncSignature = std::function<std::string(const std::vector<std::string>&)>;

} // namespace markamp::core
