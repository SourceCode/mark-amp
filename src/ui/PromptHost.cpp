/// @file PromptHost.cpp
/// @brief V19 P02-T03: Workbench-native prompt host implementation.

#include "ui/PromptHost.h"

#include "core/Logger.h"

namespace markamp::core
{

PromptHost::PromptHost(EventBus& event_bus)
    : event_bus_(event_bus)
{
    MARKAMP_LOG_DEBUG("PromptHost initialized");
}

auto PromptHost::show_prompt(const PromptConfig& config) -> PromptResult
{
    ++prompt_count_;

    // If mock mode, use the mock responder (for testing)
    if (mock_responder_ != nullptr)
    {
        MARKAMP_LOG_DEBUG("PromptHost: mock response for '{}' (severity: {})",
                          config.title,
                          static_cast<int>(config.severity));
        return mock_responder_(config);
    }

    // In production, we return a default response.
    // Full native or workbench UI rendering will be connected
    // when the UI layer implements IShellUIDelegate.
    MARKAMP_LOG_INFO("PromptHost: showing prompt '{}' — '{}'", config.title, config.message);

    PromptResult result;
    result.response = config.allow_dismiss ? PromptResponse::kCancel : PromptResponse::kNo;
    result.dismissed = true;
    return result;
}

auto PromptHost::confirm(const std::string& title,
                          const std::string& message,
                          PromptSeverity severity) -> PromptResponse
{
    PromptConfig config;
    config.title = title;
    config.message = message;
    config.severity = severity;
    config.buttons = {"Yes", "No", "Cancel"};
    config.default_button = 0;

    auto result = show_prompt(config);
    return result.response;
}

auto PromptHost::input(const std::string& title,
                        const std::string& message,
                        const std::string& default_value) -> std::optional<std::string>
{
    PromptConfig config;
    config.title = title;
    config.message = message;
    config.severity = PromptSeverity::kInfo;
    config.input_placeholder = "Enter value...";
    config.input_default_value = default_value;

    auto result = show_prompt(config);
    if (result.dismissed || result.response == PromptResponse::kCancel)
    {
        return std::nullopt;
    }
    return result.input_value;
}

void PromptHost::info(const std::string& title, const std::string& message)
{
    PromptConfig config;
    config.title = title;
    config.message = message;
    config.severity = PromptSeverity::kInfo;
    config.buttons = {"OK"};
    config.allow_dismiss = true;

    (void)show_prompt(config);
}

void PromptHost::warning(const std::string& title, const std::string& message)
{
    PromptConfig config;
    config.title = title;
    config.message = message;
    config.severity = PromptSeverity::kWarning;
    config.buttons = {"OK"};
    config.allow_dismiss = true;

    (void)show_prompt(config);
}

void PromptHost::error(const std::string& title, const std::string& message)
{
    PromptConfig config;
    config.title = title;
    config.message = message;
    config.severity = PromptSeverity::kError;
    config.buttons = {"OK"};
    config.allow_dismiss = true;

    (void)show_prompt(config);
}

void PromptHost::set_mock_responder(MockResponder responder)
{
    mock_responder_ = std::move(responder);
    MARKAMP_LOG_DEBUG("PromptHost: mock responder {}", mock_responder_ ? "set" : "cleared");
}

} // namespace markamp::core
