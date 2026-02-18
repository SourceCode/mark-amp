// ============================================================================
// File: src/core/ColorBlindnessSupport.h
// Phase 31: Accessibility System — Non-color indicator generator
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// The kind of non-color indicator paired with a color.
enum class IndicatorKind : uint8_t
{
    kLetter,    ///< Single letter (e.g., M for Modified)
    kIcon,      ///< Named icon (e.g., "error_icon")
    kUnderline, ///< Underline decoration
    kPattern,   ///< Pattern fill (e.g., hatching)
    kLabel,     ///< Full text label
};

/// Git file status for generating accessible indicators.
enum class GitFileStatus : uint8_t
{
    kModified,
    kAdded,
    kDeleted,
    kUntracked,
    kRenamed,
    kConflicted,
    kIgnored,
};

/// Diagnostic severity for generating accessible indicators.
enum class DiagnosticSeverity : uint8_t
{
    kError,
    kWarning,
    kInfo,
    kHint,
};

/// A status indicator that provides both color and non-color information.
struct StatusIndicator
{
    uint32_t color{0x000000};                   ///< RGB color
    std::string letter;                         ///< Single-letter indicator (e.g., "M")
    std::string icon_name;                      ///< Icon identifier (e.g., "error_circle")
    std::string label;                          ///< Human-readable label (e.g., "Modified")
    IndicatorKind kind{IndicatorKind::kLetter}; ///< Primary indicator kind
};

/// ColorBlindnessSupport — ensures color is never the sole information carrier.
///
/// Phase 31 Task 11: Every color-coded element also has a shape, text, or
/// pattern indicator so that color-blind users can distinguish states.
class ColorBlindnessSupport
{
public:
    ColorBlindnessSupport();

    // ── Git status indicators ──────────────────────────────────────

    /// Get the status indicator for a git file status.
    [[nodiscard]] auto indicator_for_git_status(GitFileStatus status) const -> StatusIndicator;

    /// Get a human-readable label for a git file status.
    [[nodiscard]] static auto git_status_label(GitFileStatus status) -> std::string;

    /// Get the single-letter code for a git file status.
    [[nodiscard]] static auto git_status_letter(GitFileStatus status) -> std::string;

    // ── Diagnostic indicators ──────────────────────────────────────

    /// Get the status indicator for a diagnostic severity.
    [[nodiscard]] auto indicator_for_diagnostic(DiagnosticSeverity severity) const
        -> StatusIndicator;

    /// Get the icon name for a diagnostic severity.
    [[nodiscard]] static auto diagnostic_icon(DiagnosticSeverity severity) -> std::string;

    /// Get a human-readable label for a diagnostic severity.
    [[nodiscard]] static auto diagnostic_label(DiagnosticSeverity severity) -> std::string;

    // ── Search match indicators ────────────────────────────────────

    /// Get the status indicator for a search match highlight.
    [[nodiscard]] auto indicator_for_search_match() const -> StatusIndicator;

    /// Get the status indicator for the current (active) search match.
    [[nodiscard]] auto indicator_for_current_match() const -> StatusIndicator;

    // ── Task priority indicators ───────────────────────────────────

    /// Get the status indicator for a task priority level (1=highest, 5=lowest).
    [[nodiscard]] auto indicator_for_priority(int priority_level) const -> StatusIndicator;

    // ── All indicators access ──────────────────────────────────────

    /// Get all git status indicators.
    [[nodiscard]] auto all_git_indicators() const -> std::vector<StatusIndicator>;

    /// Get all diagnostic indicators.
    [[nodiscard]] auto all_diagnostic_indicators() const -> std::vector<StatusIndicator>;
};

} // namespace markamp::core
