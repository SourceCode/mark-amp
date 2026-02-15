// ============================================================================
// File: src/core/TemplateEngine.h
// Phase 35: Template Engine — Sprig-compatible template parser/renderer
// ============================================================================
#pragma once

#include "TemplateTypes.h"

#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace markamp::core
{

class EventBus;

// TemplateEngine — parses and renders Go/Sprig-style templates.
//
// Supports:
//   {{ .variable }}              — variable interpolation
//   {{ .variable | upper }}      — pipeline with function calls
//   {{ if .cond }}...{{ end }}   — conditional blocks
//   {{ range .list }}...{{ end }}— iteration
//   {{ with .var }}...{{ end }}  — scoped context
//   {{ define "name" }}...       — named template definitions
//   {{ template "name" . }}     — template inclusion
//   {/* comment */}             — comments
//
class TemplateEngine
{
public:
    explicit TemplateEngine(EventBus& event_bus);

    // Parse a template string into an AST.
    [[nodiscard]] auto parse(const std::string& template_source)
        -> std::expected<std::vector<TemplateNode>, std::string>;

    // Render a template string with the given context.
    [[nodiscard]] auto render(const std::string& template_source,
                              const TemplateContext& context,
                              const TemplateOptions& options = {}) -> TemplateResult;

    // Render a pre-parsed AST with the given context.
    [[nodiscard]] auto render_ast(const std::vector<TemplateNode>& ast,
                                  const TemplateContext& context,
                                  const TemplateOptions& options = {}) -> TemplateResult;

    // Register a custom template function.
    auto register_function(const std::string& name, TemplateFuncSignature func) -> void;

    // Check if a function is registered.
    [[nodiscard]] auto has_function(const std::string& name) const -> bool;

    // Register a named template definition for use with {{ template "name" }}.
    auto register_template(const std::string& name, const std::string& template_source) -> void;

    // Get list of registered function names.
    [[nodiscard]] auto function_names() const -> std::vector<std::string>;

    // Get list of registered template names.
    [[nodiscard]] auto template_names() const -> std::vector<std::string>;

private:
    EventBus& event_bus_;
    std::unordered_map<std::string, TemplateFuncSignature> functions_;
    std::unordered_map<std::string, std::vector<TemplateNode>> named_templates_;

    // Lexer: tokenize template source into text runs and tag content.
    struct Token
    {
        enum class Type : uint8_t
        {
            Text,
            Tag,
            Comment
        };
        Type type{Type::Text};
        std::string content;
        int line{0};
    };

    [[nodiscard]] auto tokenize(const std::string& source, const TemplateOptions& options) const
        -> std::expected<std::vector<Token>, std::string>;

    // Parser: convert tokens into AST nodes.
    [[nodiscard]] auto parse_tokens(const std::vector<Token>& tokens, size_t& pos) const
        -> std::expected<std::vector<TemplateNode>, std::string>;

    // Parse a single tag expression (e.g., ".var | upper | trim").
    [[nodiscard]] auto parse_expression(const std::string& expr) const -> TemplateNode;

    // Renderer: evaluate AST against context.
    auto render_nodes(const std::vector<TemplateNode>& nodes,
                      TemplateContext& context,
                      const TemplateOptions& options,
                      std::string& output,
                      std::vector<std::string>& warnings,
                      int depth) const -> void;

    // Evaluate a variable expression (dot access, pipeline).
    [[nodiscard]] auto evaluate_expression(const TemplateNode& node,
                                           const TemplateContext& context,
                                           std::vector<std::string>& warnings) const -> std::string;

    // Apply a pipeline function to a value.
    [[nodiscard]] auto apply_function(const std::string& func_name,
                                      const std::string& input,
                                      const std::vector<std::string>& args,
                                      std::vector<std::string>& warnings) const -> std::string;

    // Resolve a dot-path variable from context (e.g., ".title", ".meta.author").
    [[nodiscard]] auto resolve_variable(const std::string& path,
                                        const TemplateContext& context) const
        -> std::optional<std::string>;

    // Check if a context value is truthy for conditionals.
    [[nodiscard]] auto is_truthy(const std::string& path, const TemplateContext& context) const
        -> bool;

    // HTML-escape a string.
    [[nodiscard]] static auto html_escape(const std::string& input) -> std::string;

    // Trim whitespace from template tags if configured.
    [[nodiscard]] static auto trim_tag_whitespace(const std::string& text) -> std::string;
};

} // namespace markamp::core
