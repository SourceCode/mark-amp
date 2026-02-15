# Phase 40 — Multi-Format Import & AI Integration

## Objective

Implement the final phase of the SiYuan feature port, covering two complementary subsystems: multi-format import and AI-powered writing assistance. The import system brings external documents into the knowledgebase by parsing various file formats (Markdown, HTML, DOCX, EPUB, OPML, Org-mode, plain text, CSV, and SiYuan data packages) and converting them to the internal block model. The AI integration connects to LLM providers (OpenAI, Anthropic, or local models) to offer writing assistance features: summarization, translation, continue-writing, improve-writing, explain, outline generation, and custom prompts — accessible from the editor context menu and a dedicated AI chat panel.

**Import System:** The import pipeline follows a two-stage approach. First, format-specific importers convert the source file into intermediate Markdown (using built-in parsers for Markdown/HTML/text, Pandoc for DOCX/EPUB/Org-mode, and custom parsers for OPML/CSV). Second, the Markdown is parsed into the block tree using the existing Md4cWrapper and block model. During import, images and other embedded assets are extracted and copied to the `data/assets/` directory, and links are converted to block references where the target document already exists in the knowledgebase. The ImportService manages the full pipeline and publishes events for UI progress tracking.

**AI Integration:** The AIService provides a clean abstraction over LLM HTTP APIs, supporting both streaming and non-streaming responses. It handles authentication, request construction, response parsing, token counting, and error handling for OpenAI-compatible and Anthropic APIs. The AI chat panel (AIPanel) provides a sidebar conversation interface that maintains message history and automatically includes the current document's content as context. Pre-built AI actions (summarize, translate, continue, improve, explain, outline) are available from the editor's right-click context menu, each with a tailored system prompt. Users can also enter custom prompts for ad-hoc AI interactions.

## Prerequisites

- Phase 01 (Block Data Model Core)
- Phase 03 (Notebook Data Model)
- Phase 04 (SQLite Storage Layer)
- Phase 36 (Asset Management — for image/asset import handling)

## SiYuan Source Reference

**Import:**
- `kernel/model/import.go` — ImportStdMd (standard Markdown), ImportSY (SiYuan format), ImportData (full data package), Import (dispatcher)
- `kernel/model/render.go` — Markdown-to-block-tree conversion
- `kernel/treenode/` — Block tree construction from parsed content
- `app/src/dialog/processSystem.ts` — Import dialog and progress UI

**AI:**
- `kernel/model/ai.go` — ChatGPT, ChatGPTWithAction — LLM integration functions
- `kernel/conf/ai.go` — AIConf struct (OpenAI config: APIKey, APIModel, APIMaxTokens, APITimeout, APIProxy, APIBaseURL, APIProvider, APITemperature, APIMaxContexts, APIUserAgent)
- `app/src/ai/` — Frontend AI chat panel and action menus

## MarkAmp Integration Points

**Import:**
- New header: `src/core/ImportTypes.h`
- New header: `src/core/IImportFormat.h`
- New header: `src/core/ImportService.h`
- New source: `src/core/ImportService.cpp`
- New header: `src/core/MarkdownImporter.h`
- New source: `src/core/MarkdownImporter.cpp`
- New header: `src/core/HtmlImporter.h`
- New source: `src/core/HtmlImporter.cpp`
- New header: `src/core/PandocImporter.h`
- New source: `src/core/PandocImporter.cpp`

**AI:**
- New header: `src/core/AIService.h`
- New source: `src/core/AIService.cpp`
- New header: `src/core/AITypes.h`
- New header: `src/ui/AIPanel.h`
- New source: `src/ui/AIPanel.cpp`

- Extends `Events.h` with import and AI events
- Both services added to PluginContext for extension access

## Data Structures to Implement

