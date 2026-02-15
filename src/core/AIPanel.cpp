// ============================================================================
// File: src/core/AIPanel.cpp
// Phase 40: Import & AI Integration — AIPanel implementation
// ============================================================================

#include "AIPanel.h"

#include "AIService.h"

namespace markamp::core
{

AIPanel::AIPanel(AIService& ai_service)
    : ai_service_(ai_service)
{
}

auto AIPanel::start_chat() -> void
{
    AIModelConfig model;
    model.provider = AIProvider::OpenAI;
    model.model_name = "gpt-4";
    session_id_ = ai_service_.create_session(model);
}

auto AIPanel::send_message(const std::string& message) -> AIResponse
{
    if (!session_id_)
    {
        start_chat();
    }
    auto response = ai_service_.chat(*session_id_, message);
    if (response.success)
    {
        last_response_content_ = response.content;
    }
    return response;
}

auto AIPanel::execute_action(AIAction action,
                             const std::string& selected_text,
                             const std::string& document_context) -> AIResponse
{
    auto response = ai_service_.execute_action(action, selected_text, document_context);
    if (response.success)
    {
        last_response_content_ = response.content;
    }
    return response;
}

auto AIPanel::conversation_history() const -> std::vector<AIMessage>
{
    if (!session_id_)
        return {};
    auto* session = const_cast<AIService&>(ai_service_).get_session(*session_id_);
    return session ? session->messages : std::vector<AIMessage>{};
}

auto AIPanel::clear_conversation() -> void
{
    if (session_id_)
    {
        ai_service_.delete_session(*session_id_);
        session_id_.reset();
    }
    last_response_content_.clear();
}

auto AIPanel::suggested_actions(const std::string& selected_text) -> std::vector<AIAction>
{
    std::vector<AIAction> actions;

    // Always available.
    actions.push_back(AIAction::FixGrammar);
    actions.push_back(AIAction::SimplifyContent);
    actions.push_back(AIAction::ChangeTone);
    actions.push_back(AIAction::Translate);

    // Context-dependent suggestions.
    if (selected_text.size() > 200)
    {
        actions.push_back(AIAction::Summarize);
        actions.push_back(AIAction::ExtractKeywords);
    }
    if (selected_text.size() < 100)
    {
        actions.push_back(AIAction::ExpandContent);
        actions.push_back(AIAction::ContinueWriting);
    }

    // Check if it looks like code.
    bool has_code_markers = selected_text.find("```") != std::string::npos ||
                            selected_text.find("function") != std::string::npos ||
                            selected_text.find("class ") != std::string::npos ||
                            selected_text.find("def ") != std::string::npos ||
                            selected_text.find("int ") != std::string::npos;
    if (has_code_markers)
    {
        actions.push_back(AIAction::ExplainCode);
    }

    return actions;
}

auto AIPanel::set_accept_callback(AIAcceptCallback callback) -> void
{
    accept_callback_ = std::move(callback);
}

auto AIPanel::accept_last_response() -> void
{
    if (accept_callback_ && !last_response_content_.empty())
    {
        accept_callback_(last_response_content_);
    }
}

auto AIPanel::current_session_id() const -> std::optional<std::string>
{
    return session_id_;
}

auto AIPanel::is_visible() const -> bool
{
    return visible_;
}

auto AIPanel::set_visible(bool visible) -> void
{
    visible_ = visible;
}

} // namespace markamp::core
