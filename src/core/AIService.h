// ============================================================================
// File: src/core/AIService.h
// Phase 40: Import & AI Integration — AI service
// ============================================================================
#pragma once

#include "AITypes.h"

#include <expected>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace markamp::core
{

class EventBus;
class Config;

// AIService — manages LLM interactions, sessions, and prompt construction.
class AIService
{
public:
    AIService(EventBus& event_bus, Config& config);

    // Send a request and get a response.
    [[nodiscard]] auto complete(const AIRequest& request) -> AIResponse;

    // Send a streaming request.
    auto stream(const AIRequest& request, AIStreamCallback callback) -> void;

    // Execute a predefined action on content.
    [[nodiscard]] auto execute_action(AIAction action,
                                      const std::string& content,
                                      const std::string& context = "") -> AIResponse;

    // Session management.
    [[nodiscard]] auto create_session(const AIModelConfig& model) -> std::string;
    [[nodiscard]] auto get_session(const std::string& session_id) -> AISession*;
    auto delete_session(const std::string& session_id) -> void;
    [[nodiscard]] auto list_sessions() const -> std::vector<std::string>;

    // Send a message to an active session.
    [[nodiscard]] auto chat(const std::string& session_id, const std::string& message)
        -> AIResponse;

    // Get available models for a provider.
    [[nodiscard]] auto available_models(AIProvider provider) const -> std::vector<std::string>;

    // Test API connectivity.
    [[nodiscard]] auto test_connection(const AIModelConfig& model)
        -> std::expected<void, std::string>;

private:
    [[maybe_unused]] EventBus& event_bus_;
    [[maybe_unused]] Config& config_;
    mutable std::mutex mutex_;
    std::unordered_map<std::string, AISession> sessions_;

    // Build the system prompt for an action.
    [[nodiscard]] static auto build_system_prompt(AIAction action) -> std::string;

    // Build the user prompt for an action.
    [[nodiscard]] static auto build_user_prompt(AIAction action,
                                                const std::string& content,
                                                const std::string& context) -> std::string;

    // Make an HTTP request to the API.
    [[nodiscard]] auto call_api(const AIModelConfig& model, const std::vector<AIMessage>& messages)
        -> AIResponse;

    // Parse the JSON response from the API.
    [[nodiscard]] static auto parse_response(const std::string& json_response, AIProvider provider)
        -> AIResponse;

    // Generate a session ID.
    [[nodiscard]] static auto generate_session_id() -> std::string;
};

} // namespace markamp::core