```cpp
namespace markamp::core
{

// ════════════════════════════════════════════════════════════════
// Import System Data Structures
// ════════════════════════════════════════════════════════════════

// Supported import format identifiers.
enum class ImportFormatId : uint8_t
{
    Markdown,           // .md files
    HTML,               // .html/.htm files
    DOCX,               // .docx files (via Pandoc)
    EPUB,               // .epub files (via Pandoc)
    OPML,               // .opml files (outline format)
    OrgMode,            // .org files (via Pandoc)
    PlainText,          // .txt files
    CSV,                // .csv files (into attribute view)
    SiYuanPackage,      // .zip SiYuan data package
};

// Metadata about an import format.
struct ImportFormatInfo
{
    ImportFormatId id{ImportFormatId::Markdown};
    std::string format_id;                       // String ID (e.g., "markdown", "docx")
    std::string format_name;                     // Display name
    std::vector<std::string> file_extensions;    // Supported extensions (e.g., {"md", "markdown"})
    std::string description;                     // Brief description
    bool requires_pandoc{false};                 // Whether Pandoc is needed

    [[nodiscard]] auto accepts_extension(const std::string& ext) const -> bool;
};

// Options controlling import behavior.
struct ImportOptions
{
    bool import_assets{true};                    // Copy referenced images/files to assets
    bool convert_links_to_refs{true};            // Convert links to block refs where possible
    bool preserve_frontmatter{true};             // Keep YAML frontmatter as block attributes
    bool flatten_directory{false};               // Flatten directory structure (all docs to same level)
    std::string encoding{"UTF-8"};               // Source file encoding
};

// Record of a single imported document.
struct ImportedDoc
{
    std::string root_id;                         // Block ID of the imported document root
    std::string title;                           // Document title
    std::string source_path;                     // Original source file path
    int32_t block_count{0};                      // Number of blocks created
    int32_t asset_count{0};                      // Number of assets imported
};

// Result of an import operation.
struct ImportResult
{
    std::vector<ImportedDoc> imported_docs;       // Successfully imported documents
    int32_t total_blocks{0};                     // Total blocks created
    int32_t total_assets{0};                     // Total assets imported
    int64_t elapsed_ms{0};                       // Import time
    std::vector<std::string> errors;             // Files that failed to import
    std::vector<std::string> warnings;           // Non-fatal warnings

    [[nodiscard]] auto doc_count() const -> int
    {
        return static_cast<int>(imported_docs.size());
    }

    [[nodiscard]] auto has_errors() const -> bool
    {
        return !errors.empty();
    }

    [[nodiscard]] auto has_warnings() const -> bool
    {
        return !warnings.empty();
    }

    [[nodiscard]] auto succeeded() const -> bool
    {
        return !imported_docs.empty() && errors.empty();
    }
};

// IImportFormat — interface for format-specific import implementations.
class IImportFormat
{
public:
    virtual ~IImportFormat() = default;

    // Get metadata about this import format.
    [[nodiscard]] virtual auto format_info() const -> ImportFormatInfo = 0;

    // Import a single file into the target notebook.
    [[nodiscard]] virtual auto import_file(
        const std::filesystem::path& file_path,
        const std::string& target_notebook_id,
        const ImportOptions& options)
        -> std::expected<ImportResult, std::string> = 0;

    // Import all files in a directory into the target notebook.
    [[nodiscard]] virtual auto import_directory(
        const std::filesystem::path& dir_path,
        const std::string& target_notebook_id,
        const ImportOptions& options)
        -> std::expected<ImportResult, std::string> = 0;

    // Whether this importer is currently available (dependencies present).
    [[nodiscard]] virtual auto is_available() const -> bool = 0;
};

// ImportService — manages import format registry and orchestrates imports.
class ImportService
{
public:
    ImportService(EventBus& event_bus, Config& config);

    // Register a format importer.
    auto register_importer(std::unique_ptr<IImportFormat> importer) -> void;

    // Import a single file, auto-detecting format from extension.
    [[nodiscard]] auto import_file(
        const std::filesystem::path& file_path,
        const std::string& target_notebook_id,
        const ImportOptions& options = {})
        -> std::expected<ImportResult, std::string>;

    // Import a single file with explicit format.
    [[nodiscard]] auto import_file(
        const std::filesystem::path& file_path,
        ImportFormatId format,
        const std::string& target_notebook_id,
        const ImportOptions& options = {})
        -> std::expected<ImportResult, std::string>;

    // Import all supported files in a directory.
    [[nodiscard]] auto import_directory(
        const std::filesystem::path& dir_path,
        const std::string& target_notebook_id,
        const ImportOptions& options = {})
        -> std::expected<ImportResult, std::string>;

    // Get list of all available import formats.
    [[nodiscard]] auto get_available_formats() const
        -> std::vector<ImportFormatInfo>;

    // Detect the import format from a file extension.
    [[nodiscard]] auto detect_format(const std::filesystem::path& file_path) const
        -> std::optional<ImportFormatId>;

private:
    EventBus& event_bus_;
    Config& config_;
    std::unordered_map<std::string, std::unique_ptr<IImportFormat>> importers_;

    // Find the importer for a given format ID.
    [[nodiscard]] auto find_importer(ImportFormatId format) const
        -> IImportFormat*;

    // Register all built-in importers.
    auto register_builtins() -> void;

    // Convert ImportFormatId to string.
    [[nodiscard]] static auto format_id_to_string(ImportFormatId format)
        -> std::string;

    // Extract images from content, copy to assets dir, update references.
    [[nodiscard]] auto process_assets(
        const std::string& content,
        const std::filesystem::path& source_dir,
        const std::string& notebook_id)
        -> std::pair<std::string, int>;
};

// ════════════════════════════════════════════════════════════════
// AI Integration Data Structures
// ════════════════════════════════════════════════════════════════

// Supported AI provider types.
enum class AIProvider : uint8_t
{
    OpenAI,             // OpenAI API (GPT-4, GPT-3.5)
    Anthropic,          // Anthropic API (Claude)
    Local,              // Local model (Ollama, llama.cpp)
    Custom              // Custom OpenAI-compatible endpoint
};

// Role of a message in the conversation.
enum class AIMessageRole : uint8_t
{
    System,
    User,
    Assistant
};

// Pre-built AI actions available from the editor context menu.
enum class AIAction : uint8_t
{
    Summarize,          // Summarize selected text
    ContinueWriting,    // Continue writing from cursor position
    Translate,          // Translate text to target language
    ImproveWriting,     // Improve writing quality
    Explain,            // Explain selected text
    GenerateOutline,    // Generate document outline from topic
    FixGrammar,         // Fix grammar and spelling
    MakeShorter,        // Make text more concise
    MakeLonger,         // Expand text with more detail
    CustomPrompt        // User-supplied custom prompt
};

// A single message in an AI conversation.
struct AIMessage
{
    AIMessageRole role{AIMessageRole::User};
    std::string content;
    int64_t timestamp{0};                        // Unix ms when message was created

    [[nodiscard]] auto is_user() const -> bool
    {
        return role == AIMessageRole::User;
    }

    [[nodiscard]] auto is_assistant() const -> bool
    {
        return role == AIMessageRole::Assistant;
    }

    [[nodiscard]] auto is_system() const -> bool
    {
        return role == AIMessageRole::System;
    }
};

// Response from an AI provider.
struct AIResponse
{
    std::string content;                         // Response text
    std::string model;                           // Model that generated the response
    int32_t prompt_tokens{0};                    // Tokens in the prompt
    int32_t completion_tokens{0};                // Tokens in the response
    int32_t total_tokens{0};                     // Total tokens used
    int64_t elapsed_ms{0};                       // Response time
    bool success{false};
    std::string error_message;                   // Error if request failed
    bool truncated{false};                       // Whether response was cut off by max_tokens

    [[nodiscard]] auto tokens_used() const -> int
    {
        return total_tokens;
    }

    [[nodiscard]] auto is_error() const -> bool
    {
        return !success;
    }
};

// Configuration for AI provider connection.
struct AIConfig
{
    AIProvider provider{AIProvider::OpenAI};
    std::string api_key;                         // API key for authentication
    std::string api_endpoint;                    // API base URL
    std::string model;                           // Model name (e.g., "gpt-4", "claude-3-opus")
    double temperature{0.7};                     // Sampling temperature (0.0-2.0)
    int max_tokens{4096};                        // Maximum response tokens
    int timeout_seconds{60};                     // Request timeout
    std::string proxy;                           // HTTP proxy URL (optional)
    int max_context_messages{20};                // Max conversation history messages
    std::string user_agent{"MarkAmp/1.0"};       // HTTP User-Agent header

    // Default endpoints for each provider.
    [[nodiscard]] auto default_endpoint() const -> std::string;
};

// Options for an AI request.
struct AIRequestOptions
{
    std::string system_prompt;                   // System message (for the AI's role/instructions)
    double temperature{-1.0};                    // Override config temperature (-1 = use default)
    int max_tokens{-1};                          // Override config max_tokens (-1 = use default)
    bool stream{false};                          // Enable streaming response
    std::string target_language;                 // For translate action
};

// Callback for streaming AI responses.
using AIStreamCallback = std::function<void(const std::string& token, bool is_final)>;

// AIService — provides LLM-powered writing assistance.
class AIService
{
public:
    AIService(EventBus& event_bus, Config& config);

    // Send a chat conversation to the AI provider and get a response.
    [[nodiscard]] auto chat(const std::vector<AIMessage>& messages,
                             const AIRequestOptions& options = {})
        -> std::expected<AIResponse, std::string>;

    // Send a chat with streaming response via callback.
    [[nodiscard]] auto chat_stream(const std::vector<AIMessage>& messages,
                                    AIStreamCallback callback,
                                    const AIRequestOptions& options = {})
        -> std::expected<AIResponse, std::string>;

    // Pre-built action: summarize the given text.
    [[nodiscard]] auto summarize(const std::string& text)
        -> std::expected<std::string, std::string>;

    // Pre-built action: translate text to the target language.
    [[nodiscard]] auto translate(const std::string& text,
                                  const std::string& target_language)
        -> std::expected<std::string, std::string>;

    // Pre-built action: continue writing from the given context.
    [[nodiscard]] auto continue_writing(const std::string& context)
        -> std::expected<std::string, std::string>;

    // Pre-built action: improve writing quality.
    [[nodiscard]] auto improve_writing(const std::string& text)
        -> std::expected<std::string, std::string>;

    // Pre-built action: generate an outline for a topic.
    [[nodiscard]] auto generate_outline(const std::string& topic)
        -> std::expected<std::string, std::string>;

    // Pre-built action: explain the given text.
    [[nodiscard]] auto explain(const std::string& text)
        -> std::expected<std::string, std::string>;

    // Execute a custom AI action with user-provided prompt.
    [[nodiscard]] auto custom_action(const std::string& prompt,
                                      const std::string& context)
        -> std::expected<std::string, std::string>;

    // Check if the AI service is configured and ready.
    [[nodiscard]] auto is_configured() const -> bool;

    // Test connectivity to the AI provider.
    [[nodiscard]] auto test_connection()
        -> std::expected<void, std::string>;

    // Get the current AI configuration.
    [[nodiscard]] auto ai_config() const -> const AIConfig&;

    // Get the system prompt for a pre-built action.
    [[nodiscard]] static auto system_prompt_for_action(AIAction action)
        -> std::string;

private:
    EventBus& event_bus_;
    Config& config_;
    AIConfig ai_config_;

    // Build the HTTP request body for the configured provider.
    [[nodiscard]] auto build_request_body(
        const std::vector<AIMessage>& messages,
        const AIRequestOptions& options) const -> std::string;

    // Parse the HTTP response body from the configured provider.
    [[nodiscard]] auto parse_response(const std::string& response_body) const
        -> std::expected<AIResponse, std::string>;

    // Send an HTTP POST request to the AI provider.
    [[nodiscard]] auto send_request(const std::string& url,
                                     const std::string& body,
                                     const std::string& api_key) const
        -> std::expected<std::string, std::string>;

    // Send an HTTP POST with streaming response.
    [[nodiscard]] auto send_streaming_request(
        const std::string& url,
        const std::string& body,
        const std::string& api_key,
        AIStreamCallback callback) const
        -> std::expected<AIResponse, std::string>;

    // Execute a pre-built action with the given text/context.
    [[nodiscard]] auto execute_action(AIAction action,
                                       const std::string& text,
                                       const AIRequestOptions& options = {})
        -> std::expected<std::string, std::string>;

    // Load AI configuration from Config.
    auto load_config() -> void;
};

} // namespace markamp::core
```

