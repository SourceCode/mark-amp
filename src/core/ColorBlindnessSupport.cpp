// ============================================================================
// File: src/core/ColorBlindnessSupport.cpp
// Phase 31: Accessibility System — Non-color indicator generator
// ============================================================================

#include "ColorBlindnessSupport.h"

namespace markamp::core
{

ColorBlindnessSupport::ColorBlindnessSupport() = default;

// ── Git status indicators ──────────────────────────────────────────────────

auto ColorBlindnessSupport::indicator_for_git_status(GitFileStatus status) const -> StatusIndicator
{
    StatusIndicator ind;
    ind.kind = IndicatorKind::kLetter;
    ind.letter = git_status_letter(status);
    ind.label = git_status_label(status);

    switch (status)
    {
        case GitFileStatus::kModified:
            ind.color = 0xE2C08D; // warm yellow
            ind.icon_name = "pencil";
            break;
        case GitFileStatus::kAdded:
            ind.color = 0x73C991; // green
            ind.icon_name = "plus";
            break;
        case GitFileStatus::kDeleted:
            ind.color = 0xE06C75; // red
            ind.icon_name = "minus";
            break;
        case GitFileStatus::kUntracked:
            ind.color = 0xABB2BF; // grey
            ind.icon_name = "question";
            break;
        case GitFileStatus::kRenamed:
            ind.color = 0x56B6C2; // cyan
            ind.icon_name = "arrow_right";
            break;
        case GitFileStatus::kConflicted:
            ind.color = 0xBE5046; // dark red
            ind.icon_name = "warning";
            break;
        case GitFileStatus::kIgnored:
            ind.color = 0x5C6370; // dim grey
            ind.icon_name = "eye_off";
            break;
    }

    return ind;
}

auto ColorBlindnessSupport::git_status_label(GitFileStatus status) -> std::string
{
    switch (status)
    {
        case GitFileStatus::kModified:
            return "Modified";
        case GitFileStatus::kAdded:
            return "Added";
        case GitFileStatus::kDeleted:
            return "Deleted";
        case GitFileStatus::kUntracked:
            return "Untracked";
        case GitFileStatus::kRenamed:
            return "Renamed";
        case GitFileStatus::kConflicted:
            return "Conflicted";
        case GitFileStatus::kIgnored:
            return "Ignored";
    }
    return "Unknown";
}

auto ColorBlindnessSupport::git_status_letter(GitFileStatus status) -> std::string
{
    switch (status)
    {
        case GitFileStatus::kModified:
            return "M";
        case GitFileStatus::kAdded:
            return "A";
        case GitFileStatus::kDeleted:
            return "D";
        case GitFileStatus::kUntracked:
            return "U";
        case GitFileStatus::kRenamed:
            return "R";
        case GitFileStatus::kConflicted:
            return "!";
        case GitFileStatus::kIgnored:
            return "I";
    }
    return "?";
}

// ── Diagnostic indicators ──────────────────────────────────────────────────

auto ColorBlindnessSupport::indicator_for_diagnostic(DiagnosticSeverity severity) const
    -> StatusIndicator
{
    StatusIndicator ind;
    ind.kind = IndicatorKind::kIcon;
    ind.icon_name = diagnostic_icon(severity);
    ind.label = diagnostic_label(severity);

    switch (severity)
    {
        case DiagnosticSeverity::kError:
            ind.color = 0xE06C75; // red
            ind.letter = "E";
            break;
        case DiagnosticSeverity::kWarning:
            ind.color = 0xE2C08D; // yellow
            ind.letter = "W";
            break;
        case DiagnosticSeverity::kInfo:
            ind.color = 0x61AFEF; // blue
            ind.letter = "I";
            break;
        case DiagnosticSeverity::kHint:
            ind.color = 0x56B6C2; // cyan
            ind.letter = "H";
            break;
    }

    return ind;
}

auto ColorBlindnessSupport::diagnostic_icon(DiagnosticSeverity severity) -> std::string
{
    switch (severity)
    {
        case DiagnosticSeverity::kError:
            return "error_circle";
        case DiagnosticSeverity::kWarning:
            return "warning_triangle";
        case DiagnosticSeverity::kInfo:
            return "info_circle";
        case DiagnosticSeverity::kHint:
            return "lightbulb";
    }
    return "circle";
}

auto ColorBlindnessSupport::diagnostic_label(DiagnosticSeverity severity) -> std::string
{
    switch (severity)
    {
        case DiagnosticSeverity::kError:
            return "Error";
        case DiagnosticSeverity::kWarning:
            return "Warning";
        case DiagnosticSeverity::kInfo:
            return "Info";
        case DiagnosticSeverity::kHint:
            return "Hint";
    }
    return "Unknown";
}

// ── Search match indicators ────────────────────────────────────────────────

auto ColorBlindnessSupport::indicator_for_search_match() const -> StatusIndicator
{
    StatusIndicator ind;
    ind.kind = IndicatorKind::kUnderline;
    ind.color = 0x61AFEF; // blue highlight
    ind.icon_name = "search";
    ind.label = "Search match";
    ind.letter = "";
    return ind;
}

auto ColorBlindnessSupport::indicator_for_current_match() const -> StatusIndicator
{
    StatusIndicator ind;
    ind.kind = IndicatorKind::kUnderline;
    ind.color = 0xE2C08D; // orange/yellow highlight
    ind.icon_name = "search_active";
    ind.label = "Current match";
    ind.letter = "";
    return ind;
}

// ── Task priority indicators ───────────────────────────────────────────────

auto ColorBlindnessSupport::indicator_for_priority(int priority_level) const -> StatusIndicator
{
    StatusIndicator ind;
    ind.kind = IndicatorKind::kLabel;

    switch (priority_level)
    {
        case 1:
            ind.color = 0xE06C75;
            ind.label = "Critical";
            ind.letter = "1";
            ind.icon_name = "priority_critical";
            break;
        case 2:
            ind.color = 0xE2C08D;
            ind.label = "High";
            ind.letter = "2";
            ind.icon_name = "priority_high";
            break;
        case 3:
            ind.color = 0x61AFEF;
            ind.label = "Medium";
            ind.letter = "3";
            ind.icon_name = "priority_medium";
            break;
        case 4:
            ind.color = 0x73C991;
            ind.label = "Low";
            ind.letter = "4";
            ind.icon_name = "priority_low";
            break;
        default:
            ind.color = 0xABB2BF;
            ind.label = "None";
            ind.letter = "5";
            ind.icon_name = "priority_none";
            break;
    }

    return ind;
}

// ── All indicators ─────────────────────────────────────────────────────────

auto ColorBlindnessSupport::all_git_indicators() const -> std::vector<StatusIndicator>
{
    return {
        indicator_for_git_status(GitFileStatus::kModified),
        indicator_for_git_status(GitFileStatus::kAdded),
        indicator_for_git_status(GitFileStatus::kDeleted),
        indicator_for_git_status(GitFileStatus::kUntracked),
        indicator_for_git_status(GitFileStatus::kRenamed),
        indicator_for_git_status(GitFileStatus::kConflicted),
        indicator_for_git_status(GitFileStatus::kIgnored),
    };
}

auto ColorBlindnessSupport::all_diagnostic_indicators() const -> std::vector<StatusIndicator>
{
    return {
        indicator_for_diagnostic(DiagnosticSeverity::kError),
        indicator_for_diagnostic(DiagnosticSeverity::kWarning),
        indicator_for_diagnostic(DiagnosticSeverity::kInfo),
        indicator_for_diagnostic(DiagnosticSeverity::kHint),
    };
}

} // namespace markamp::core
