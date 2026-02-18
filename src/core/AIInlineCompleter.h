// ============================================================================
// File: src/core/AIInlineCompleter.h
// Phase 26: AI Integration — ghost text inline completion engine
// ============================================================================
#pragma once

#include "AITypes.h"

#include <expected>
#include <string>

namespace markamp::core
{

class AIService;
class Config;

/// Ghost text completion engine. Provides inline suggestions that appear
/// as dimmed text after the cursor. Tab to accept, Escape to dismiss,
/// Cmd+Right for word-by-word acceptance.
class AIInlineCompleter
{
public:
    AIInlineCompleter(AIService& ai_service, Config& config);

    /// Request a completion based on surrounding context.
    [[nodiscard]] auto request_completion(const std::string& context_before,
                                          const std::string& context_after = "")
        -> std::expected<InlineSuggestion, std::string>;

    /// Accept the full suggestion. Records acceptance for analytics.
    auto accept_suggestion(const std::string& request_id) -> void;

    /// Accept the next word from the current suggestion.
    /// Returns the accepted word, or empty if no pending suggestion.
    [[nodiscard]] auto accept_word(const std::string& request_id) -> std::string;

    /// Dismiss the current suggestion. Records dismissal for analytics.
    auto dismiss(const std::string& request_id) -> void;

    /// Check if inline completion is enabled.
    [[nodiscard]] auto is_enabled() const -> bool;

    /// Enable or disable inline completion.
    auto set_enabled(bool enabled) -> void;

    /// Get the debounce time in milliseconds.
    [[nodiscard]] auto debounce_ms() const -> int;

    /// Set the debounce time in milliseconds.
    auto set_debounce_ms(int milliseconds) -> void;

    /// Get acceptance rate (accepted / total requests).
    [[nodiscard]] auto acceptance_rate() const -> double;

    /// Get pending suggestion text for a request, or empty if none.
    [[nodiscard]] auto pending_text(const std::string& request_id) const -> std::string;

private:
    AIService& ai_service_;
    Config& config_;
    bool enabled_{true};
    int debounce_ms_{1500};
    int32_t next_request_id_{0};

    // Analytics
    int32_t total_requests_{0};
    int32_t accepted_count_{0};
    int32_t dismissed_count_{0};

    // Current suggestion state
    std::string current_request_id_;
    std::string remaining_text_; // Remaining text for word-by-word acceptance

    /// Generate a unique request ID.
    [[nodiscard]] auto generate_request_id() -> std::string;
};

} // namespace markamp::core
