/// @file PromptHost.h
/// @brief V19 P02-T03: Workbench-native prompt host.
///
/// Replaces raw wxMessageBox / wxTextEntryDialog calls with a structured
/// prompt system that can be themed, tested, and intercepted by extensions.
/// In production, prompts are rendered in the workbench UI. In tests, the
/// host can be configured to auto-respond.
#pragma once

#include "core/EventBus.h"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Prompt severity level (controls icon and theming).
enum class PromptSeverity
{
    kInfo,
    kWarning,
    kError,
    kConfirmation
};

/// Response from a prompt interaction.
enum class PromptResponse
{
    kYes,
    kNo,
    kCancel,
    kCustom   ///< For prompts with custom button labels
};

/// Configuration for a single prompt.
struct PromptConfig
{
    std::string title;
    std::string message;
    PromptSeverity severity{PromptSeverity::kInfo};
    std::vector<std::string> buttons;    ///< Custom button labels (empty = default Yes/No/Cancel)
    int default_button{0};               ///< 0-based index of the default button
    bool allow_dismiss{true};            ///< Whether the prompt can be dismissed (Esc key)
    std::string input_placeholder;       ///< Non-empty = show text input field
    std::string input_default_value;     ///< Default value for text input
};

/// Result from a prompt interaction.
struct PromptResult
{
    PromptResponse response{PromptResponse::kCancel};
    int button_index{-1};             ///< Which button was clicked (0-based)
    std::string input_value;          ///< Text input value (if applicable)
    bool dismissed{false};            ///< True if user dismissed without responding
};

/// Workbench-native prompt host.
///
/// Provides a structured API for showing prompts, confirmations, and text
/// input dialogs. In test/headless mode, a mock responder can be set to
/// auto-respond to all prompts.
///
/// Example:
/// ```cpp
/// PromptHost host(event_bus);
/// PromptConfig cfg;
/// cfg.title = "Save Changes";
/// cfg.message = "Do you want to save before closing?";
/// cfg.severity = PromptSeverity::kConfirmation;
/// auto result = host.show_prompt(cfg);
/// if (result.response == PromptResponse::kYes) { /* save */ }
/// ```
class PromptHost
{
public:
    explicit PromptHost(EventBus& event_bus);

    /// Show a prompt and return the result synchronously.
    [[nodiscard]] auto show_prompt(const PromptConfig& config) -> PromptResult;

    /// Show a simple confirmation prompt (Yes / No / Cancel).
    [[nodiscard]] auto confirm(const std::string& title,
                               const std::string& message,
                               PromptSeverity severity = PromptSeverity::kConfirmation)
        -> PromptResponse;

    /// Show a text input prompt.
    [[nodiscard]] auto input(const std::string& title,
                             const std::string& message,
                             const std::string& default_value = "")
        -> std::optional<std::string>;

    /// Show a simple info message (OK only).
    void info(const std::string& title, const std::string& message);

    /// Show a warning message (OK only).
    void warning(const std::string& title, const std::string& message);

    /// Show an error message (OK only).
    void error(const std::string& title, const std::string& message);

    // ── Test Support ──

    /// Set a mock responder that auto-responds to all prompts.
    /// Pass nullptr to clear and return to normal (native dialog) mode.
    using MockResponder = std::function<PromptResult(const PromptConfig&)>;
    void set_mock_responder(MockResponder responder);

    /// Whether a mock responder is active.
    [[nodiscard]] auto is_mock_mode() const -> bool { return mock_responder_ != nullptr; }

    /// Count of prompts shown since creation or last reset.
    [[nodiscard]] auto prompt_count() const -> int { return prompt_count_; }

private:
    [[maybe_unused]] EventBus& event_bus_;
    MockResponder mock_responder_;
    int prompt_count_{0};
};

} // namespace markamp::core
