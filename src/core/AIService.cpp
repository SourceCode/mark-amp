// ============================================================================
// File: src/core/AIService.cpp
// Phase 40: Import & AI Integration — AIService implementation
// ============================================================================

#include "AIService.h"

#include "Events.h"

#include <chrono>
#include <random>
#include <sstream>

namespace markamp::core
{

AIService::AIService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

auto AIService::complete(const AIRequest& request) -> AIResponse
{
    auto start = std::chrono::steady_clock::now();

    // Build messages list.
    std::vector<AIMessage> messages;

    // System prompt.
    if (!request.model.system_prompt.empty())
    {
        messages.push_back({AIRole::System, request.model.system_prompt, 0});
    }
    else if (request.action != AIAction::Chat)
    {
        messages.push_back({AIRole::System, build_system_prompt(request.action), 0});
    }

    // Context if provided.
    if (!request.context.empty())
    {
        messages.push_back({AIRole::User, "Document context:\n\n" + request.context, 0});
    }

    // Conversation history.
    for (const auto& msg : request.messages)
    {
        messages.push_back(msg);
    }

    // If using a predefined action, build the prompt.
    if (request.action != AIAction::Chat)
    {
        auto user_prompt =
            build_user_prompt(request.action, request.selected_text, request.context);
        if (!request.custom_prompt.empty())
        {
            user_prompt = request.custom_prompt;
        }
        messages.push_back({AIRole::User, user_prompt, 0});
    }

    // Call the API.
    auto response = call_api(request.model, messages);

    auto end = std::chrono::steady_clock::now();
    response.elapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return response;
}

auto AIService::stream(const AIRequest& request, AIStreamCallback callback) -> void
{
    // Stub: In production, use SSE/chunked HTTP.
    auto response = complete(request);
    if (callback)
    {
        callback(response.content, true);
    }
}

auto AIService::execute_action(AIAction action,
                               const std::string& content,
                               const std::string& context) -> AIResponse
{
    AIRequest request;
    request.action = action;
    request.selected_text = content;
    request.context = context;

    // Use default model from config.
    request.model.provider = AIProvider::OpenAI;
    request.model.model_name = "gpt-4";

    return complete(request);
}

auto AIService::create_session(const AIModelConfig& model) -> std::string
{
    std::lock_guard lock(mutex_);
    auto id = generate_session_id();

    AISession session;
    session.session_id = id;
    session.model = model;

    auto now = std::chrono::system_clock::now();
    session.created_at =
        std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    session.last_active = session.created_at;

    sessions_[id] = std::move(session);
    return id;
}

auto AIService::get_session(const std::string& session_id) -> AISession*
{
    std::lock_guard lock(mutex_);
    auto it = sessions_.find(session_id);
    return it != sessions_.end() ? &it->second : nullptr;
}

auto AIService::delete_session(const std::string& session_id) -> void
{
    std::lock_guard lock(mutex_);
    sessions_.erase(session_id);
}

auto AIService::list_sessions() const -> std::vector<std::string>
{
    std::lock_guard lock(mutex_);
    std::vector<std::string> ids;
    ids.reserve(sessions_.size());
    for (const auto& [id, _] : sessions_)
    {
        ids.push_back(id);
    }
    return ids;
}

auto AIService::chat(const std::string& session_id, const std::string& message) -> AIResponse
{
    auto* session = get_session(session_id);
    if (!session)
    {
        AIResponse resp;
        resp.error_message = "Session not found: " + session_id;
        return resp;
    }

    session->add_message(AIRole::User, message);

    AIRequest request;
    request.action = AIAction::Chat;
    request.messages = session->messages;
    request.model = session->model;

    auto response = complete(request);
    if (response.success)
    {
        session->add_message(AIRole::Assistant, response.content);
        session->total_tokens_used += response.total_tokens;
    }

    return response;
}

auto AIService::available_models(AIProvider provider) const -> std::vector<std::string>
{
    switch (provider)
    {
        case AIProvider::OpenAI:
            return {"gpt-4", "gpt-4-turbo", "gpt-3.5-turbo", "gpt-4o", "gpt-4o-mini"};
        case AIProvider::Anthropic:
            return {
                "claude-3-5-sonnet-20241022", "claude-3-haiku-20240307", "claude-3-opus-20240229"};
        case AIProvider::Local:
            return {"llama-3", "mistral-7b", "phi-3"};
        default:
            return {};
    }
}

auto AIService::test_connection(const AIModelConfig& model) -> std::expected<void, std::string>
{
    if (model.api_key.empty() && model.provider != AIProvider::Local)
    {
        return std::unexpected("API key is required");
    }
    // Stub: In production, make a test API call.
    return {};
}

// ── Private helpers ─────────────────────────────────────────────────────────

auto AIService::build_system_prompt(AIAction action) -> std::string
{
    switch (action)
    {
        case AIAction::Summarize:
            return "You are a concise summarizer. Provide clear, well-structured "
                   "summaries that capture the key points.";
        case AIAction::Translate:
            return "You are a professional translator. Provide accurate, natural "
                   "translations preserving the original meaning and tone.";
        case AIAction::ExpandContent:
            return "You are a writing assistant. Expand on the given content with "
                   "additional details, examples, and explanations while "
                   "maintaining the original tone.";
        case AIAction::SimplifyContent:
            return "You are a clarity editor. Simplify the language to be clear "
                   "and accessible, targeting a general audience.";
        case AIAction::FixGrammar:
            return "You are a grammar and spelling editor. Fix errors while "
                   "preserving the author's voice and style. Return only the "
                   "corrected text.";
        case AIAction::GenerateOutline:
            return "You are an outline generator. Create a structured outline "
                   "with headings and subheadings in Markdown format.";
        case AIAction::ExplainCode:
            return "You are a code explainer. Provide clear, detailed "
                   "explanations of code, including purpose, logic, and any "
                   "important patterns.";
        case AIAction::ContinueWriting:
            return "You are a writing assistant. Continue the text naturally, "
                   "matching the style, tone, and topic of the existing content.";
        case AIAction::ChangeTone:
            return "You are a tone editor. Rewrite the text to change its tone "
                   "while preserving the meaning.";
        case AIAction::ExtractKeywords:
            return "Extract the key topics and keywords from the text. Return "
                   "them as a comma-separated list.";
        case AIAction::GenerateTitle:
            return "Generate a concise, descriptive title for the given content. "
                   "Return only the title.";
        default:
            return "You are a helpful writing assistant.";
    }
}

auto AIService::build_user_prompt(AIAction action,
                                  const std::string& content,
                                  const std::string& context) -> std::string
{
    (void)context;
    switch (action)
    {
        case AIAction::Summarize:
            return "Summarize the following:\n\n" + content;
        case AIAction::Translate:
            return "Translate the following:\n\n" + content;
        case AIAction::ExpandContent:
            return "Expand on the following:\n\n" + content;
        case AIAction::SimplifyContent:
            return "Simplify the following:\n\n" + content;
        case AIAction::FixGrammar:
            return "Fix grammar and spelling:\n\n" + content;
        case AIAction::GenerateOutline:
            return "Generate an outline for:\n\n" + content;
        case AIAction::ExplainCode:
            return "Explain this code:\n\n```\n" + content + "\n```";
        case AIAction::ContinueWriting:
            return "Continue writing from:\n\n" + content;
        case AIAction::ExtractKeywords:
            return "Extract keywords from:\n\n" + content;
        case AIAction::GenerateTitle:
            return "Generate a title for:\n\n" + content;
        default:
            return content;
    }
}

auto AIService::call_api(const AIModelConfig& model, const std::vector<AIMessage>& messages)
    -> AIResponse
{
    // Stub: In production, use libcurl to make HTTP requests.
    // Build JSON request body and POST to the appropriate endpoint.
    (void)messages;
    AIResponse response;

    // Determine endpoint.
    std::string endpoint;
    switch (model.provider)
    {
        case AIProvider::OpenAI:
            endpoint = model.api_base_url.empty() ? "https://api.openai.com/v1/chat/completions"
                                                  : model.api_base_url + "/chat/completions";
            break;
        case AIProvider::Anthropic:
            endpoint = model.api_base_url.empty() ? "https://api.anthropic.com/v1/messages"
                                                  : model.api_base_url + "/messages";
            break;
        case AIProvider::Local:
            endpoint =
                model.api_base_url.empty() ? "http://localhost:11434/api/chat" : model.api_base_url;
            break;
        default:
            response.error_message = "Unknown provider";
            return response;
    }

    // Stub response for compilation.
    response.success = false;
    response.error_message = "API integration requires libcurl. "
                             "Configure API key and endpoint.";
    response.model_used = model.model_name;

    return response;
}

auto AIService::parse_response(const std::string& json_response, AIProvider provider) -> AIResponse
{
    // Stub: In production, parse JSON using nlohmann/json or similar.
    AIResponse response;
    (void)json_response;
    (void)provider;
    response.error_message = "JSON parsing not yet implemented";
    return response;
}

auto AIService::generate_session_id() -> std::string
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 15);
    const char hex_chars[] = "0123456789abcdef";

    std::string id = "ai-";
    for (int i = 0; i < 16; ++i)
    {
        id += hex_chars[dist(gen)];
    }
    return id;
}

} // namespace markamp::core
