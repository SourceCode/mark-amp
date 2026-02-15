# Phase 14 -- Templates System with Variables

## Objective

Implement a full template engine: create, manage, and apply templates with dynamic variable substitution. Templates are Markdown files in a designated templates folder. Variables like `{{date}}`, `{{title}}`, `{{author}}`, and custom user-defined variables are replaced at insertion time. Templates support cursor placement markers and prompt-for-input variables.

## Prerequisites

- Phase 02 (VaultService -- document creation)
- Phase 09 (DailyNoteService -- uses templates)
- Existing CommandPalette, Config

## Feature References (PRD)

- PRD #13: Templates
- PRD #40: Note Templates with Variables

## Data Structures to Implement

### File: `src/core/TemplateEngine.h`

```cpp
#pragma once

#include <chrono>
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

struct TemplateVariable
{
    std::string name;               // Variable name (e.g., "date")
    std::string default_value;      // Default if not provided
    std::string description;        // For prompt UI
    bool requires_input{false};     // True = prompt user at insertion time
    bool is_builtin{false};         // True = system variable (date, time, etc.)

    enum class Type : uint8_t { String, Date, Choice, Number } type{Type::String};
    std::vector<std::string> choices;  // For Type::Choice
};

struct TemplateDefinition
{
    std::string name;
    std::string description;
    std::filesystem::path file_path;
    std::string content;            // Raw template content with {{variables}}
    std::vector<TemplateVariable> variables;
    std::string cursor_marker;      // Position cursor after insertion (e.g., "{{cursor}}")
    std::vector<std::string> tags;  // Tags to add to created documents

    [[nodiscard]] auto variable_names() const -> std::vector<std::string>;
};

struct TemplateContext
{
    std::unordered_map<std::string, std::string> values;  // variable_name -> value
    std::string document_title;
    std::string vault_name;
    std::chrono::system_clock::time_point timestamp;
};

class TemplateEngine
{
public:
    TemplateEngine(EventBus& event_bus, Config& config, VaultService& vault_service);

    /// Load all templates from the templates folder.
    auto load_templates() -> void;

    /// List all available templates.
    [[nodiscard]] auto list_templates() const -> std::vector<TemplateDefinition>;

    /// Get a template by name.
    [[nodiscard]] auto get_template(const std::string& name) const
        -> std::optional<TemplateDefinition>;

    /// Apply a template with the given variable context.
    /// Returns the expanded content with all variables replaced.
    [[nodiscard]] auto apply(const TemplateDefinition& tmpl,
                              const TemplateContext& context) const
        -> std::string;

    /// Apply a template and create a new document from it.
    [[nodiscard]] auto create_from_template(const std::string& template_name,
                                             const std::string& document_title,
                                             const TemplateContext& context)
        -> std::expected<std::string, std::string>;

    /// Insert a template into the current document at cursor position.
    [[nodiscard]] auto expand_template(const std::string& template_name,
                                        const TemplateContext& context) const
        -> std::expected<std::string, std::string>;

    /// Register a custom variable provider.
    auto register_variable(const std::string& name,
                            std::function<std::string()> provider) -> void;

    /// Parse template content to extract variable definitions.
    [[nodiscard]] auto parse_variables(const std::string& content) const
        -> std::vector<TemplateVariable>;

    /// Create a new template from content.
    [[nodiscard]] auto create_template(const std::string& name,
                                        const std::string& content,
                                        const std::string& description = "")
        -> std::expected<void, std::string>;

    /// Delete a template.
    [[nodiscard]] auto delete_template(const std::string& name)
        -> std::expected<void, std::string>;

private:
    EventBus& event_bus_;
    Config& config_;
    VaultService& vault_service_;

    std::vector<TemplateDefinition> templates_;
    std::unordered_map<std::string, std::function<std::string()>> custom_variables_;

    [[nodiscard]] auto templates_folder() const -> std::filesystem::path;
    [[nodiscard]] auto resolve_variable(const std::string& name,
                                         const TemplateContext& context) const -> std::string;
    [[nodiscard]] auto builtin_variable(const std::string& name,
                                         const TemplateContext& context) const -> std::string;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`load_templates()`** -- Scan templates folder for .md files. For each, read content, parse variables, build TemplateDefinition. Store in templates_ vector.

2. **`apply(tmpl, context)`** -- Replace all `{{variable}}` patterns in template content. For each variable: check context.values first, then custom_variables_, then builtin. Remove `{{cursor}}` marker and record its position. Return expanded content.

3. **`builtin_variable(name, context)`** -- Handle built-in variables:
   - `{{date}}` -> current date (YYYY-MM-DD)
   - `{{time}}` -> current time (HH:MM)
   - `{{datetime}}` -> full datetime
   - `{{title}}` -> document title from context
   - `{{author}}` -> from config
   - `{{vault}}` -> vault name
   - `{{yesterday}}` -> yesterday's date
   - `{{tomorrow}}` -> tomorrow's date
   - `{{day}}` -> day name (Monday, etc.)

4. **`parse_variables(content)`** -- Regex scan for `{{variable_name}}` and `{{variable_name:default_value}}` patterns. Extract variable names and default values. Detect special syntax: `{{input:Description}}` for prompted variables, `{{choice:opt1|opt2|opt3}}` for choices.

5. **`create_from_template(template_name, title, context)`** -- Get template. Apply variables. Create document via VaultService. Return document ID.

6. **`register_variable(name, provider)`** -- Register a custom variable that plugins can provide. Stored in custom_variables_ map.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TemplateAppliedEvent)
std::string template_name;
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TemplateInsertRequestEvent)
std::string template_name;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `templates.folder` | string | `"_templates"` | Templates folder name in vault |
| `templates.default_author` | string | `""` | Default author name for {{author}} |
| `templates.date_format` | string | `"%Y-%m-%d"` | Date format for {{date}} |
| `templates.time_format` | string | `"%H:%M"` | Time format for {{time}} |

## Test Cases

File: `tests/unit/test_template_engine.cpp`

1. **Load templates** -- Templates folder with 3 .md files. Verify 3 templates loaded.
2. **Apply simple variables** -- Template: `# {{title}}\nDate: {{date}}`. Verify both replaced.
3. **Built-in date** -- `{{date}}` produces current date in configured format.
4. **Built-in time** -- `{{time}}` produces current time.
5. **Default values** -- `{{status:draft}}` with no context value. Verify "draft" used.
6. **Custom variable** -- Register "project" variable. Template uses `{{project}}`. Verify resolved.
7. **Cursor placement** -- Template with `{{cursor}}`. Verify marker removed from output.
8. **Parse variables** -- Template with 5 different variables. parse_variables() returns all 5.
9. **Input variable** -- `{{input:Enter description}}` parsed with requires_input=true.
10. **Choice variable** -- `{{choice:draft|published|archived}}` parsed with 3 choices.
11. **Create from template** -- Create document from template. Verify document exists with expanded content.
12. **Missing variable** -- Template uses `{{unknown}}`. Verify it's kept as-is or replaced with empty string.

