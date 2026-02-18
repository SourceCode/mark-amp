// ============================================================================
// File: src/core/ExportTemplateEngine.h
// Phase 24: Export & Publishing — Customizable export template engine
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Data structures
// ============================================================================

/// Variable definition for template substitution.
struct TemplateVariable
{
    std::string name;          ///< Variable name (e.g., "title")
    std::string default_value; ///< Default if not provided
    std::string description;   ///< Help text
    bool required{false};      ///< Must be supplied by caller
};

/// Built-in template identifiers.
enum class BuiltinTemplate : uint8_t
{
    kArticle, ///< Clean article layout
    kReport,  ///< Academic / business report
    kSlides,  ///< Slide-deck style
    kResume,  ///< Resume / CV layout
    kMinimal  ///< Bare-bones, no chrome
};

/// A registered export template.
struct ExportTemplate
{
    std::string template_id;                 ///< Unique identifier
    std::string name;                        ///< Display name
    std::string html_template;               ///< HTML with {{variable}} placeholders
    std::string css;                         ///< Associated stylesheet
    std::vector<TemplateVariable> variables; ///< Declared variables
    bool is_builtin{false};                  ///< True for shipped defaults
};

/// Validation result for template syntax.
struct TemplateValidationResult
{
    bool valid{true};
    std::vector<std::string> errors;
    std::vector<std::string> unresolved_variables; ///< Variables in template not in registry
};

/// Render context — values to substitute into a template.
using TemplateContext = std::unordered_map<std::string, std::string>;

// ============================================================================
// ExportTemplateEngine
// ============================================================================

/// ExportTemplateEngine — manages and renders export templates with
/// mustache-style `{{variable}}` substitution.
class ExportTemplateEngine
{
public:
    ExportTemplateEngine();

    // ----- Template registry ------------------------------------------------

    /// Register a custom template. Returns false if id already exists.
    auto register_template(ExportTemplate tpl) -> bool;

    /// Retrieve a template by id. Returns nullptr if not found.
    [[nodiscard]] auto get_template(const std::string& template_id) const -> const ExportTemplate*;

    /// List all registered template ids and names.
    [[nodiscard]] auto list_templates() const -> std::vector<ExportTemplate>;

    /// Remove a template by id. Built-in templates cannot be removed.
    auto remove_template(const std::string& template_id) -> bool;

    /// Register the shipped default templates (article, report, slides, resume, minimal).
    auto load_builtin_templates() -> void;

    // ----- Rendering --------------------------------------------------------

    /// Render a template by id, substituting context variables into the
    /// HTML template and wrapping with CSS.  Returns rendered HTML string or
    /// error message.
    [[nodiscard]] auto render(const std::string& template_id,
                              const std::string& content_html,
                              const TemplateContext& context = {}) const -> std::string;

    /// Render using a template object directly.
    [[nodiscard]] auto render_template(const ExportTemplate& tpl,
                                       const std::string& content_html,
                                       const TemplateContext& context = {}) const -> std::string;

    // ----- Validation -------------------------------------------------------

    /// Validate a template for syntax errors and unresolved variables.
    [[nodiscard]] auto validate_template(const ExportTemplate& tpl) const
        -> TemplateValidationResult;

    /// Extract all `{{variable}}` names found in a template string.
    [[nodiscard]] static auto extract_variables(const std::string& html_template)
        -> std::vector<std::string>;

    // ----- Built-in template helpers ----------------------------------------

    /// Get the builtin template for a given identifier.
    [[nodiscard]] auto builtin_template(BuiltinTemplate bt) const -> const ExportTemplate*;

    /// Return the template_id string for a BuiltinTemplate enum value.
    [[nodiscard]] static auto builtin_id(BuiltinTemplate bt) -> std::string;

private:
    std::unordered_map<std::string, ExportTemplate> templates_;

    /// Substitute `{{key}}` placeholders in text with values from context.
    [[nodiscard]] static auto substitute(const std::string& text, const TemplateContext& context)
        -> std::string;

    /// Generate CSS for a built-in template.
    [[nodiscard]] static auto builtin_css(BuiltinTemplate bt) -> std::string;

    /// Generate HTML skeleton for a built-in template.
    [[nodiscard]] static auto builtin_html(BuiltinTemplate bt) -> std::string;
};

} // namespace markamp::core
