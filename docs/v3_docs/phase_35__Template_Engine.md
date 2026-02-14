# Phase 35 — Template Engine (Sprig-Compatible)

## Objective

Implement a template engine for document templates, porting SiYuan's template system that uses Go-template-like syntax with Sprig function support. Templates are markdown files stored in a `data/templates/` directory that contain placeholder expressions such as `{{.now | date "2006-01-02"}}`, `{{.title}}`, and `{{random 6}}`. When a user inserts a template, the engine evaluates all expressions against a set of context variables (current date/time, document title, notebook name, etc.) and produces rendered markdown that is inserted into the document as new blocks.

The template system consists of two main components. The TemplateEngine class handles parsing and evaluating template expressions, supporting a Sprig-compatible function library with date/time manipulation, string operations, math functions, list operations, conditional logic, and random/UUID generation. The TemplateService class manages the template file lifecycle — listing available templates, creating new templates from scratch or by saving an existing document as a template, searching templates by name, and orchestrating the rendering and insertion workflow.

This phase brings significant usability value by allowing users to create reusable document scaffolds — meeting notes, project plans, daily journals, code review checklists — with dynamic content that is populated at insertion time. The template syntax is intentionally compatible with Go's `text/template` package and the Sprig function library, ensuring familiarity for users coming from SiYuan, Hugo, Helm, or other Go-template-based tools.

## Prerequisites

- Phase 01 (Block Data Model Core)
- Phase 03 (Notebook Data Model)

## SiYuan Source Reference

- `kernel/model/template.go` — RenderTemplate, SearchTemplate, DocSaveAsTemplate, RemoveTemplate functions
- `kernel/util/template.go` — Template rendering utilities, Sprig function bindings
- `app/src/dialog/processSystem.ts` — Template insertion UI in the frontend
- `kernel/treenode/` — Block tree construction from rendered template content
- Sprig function reference: https://masterminds.github.io/sprig/ — canonical list of template functions

## MarkAmp Integration Points

- New header: `src/core/TemplateEngine.h`
- New source: `src/core/TemplateEngine.cpp`
- New header: `src/core/TemplateService.h`
- New source: `src/core/TemplateService.cpp`
- New header: `src/core/TemplateTypes.h`
- Extends `Events.h` with template events
- Template files stored in `<workspace>/data/templates/` directory
- Integrates with Block model (Phase 01) for template-to-block conversion
- TemplateService added to PluginContext for extension access

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Metadata about a template file.
struct TemplateInfo
{
    std::string name;                            // Template name (derived from filename)
    std::filesystem::path path;                  // Full path to the template file
    std::string preview;                         // First 200 chars of rendered content
    int64_t created{0};                          // File creation time (Unix ms)
    int64_t updated{0};                          // File modification time (Unix ms)
    int64_t size_bytes{0};                       // File size in bytes

    [[nodiscard]] auto formatted_date() const -> std::string;
    [[nodiscard]] auto formatted_size() const -> std::string;
};

// Error information from template validation.
struct TemplateError
{
    int line{0};                                 // Line number (1-based) of the error
    int column{0};                               // Column number (1-based) of the error
    std::string message;                         // Human-readable error description
    std::string context;                         // The problematic template fragment

    [[nodiscard]] auto formatted() const -> std::string;
};

// Result of a template rendering operation.
struct TemplateRenderResult
{
    std::string rendered_content;                // Fully rendered markdown output
    std::vector<TemplateError> warnings;         // Non-fatal warnings during render
    int64_t elapsed_ms{0};                       // Rendering time

    [[nodiscard]] auto has_warnings() const -> bool
    {
        return !warnings.empty();
    }

    [[nodiscard]] auto is_empty() const -> bool
    {
        return rendered_content.empty();
    }
};

// Token types for the template lexer.
enum class TemplateTokenType : uint8_t
{
    Text,          // Literal text outside {{ }}
    ExprStart,     // {{ opening delimiter
    ExprEnd,       // }} closing delimiter
    Identifier,    // Variable name or function name (e.g., .title, now, date)
    StringLiteral, // Quoted string argument (e.g., "2006-01-02")
    IntLiteral,    // Integer argument (e.g., 6)
    FloatLiteral,  // Float argument (e.g., 3.14)
    Pipe,          // | pipe operator
    Dot,           // . property access
    If,            // if keyword
    Else,          // else keyword
    End,           // end keyword
    Range,         // range keyword
    With,          // with keyword
};

