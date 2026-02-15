// ============================================================================
// File: src/core/AIPanel.h
// Phase 40: Import & AI Integration — AI panel UI
// ============================================================================
#pragma once

#include "AITypes.h"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class AIService;

// Callback for when user accepts AI-generated content.
using AIAcceptCallback = std::function<void(const std::string&)>;

// AIPanel — side panel UI for AI interactions.
class AIPanel
{
public:
    explicit AIPanel(AIService& ai_service);

    // Open a new chat session.
    auto start_chat() -> void;

    // Send a message in the current chat.
    [[nodiscard]] auto send_message(const std::string& message) -> AIResponse;

    // Execute an action on selected content.
    [[nodiscard]] auto execute_action(AIAction action,
                                      const std::string& selected_text,
                                      const std::string& document_context = "") -> AIResponse;

    // Get the current conversation history.
    [[nodiscard]] auto conversation_history() const -> std::vector<AIMessage>;

    // Clear conversation.
    auto clear_conversation() -> void;

    // Get suggested actions for selected text.
    [[nodiscard]] static auto suggested_actions(const std::string& selected_text)
        -> std::vector<AIAction>;

    // Set the callback for accepting AI content.
    auto set_accept_callback(AIAcceptCallback callback) -> void;

    // Accept the last AI response (insert into document).
    auto accept_last_response() -> void;

    // Get the current session ID.
    [[nodiscard]] auto current_session_id() const -> std::optional<std::string>;

    // Panel visibility.
    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

private:
    AIService& ai_service_;
    std::optional<std::string> session_id_;
    AIAcceptCallback accept_callback_;
    std::string last_response_content_;
    bool visible_{false};
};

} // namespace markamp::core
