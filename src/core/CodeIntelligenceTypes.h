#pragma once

/// @file CodeIntelligenceTypes.h
/// @brief V9 Phase 19 — Shared types for all code intelligence providers.
///
/// Mirrors VS Code's LSP types: completion items, hover content, location links,
/// code actions, semantic tokens, inlay hints, and folding ranges.

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Completion Types (Task 1)
// ============================================================================

/// The kind of a completion entry — mirrors VS Code's CompletionItemKind.
enum class CompletionKind : std::uint8_t
{
    kKeyword = 0,   ///< Markdown syntax keyword (e.g. `---`, `> [!NOTE]`)
    kSnippet = 1,   ///< Snippet from SnippetEngine
    kLink = 2,      ///< Wiki-link or URL from LinkSuggestionService
    kHeading = 3,   ///< Heading reference from OutlineService
    kTag = 4,       ///< Tag autocomplete (#tag)
    kEmoji = 5,     ///< Emoji shortcode (:smile:)
    kFile = 6,      ///< File path completion
    kFolder = 7,    ///< Folder path completion
    kProperty = 8,  ///< YAML frontmatter property
    kValue = 9,     ///< Property value
    kFunction = 10, ///< Template/macro function
    kConstant = 11  ///< Constant value
};

/// What triggered a completion request.
enum class CompletionTriggerKind : std::uint8_t
{
    kInvoked = 0,          ///< Explicitly invoked (Ctrl+Space)
    kTriggerCharacter = 1, ///< Triggered by a character (e.g. `[`, `#`, `:`)
    kIncomplete = 2        ///< Re-triggered for incomplete results
};

/// Context describing how a completion was triggered.
struct CompletionContext
{
    CompletionTriggerKind trigger_kind{CompletionTriggerKind::kInvoked};
    std::string
        trigger_character; ///< The character that triggered completion (if kTriggerCharacter)
};

/// A single completion item.
struct CompletionItem
{
    std::string label;         ///< Display text shown in the completion list
    std::string detail;        ///< Additional detail (e.g. file path, snippet description)
    std::string documentation; ///< Markdown documentation shown in the detail pane
    std::string insert_text;   ///< Text to insert when selected
    std::string filter_text;   ///< Text used for filtering (defaults to label if empty)
    std::string sort_text;     ///< Text used for sorting (defaults to label if empty)
    CompletionKind kind{CompletionKind::kKeyword};
    bool is_snippet{false}; ///< If true, insert_text contains tab stops ($1, $2, etc.)
    double score{0.0};      ///< Relevance score for ranking
};

/// A list of completion items with metadata.
struct CompletionList
{
    std::vector<CompletionItem> items;
    bool is_incomplete{false}; ///< If true, further typing should re-trigger completion
};

/// Bitmask for enabling/disabling completion sources.
enum class CompletionSource : std::uint16_t
{
    kNone = 0,
    kSnippets = 1 << 0,       ///< SnippetEngine completions
    kLinks = 1 << 1,          ///< LinkSuggestionService wiki-link completions
    kHeadings = 1 << 2,       ///< OutlineService heading completions
    kMarkdownSyntax = 1 << 3, ///< Markdown syntax completions (lists, callouts, fences)
    kEmoji = 1 << 4,          ///< Emoji shortcodes
    kTags = 1 << 5,           ///< Tag completions
    kFiles = 1 << 6,          ///< File path completions
    kProperties = 1 << 7,     ///< YAML frontmatter property completions
    kAll = 0xFFFF             ///< All sources enabled
};

/// Bitwise OR for CompletionSource.
inline auto operator|(CompletionSource lhs, CompletionSource rhs) -> CompletionSource
{
    return static_cast<CompletionSource>(static_cast<std::uint16_t>(lhs) |
                                         static_cast<std::uint16_t>(rhs));
}

/// Bitwise AND for CompletionSource.
inline auto operator&(CompletionSource lhs, CompletionSource rhs) -> CompletionSource
{
    return static_cast<CompletionSource>(static_cast<std::uint16_t>(lhs) &
                                         static_cast<std::uint16_t>(rhs));
}

// ============================================================================
// Hover Types (Task 2)
// ============================================================================

