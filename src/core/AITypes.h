// ============================================================================
// File: src/core/AITypes.h
// Phase 40: Import & AI Integration — AI types and configuration
// ============================================================================
#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

// AI provider type.
enum class AIProvider : uint8_t
{
    OpenAI,    // OpenAI API (GPT-4, GPT-3.5, etc.)
    Anthropic, // Anthropic API (Claude)
    Local,     // Local model (llama.cpp, Ollama, etc.)
    Custom     // User-configured endpoint
};

// Structured error codes for AI API responses (#29).
enum class AIErrorCode : uint8_t
{
    None,              // No error
    NetworkError,      // Connection failure / DNS resolution / timeout
    AuthError,         // Invalid API key or unauthorized
    RateLimited,       // Rate limit exceeded (HTTP 429)
    InvalidRequest,    // Malformed request (HTTP 400)
    ModelNotFound,     // Requested model not available
    ContentFiltered,   // Content blocked by safety filters
    ContextTooLong,    // Input exceeds model's context window
    ServerError,       // Provider server error (HTTP 5xx)
    Cancelled,         // Request was cancelled by user
    ProviderUnavailable, // Provider endpoint unreachable
    ParseError,        // Failed to parse provider response
    BudgetExceeded,    // Monthly spending limit reached
    ConfigError        // Missing or invalid configuration
};

// AI action type.
enum class AIAction : uint8_t
{
    Chat,            // General conversation
    Summarize,       // Summarize content
    Translate,       // Translate to another language
    ExpandContent,   // Expand/elaborate on content
    SimplifyContent, // Simplify language
    FixGrammar,      // Grammar and spelling correction
    GenerateOutline, // Create document outline
    ExplainCode,     // Explain code block
    ContinueWriting, // Continue from current content
    ChangeTone,      // Change writing tone
    ExtractKeywords, // Extract keywords/tags
    GenerateTitle,   // Generate document title
    Custom           // Custom prompt
};

// Message role in a conversation.
enum class AIRole : uint8_t
{
    System,
    User,
    Assistant
};

// A single message in an AI conversation.
struct AIMessage
{
    AIRole role{AIRole::User};
    std::string content;
    int64_t timestamp{0};
    int32_t token_count{0}; // (#14) Token count for this message
};

// AI model configuration.
struct AIModelConfig
{
    AIProvider provider{AIProvider::OpenAI};
    std::string model_name{"gpt-4"}; // Model identifier
    std::string api_key;             // API key
    std::string api_base_url;        // Base URL (for custom/local)
    double temperature{0.7};         // Sampling temperature
    int32_t max_tokens{2048};        // Max response tokens
    double top_p{1.0};               // Nucleus sampling
    double frequency_penalty{0.0};
    double presence_penalty{0.0};
    std::string system_prompt;         // System message
    int32_t request_timeout_ms{30000}; // Request timeout in ms (#5)
    int32_t max_retries{3};            // Max retry attempts on transient failures (#6)
    int32_t retry_base_delay_ms{1000}; // Base delay for exponential backoff (#6)
};

// Request to the AI service.
struct AIRequest
{
    AIAction action{AIAction::Chat};
    std::vector<AIMessage> messages; // Conversation history
    std::string context;             // Current document context
    std::string selected_text;       // User selection
    std::string target_language;     // For translation
    std::string custom_prompt;       // For custom actions
    AIModelConfig model;
};

// Response from the AI service.
struct AIResponse
{
    bool success{false};
    std::string content;      // Generated content
    std::string model_used;   // Actual model name
    int32_t prompt_tokens{0}; // Token usage
    int32_t completion_tokens{0};
    int32_t total_tokens{0};
    int64_t elapsed_ms{0};
    std::string error_message;
    AIErrorCode error_code{AIErrorCode::None}; // Structured error code (#29)
    int32_t http_status{0};                    // Raw HTTP status code
    int32_t retry_after_ms{0};                 // Retry-after hint for rate limits
};

// Streaming callback: (partial_content, is_done).
using AIStreamCallback = std::function<void(const std::string&, bool)>;

// AI conversation session.
struct AISession
{
    std::string session_id;
    std::vector<AIMessage> messages;
    AIModelConfig model;
    int64_t created_at{0};
    int64_t last_active{0};
    int32_t total_tokens_used{0};

