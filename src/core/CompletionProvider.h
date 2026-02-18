#pragma once

/// @file CompletionProvider.h
/// @brief V9 Phase 19 — Unified completion provider aggregating multiple sources.
///
/// Combines SnippetEngine, LinkSuggestionService, OutlineService, and markdown
/// syntax completions into a single provider with configurable sources.

#include "CodeIntelligenceTypes.h"

#include <string>
#include <vector>

namespace markamp::core
{

// Forward declarations — avoid heavy includes
class SnippetEngine;
class OutlineService;

/// Unified completion provider that aggregates multiple completion sources.
///
/// Usage:
/// ```cpp
/// CompletionProvider provider(snippet_engine, outline_service);
/// auto context = CompletionContext{CompletionTriggerKind::kTriggerCharacter, "["};
/// auto list = provider.provide_completions("# Hello\n\nSome [[", 2, 7, context);
/// ```
class CompletionProvider
{
public:
    CompletionProvider(SnippetEngine& snippet_engine, OutlineService& outline_service);

    /// Provide completions at the given cursor position within the document.
    /// @param content Full document content
    /// @param line Cursor line (0-based)
    /// @param character Cursor character offset (0-based)
    /// @param context Trigger context
    /// @return CompletionList with ranked items
    [[nodiscard]] auto provide_completions(const std::string& content,
                                           int line,
                                           int character,
                                           const CompletionContext& context) const
        -> CompletionList;

    /// Provide signature help for markdown constructs (images, links).
    /// @param content Full document content
    /// @param line Cursor line (0-based)
    /// @param character Cursor character offset (0-based)
    /// @return SignatureHelpResult with parameter hints
    [[nodiscard]] auto provide_signature_help(const std::string& content,
                                              int line,
                                              int character) const -> SignatureHelpResult;

    /// Set the enabled completion sources (default: all).
    void set_enabled_sources(CompletionSource sources);

    /// Get the currently enabled sources.
    [[nodiscard]] auto enabled_sources() const -> CompletionSource;

    /// Maximum number of items to return per source.
    void set_max_items_per_source(int max_items);

private:
    SnippetEngine& snippet_engine_;
    OutlineService& outline_service_;
    CompletionSource enabled_sources_{CompletionSource::kAll};
    int max_items_per_source_{15};

    /// Extract the word/prefix being typed at cursor position.
    [[nodiscard]] static auto extract_prefix(const std::string& content, int line, int character)
        -> std::string;

    /// Get the current line text from content.
    [[nodiscard]] static auto get_line_text(const std::string& content, int line) -> std::string;

    /// Detect if cursor is inside a wiki-link ([[...]])
    [[nodiscard]] static auto is_in_wiki_link(const std::string& line_text, int character) -> bool;

    /// Detect if cursor is inside a heading reference (#...)
    [[nodiscard]] static auto is_heading_ref(const std::string& prefix) -> bool;

    /// Detect if cursor is inside an emoji shortcode (:...:)
    [[nodiscard]] static auto is_emoji_shortcode(const std::string& line_text, int character)
        -> bool;

    /// Check if a source is enabled.
    [[nodiscard]] auto is_source_enabled(CompletionSource source) const -> bool;

    // Source-specific completion generators
    [[nodiscard]] auto get_snippet_completions(const std::string& prefix) const
        -> std::vector<CompletionItem>;

    [[nodiscard]] auto get_heading_completions(const std::string& prefix,
                                               const std::string& content) const
        -> std::vector<CompletionItem>;

    [[nodiscard]] static auto get_markdown_syntax_completions(const std::string& prefix)
        -> std::vector<CompletionItem>;

    [[nodiscard]] static auto get_emoji_completions(const std::string& prefix)
        -> std::vector<CompletionItem>;
};

} // namespace markamp::core
