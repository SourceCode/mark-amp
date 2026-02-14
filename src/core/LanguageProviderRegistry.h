#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Document position for language features.
struct DocumentPosition
{
    int line{0};
    int character{0};
};

/// Document range for language features.
struct DocumentRange
{
    DocumentPosition start;
    DocumentPosition end;
};

// ── Completion ──

/// A completion item (mirrors VS Code's CompletionItem).
struct CompletionItem
{
    std::string label;
    std::string detail;
    std::string documentation;
    std::string insert_text;
    std::string sort_text;
    std::string filter_text;
    int kind{0}; // CompletionItemKind enum value
};

/// Result of a completion request.
struct CompletionList
{
    std::vector<CompletionItem> items;
    bool is_incomplete{false};
};

/// Interface for completion providers (mirrors VS Code's CompletionItemProvider).
class ICompletionProvider
{
public:
    virtual ~ICompletionProvider() = default;

    [[nodiscard]] virtual auto provide_completions(const std::string& document_uri,
                                                   DocumentPosition position) -> CompletionList = 0;
};

// ── Hover ──

/// Hover information (mirrors VS Code's Hover).
struct HoverInfo
{
    std::string contents; // Markdown content
    DocumentRange range;
    bool has_content{false};
};

/// Interface for hover providers (mirrors VS Code's HoverProvider).
class IHoverProvider
{
public:
    virtual ~IHoverProvider() = default;

    [[nodiscard]] virtual auto provide_hover(const std::string& document_uri,
                                             DocumentPosition position) -> HoverInfo = 0;
};

// ── Code Action ──

/// A code action (quick fix, refactor, etc.) (mirrors VS Code's CodeAction).
struct CodeAction
{
    std::string title;
    std::string kind; // "quickfix", "refactor", "source"
    std::string command;
    bool is_preferred{false};
};

/// Context for code action requests.
struct CodeActionContext
{
    std::vector<std::string> diagnostics; // Associated diagnostic codes
    std::vector<std::string> only;        // Filter by kind
};

/// Interface for code action providers (mirrors VS Code's CodeActionProvider).
class ICodeActionProvider
{
public:
    virtual ~ICodeActionProvider() = default;

    [[nodiscard]] virtual auto provide_code_actions(const std::string& document_uri,
                                                    DocumentRange range,
                                                    const CodeActionContext& context)
        -> std::vector<CodeAction> = 0;
};

// ── Document Formatting ──

/// Formatting options.
struct FormattingOptions
{
    int tab_size{4};
    bool insert_spaces{true};
};

/// A text edit resulting from formatting.
struct TextEdit
{
    DocumentRange range;
    std::string new_text;
};

/// Interface for document formatting providers (mirrors VS Code's DocumentFormattingEditProvider).
class IDocumentFormattingProvider
{
public:
    virtual ~IDocumentFormattingProvider() = default;

    [[nodiscard]] virtual auto provide_formatting(const std::string& document_uri,
                                                  const FormattingOptions& options)
        -> std::vector<TextEdit> = 0;
};

// ── Unified Registry ──

/// Registry for all language feature providers, keyed by language ID.
class LanguageProviderRegistry
{
public:
    LanguageProviderRegistry() = default;

    // ── Completion ──
    void register_completion_provider(const std::string& language_id,
                                      std::shared_ptr<ICompletionProvider> provider);
    [[nodiscard]] auto get_completion_providers(const std::string& language_id) const
        -> std::vector<std::shared_ptr<ICompletionProvider>>;

    // ── Hover ──
    void register_hover_provider(const std::string& language_id,
                                 std::shared_ptr<IHoverProvider> provider);
    [[nodiscard]] auto get_hover_providers(const std::string& language_id) const
        -> std::vector<std::shared_ptr<IHoverProvider>>;

    // ── Code Action ──
    void register_code_action_provider(const std::string& language_id,
                                       std::shared_ptr<ICodeActionProvider> provider);
    [[nodiscard]] auto get_code_action_providers(const std::string& language_id) const
        -> std::vector<std::shared_ptr<ICodeActionProvider>>;

    // ── Document Formatting ──
    void register_formatting_provider(const std::string& language_id,
                                      std::shared_ptr<IDocumentFormattingProvider> provider);
    [[nodiscard]] auto get_formatting_providers(const std::string& language_id) const
        -> std::vector<std::shared_ptr<IDocumentFormattingProvider>>;

    // ── Utility ──
    [[nodiscard]] auto registered_languages() const -> std::vector<std::string>;

private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<ICompletionProvider>>>
        completion_providers_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<IHoverProvider>>> hover_providers_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<ICodeActionProvider>>>
        code_action_providers_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<IDocumentFormattingProvider>>>
        formatting_providers_;
};

} // namespace markamp::core
