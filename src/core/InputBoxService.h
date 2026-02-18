#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Options for showing an input box to the user.
/// Mirrors VS Code's `vscode.window.showInputBox()`.
struct InputBoxOptions
{
    std::string title;
    std::string prompt;
    std::string value;       // Pre-filled value
    std::string placeholder; // Greyed-out placeholder text
    bool password{false};    // Mask input like a password field
};

class EventBus; // Forward declaration

/// Service for extensions to prompt user for text input.
///
/// Mirrors VS Code's `vscode.window.showInputBox()`.
///
/// Injected into `PluginContext` so extensions can call:
///   `ctx.input_box_service->show({.prompt = "Enter name"}, [](auto result) { ... });`
class InputBoxService
{
public:
    using ResultCallback = std::function<void(const std::optional<std::string>&)>;

    InputBoxService() = default;

    /// Show an input box. Calls `on_result` with the entered text,
    /// or `std::nullopt` if the user cancelled.
    void show(const InputBoxOptions& options, ResultCallback on_result);

    /// Synchronous check: is an input box currently visible?
    [[nodiscard]] auto is_visible() const -> bool
    {
        return visible_;
    }

    /// For testing: simulate a user response.
    void test_accept(const std::string& value);

    /// For testing: simulate user cancellation.
    void test_cancel();

    /// Set the EventBus to publish UI request events.
    void set_event_bus(EventBus* bus);

private:
    bool visible_{false};
    InputBoxOptions current_options_;
    ResultCallback current_callback_;
    EventBus* event_bus_{nullptr};
};

// ============================================================================
// V9 Phase 36 Task 14 — Input history and validation extensions
// ============================================================================

/// Maintains per-prompt-type input history for up-arrow recall.
class InputHistory
{
public:
    InputHistory() = default;

    /// Add an input to history for a given prompt type.
    void add(const std::string& prompt_type, const std::string& value);

    /// Get the Nth previous input (0 = most recent).
    [[nodiscard]] auto get_previous(const std::string& prompt_type, int offset = 0) const
        -> std::string;

    /// Get all history for a prompt type.
    [[nodiscard]] auto get_all(const std::string& prompt_type) const -> std::vector<std::string>;

    /// Clear history for a specific prompt type.
    void clear(const std::string& prompt_type);

    /// Clear all history.
    void clear_all();

    /// Maximum entries per prompt type.
    static constexpr int kMaxPerType = 50;

private:
    std::unordered_map<std::string, std::vector<std::string>> history_;
};

/// Validate an input value against InputBoxOptions constraints.
/// Returns error message string, or nullopt if valid.
[[nodiscard]] inline auto validate_input_box(const std::string& value,
                                             const InputBoxOptions& /*options*/)
    -> std::optional<std::string>
{
    if (value.empty())
    {
        return "Value cannot be empty";
    }
    return std::nullopt;
}

} // namespace markamp::core
