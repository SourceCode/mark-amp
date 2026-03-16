// ============================================================================
// File: src/core/AIService.cpp
// Phase 40: Import & AI Integration — AIService implementation
// ============================================================================

#include "AIService.h"

#include "Events.h"

#include <array>
#include <chrono>
#include <random>
#include <sstream>
#include <thread>

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
    // (#1) Chunked streaming: split completed response into smaller chunks
    // for progressive UI display. Real SSE would use event-stream parsing.
    auto response = complete(request);
    if (!callback)
    {
        return;
    }
    if (!response.success)
    {
        callback(response.error_message, true);
        return;
    }

    // Simulate streaming by delivering content in word-boundary chunks.
    const auto& content = response.content;
    constexpr size_t kChunkSize = 12; // ~3 words per chunk
    size_t pos = 0;
    while (pos < content.size())
    {
        // Find a word boundary near chunk_size.
        size_t end = std::min(pos + kChunkSize, content.size());
        if (end < content.size())
        {
            // Extend to next space to avoid splitting words.
            auto space = content.find(' ', end);
            if (space != std::string::npos && space - pos < kChunkSize * 2)
            {
                end = space + 1;
            }
        }
        const bool is_done = (end >= content.size());
        callback(content.substr(pos, end - pos), is_done);
        pos = end;
    }
    if (pos == 0)
    {
        callback("", true);
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
    const std::lock_guard lock(mutex_);
    auto session_id_val = generate_session_id();

    AISession session;
    session.session_id = session_id_val;
    session.model = model;

    auto now = std::chrono::system_clock::now();
    session.created_at =
        std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    session.last_active = session.created_at;

    sessions_[session_id_val] = std::move(session);
    return session_id_val;
}

auto AIService::get_session(const std::string& session_id) -> AISession*
{
    const std::lock_guard lock(mutex_);
    auto iter = sessions_.find(session_id);
    return iter != sessions_.end() ? &iter->second : nullptr;
}

auto AIService::delete_session(const std::string& session_id) -> void
{
    const std::lock_guard lock(mutex_);
    sessions_.erase(session_id);
}

auto AIService::list_sessions() const -> std::vector<std::string>
{
    const std::lock_guard lock(mutex_);
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
    if (session == nullptr)
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
    // (#2) Validate configuration before attempting connection.
    if (model.api_key.empty() && model.provider != AIProvider::Local)
    {
        return std::unexpected("API key is required for " +
                               provider_name(model.provider) + " provider");
    }

    if (model.model_name.empty())
    {
        return std::unexpected("Model name is required");
    }

    // Validate endpoint URL format (#8).
    if (!model.api_base_url.empty())
    {
        if (model.api_base_url.find("http://") != 0 &&
            model.api_base_url.find("https://") != 0)
        {
            return std::unexpected("API base URL must start with http:// or https://");
        }
    }

    // Attempt a minimal API call to verify connectivity.
    std::vector<AIMessage> test_messages;
    test_messages.push_back({AIRole::User, "Hello", 0});

    AIModelConfig test_model = model;
    test_model.max_tokens = 1; // Minimal token usage for test

    auto response = call_api(test_model, test_messages);
    if (!response.success && response.error_code != AIErrorCode::None)
    {
        return std::unexpected("Connection test failed: " + response.error_message);
    }

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
    // (#1-4) Build proper JSON request body for provider dispatch.
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
            response.error_code = AIErrorCode::ConfigError;
            publish_error_notification(response);
            return response;
    }

    // Build JSON request body (#3).
    const std::string json_body = build_request_json(model, messages);

    // Retry loop with exponential backoff (#6).
    int32_t attempts = 0;
    const int32_t kMaxAttempts = std::max(1, model.max_retries);
    int32_t delay_ms = model.retry_base_delay_ms;

    while (attempts < kMaxAttempts)
    {
        ++attempts;

        // Check cancellation (#7).
        if (cancel_requested_.load())
        {
            response.error_message = "Request cancelled";
            response.error_code = AIErrorCode::Cancelled;
            return response;
        }

        // (#1) In production, this would use libcurl to POST json_body to endpoint
        // with Authorization header and timeout. For now, we construct the full
        // request but return a structured error since libcurl isn't linked.
        response.success = false;
        response.error_code = AIErrorCode::ProviderUnavailable;
        response.error_message =
            "HTTP transport not yet linked. Endpoint: " + endpoint +
            ", body size: " + std::to_string(json_body.size()) + " bytes";
        response.model_used = model.model_name;

        // Determine if this is a retryable error (#6).
        const bool retryable = (response.error_code == AIErrorCode::NetworkError ||
                          response.error_code == AIErrorCode::ServerError ||
                          response.error_code == AIErrorCode::RateLimited);

        if (response.success || !retryable || attempts >= kMaxAttempts)
        {
            break;
        }

        // Exponential backoff with jitter (#6).
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        delay_ms = std::min(delay_ms * 2, 30000); // Cap at 30 seconds
    }

    // Emit notification on error (#30).
    if (!response.success)
    {
        publish_error_notification(response);
    }

    return response;
}

