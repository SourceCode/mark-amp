#pragma once

/// @file CodeActionProvider.h
/// @brief V9 Phase 19 — Code actions (quick fixes and refactoring) for markdown.
///
/// Provides context-aware code actions including diagnostic quick-fixes,
/// heading hierarchy fixes, link conversions, and extract operations.

#include "CodeIntelligenceTypes.h"
#include "DiagnosticsService.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Provider for code actions (quick fixes, refactoring) in markdown documents.
///
/// Usage:
/// ```cpp
/// CodeActionProvider provider(diagnostics_service);
/// auto actions = provider.provide_actions(content, 5, 0, 5, 40);
/// for (const auto& action : actions) {
///     // Apply action.edits
/// }
/// ```
class CodeActionProvider
{
public:
    explicit CodeActionProvider(DiagnosticsService& diagnostics_service);

    /// Provide code actions for the given range.
    /// @param content Full document content
    /// @param start_line Selection start line (0-based)
    /// @param start_char Selection start character
    /// @param end_line Selection end line
    /// @param end_char Selection end character
    /// @param document_uri URI for diagnostic lookup
    /// @return Available code actions
    [[nodiscard]] auto provide_actions(const std::string& content,
                                       int start_line,
                                       int start_char,
                                       int end_line,
                                       int end_char,
                                       const std::string& document_uri = "") const
        -> std::vector<CodeActionInfo>;

private:
    DiagnosticsService& diagnostics_service_;

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

    /// Get the line text at a given line number.
    [[nodiscard]] static auto get_line_text(const std::string& content, int line) -> std::string;

    /// Count existing reference links in the document.
    [[nodiscard]] static auto count_reference_links(const std::string& content) -> int;
};

} // namespace markamp::core
