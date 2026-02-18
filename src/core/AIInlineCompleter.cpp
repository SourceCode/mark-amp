// ============================================================================
// File: src/core/AIInlineCompleter.cpp
// Phase 26: AI Integration — inline completion implementation
// ============================================================================

#include "AIInlineCompleter.h"

#include "AIService.h"
#include "Config.h"

#include <fmt/format.h>

namespace markamp::core
{

AIInlineCompleter::AIInlineCompleter(AIService& ai_service, Config& config)
    : ai_service_(ai_service)
    , config_(config)
{
    enabled_ = config_.get_bool("ai.inline_completion.enabled", true);
    debounce_ms_ = config_.get_int("ai.inline_completion.debounce_ms", 1500);
}

auto AIInlineCompleter::request_completion(const std::string& context_before,
                                           const std::string& context_after)
    -> std::expected<InlineSuggestion, std::string>
{
    if (!enabled_)
    {
        return std::unexpected("Inline completion is disabled");
    }

    ++total_requests_;

    AIRequest request;
    request.action = AIAction::ContinueWriting;
    request.context = context_before;
    if (!context_after.empty())
    {
        request.selected_text = context_after;
    }

    const auto response = ai_service_.complete(request);
    if (!response.success)
    {
        return std::unexpected(response.error_message.empty() ? "Completion failed"
                                                              : response.error_message);
    }

    const auto req_id = generate_request_id();
    current_request_id_ = req_id;
    remaining_text_ = response.content;

    InlineSuggestion suggestion;
    suggestion.text = response.content;
    suggestion.confidence = response.total_tokens > 0 ? 0.8 : 0.5;
    suggestion.request_id = req_id;
    return suggestion;
}

auto AIInlineCompleter::accept_suggestion(const std::string& request_id) -> void
{
    if (request_id == current_request_id_)
    {
        ++accepted_count_;
        current_request_id_.clear();
        remaining_text_.clear();
    }
}

auto AIInlineCompleter::accept_word(const std::string& request_id) -> std::string
{
    if (request_id != current_request_id_ || remaining_text_.empty())
    {
        return "";
    }

    // Find the end of the next word (skip leading whitespace, then find next space).
    size_t pos = 0;
    // Skip leading whitespace.
    while (pos < remaining_text_.size() && std::isspace(remaining_text_[pos]) != 0)
    {
        ++pos;
    }
    // Find end of word.
    while (pos < remaining_text_.size() && std::isspace(remaining_text_[pos]) == 0)
    {
        ++pos;
    }

    const std::string accepted_word = remaining_text_.substr(0, pos);
    remaining_text_ = remaining_text_.substr(pos);

    if (remaining_text_.empty())
    {
        ++accepted_count_;
        current_request_id_.clear();
    }

    return accepted_word;
}

auto AIInlineCompleter::dismiss(const std::string& request_id) -> void
{
    if (request_id == current_request_id_)
    {
        ++dismissed_count_;
        current_request_id_.clear();
        remaining_text_.clear();
    }
}

auto AIInlineCompleter::is_enabled() const -> bool
{
    return enabled_;
}

auto AIInlineCompleter::set_enabled(bool enabled) -> void
{
    enabled_ = enabled;
}

auto AIInlineCompleter::debounce_ms() const -> int
{
    return debounce_ms_;
}

auto AIInlineCompleter::set_debounce_ms(int milliseconds) -> void
{
    debounce_ms_ = milliseconds;
}

auto AIInlineCompleter::acceptance_rate() const -> double
{
    if (total_requests_ == 0)
    {
        return 0.0;
    }
    return static_cast<double>(accepted_count_) / static_cast<double>(total_requests_);
}

auto AIInlineCompleter::pending_text(const std::string& request_id) const -> std::string
{
    if (request_id != current_request_id_)
    {
        return "";
    }
    return remaining_text_;
}

auto AIInlineCompleter::generate_request_id() -> std::string
{
    return fmt::format("inline-{}", next_request_id_++);
}

} // namespace markamp::core