/// Content returned by a hover provider — markdown string with optional source range.
struct HoverContent
{
    std::string markdown; ///< Markdown-formatted hover content
    int start_line{0};    ///< Start line of the hovered element (0-based)
    int start_char{0};    ///< Start character of the hovered element
    int end_line{0};      ///< End line of the hovered element
    int end_char{0};      ///< End character of the hovered element

    [[nodiscard]] auto empty() const -> bool
    {
        return markdown.empty();
    }
};

// ============================================================================
// Signature Help Types (Task 2)
// ============================================================================

/// A single parameter in a signature.
struct ParameterInfo
{
    std::string label;         ///< Parameter label (e.g. "alt text")
    std::string documentation; ///< Markdown documentation for this parameter
};

/// A function/construct signature with parameters.
struct SignatureInfo
{
    std::string label;                     ///< Full signature label (e.g. "![alt](url \"title\")")
    std::string documentation;             ///< Markdown documentation for the signature
    std::vector<ParameterInfo> parameters; ///< Parameter information
    int active_parameter{0};               ///< Index of the currently active parameter
};

/// Result of a signature help request.
struct SignatureHelpResult
{
    std::vector<SignatureInfo> signatures;
    int active_signature{0}; ///< Index of the active signature
    bool has_result{false};  ///< Whether any signatures were found
};

// ============================================================================
// Go-To Definition Types (Task 2)
// ============================================================================

/// A location link — target file + range, with origin selection range.
struct LocationLink
{
    std::string target_uri;   ///< Target file URI or path
    int target_start_line{0}; ///< Target start line (0-based)
    int target_start_char{0}; ///< Target start character
    int target_end_line{0};   ///< Target end line
    int target_end_char{0};   ///< Target end character
    int origin_start_line{0}; ///< Origin selection start line
    int origin_start_char{0}; ///< Origin selection start character
    int origin_end_line{0};   ///< Origin selection end line
    int origin_end_char{0};   ///< Origin selection end character
};

// ============================================================================
// Code Action Types (Task 2)
// ============================================================================

/// The kind of a code action.
enum class CodeActionKind : std::uint8_t
{
    kQuickFix = 0,        ///< Fix a diagnostic
    kRefactor = 1,        ///< Refactoring action
    kRefactorExtract = 2, ///< Extract to new element
    kRefactorInline = 3,  ///< Inline an element
    kSource = 4,          ///< Source-level action (organize, fix all)
    kSourceFixAll = 5     ///< Fix all auto-fixable issues
};

/// A text edit within a code action.
struct CodeActionEdit
{
    int start_line{0};
    int start_char{0};
    int end_line{0};
    int end_char{0};
    std::string new_text; ///< Replacement text
};

/// A single code action (quick fix, refactoring, etc.).
struct CodeActionInfo
{
    std::string title; ///< Display title
    CodeActionKind kind{CodeActionKind::kQuickFix};
    std::vector<CodeActionEdit> edits; ///< Edits to apply
    std::string diagnostic_code;       ///< Associated diagnostic code (if any)
    bool is_preferred{false};          ///< Whether this is the preferred action
};

// ============================================================================
// Semantic Token Types (Task 2)
// ============================================================================

/// Semantic token type for markdown-aware highlighting.
enum class SemanticTokenType : std::uint8_t
{
    kHeading = 0,
    kLink = 1,
    kEmphasis = 2,
    kStrong = 3,
    kCode = 4,
    kBlockquote = 5,
    kListItem = 6,
    kFootnoteRef = 7,
    kImage = 8,
    kTag = 9,
    kFrontmatter = 10,
    kHtmlTag = 11,
    kComment = 12,
    kKeyword = 13,
    kString = 14,
    kNumber = 15
};

/// Modifier flags for semantic tokens.
enum class SemanticTokenModifier : std::uint8_t
{
    kNone = 0,
    kDeclaration = 1 << 0, ///< Definition site (heading definition, footnote definition)
    kReference = 1 << 1,   ///< Reference site (heading ref, footnote ref)
    kDeprecated = 1 << 2,  ///< Strikethrough / deprecated
    kReadonly = 1 << 3     ///< Read-only context (code block content)
};

