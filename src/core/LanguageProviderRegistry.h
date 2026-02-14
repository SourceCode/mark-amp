#pragma once

#include <functional>
#include <memory>
#include <optional>
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

// ── Definition ──

/// A location in a document (file URI + range). Used for go-to-definition, references, etc.
struct Location
{
    std::string uri;
    DocumentRange range;
};

/// Interface for definition providers (mirrors VS Code's DefinitionProvider).
/// Enables "Go to Definition" (Ctrl+Click / F12).
class IDefinitionProvider
{
public:
    virtual ~IDefinitionProvider() = default;

    [[nodiscard]] virtual auto provide_definition(const std::string& document_uri,
                                                  DocumentPosition position)
        -> std::vector<Location> = 0;
};

// ── Reference ──

/// Interface for reference providers (mirrors VS Code's ReferenceProvider).
/// Enables "Find All References".
class IReferenceProvider
{
public:
    virtual ~IReferenceProvider() = default;

    /// If include_declaration is true, the declaration site is included in results.
    [[nodiscard]] virtual auto provide_references(const std::string& document_uri,
                                                  DocumentPosition position,
                                                  bool include_declaration)
        -> std::vector<Location> = 0;
};

// ── Document Symbol ──

/// Kind of a document symbol (mirrors VS Code's SymbolKind subset).
enum class SymbolKind
{
    kFile = 0,
    kModule = 1,
    kNamespace = 2,
    kPackage = 3,
    kClass = 4,
    kMethod = 5,
    kProperty = 6,
    kField = 7,
    kConstructor = 8,
    kEnum = 9,
    kInterface = 10,
    kFunction = 11,
    kVariable = 12,
    kConstant = 13,
    kString = 14,
    kNumber = 15,
    kBoolean = 16,
    kArray = 17,
    kObject = 18,
    kKey = 19,
    kNull = 20,
    kStruct = 22,
    kOperator = 24,
};

/// A symbol in a document (mirrors VS Code's DocumentSymbol).
/// Used for the Outline view and Breadcrumbs.
struct DocumentSymbol
{
    std::string name;
    std::string detail;
    SymbolKind kind{SymbolKind::kVariable};
    DocumentRange range;           // Full range including body
    DocumentRange selection_range; // Range of the symbol name itself
    std::vector<DocumentSymbol> children;
};

/// Interface for document symbol providers (mirrors VS Code's DocumentSymbolProvider).
class IDocumentSymbolProvider
{
public:
    virtual ~IDocumentSymbolProvider() = default;

    [[nodiscard]] virtual auto provide_document_symbols(const std::string& document_uri)
        -> std::vector<DocumentSymbol> = 0;
};

// ── Rename ──

/// Result of a rename preparation check.
struct RenameRange
{
    DocumentRange range;
    std::string placeholder; // Current symbol text
};

/// A workspace edit containing text changes across one or more files.
struct WorkspaceEdit
{
    /// Map of document URI → list of text edits.
    std::unordered_map<std::string, std::vector<TextEdit>> changes;
};

/// Interface for rename providers (mirrors VS Code's RenameProvider).
class IRenameProvider
{
public:
    virtual ~IRenameProvider() = default;

    /// Check whether rename is possible at the given position.
    [[nodiscard]] virtual auto prepare_rename(const std::string& document_uri,
                                              DocumentPosition position)
        -> std::optional<RenameRange> = 0;

    /// Compute all edits needed to rename the symbol to `new_name`.
    [[nodiscard]] virtual auto provide_rename_edits(const std::string& document_uri,
                                                    DocumentPosition position,
                                                    const std::string& new_name)
        -> WorkspaceEdit = 0;
};

// ── Folding Range ──

/// Kind of folding range (mirrors VS Code's FoldingRangeKind).
enum class FoldingRangeKind
{
    kComment,
    kImports,
    kRegion,
};

/// A foldable range in a document (mirrors VS Code's FoldingRange).
struct FoldingRange
{
    int start_line{0};
    int end_line{0};
    FoldingRangeKind kind{FoldingRangeKind::kRegion};
    std::string collapsed_text; // Optional label when collapsed
};

/// Interface for folding range providers (mirrors VS Code's FoldingRangeProvider).
class IFoldingRangeProvider
{
public:
    virtual ~IFoldingRangeProvider() = default;

    [[nodiscard]] virtual auto provide_folding_ranges(const std::string& document_uri)
        -> std::vector<FoldingRange> = 0;
};

// ── Signature Help ──

/// A single parameter label within a signature.
struct ParameterInformation
{
    std::string label;
    std::string documentation; // Markdown-formatted
};

/// A single function/method signature.
struct SignatureInformation
{
    std::string label;
    std::string documentation; // Markdown-formatted
    std::vector<ParameterInformation> parameters;
};

/// Signature help result (mirrors VS Code's SignatureHelp).
struct SignatureHelp
{
    std::vector<SignatureInformation> signatures;
    int active_signature{0};
    int active_parameter{0};
};

