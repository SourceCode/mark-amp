#pragma once

/// @file CodeActionProvider.h
/// @brief V9 Phase 19 + V13 Phase 29 — Code actions (quick fixes and refactoring) for markdown.
///
/// Provides context-aware code actions including diagnostic quick-fixes,
/// heading hierarchy fixes, link conversions, extract operations, and
/// an extension provider registration API.

#include "CodeActionTypes.h"
#include "CodeIntelligenceTypes.h"
#include "DiagnosticsService.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Callback type for extension-registered code action providers.
/// Takes (content, start_line, start_char, end_line, end_char, document_uri)
/// and returns a vector of CodeActionInfo.
using ExtensionActionProvider =
    std::function<std::vector<CodeActionInfo>(const std::string& content,
                                              int start_line,
                                              int start_char,
                                              int end_line,
                                              int end_char,
                                              const std::string& document_uri)>;

/// Provider for code actions (quick fixes, refactoring) in markdown documents.
///
/// V13 Phase 29 additions:
/// - `provide_action_set()` returns categorized `CodeActionSet`
/// - `register_provider()` allows extensions to contribute custom actions
/// - Built-in: organize links, format document, remove unused references
///
/// Usage:
/// ```cpp
/// CodeActionProvider provider(diagnostics_service);
/// auto set = provider.provide_action_set(content, 5, 0, 5, 40, "file.md");
/// if (set.has_preferred()) { /* auto-apply */ }
/// ```
class CodeActionProvider
{
public:
    explicit CodeActionProvider(DiagnosticsService& diagnostics_service);

    /// V9: Provide code actions for the given range (flat list).
    [[nodiscard]] auto provide_actions(const std::string& content,
                                       int start_line,
                                       int start_char,
                                       int end_line,
                                       int end_char,
                                       const std::string& document_uri = "") const
        -> std::vector<CodeActionInfo>;

    /// V13 Phase 29: Provide a categorized set of code actions.
    [[nodiscard]] auto provide_action_set(const std::string& content,
                                          int start_line,
                                          int start_char,
                                          int end_line,
                                          int end_char,
                                          const std::string& document_uri = "") const
        -> CodeActionSet;

    /// Register an extension-provided code action provider.
    /// @param provider_id Unique identifier for the provider
    /// @param provider Callback that returns code actions for a given range
    void register_provider(const std::string& provider_id, ExtensionActionProvider provider);

    /// Unregister a previously registered extension provider.
    void unregister_provider(const std::string& provider_id);

    /// Get the number of registered extension providers.
    [[nodiscard]] auto extension_provider_count() const -> size_t;

    /// Check if a provider with the given ID is registered.
    [[nodiscard]] auto has_provider(const std::string& provider_id) const -> bool
    {
        return extension_providers_.contains(provider_id);
    }

    /// Number of distinct code action kinds (QuickFix, Refactor, RefactorExtract, Source, SourceFixAll).
    [[nodiscard]] static constexpr auto total_action_kinds() noexcept -> int
    {
        return 5;
    }

private:
    DiagnosticsService& diagnostics_service_;

    /// Extension-registered action providers.
    std::unordered_map<std::string, ExtensionActionProvider> extension_providers_;

    /// Generate quick-fix actions from diagnostics in the range.
    [[nodiscard]] auto
    get_diagnostic_fixes(int start_line, int end_line, const std::string& document_uri) const
        -> std::vector<CodeActionInfo>;

    /// Generate "extract to heading" action if text is selected.
    [[nodiscard]] static auto get_extract_heading_action(
        const std::string& content, int start_line, int start_char, int end_line, int end_char)
        -> std::vector<CodeActionInfo>;

    /// Generate heading hierarchy fix if skip-level detected.
    [[nodiscard]] static auto get_heading_hierarchy_fixes(const std::string& content, int line)
        -> std::vector<CodeActionInfo>;

    /// Generate inline-to-reference link conversion.
    [[nodiscard]] static auto
    get_link_conversion_actions(const std::string& line_text, int line, const std::string& content)
        -> std::vector<CodeActionInfo>;

    /// Generate trailing whitespace removal action.
    [[nodiscard]] static auto get_trailing_whitespace_action(const std::string& line_text, int line)
        -> std::vector<CodeActionInfo>;

    /// V13 Phase 29: Organize reference links (sort alphabetically, remove unused).
    [[nodiscard]] static auto get_organize_links_action(const std::string& content)
        -> std::vector<CodeActionInfo>;

    /// V13 Phase 29: Format entire document (normalize spacing, fences, lists).
    [[nodiscard]] static auto get_format_document_action(const std::string& content)
        -> std::vector<CodeActionInfo>;

    /// V13 Phase 29: Remove unused reference link definitions.
    [[nodiscard]] static auto get_remove_unused_references_action(const std::string& content)
        -> std::vector<CodeActionInfo>;

    /// Get the line text at a given line number.
    [[nodiscard]] static auto get_line_text(const std::string& content, int line) -> std::string;

    /// Count existing reference links in the document.
    [[nodiscard]] static auto count_reference_links(const std::string& content) -> int;
};

} // namespace markamp::core