    auto add_message(AIRole role, const std::string& content) -> void
    {
        AIMessage msg;
        msg.role = role;
        msg.content = content;
        messages.push_back(std::move(msg));
    }

    /// Rough token estimate (~4 chars per token) for context window management (#11).
    [[nodiscard]] auto estimated_token_count() const -> int32_t
    {
        int32_t total = 0;
        for (const auto& msg : messages)
        {
            total += static_cast<int32_t>(msg.content.size()) / 4 + 1;
        }
        return total;
    }

    /// Prune oldest non-system messages to fit within token budget (#12).
    auto prune_to_token_limit(int32_t max_tokens) -> int
    {
        int pruned = 0;
        while (estimated_token_count() > max_tokens && messages.size() > 1)
        {
            // Find first non-system message to remove.
            auto iter = std::find_if(
                messages.begin(),
                messages.end(),
                [](const AIMessage& msg) { return msg.role != AIRole::System; });
            if (iter == messages.end())
            {
                break;
            }
            messages.erase(iter);
            ++pruned;
        }
        return pruned;
    }
};

} // namespace markamp::core

// ============================================================================
// Phase 26: AI Integration — extended types
// ============================================================================
namespace markamp::core
{

// AI provider connectivity status.
enum class AIProviderStatus : uint8_t
{
    kConnected,    // Provider is reachable and authenticated
    kDisconnected, // No network / provider down
    kRateLimited,  // Rate limit exceeded
    kNoApiKey      // No API key configured
};

// AI writing tone for tone adjustment actions.
enum class AIWritingTone : uint8_t
{
    kFormal,
    kCasual,
    kTechnical,
    kFriendly,
    kAcademic,
    kConcise
};

// Document generation style.
enum class DocumentStyle : uint8_t
{
    kArticle, // Structured prose with headings
    kNotes,   // Bullet-point notes
    kOutline, // Hierarchical outline
    kReport   // Formal report with sections
};

// Document generation state.
enum class GenerationState : uint8_t
{
    kIdle,
    kGenerating,
    kDone,
    kError,
    kCancelled
};

// Flashcard type.
enum class CardType : uint8_t
{
    kBasic, // Q&A pair
    kCloze  // Fill-in-the-blank
};

// API usage statistics for rate limiting and cost tracking.
struct AIUsageStats
{
    int64_t prompt_tokens{0};
    int64_t completion_tokens{0};
    int64_t total_tokens{0};
    int32_t request_count{0};
    double estimated_cost{0.0};
};

// User-defined custom AI prompt.
struct AICustomPrompt
{
    std::string name;
    std::string description;
    std::string system_prompt;
    std::vector<std::string> variables; // e.g. "{{selection}}", "{{file}}"
};

// Summary entry for a saved AI conversation.
struct AIConversationEntry
{
    std::string session_id;
    std::string title;
    int64_t created_at{0};
    int64_t last_active{0};
    int32_t message_count{0};
    std::string model_name;   // (#13) Model used for this conversation
    int32_t provider{0};      // (#13) AIProvider as int
    int32_t total_tokens{0};  // (#14) Cumulative token usage
};

// Inline completion suggestion (ghost text).
struct InlineSuggestion
{
    std::string text;
    double confidence{0.0};
    std::string request_id;
};

// Document generation progress status.
struct GenerationStatus
{
    GenerationState state{GenerationState::kIdle};
    double progress{0.0};
    std::string content_so_far;
    std::string error_message;
};

// AI-suggested tag for a document.
struct TagSuggestion
{
    std::string tag;
    double confidence{0.0};
    bool is_existing{false}; // Exists in workspace
    std::string reason;
};

// AI-suggested wiki-link.
struct LinkSuggestion
{
    std::string target_document;
    std::string anchor_text;
    double relevance{0.0};
    std::string reason;
};

// AI-generated flashcard.
struct GeneratedCard
{
    std::string front;
    std::string back;
    CardType card_type{CardType::kBasic};
    double difficulty{0.5};
};

// Semantic search result.
struct SemanticSearchResult
{
    std::string document_id;
    double relevance{0.0};
    std::string explanation;
    std::string snippet;
};

// AI answer with source citations.
struct AIAnswerResult
{
    std::string answer;
    std::vector<std::string> source_documents;
    double confidence{0.0};
};

} // namespace markamp::core