/// Interface for signature help providers (mirrors VS Code's SignatureHelpProvider).
class ISignatureHelpProvider
{
public:
    virtual ~ISignatureHelpProvider() = default;

    [[nodiscard]] virtual auto provide_signature_help(const std::string& document_uri,
                                                      DocumentPosition position)
        -> SignatureHelp = 0;
};

// ── Inlay Hints ──

/// Kind of inlay hint (mirrors VS Code's InlayHintKind).
enum class InlayHintKind
{
    kType = 1,
    kParameter = 2,
};

/// An inlay hint displayed inline in the editor (mirrors VS Code's InlayHint).
struct InlayHint
{
    DocumentPosition position;
    std::string label;
    InlayHintKind kind{InlayHintKind::kType};
    std::string tooltip;
    bool padding_left{false};
    bool padding_right{false};
};

/// Interface for inlay hints providers (mirrors VS Code's InlayHintsProvider).
class IInlayHintsProvider
{
public:
    virtual ~IInlayHintsProvider() = default;

    [[nodiscard]] virtual auto provide_inlay_hints(const std::string& document_uri,
                                                   DocumentRange visible_range)
        -> std::vector<InlayHint> = 0;
};

// ── Document Link ──

/// A clickable link in a document (mirrors VS Code's DocumentLink).
struct DocumentLink
{
    DocumentRange range;
    std::string target; // URI to navigate to
    std::string tooltip;
};

/// Interface for document link providers (mirrors VS Code's DocumentLinkProvider).
class IDocumentLinkProvider
{
public:
    virtual ~IDocumentLinkProvider() = default;

    [[nodiscard]] virtual auto provide_document_links(const std::string& document_uri)
        -> std::vector<DocumentLink> = 0;

    /// Optionally resolve additional info for a link (e.g. lazy target resolution).
    virtual auto resolve_document_link(DocumentLink link) -> DocumentLink
    {
        return link;
    }
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

    // ── Definition ──
    void register_definition_provider(const std::string& language_id,
                                      std::shared_ptr<IDefinitionProvider> provider);
    [[nodiscard]] auto get_definition_providers(const std::string& language_id) const
        -> std::vector<std::shared_ptr<IDefinitionProvider>>;

    // ── Reference ──
    void register_reference_provider(const std::string& language_id,
                                     std::shared_ptr<IReferenceProvider> provider);
    [[nodiscard]] auto get_reference_providers(const std::string& language_id) const
        -> std::vector<std::shared_ptr<IReferenceProvider>>;

    // ── Document Symbol ──
    void register_document_symbol_provider(const std::string& language_id,
                                           std::shared_ptr<IDocumentSymbolProvider> provider);
    [[nodiscard]] auto get_document_symbol_providers(const std::string& language_id) const
        -> std::vector<std::shared_ptr<IDocumentSymbolProvider>>;

    // ── Rename ──
    void register_rename_provider(const std::string& language_id,
                                  std::shared_ptr<IRenameProvider> provider);
    [[nodiscard]] auto get_rename_providers(const std::string& language_id) const
        -> std::vector<std::shared_ptr<IRenameProvider>>;

    // ── Folding Range ──
    void register_folding_range_provider(const std::string& language_id,
                                         std::shared_ptr<IFoldingRangeProvider> provider);
    [[nodiscard]] auto get_folding_range_providers(const std::string& language_id) const
        -> std::vector<std::shared_ptr<IFoldingRangeProvider>>;

    // ── Signature Help ──
    void register_signature_help_provider(const std::string& language_id,
                                          std::shared_ptr<ISignatureHelpProvider> provider);
    [[nodiscard]] auto get_signature_help_providers(const std::string& language_id) const
        -> std::vector<std::shared_ptr<ISignatureHelpProvider>>;

    // ── Inlay Hints ──
    void register_inlay_hints_provider(const std::string& language_id,
                                       std::shared_ptr<IInlayHintsProvider> provider);
    [[nodiscard]] auto get_inlay_hints_providers(const std::string& language_id) const
        -> std::vector<std::shared_ptr<IInlayHintsProvider>>;

    // ── Document Link ──
    void register_document_link_provider(const std::string& language_id,
                                         std::shared_ptr<IDocumentLinkProvider> provider);
    [[nodiscard]] auto get_document_link_providers(const std::string& language_id) const
        -> std::vector<std::shared_ptr<IDocumentLinkProvider>>;

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
    std::unordered_map<std::string, std::vector<std::shared_ptr<IDefinitionProvider>>>
        definition_providers_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<IReferenceProvider>>>
        reference_providers_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<IDocumentSymbolProvider>>>
        document_symbol_providers_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<IRenameProvider>>>
        rename_providers_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<IFoldingRangeProvider>>>
        folding_range_providers_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<ISignatureHelpProvider>>>
        signature_help_providers_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<IInlayHintsProvider>>>
        inlay_hints_providers_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<IDocumentLinkProvider>>>
        document_link_providers_;
};

} // namespace markamp::core
