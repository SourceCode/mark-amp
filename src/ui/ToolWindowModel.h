#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Problem severity level.
enum class ProblemSeverity : uint8_t
{
    kError,
    kWarning,
    kInfo,
    kHint,
};

/// Problem entry for the problems panel.
struct ProblemEntry
{
    std::string problem_id;
    std::string message;
    std::string file_path;
    int line_number{0};
    ProblemSeverity severity{ProblemSeverity::kInfo};
    std::string source; ///< e.g., "TypeScript", "ESLint"
    bool has_quick_fix{false};
};

/// Output channel.
struct OutputChannel
{
    std::string channel_id;
    std::string name;
    bool is_active{false};
};

/// Testable model for Tool Window Controls (Phase 29).
///
/// Encapsulates:
/// - Problems panel: severity filtering, grouping, quick-fix count
/// - Output panel: channel list, follow-tail, word-wrap
/// - Panel lifecycle: show/hide/reset
class ToolWindowModel
{
public:
    // ── Problems ────────────────────────────────────────────────────

    void set_problems(std::vector<ProblemEntry> problems);
    [[nodiscard]] auto all_problems() const -> const std::vector<ProblemEntry>&;
    [[nodiscard]] auto by_severity(ProblemSeverity severity) const -> std::vector<ProblemEntry>;
    [[nodiscard]] auto by_source(const std::string& source) const -> std::vector<ProblemEntry>;
    [[nodiscard]] auto error_count() const -> int;
    [[nodiscard]] auto warning_count() const -> int;
    [[nodiscard]] auto quick_fix_count() const -> int;

    // ── Output channels ─────────────────────────────────────────────

    void set_channels(std::vector<OutputChannel> channels);
    [[nodiscard]] auto channels() const -> const std::vector<OutputChannel>&;
    void set_active_channel(const std::string& channel_id);
    [[nodiscard]] auto active_channel() const -> const OutputChannel*;

    // ── Output controls ─────────────────────────────────────────────

    void set_follow_tail(bool enabled);
    [[nodiscard]] auto follow_tail() const -> bool;

    void set_word_wrap(bool enabled);
    [[nodiscard]] auto word_wrap() const -> bool;

    // ── Panel visibility ────────────────────────────────────────────

    void set_panel_visible(const std::string& panel_id, bool visible);
    [[nodiscard]] auto is_panel_visible(const std::string& panel_id) const -> bool;
    void reset_layout();

private:
    std::vector<ProblemEntry> problems_;
    std::vector<OutputChannel> channels_;
    bool follow_tail_{true};
    bool word_wrap_{false};
    std::vector<std::string> hidden_panels_;
};

} // namespace markamp::ui