auto AIService::parse_response(const std::string& json_response, AIProvider provider) -> AIResponse
{
    AIResponse response;

    if (json_response.empty())
    {
        response.error_message = "Empty response from API";
        return response;
    }

    // Simple JSON field extraction (production should use nlohmann/json).
    auto extract_field = [](const std::string& json, const std::string& key) -> std::string
    {
        const std::string search_key = "\"" + key + "\"";
        auto key_pos = json.find(search_key);
        if (key_pos == std::string::npos)
        {
            return "";
        }
        auto colon_pos = json.find(':', key_pos + search_key.size());
        if (colon_pos == std::string::npos)
        {
            return "";
        }
        // Skip whitespace after colon.
        auto val_start = json.find_first_not_of(" \t\n\r", colon_pos + 1);
        if (val_start == std::string::npos)
        {
            return "";
        }
        if (json[val_start] == '"')
        {
            // String value.
            auto val_end = json.find('"', val_start + 1);
            if (val_end == std::string::npos)
            {
                return "";
            }
            return json.substr(val_start + 1, val_end - val_start - 1);
        }
        // Numeric or other value.
        auto val_end = json.find_first_of(",}\n\r", val_start);
        if (val_end == std::string::npos)
        {
            val_end = json.size();
        }
        return json.substr(val_start, val_end - val_start);
    };

    auto extract_int = [&extract_field](const std::string& json, const std::string& key) -> int
    {
        auto val = extract_field(json, key);
        if (val.empty())
        {
            return 0;
        }
        try
        {
            return std::stoi(val);
        }
        catch (...)
        {
            return 0;
        }
    };

    switch (provider)
    {
        case AIProvider::OpenAI:
        {
            // OpenAI format: {"choices":[{"message":{"content":"..."}, "finish_reason":"stop"}],
            //                 "usage":{"prompt_tokens":N,"completion_tokens":N,"total_tokens":N},
            //                 "model":"gpt-4"}
            response.content = extract_field(json_response, "content");
            response.model_used = extract_field(json_response, "model");
            response.prompt_tokens = extract_int(json_response, "prompt_tokens");
            response.completion_tokens = extract_int(json_response, "completion_tokens");
            response.total_tokens = extract_int(json_response, "total_tokens");
            response.success = !response.content.empty();
            break;
        }
        case AIProvider::Anthropic:
        {
            // Anthropic format: {"content":[{"text":"..."}], "stop_reason":"end_turn",
            //                    "usage":{"input_tokens":N,"output_tokens":N}, "model":"claude-..."}
            response.content = extract_field(json_response, "text");
            response.model_used = extract_field(json_response, "model");
            response.prompt_tokens = extract_int(json_response, "input_tokens");
            response.completion_tokens = extract_int(json_response, "output_tokens");
            response.total_tokens = response.prompt_tokens + response.completion_tokens;
            response.success = !response.content.empty();
            break;
        }
        case AIProvider::Local:
        {
            // Ollama format: {"message":{"content":"..."}, "model":"llama3"}
            response.content = extract_field(json_response, "content");
            response.model_used = extract_field(json_response, "model");
            response.success = !response.content.empty();
            break;
        }
        default:
            response.error_message = "Unknown provider for response parsing";
            break;
    }

    // Check for API error responses.
    if (!response.success)
    {
        auto error_msg = extract_field(json_response, "error");
        if (!error_msg.empty())
        {
            response.error_message = error_msg;
        }
        else if (response.error_message.empty())
        {
            response.error_message = "Failed to parse response content";
        }
    }

    return response;
}

auto AIService::generate_session_id() -> std::string
{
    std::random_device rng;
    std::mt19937 gen(rng());
    std::uniform_int_distribution<int> dist(0, 15);
    static constexpr std::array<char, 16> kHexChars = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    std::string sid = "ai-";
    for (int idx = 0; idx < 16; ++idx)
    {
        sid += kHexChars[static_cast<size_t>(dist(gen))];
    }
    return sid;
}

// (#7) Cancel any in-flight request.
auto AIService::abort_generation() -> void
{
    cancel_requested_.store(true);
}

