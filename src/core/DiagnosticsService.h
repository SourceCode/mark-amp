#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Severity levels for diagnostics (mirrors VS Code's DiagnosticSeverity).
enum class DiagnosticSeverity : std::uint8_t
{
    kError = 0,
    kWarning = 1,
    kInformation = 2,
    kHint = 3
};

/// A position within a document.
struct DiagnosticPosition
{
    int line{0};
    int character{0};
};

/// A range within a document.
struct DiagnosticRange
{
    DiagnosticPosition start;
    DiagnosticPosition end;
};

/// A single diagnostic entry (error, warning, etc.).
struct Diagnostic
{
    DiagnosticRange range;
    std::string message;
    DiagnosticSeverity severity{DiagnosticSeverity::kError};
    std::string source; // e.g. "eslint", "tsc"
    std::string code;   // e.g. "TS2304", "no-unused-vars"

    /// Related information links (uri + message pairs).
    struct RelatedInfo
    {
        std::string uri;
        DiagnosticPosition position;
        std::string message;
    };
    std::vector<RelatedInfo> related_information;
};

/// Service that manages diagnostics per URI (equivalent to VS Code's DiagnosticCollection).
class DiagnosticsService
{
public:
    DiagnosticsService() = default;

    /// Set diagnostics for a URI (replaces existing).
    void set(const std::string& uri, std::vector<Diagnostic> diagnostics);

    /// Get diagnostics for a URI.
    [[nodiscard]] auto get(const std::string& uri) const -> const std::vector<Diagnostic>&;

    /// Get all URIs that have diagnostics.
    [[nodiscard]] auto uris() const -> std::vector<std::string>;

    /// Remove diagnostics for a URI.
    void remove(const std::string& uri);

    /// Clear all diagnostics.
    void clear();

    /// Clear all diagnostics from a specific source (e.g., "gcc", "eslint").
    void clear_by_source(const std::string& source);

    /// Clear all diagnostics of a specific severity.
    void clear_by_severity(DiagnosticSeverity severity);

    /// Get all diagnostics matching a severity across all URIs.
    [[nodiscard]] auto diagnostics_for_severity(DiagnosticSeverity severity) const
        -> std::vector<std::pair<std::string, Diagnostic>>;

    /// Count diagnostics by severity across all URIs.
    [[nodiscard]] auto count_by_severity(DiagnosticSeverity severity) const -> std::size_t;

    /// Total diagnostic count.
    [[nodiscard]] auto total_count() const -> std::size_t;

    /// Listener for diagnostic changes.
    using ChangeListener = std::function<void(const std::string& uri)>;
    auto on_change(ChangeListener listener) -> std::size_t;
    void remove_listener(std::size_t listener_id);

private:
    std::unordered_map<std::string, std::vector<Diagnostic>> diagnostics_;
    static const std::vector<Diagnostic> kEmptyDiagnostics;

    std::vector<std::pair<std::size_t, ChangeListener>> listeners_;
    std::size_t next_listener_id_{0};

    void fire_change(const std::string& uri);
};

// ============================================================================
// Phase 35: Error Help Integration
// ============================================================================

/// An error-help entry providing user-facing guidance for a specific error code.
struct ErrorHelpEntry
{
    std::string error_code;   ///< e.g. "MD001", "SYN_UNCLOSED_FENCE"
    std::string title;        ///< Short title
    std::string explanation;  ///< Detailed explanation
    std::string fix_hint;     ///< Suggested fix
    std::string related_help; ///< Help article ID for further reading
};

/// Provides help text suggestions for known diagnostic error codes.
///
/// Built-in entries cover common Markdown linting errors and syntax issues.
///
/// Usage:
/// ```cpp
/// DiagnosticsErrorHelp help;
/// auto entry = help.lookup("MD001");
/// auto suggestions = help.suggest_fix(diagnostic);
/// ```
class DiagnosticsErrorHelp
{
public:
    DiagnosticsErrorHelp()
    {
        register_built_in();
    }

    /// Look up an error help entry by error code.
    [[nodiscard]] auto lookup(const std::string& error_code) const -> const ErrorHelpEntry*
    {
        for (const auto& entry : entries_)
        {
            if (entry.error_code == error_code)
            {
                return &entry;
            }
        }
        return nullptr;
    }

    /// Suggest fixes for a diagnostic.
    [[nodiscard]] auto suggest_fix(const Diagnostic& diag) const -> std::string
    {
        const auto* entry = lookup(diag.code);
        if (entry != nullptr)
        {
            return entry->fix_hint;
        }
        return {};
    }

    /// Get all registered entries.
    [[nodiscard]] auto all_entries() const -> const std::vector<ErrorHelpEntry>&
    {
        return entries_;
    }

    /// Register an additional error help entry.
    void register_entry(ErrorHelpEntry entry)
    {
        entries_.push_back(std::move(entry));
    }

    [[nodiscard]] auto entry_count() const -> std::size_t
    {
        return entries_.size();
    }

private:
    std::vector<ErrorHelpEntry> entries_;

    void register_built_in()
    {
        entries_ = {
            {.error_code = "MD001",
             .title = "Heading Levels Should Only Increment by One",
             .explanation =
                 "Headings should not skip levels, e.g. jumping from # (h1) to ### (h3).",
             .fix_hint = "Insert a ## (h2) heading between # and ###.",
             .related_help = "help.markdownReference"},
            {.error_code = "MD009",
             .title = "Trailing Spaces",
             .explanation = "Lines should not end with trailing whitespace.",
             .fix_hint = "Remove trailing spaces at the end of the line.",
             .related_help = "help.markdownReference"},
            {.error_code = "SYN_UNCLOSED_FENCE",
             .title = "Unclosed Code Fence",
             .explanation = "A code fence (```) was opened but never closed.",
             .fix_hint = "Add a closing ``` on a new line after your code block.",
             .related_help = "help.markdownReference"},
            {.error_code = "SYN_BAD_LINK",
             .title = "Malformed Link",
             .explanation = "Link syntax [text](url) is incomplete or malformed.",
             .fix_hint = "Ensure both brackets and parentheses are properly closed.",
             .related_help = "help.markdownReference"},
            {.error_code = "SYN_BROKEN_WIKILINK",
             .title = "Broken Wiki-Link",
             .explanation = "The target document for a [[wiki-link]] could not be found.",
             .fix_hint = "Check the filename matches an existing document in the workspace.",
             .related_help = "help.markdownReference"},
            {.error_code = "MERMAID_SYNTAX",
             .title = "Mermaid Diagram Syntax Error",
             .explanation = "The Mermaid diagram contains a syntax error.",
             .fix_hint = "Check for missing arrows (-->), unclosed brackets, or typos "
                         "in node IDs.",
             .related_help = "help.markdownReference"},
        };
    }
};

} // namespace markamp::core
