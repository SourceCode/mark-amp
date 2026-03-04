#pragma once

/// @file DebugConsoleService.h
/// @brief Debug Console service — REPL engine, expression evaluation, object expansion.
///
/// Features:
///   - Console entry management (input/output/result/error/system/debug)
///   - REPL input with command history navigation
///   - Expression evaluation via ExpressionEvaluator
///   - Object/array expansion with tree rendering
///   - Stack trace link detection (GCC/Clang/Python/Node.js)
///   - Console.log() interception from embedded scripts
///   - Log level filtering, clear console, timestamps
///   - Variable inspection on hover (inspect_variable)
///   - Input auto-complete from history (auto_complete)
///   - ConsoleOutputRenderer (format_entry_for_display)
///   - PluginContext integration for extension API access
///
/// @see ExpressionEvaluator, DebugConsolePanel, ConsoleObjectValue

#include "core/LogLevel.h"

#include <chrono>
#include <deque>
#include <functional>
#include <optional>
#include <regex>
#include <string>
#include <vector>

namespace markamp::core
{

/// Type of debug console entry.
enum class ConsoleEntryType
{
    kOutput, ///< Normal program output
    kInput,  ///< User-typed expression
    kResult, ///< Evaluation result
    kError,  ///< Error output
    kSystem, ///< System message
    kDebug,  ///< Debug-level output
};

/// A tree-expandable value for object inspection.
struct ConsoleObjectValue
{
    std::string name;
    std::string value_text;
    std::string type_name;
    std::vector<ConsoleObjectValue> children;
    bool expandable{false};
    bool expanded{false};
};

/// A single entry in the debug console.
struct ConsoleEntry
{
    ConsoleEntryType type{ConsoleEntryType::kOutput};
    std::string text;
    std::string source;
    LogLevel level{LogLevel::kInfo};
    std::chrono::system_clock::time_point timestamp{std::chrono::system_clock::now()};
    ConsoleObjectValue object_value; ///< For expandable object results
    bool has_object{false};
    std::vector<std::string> related_info; ///< Related information links/URIs
};

/// A detected stack trace link within console output.
struct StackTraceLink
{
    std::string file;
    int line{0};
    int column{0};
    std::size_t match_start{0};  ///< Start offset in the text
    std::size_t match_length{0}; ///< Length of the matched path:line portion
};

/// Service for managing debug console entries, command history, and filtering.
/// Phase 24: Backs the DebugConsolePanel REPL UI.
class DebugConsoleService
{
public:
    /// Maximum entries retained (50K per PRD).
    static constexpr std::size_t kMaxEntries = 50000;
    /// Maximum command history depth.
    static constexpr std::size_t kMaxHistory = 500;

    DebugConsoleService() = default;

    // ── Entry management ──

    /// Add an output entry.
    void add_output(const std::string& text,
                    ConsoleEntryType type = ConsoleEntryType::kOutput,
                    const std::string& source = "");

    /// Add an output with explicit log level.
    void add_output(const std::string& text, LogLevel level, const std::string& source = "");

    /// Add an object result entry.
    void add_object_result(const std::string& expression, ConsoleObjectValue value);

    /// Toggle expansion state of an object value in a console entry.
    /// Returns true if the entry was found and toggled.
    auto toggle_expansion(std::size_t entry_index, const std::string& path = "") -> bool;

    /// Intercept console output — captures text as a console entry.
    /// Used for Console.log() interception from embedded scripts.
    void intercept_console_output(const std::string& text, LogLevel level = LogLevel::kInfo);

    /// All entries.
    [[nodiscard]] auto entries() const -> const std::deque<ConsoleEntry>&;

    /// Entries filtered by minimum log level.
    [[nodiscard]] auto entries_filtered(LogLevel min_level) const -> std::vector<ConsoleEntry>;

    /// Auto-complete: get history entries matching prefix.
    [[nodiscard]] auto auto_complete(const std::string& prefix) const -> std::vector<std::string>;

    /// Variable inspection: look up an object value by variable name.
    /// Returns the object value if found in entries, or std::nullopt.
    [[nodiscard]] auto inspect_variable(const std::string& name) const
        -> std::optional<ConsoleObjectValue>;

    /// Console output rendering: format an entry for display.
    /// Includes tree expansion markers for expandable objects.
    [[nodiscard]] static auto format_entry_for_display(const ConsoleEntry& entry) -> std::string;

    /// Clear all entries.
    void clear();

    /// Entry count.
    [[nodiscard]] auto entry_count() const -> std::size_t;

    // ── Command history ──

    /// Push a command to history.
    void push_history(const std::string& command);

    /// Navigate history: -1 = previous, +1 = next. Returns the command at the new position.
    [[nodiscard]] auto navigate_history(int direction) -> std::string;

    /// Reset history navigation position.
    void reset_history_position();

    /// All history entries.
    [[nodiscard]] auto history() const -> const std::deque<std::string>&;

    /// Current history position.
    [[nodiscard]] auto history_position() const -> int;

    // ── Change listener ──
    using ChangeListener = std::function<void()>;
    auto on_change(ChangeListener listener) -> std::size_t;
    void remove_listener(std::size_t listener_id);

    // ── Stack trace detection (Phase 24 Task 12) ──

    /// Detect stack trace links in a text string.
    /// Recognizes GCC/Clang (file:line:col), Python (File "...", line N),
    /// and Node.js (at Object (file:line:col)) patterns.
    [[nodiscard]] static auto detect_stack_traces(const std::string& text)
        -> std::vector<StackTraceLink>;

private:
    std::deque<ConsoleEntry> entries_;
    std::deque<std::string> history_;
    int history_pos_{-1}; // -1 = at bottom (new input)

    std::vector<std::pair<std::size_t, ChangeListener>> listeners_;
    std::size_t next_listener_id_{0};

    void trim_entries();
    void fire_change();
};

} // namespace markamp::core