// A single token from the template lexer.
struct TemplateToken
{
    TemplateTokenType type{TemplateTokenType::Text};
    std::string value;
    int line{1};
    int column{1};
};

// Context variables available during template rendering.
struct TemplateContext
{
    std::chrono::system_clock::time_point now;    // Current time
    std::string title;                            // Current document title
    std::string id;                               // New block ID for insertion
    std::string notebook;                         // Current notebook name
    std::string workspace;                        // Current workspace path
    std::string username;                         // Current OS username
    std::unordered_map<std::string, std::string> custom_vars;  // User-defined variables

    // Resolve a dotted variable name to its value.
    [[nodiscard]] auto resolve(const std::string& var_name) const
        -> std::optional<std::string>;
};

// Type alias for template function handlers.
// Functions take a vector of string arguments and return a string result.
using TemplateFuncHandler = std::function<std::string(const std::vector<std::string>&)>;

// TemplateEngine — parses and evaluates template expressions.
class TemplateEngine
{
public:
    TemplateEngine();

    // Render a template string with the given context variables.
    [[nodiscard]] auto render(const std::string& template_content,
                               const TemplateContext& context)
        -> std::expected<TemplateRenderResult, std::string>;

    // Validate a template string without rendering.
    // Returns errors found during parsing.
    [[nodiscard]] auto validate(const std::string& template_content)
        -> std::vector<TemplateError>;

    // Register a custom template function.
    auto register_function(const std::string& name,
                            TemplateFuncHandler handler) -> void;

    // Check if a function is registered.
    [[nodiscard]] auto has_function(const std::string& name) const -> bool;

    // List all registered function names.
    [[nodiscard]] auto list_functions() const -> std::vector<std::string>;

private:
    std::unordered_map<std::string, TemplateFuncHandler> functions_;

    // Register all built-in Sprig-compatible functions.
    auto register_builtins() -> void;

    // Tokenize template content into a token stream.
    [[nodiscard]] auto tokenize(const std::string& content) const
        -> std::expected<std::vector<TemplateToken>, TemplateError>;

    // Evaluate a pipe-separated expression chain.
    [[nodiscard]] auto evaluate_expression(
        const std::vector<TemplateToken>& tokens,
        const TemplateContext& context) const
        -> std::expected<std::string, TemplateError>;

    // Evaluate a single function call with arguments.
    [[nodiscard]] auto call_function(const std::string& func_name,
                                      const std::vector<std::string>& args) const
        -> std::expected<std::string, TemplateError>;

    // ── Built-in Sprig function implementations ──