/// Bitwise OR for SemanticTokenModifier.
inline auto operator|(SemanticTokenModifier lhs, SemanticTokenModifier rhs) -> SemanticTokenModifier
{
    return static_cast<SemanticTokenModifier>(static_cast<std::uint8_t>(lhs) |
                                              static_cast<std::uint8_t>(rhs));
}

/// Bitwise AND for SemanticTokenModifier.
inline auto operator&(SemanticTokenModifier lhs, SemanticTokenModifier rhs) -> SemanticTokenModifier
{
    return static_cast<SemanticTokenModifier>(static_cast<std::uint8_t>(lhs) &
                                              static_cast<std::uint8_t>(rhs));
}

/// A single semantic token (absolute positioning).
struct SemanticToken
{
    int line{0};       ///< Line number (0-based)
    int start_char{0}; ///< Start character (0-based)
    int length{0};     ///< Token length in characters
    SemanticTokenType token_type{SemanticTokenType::kCode};
    SemanticTokenModifier modifiers{SemanticTokenModifier::kNone};
};

// ============================================================================
// Inlay Hint Types (Task 2)
// ============================================================================

/// The kind of an inlay hint.
enum class InlayHintKind : std::uint8_t
{
    kWordCount = 0,    ///< Word count after headings
    kLinkTarget = 1,   ///< Shortened link target path
    kImageSize = 2,    ///< Image dimensions
    kTableColumns = 3, ///< Table column count
    kParameter = 4     ///< Parameter name hint
};

/// A single inlay hint displayed inline in the editor.
struct InlayHintInfo
{
    int line{0};       ///< Line number (0-based)
    int character{0};  ///< Character position (0-based)
    std::string label; ///< Display text
    InlayHintKind kind{InlayHintKind::kWordCount};
    bool padding_left{false};  ///< Add padding before the hint
    bool padding_right{false}; ///< Add padding after the hint
};

// ============================================================================
// Folding Range Types (Task 2)
// ============================================================================

/// The kind of a folding range.
enum class FoldingRangeKind : std::uint8_t
{
    kHeading = 0,     ///< Heading section fold
    kCodeFence = 1,   ///< Code fence block fold
    kFrontmatter = 2, ///< YAML frontmatter fold
    kHtmlBlock = 3,   ///< HTML block fold
    kComment = 4,     ///< Comment fold
    kList = 5,        ///< Nested list fold
    kTable = 6,       ///< Multi-line table fold
    kCollapsible = 7  ///< <details> collapsible section fold
};

/// A folding range in the document.
struct FoldingRange
{
    int start_line{0}; ///< Start line (0-based, inclusive)
    int end_line{0};   ///< End line (0-based, inclusive)
    FoldingRangeKind kind{FoldingRangeKind::kHeading};
    std::string collapsed_text; ///< Optional text shown when collapsed
};

// ============================================================================
// Document Highlight Types (Task 2)
// ============================================================================

/// Kind of document highlight.
enum class DocumentHighlightKind : std::uint8_t
{
    kText = 0, ///< Simple text highlight
    kRead = 1, ///< Read access highlight
    kWrite = 2 ///< Write access highlight
};

/// A highlighted range in the document.
struct DocumentHighlight
{
    int start_line{0};
    int start_char{0};
    int end_line{0};
    int end_char{0};
    DocumentHighlightKind kind{DocumentHighlightKind::kText};
};

// ============================================================================
// Workspace Symbol Types (Task 2)
// ============================================================================

/// Kind of workspace symbol.
enum class SymbolKind : std::uint8_t
{
    kHeading = 0,
    kLink = 1,
    kTag = 2,
    kFootnote = 3,
    kCodeBlock = 4,
    kImage = 5,
    kTable = 6
};

/// A symbol found in the workspace.
struct WorkspaceSymbol
{
    std::string name; ///< Symbol name/text
    SymbolKind kind{SymbolKind::kHeading};
    std::string container_name; ///< Parent heading/section name
    std::string uri;            ///< File URI containing the symbol
    int line{0};                ///< Line number (0-based)
    int character{0};           ///< Character position (0-based)
};

} // namespace markamp::core