## Key Functions to Implement

**Import System (8 functions):**

1. `ImportService::register_builtins()` — Create and register MarkdownImporter, HtmlImporter, PlainTextImporter (built-in). If Pandoc available, register PandocImporter instances for DOCX, EPUB, OrgMode. Register OPMLImporter and CSVImporter (built-in custom parsers). Register SiYuanPackageImporter (ZIP extraction + block import).
2. `ImportService::import_file()` (auto-detect) — Detect format from file extension using `detect_format()`. If unrecognized, return error. Delegate to the format-overloaded `import_file()`.
3. `ImportService::import_file()` (explicit format) — Find the importer. Call `importer->import_file()`. After import, if `import_assets` enabled, call `process_assets()` to copy images and update references. Publish ImportCompletedEvent. Return ImportResult.
4. `ImportService::import_directory()` — Scan directory for all files with recognized extensions. Sort by path for deterministic order. Import each file sequentially. Aggregate results. Handle errors per-file (continue on failure). Publish ImportCompletedEvent with aggregate stats.
5. `MarkdownImporter::import_file()` — Read file contents (handle encoding detection). Parse markdown using Md4cWrapper to build AST. Convert AST to Block tree (Document block with child blocks). Assign block IDs. Store blocks via SQLite storage. Return ImportResult with imported doc info.
6. `HtmlImporter::import_file()` — Read HTML file. Strip `<script>` and `<style>` tags. Convert HTML to markdown using a simple HTML-to-MD converter (handle `<h1>`-`<h6>`, `<p>`, `<a>`, `<img>`, `<ul>/<ol>/<li>`, `<pre><code>`, `<em>`, `<strong>`, `<blockquote>`, `<table>`). Delegate to MarkdownImporter for block creation.
7. `PandocImporter::import_file()` — Run `pandoc input_file -f <format> -t markdown -o temp.md`. Read the resulting markdown. Delegate to MarkdownImporter for block creation. Clean up temp file.
8. `ImportService::process_assets()` — Scan markdown content for image references (`![](path)`, `<img src="path">`). For each, check if the path is relative to the source directory. Copy the image to `data/assets/<notebook>/`. Update the markdown content with the new asset path. Return (updated_content, assets_copied_count).