## Acceptance Criteria

- [ ] Templates loaded from vault templates folder
- [ ] Built-in variables (date, time, title, author) resolve correctly
- [ ] Custom user variables with default values work
- [ ] Input-prompted variables are detected during parsing
- [ ] Choice variables offer selection options
- [ ] Cursor marker identifies post-insertion cursor position
- [ ] Create-from-template produces a new document with expanded content
- [ ] Template CRUD (create, list, delete) works
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/TemplateEngine.h` | TemplateEngine, TemplateDefinition, TemplateVariable |
| CREATE | `src/core/TemplateEngine.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 2 template events |
| MODIFY | `src/core/PluginContext.h` | Add `TemplateEngine* template_engine{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add TemplateEngine.cpp |
| CREATE | `tests/unit/test_template_engine.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_template_engine target |

## Architecture Notes

- Templates are stored as plain Markdown files in the vault (portable, version-controllable)
- The variable syntax `{{name}}` is chosen to avoid conflict with Markdown syntax
- DailyNoteService (Phase 09) can use TemplateEngine for daily note templates
- CommandPalette integration: "Insert Template" command lists templates in palette
- Constructor injection: TemplateEngine(EventBus&, Config&, VaultService&)

## Estimated Complexity

**M** -- Variable parsing, built-in resolution, template CRUD, 12 tests.