    // Date/time functions
    [[nodiscard]] static auto fn_now(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_date(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_date_modify(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_date_in_zone(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_duration(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_to_date(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_unix_epoch(const std::vector<std::string>& args) -> std::string;

    // String functions
    [[nodiscard]] static auto fn_trim(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_upper(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_lower(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_title(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_repeat(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_substr(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_replace(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_contains(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_has_prefix(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_has_suffix(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_quote(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_squote(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_nospace(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_indent(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_nindent(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_abbrev(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_wrap(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_split_list(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_join(const std::vector<std::string>& args) -> std::string;

    // Math functions
    [[nodiscard]] static auto fn_add(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_sub(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_mul(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_div(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_mod(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_max(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_min(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_ceil(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_floor(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_round(const std::vector<std::string>& args) -> std::string;

    // Logic functions
    [[nodiscard]] static auto fn_default(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_empty(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_coalesce(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_ternary(const std::vector<std::string>& args) -> std::string;

    // List functions
    [[nodiscard]] static auto fn_list(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_first(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_last(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_rest(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_initial(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_append(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_prepend(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_concat(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_reverse(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_uniq(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_sort_alpha(const std::vector<std::string>& args) -> std::string;

    // Utility functions
    [[nodiscard]] static auto fn_uuid(const std::vector<std::string>& args) -> std::string;
    [[nodiscard]] static auto fn_random(const std::vector<std::string>& args) -> std::string;
};

// TemplateService — manages template files and orchestrates rendering.
class TemplateService
{
public:
    TemplateService(EventBus& event_bus, Config& config,
                    TemplateEngine& engine);

    // Initialize the templates directory.
    [[nodiscard]] auto initialize(const std::filesystem::path& workspace_path)
        -> std::expected<void, std::string>;

    // List all available templates.
    [[nodiscard]] auto list_templates() const -> std::vector<TemplateInfo>;

    // Get the raw content of a template file.
    [[nodiscard]] auto get_template(const std::filesystem::path& path) const
        -> std::expected<std::string, std::string>;

    // Render a template file with the given context.
    [[nodiscard]] auto render_template(const std::filesystem::path& path,
                                        const TemplateContext& context)
        -> std::expected<TemplateRenderResult, std::string>;

    // Create a new template file.
    [[nodiscard]] auto create_template(const std::string& name,
                                        const std::string& content)
        -> std::expected<TemplateInfo, std::string>;

    // Delete a template file.
    [[nodiscard]] auto delete_template(const std::filesystem::path& path)
        -> std::expected<void, std::string>;

    // Save an existing document as a template.
    [[nodiscard]] auto save_doc_as_template(const std::string& root_id,
                                             const std::string& template_name)
        -> std::expected<TemplateInfo, std::string>;

    // Search templates by name (case-insensitive substring match).
    [[nodiscard]] auto search_templates(const std::string& query) const
        -> std::vector<TemplateInfo>;

    // Get the templates directory path.
    [[nodiscard]] auto templates_dir() const -> std::filesystem::path;

private:
    EventBus& event_bus_;
    Config& config_;
    TemplateEngine& engine_;
    std::filesystem::path templates_dir_;

    // Build TemplateInfo from a file path.
    [[nodiscard]] auto build_template_info(const std::filesystem::path& path) const
        -> std::expected<TemplateInfo, std::string>;

    // Build default TemplateContext for the current state.
    [[nodiscard]] auto build_default_context() const -> TemplateContext;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. `TemplateEngine::render()` — Scan the template content for `{{ }}` delimiters. For each expression block, tokenize the expression, evaluate it against the TemplateContext (resolving variables, applying pipe-chained functions), and replace the `{{ }}` block with the result. Concatenate all literal text and evaluated expressions. Record elapsed time. Return TemplateRenderResult.
2. `TemplateEngine::validate()` — Tokenize the entire template. Check for unmatched `{{ }}` delimiters, unknown function names, incorrect argument counts, unterminated string literals. Return a vector of TemplateError for each issue found.
3. `TemplateEngine::register_builtins()` — Register all 50+ Sprig-compatible functions into the functions_ map. Group by category: date/time (7), string (19), math (10), logic (4), list (11), utility (2).
4. `TemplateEngine::tokenize()` — Scan the template string character by character. Outside `{{ }}` delimiters, emit Text tokens. Inside delimiters, parse identifiers, string literals (handling escape sequences), integer/float literals, pipe operators, and dot accessors. Track line and column numbers for error reporting.
5. `TemplateEngine::evaluate_expression()` — Parse a sequence of tokens as a pipe chain: `value | func1 arg1 | func2 arg2`. Resolve the initial value (variable lookup or literal), then pass it through each piped function as the last argument.
6. `TemplateContext::resolve()` — Map `.now` to formatted current time, `.title` to the document title, `.id` to the block ID, `.notebook` to notebook name, `.workspace` to workspace path, `.username` to OS username. Check custom_vars for unknown names. Return nullopt if not found.
7. `fn_date()` — Format a time value using a Go-style date layout string (reference time: Mon Jan 2 15:04:05 MST 2006). Convert Go layout tokens to C++ chrono format specifiers.
8. `fn_random()` — Generate a random alphanumeric string of the specified length. Use `std::mt19937` seeded with `std::random_device`.
9. `fn_uuid()` — Generate a v4 UUID string using random bytes formatted as `xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx`.
10. `TemplateService::render_template()` — Read the template file content, build a TemplateContext with current state, call engine_.render(), publish TemplateInsertedEvent on success.
11. `TemplateService::save_doc_as_template()` — Load the document's markdown content by root_id, write it to a new file in the templates directory with the given name, return TemplateInfo.
12. `TemplateService::search_templates()` — List all templates, filter by case-insensitive substring match on the template name. Return matching TemplateInfo records.
13. `TemplateService::list_templates()` — Scan the templates directory for `.md` files. Build TemplateInfo for each. Sort by name ascending.
14. `TemplateService::initialize()` — Create `data/templates/` directory if missing. Store the path. Use `std::error_code` overloads for filesystem operations.

## Events to Add (in Events.h)

```cpp
// ============================================================================
// Template events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TemplateInsertedEvent)
std::string template_path;
std::string root_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TemplateCreatedEvent)
std::string template_name;
std::string template_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TemplateDeletedEvent)
std::string template_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TemplateRenderErrorEvent)
std::string template_path;
std::string error_message;
int line{0};
int column{0};
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

- `knowledgebase.templates.path` — Relative path to templates directory within workspace (default: `"data/templates/"`)
- `knowledgebase.templates.date_format` — Default date format string for `{{.now}}` rendering (default: `"YYYY-MM-DD"`)
- `knowledgebase.templates.time_format` — Default time format string (default: `"HH:mm:ss"`)
- `knowledgebase.templates.auto_preview` — Show preview when browsing templates (default: `true`)

## Test Cases (Catch2)

File: `tests/unit/test_template_engine.cpp`

1. **Render simple variable substitution** — Template `"Hello {{.title}}"` with title="World". Verify output is `"Hello World"`. Verify elapsed_ms >= 0 and no warnings.
2. **Date functions** — Template `"{{.now | date \"2006-01-02\"}}"`. Verify output matches today's date in YYYY-MM-DD format. Test `dateModify` with "+24h" adds one day.
3. **String functions** — Template `"{{"hello" | upper}}"` produces `"HELLO"`. Test `lower`, `title`, `trim`, `replace`, `contains`, `repeat`, `substr`. Verify each produces expected output.
4. **Math functions** — Template `"{{add 3 5}}"` produces `"8"`. Test `sub`, `mul`, `div`, `mod`, `max`, `min`, `ceil`, `floor`, `round` with various numeric inputs.
5. **Pipe chaining** — Template `"{{"  HELLO  " | trim | lower}}"` produces `"hello"`. Template `"{{.title | upper | repeat 2}}"` with title="hi" produces `"HIHI"`.
6. **Variable resolution** — Verify `.now`, `.title`, `.id`, `.notebook`, `.workspace`, `.username` all resolve to non-empty strings. Verify unknown variable `.nonexistent` produces empty string or error.
7. **Invalid template syntax error** — Template `"{{unclosed"` returns error with line/column info. Template `"{{unknownFunc}}"` returns error about unknown function. Template `"{{add "not_a_number" 5}}"` returns error about invalid argument.
8. **List templates from directory** — Create 3 template files in a temp directory. Call `list_templates()`. Verify 3 TemplateInfo records with correct names, paths, and sizes.
9. **Save document as template** — Create a mock document with markdown content. Call `save_doc_as_template()`. Verify template file is created in templates directory. Verify reading it back returns the original markdown.
10. **Template insertion into document** — Render a template that produces markdown with a heading and paragraph. Verify rendered_content contains valid markdown. Verify TemplateInsertedEvent is published with correct template_path and root_id.

## Acceptance Criteria

- [ ] TemplateEngine parses `{{ }}` delimited expressions correctly
- [ ] All 50+ Sprig-compatible functions are registered and functional
- [ ] Pipe chaining evaluates functions left-to-right, passing result as last argument
- [ ] TemplateContext resolves `.now`, `.title`, `.id`, `.notebook`, `.workspace`, `.username`
- [ ] TemplateService manages template files in `data/templates/` directory
- [ ] Template validation catches syntax errors with line/column positions
- [ ] Save-as-template extracts document markdown and stores it as a template file
- [ ] All 10 test cases pass
- [ ] Uses `[[nodiscard]]` on all query and render methods
- [ ] Uses `std::error_code` overloads for all filesystem operations

## Files to Create/Modify

- CREATE: `src/core/TemplateTypes.h`
- CREATE: `src/core/TemplateEngine.h`
- CREATE: `src/core/TemplateEngine.cpp`
- CREATE: `src/core/TemplateService.h`
- CREATE: `src/core/TemplateService.cpp`
- MODIFY: `src/core/Events.h` (add TemplateInsertedEvent, TemplateCreatedEvent, TemplateDeletedEvent, TemplateRenderErrorEvent)
- MODIFY: `src/core/PluginContext.h` (add `TemplateEngine*` and `TemplateService*` pointers)
- MODIFY: `src/app/MarkAmpApp.cpp` (instantiate TemplateEngine and TemplateService, wire to PluginContext)
- MODIFY: `src/CMakeLists.txt` (add TemplateEngine.cpp, TemplateService.cpp to source list)
- CREATE: `tests/unit/test_template_engine.cpp`
- MODIFY: `tests/CMakeLists.txt` (add test_template_engine target)