**AI System (7 functions):**

9. `AIService::chat()` — Build request body with messages formatted for the configured provider. Send HTTP POST to the API endpoint. Parse response. Publish AIResponseReceivedEvent. Return AIResponse.
10. `AIService::chat_stream()` — Send HTTP POST with `stream: true`. Read SSE (Server-Sent Events) lines. For each `data:` line, parse the delta token and call the streaming callback. Accumulate full response. Return final AIResponse with token counts.
11. `AIService::build_request_body()` — For OpenAI/Custom: JSON with `model`, `messages` array, `temperature`, `max_tokens`, `stream`. For Anthropic: JSON with `model`, `messages` array (separate system from user), `max_tokens`, `temperature`.
12. `AIService::parse_response()` — For OpenAI: extract `choices[0].message.content`, `usage.prompt_tokens`, `usage.completion_tokens`. For Anthropic: extract `content[0].text`, `usage.input_tokens`, `usage.output_tokens`.
13. `AIService::system_prompt_for_action()` — Return tailored system prompts: Summarize="Provide a concise summary...", ContinueWriting="Continue writing naturally from...", Translate="Translate the following text to {lang}...", ImproveWriting="Improve the following text for clarity...", Explain="Explain the following text in simple terms...", GenerateOutline="Generate a detailed outline for...".
14. `AIService::execute_action()` — Build a message list with the system prompt for the action and the user text as the user message. Call `chat()`. Return the response content.
15. `AIPanel` constructor — Create wxTextCtrl for message input (multiline, with Enter-to-send). Create wxScrolledWindow for conversation history display. Create wxChoice for action selector (Summarize, Translate, etc.). Wire event handlers for send button and action execution. Display messages as alternating user/assistant bubbles with timestamps.

