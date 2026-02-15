#pragma once

/// @file NoteTemplateEngine.h
/// @brief V4 Phase 14 – Note Templates with variable substitution.
///
/// Distinct from Phase 35's TemplateEngine (Sprig-compatible parser).
/// This handles note-level templates: loading from vault, built-in
/// variables ({{date}}, {{title}}), custom user variables, and
/// document creation from templates.

#include <chrono>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class VaultService;

/// Describes a single template variable.
struct NoteTemplateVariable
{
    std::string name;           ///< Variable name (e.g., "date")
    std::string default_value;  ///< Default value if not provided
    std::string description;    ///< For prompt UI
    bool requires_input{false}; ///< True = prompt user at insertion time
    bool is_builtin{false};     ///< True = system variable (date, time, etc.)

    enum class Type : uint8_t
    {
        String,
        Date,
        Choice,
        Number
    } type{Type::String};

    std::vector<std::string> choices; ///< For Type::Choice
};

/// Describes a complete note template.
struct NoteTemplateDefinition
{
    std::string name;
    std::string description;
    std::filesystem::path file_path;
    std::string content; ///< Raw template content with {{variables}}
    std::vector<NoteTemplateVariable> variables;
    std::string cursor_marker{"{{cursor}}"}; ///< Position cursor after insertion
    std::vector<std::string> tags;           ///< Tags to add to created documents

    [[nodiscard]] auto variable_names() const -> std::vector<std::string>;
};

/// Context for template variable resolution at insertion time.
struct NoteTemplateContext
{
    std::unordered_map<std::string, std::string> values; ///< variable_name -> value
    std::string document_title;
    std::string vault_name;
    std::chrono::system_clock::time_point timestamp{std::chrono::system_clock::now()};
};

/// Note template engine: load, apply, and manage vault templates.
class NoteTemplateEngine
{
public:
    NoteTemplateEngine(EventBus& event_bus, Config& config, VaultService& vault_service);

    /// Load all templates from the vault's templates folder.
    auto load_templates() -> void;

    /// List all available templates.
    [[nodiscard]] auto list_templates() const -> std::vector<NoteTemplateDefinition>;

    /// Get a template by name.
    [[nodiscard]] auto get_template(const std::string& name) const
        -> std::optional<NoteTemplateDefinition>;

    /// Apply a template with the given variable context.
    [[nodiscard]] auto apply(const NoteTemplateDefinition& tmpl,
                             const NoteTemplateContext& context) const -> std::string;

    /// Apply a template and create a new document from it via VaultService.
    [[nodiscard]] auto create_from_template(const std::string& template_name,
                                            const std::string& document_title,
                                            const NoteTemplateContext& context)
        -> std::expected<std::string, std::string>;

    /// Expand a template (without creating a document).
    [[nodiscard]] auto expand_template(const std::string& template_name,
                                       const NoteTemplateContext& context) const
        -> std::expected<std::string, std::string>;

    /// Register a custom variable provider.
    auto register_variable(const std::string& name, std::function<std::string()> provider) -> void;

    /// Parse template content to extract variable definitions.
    [[nodiscard]] auto parse_variables(const std::string& content) const
        -> std::vector<NoteTemplateVariable>;

    /// Create a new template file in the vault's templates folder.
    [[nodiscard]] auto create_template(const std::string& name,
                                       const std::string& content,
                                       const std::string& description = "")
        -> std::expected<void, std::string>;

    /// Delete a template file.
    [[nodiscard]] auto delete_template(const std::string& name) -> std::expected<void, std::string>;

private:
    EventBus& event_bus_;
    Config& config_;
    VaultService& vault_service_;

    std::vector<NoteTemplateDefinition> templates_;
    std::unordered_map<std::string, std::function<std::string()>> custom_variables_;

    [[nodiscard]] auto templates_folder() const -> std::filesystem::path;
    [[nodiscard]] auto resolve_variable(const std::string& name,
                                        const NoteTemplateContext& context) const -> std::string;
    [[nodiscard]] auto builtin_variable(const std::string& name,
                                        const NoteTemplateContext& context) const -> std::string;
};

} // namespace markamp::core