// Helper: human-readable provider name.
auto AIService::provider_name(AIProvider provider) -> std::string
{
    switch (provider)
    {
        case AIProvider::OpenAI:
            return "OpenAI";
        case AIProvider::Anthropic:
            return "Anthropic";
        case AIProvider::Local:
            return "Local";
        case AIProvider::Custom:
            return "Custom";
    }
    return "Unknown";
}

// (#3) Build JSON request body for a specific provider.
auto AIService::build_request_json(const AIModelConfig& model,
                                    const std::vector<AIMessage>& messages) -> std::string
{
    // Helper to escape JSON string values.
    auto escape_json = [](const std::string& input) -> std::string
    {
        std::string output;
        output.reserve(input.size() + input.size() / 8);
        for (char chr : input)
        {
            switch (chr)
            {
                case '"':
                    output += "\\\"";
                    break;
                case '\\':
                    output += "\\\\";
                    break;
                case '\n':
                    output += "\\n";
                    break;
                case '\r':
                    output += "\\r";
                    break;
                case '\t':
                    output += "\\t";
                    break;
                default:
                    output += chr;
                    break;
            }
        }
        return output;
    };

    // Helper to get role string.
    auto role_str = [](AIRole role) -> std::string
    {
        switch (role)
        {
            case AIRole::System:
                return "system";
            case AIRole::User:
                return "user";
            case AIRole::Assistant:
                return "assistant";
        }
        return "user";
    };

    std::ostringstream json;

    switch (model.provider)
    {
        case AIProvider::OpenAI:
        case AIProvider::Local:
        case AIProvider::Custom:
        {
            // OpenAI Chat Completions format (also used by Ollama and custom endpoints).
            json << "{\"model\":\"" << escape_json(model.model_name) << "\",";
            json << "\"messages\":[";
            for (size_t idx = 0; idx < messages.size(); ++idx)
            {
                if (idx > 0)
                {
                    json << ",";
                }
                json << "{\"role\":\"" << role_str(messages[idx].role) << "\",";
                json << "\"content\":\"" << escape_json(messages[idx].content) << "\"}";
            }
            json << "],";
            json << "\"temperature\":" << model.temperature << ",";
            json << "\"max_tokens\":" << model.max_tokens << ",";
            json << "\"top_p\":" << model.top_p;
            if (model.frequency_penalty != 0.0)
            {
                json << ",\"frequency_penalty\":" << model.frequency_penalty;
            }
            if (model.presence_penalty != 0.0)
            {
                json << ",\"presence_penalty\":" << model.presence_penalty;
            }
            json << "}";
            break;
        }
        case AIProvider::Anthropic:
        {
            // Anthropic Messages API format.
            json << "{\"model\":\"" << escape_json(model.model_name) << "\",";

            // Separate system prompt from messages.
            for (const auto& msg : messages)
            {
                if (msg.role == AIRole::System)
                {
                    json << "\"system\":\"" << escape_json(msg.content) << "\",";
                    break;
                }
            }

            json << "\"messages\":[";
            bool first = true;
            for (const auto& msg : messages)
            {
                if (msg.role == AIRole::System)
                {
                    continue; // Already handled above.
                }
                if (!first)
                {
                    json << ",";
                }
                first = false;
                std::string role = (msg.role == AIRole::User) ? "user" : "assistant";
                json << "{\"role\":\"" << role << "\",";
                json << "\"content\":\"" << escape_json(msg.content) << "\"}";
            }
            json << "],";
            json << "\"max_tokens\":" << model.max_tokens << ",";
            json << "\"temperature\":" << model.temperature;
            json << "}";
            break;
        }
    }

    return json.str();
}

// (#30) Emit a NotificationEvent for API errors.
auto AIService::publish_error_notification(const AIResponse& response) -> void
{
    events::NotificationEvent evt;
    evt.message = "AI request failed: " + response.error_message;
    evt.level = events::NotificationLevel::Error;
    evt.duration_ms = 5000;
    event_bus_.publish(evt);
}

// ── Batch 34 (#196-198) ─────────────────────────────────────────────────────

/// (#196) Return the number of active chat sessions.
auto AIService::session_count() const -> std::size_t
{
    const std::lock_guard lock(mutex_);
    return sessions_.size();
}

/// (#197) Check if an abort/cancel has been requested.
auto AIService::is_cancel_requested() const -> bool
{
    return cancel_requested_.load();
}

/// (#198) Return the number of predefined AI actions (excluding Chat and Custom).
auto AIService::action_count() -> std::size_t
{
    // Summarize, Translate, ExpandContent, SimplifyContent, FixGrammar,
    // GenerateOutline, ExplainCode, ContinueWriting, ChangeTone,
    // ExtractKeywords, GenerateTitle = 11
    return 11;
}

} // namespace markamp::core