## Events to Add (in Events.h)

```cpp
// ============================================================================
// Import events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImportStartedEvent)
std::string format_id;
std::string source_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImportCompletedEvent)
std::string format_id;
int32_t docs_imported{0};
int32_t blocks_created{0};
int32_t assets_imported{0};
int64_t elapsed_ms{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImportFailedEvent)
std::string format_id;
std::string source_path;
std::string error_message;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImportProgressEvent)
std::string current_file;
int progress_percent{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(ImportDialogRequestEvent);

// ============================================================================
// AI events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIRequestStartedEvent)
std::string action;
std::string model;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIResponseReceivedEvent)
std::string content;
int32_t tokens_used{0};
int64_t elapsed_ms{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIStreamTokenEvent)
std::string token;
bool is_final{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIErrorEvent)
std::string error_message;
std::string action;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(AIConfigChangedEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIActionRequestEvent)
AIAction action{AIAction::CustomPrompt};
std::string selected_text;
std::string custom_prompt;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

**Import:**
- `knowledgebase.import.default_encoding` — Default encoding for imported files (default: `"UTF-8"`)
- `knowledgebase.import.convert_links` — Convert links to block refs during import (default: `true`)
- `knowledgebase.import.import_assets` — Copy referenced assets during import (default: `true`)
- `knowledgebase.import.pandoc_path` — Path to Pandoc for import (shares with export if set; default: `""`)

**AI:**
- `knowledgebase.ai.provider` — AI provider: `"openai"`, `"anthropic"`, `"local"`, `"custom"` (default: `"openai"`)
- `knowledgebase.ai.api_key` — API key for the AI provider (default: `""`)
- `knowledgebase.ai.endpoint` — Custom API endpoint URL (default: `""` = use provider default)
- `knowledgebase.ai.model` — Model name (default: `"gpt-4"`)
- `knowledgebase.ai.temperature` — Sampling temperature 0.0-2.0 (default: `0.7`)
- `knowledgebase.ai.max_tokens` — Maximum response tokens (default: `4096`)
- `knowledgebase.ai.timeout_seconds` — Request timeout in seconds (default: `60`)
- `knowledgebase.ai.proxy` — HTTP proxy URL for API requests (default: `""`)
- `knowledgebase.ai.max_context_messages` — Max conversation history length (default: `20`)

## Test Cases (Catch2)

**Import Test Cases** — File: `tests/unit/test_import_service.cpp`

1. **Markdown import creates blocks** — Import a `.md` file with a heading, 2 paragraphs, and a code block. Verify ImportResult: doc_count == 1, total_blocks >= 4. Verify the imported document has the correct title from the heading.
2. **HTML import converts to blocks** — Import an `.html` file with `<h1>`, `<p>`, `<ul>`, `<a>`. Verify blocks are created. Verify heading text matches `<h1>` content. Verify list items are imported.
3. **Import with image asset processing** — Import a `.md` file containing `![alt](image.png)` where `image.png` exists alongside the markdown file. Verify image is copied to `data/assets/`. Verify block content references the new asset path.
4. **Import directory imports multiple files** — Create a temp directory with 3 `.md` files. Call `import_directory()`. Verify ImportResult: doc_count == 3. Verify each file produced an ImportedDoc.
5. **Format auto-detection** — Verify `detect_format()` returns correct ImportFormatId for: `.md` -> Markdown, `.html` -> HTML, `.txt` -> PlainText, `.csv` -> CSV, `.opml` -> OPML. Verify `.xyz` returns nullopt.
6. **CSV import creates attribute view** — Import a `.csv` file with headers "Name,Age,City" and 3 data rows. Verify an attribute view block is created. Verify the data can be queried.
7. **Import handles encoding** — Import a UTF-8 markdown file with Unicode characters (Chinese, emoji, accented characters). Verify all characters are preserved in the imported blocks.
8. **Import error handling** — Attempt to import a non-existent file. Verify error is returned. Attempt to import a binary file (e.g., `.exe`). Verify error or warning is returned. Verify other files in a directory import continue despite one failure.
9. **Pandoc importer builds correct command** — Mock Pandoc execution. Verify PandocImporter for DOCX builds `pandoc input.docx -f docx -t markdown -o temp.md`. Verify output markdown is fed to MarkdownImporter.
10. **Import progress events** — Import a directory with 5 files. Subscribe to ImportProgressEvent. Verify at least 5 progress events received with increasing progress_percent.

**AI Test Cases** — File: `tests/unit/test_ai_service.cpp`

1. **Chat produces response** — Configure AIService with a mock HTTP handler that returns a valid OpenAI JSON response. Call `chat()` with a simple message. Verify AIResponse: success == true, content is non-empty, tokens_used > 0.
2. **Streaming chat delivers tokens** — Configure mock to return SSE stream with 5 tokens. Call `chat_stream()` with a callback. Verify callback was called 5 times with individual tokens plus 1 final call with is_final=true.
3. **Summarize action uses correct prompt** — Call `summarize("Long text here...")`. Verify the system prompt passed to chat contains "summary" or "summarize". Verify a response is returned.
4. **Translate action includes target language** — Call `translate("Hello world", "Spanish")`. Verify the prompt includes "Spanish". Verify response is returned.
5. **System prompts are correct** — Verify `system_prompt_for_action()` returns non-empty strings for all AIAction values. Verify Summarize prompt contains "summar". Verify Translate prompt contains "translat". Verify Explain prompt contains "explain".
6. **Error handling for invalid API key** — Configure mock to return HTTP 401. Call `chat()`. Verify AIResponse: success == false, error_message contains "unauthorized" or "authentication".
7. **Request body format for OpenAI** — Configure provider as OpenAI. Build request body. Parse as JSON. Verify keys: "model", "messages" (array), "temperature", "max_tokens". Verify messages array has role/content objects.
8. **Request body format for Anthropic** — Configure provider as Anthropic. Build request body. Parse as JSON. Verify keys: "model", "messages" (array), "max_tokens". Verify system message is separated from user messages per Anthropic API format.

## Acceptance Criteria

**Import:**
- [ ] Markdown importer correctly parses `.md` files into block trees
- [ ] HTML importer converts HTML tags to blocks
- [ ] Pandoc-based importers work for DOCX, EPUB, Org-mode when Pandoc is installed
- [ ] Image assets are copied to `data/assets/` during import
- [ ] CSV import creates attribute view blocks
- [ ] Directory import handles multiple files with per-file error isolation
- [ ] Import progress events published during long imports

**AI:**
- [ ] AIService sends correctly formatted requests to OpenAI and Anthropic APIs
- [ ] Streaming responses deliver tokens incrementally via callback
- [ ] Pre-built actions (summarize, translate, continue, improve, explain, outline) use appropriate prompts
- [ ] AIPanel provides chat interface with conversation history
- [ ] AI actions accessible from editor context menu
- [ ] Error handling covers authentication failures, timeouts, and malformed responses
- [ ] All 18 test cases pass (10 import + 8 AI)

## Files to Create/Modify

**Import:**
- CREATE: `src/core/ImportTypes.h`
- CREATE: `src/core/IImportFormat.h`
- CREATE: `src/core/ImportService.h`
- CREATE: `src/core/ImportService.cpp`
- CREATE: `src/core/MarkdownImporter.h`
- CREATE: `src/core/MarkdownImporter.cpp`
- CREATE: `src/core/HtmlImporter.h`
- CREATE: `src/core/HtmlImporter.cpp`
- CREATE: `src/core/PandocImporter.h`
- CREATE: `src/core/PandocImporter.cpp`
- CREATE: `src/core/OPMLImporter.h`
- CREATE: `src/core/OPMLImporter.cpp`
- CREATE: `src/core/CSVImporter.h`
- CREATE: `src/core/CSVImporter.cpp`

**AI:**
- CREATE: `src/core/AITypes.h`
- CREATE: `src/core/AIService.h`
- CREATE: `src/core/AIService.cpp`
- CREATE: `src/ui/AIPanel.h`
- CREATE: `src/ui/AIPanel.cpp`

**Shared Modifications:**
- MODIFY: `src/core/Events.h` (add ImportStartedEvent, ImportCompletedEvent, ImportFailedEvent, ImportProgressEvent, ImportDialogRequestEvent, AIRequestStartedEvent, AIResponseReceivedEvent, AIStreamTokenEvent, AIErrorEvent, AIConfigChangedEvent, AIActionRequestEvent)
- MODIFY: `src/core/PluginContext.h` (add `ImportService*` and `AIService*` pointers)
- MODIFY: `src/app/MarkAmpApp.cpp` (instantiate ImportService and AIService, wire to PluginContext)
- MODIFY: `src/ui/MainFrame.cpp` (add AI panel to sidebar, add import/AI menu items, add AI actions to editor context menu)
- MODIFY: `src/CMakeLists.txt` (add all new .cpp files to source list; add libcurl for AI HTTP requests)
- CREATE: `tests/unit/test_import_service.cpp`
- CREATE: `tests/unit/test_ai_service.cpp`
- MODIFY: `tests/CMakeLists.txt` (add test_import_service and test_ai_service targets)
