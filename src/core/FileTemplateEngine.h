/// @file FileTemplateEngine.h
/// @brief Phase 20 – File template engine for "New from Template" functionality.

#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;

// ============================================================================
// Data Structures
// ============================================================================

/// A variable placeholder within a template.
struct TemplateVariable
{
    std::string name;          ///< Variable name (used as {{name}} in template).
    std::string default_value; ///< Default value if not provided.
    std::string description;   ///< Human-readable description.
    bool required{false};      ///< Whether a value must be provided.
};

/// A file template definition.
struct FileTemplate
{
    std::string id;               ///< Unique template ID.
    std::string name;             ///< Display name.
    std::string description;      ///< Short description.
    std::string category;         ///< Category for grouping (e.g., "Document", "Notebook").
    std::string extension;        ///< Default file extension (e.g., "md", "ipynb").
    std::string content_template; ///< Template content with {{variable}} placeholders.
    std::vector<TemplateVariable> variables; ///< Variables used in this template.
    bool is_builtin{false};                  ///< Whether this is a built-in template.
};

// ============================================================================
// FileTemplateEngine
// ============================================================================

/// Provides "New from Template" functionality with variable substitution.
///
/// Ships with built-in templates for common file types and supports
/// user-defined templates.
///
/// Usage:
/// ```cpp
/// FileTemplateEngine engine(event_bus);
/// auto result = engine.create_from_template("blank-md", "/path/to/file.md", {{"title", "Hello"}});
/// ```
class FileTemplateEngine
{
public:
    explicit FileTemplateEngine(EventBus& event_bus);

    // ── Template Management ──

    /// Register a new template.
    void register_template(FileTemplate tmpl);

    /// Unregister a template by ID.
    auto unregister_template(const std::string& template_id) -> bool;

    /// List all registered templates.
    [[nodiscard]] auto list_templates() const -> std::vector<FileTemplate>;

    /// Find templates for a given file extension.
    [[nodiscard]] auto templates_for_extension(const std::string& ext) const
        -> std::vector<FileTemplate>;

    /// Search templates by name or description.
    [[nodiscard]] auto search_templates(const std::string& query) const
        -> std::vector<FileTemplate>;

    /// Find a template by ID.
    [[nodiscard]] auto find_template(const std::string& template_id) const -> const FileTemplate*;

    /// Total number of registered templates.
    [[nodiscard]] auto template_count() const -> std::size_t;

    // ── File Creation ──

    /// Create a file from a template with variable substitutions.
    [[nodiscard]] auto
    create_from_template(const std::string& template_id,
                         const std::string& output_path,
                         const std::unordered_map<std::string, std::string>& variables = {})
        -> std::expected<std::string, std::string>;

    // ── User Templates ──

    /// Add a user-defined template.
    auto add_user_template(const std::string& name,
                           const std::string& content,
                           const std::string& extension,
                           const std::string& category = "Custom") -> std::string;

    // ── Utilities ──

    /// Apply variable substitution to a content string.
    [[nodiscard]] static auto
    substitute_variables(const std::string& content,
                         const std::unordered_map<std::string, std::string>& variables)
        -> std::string;

    /// Get all categories.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Clear all templates.
    void clear();

private:
    EventBus& event_bus_;
    std::unordered_map<std::string, FileTemplate> templates_;
    int next_user_id_{1};

    /// Register built-in templates.
    void register_builtins();
};

} // namespace markamp::core
