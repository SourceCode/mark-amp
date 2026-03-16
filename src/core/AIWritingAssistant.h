// ============================================================================
// File: src/core/AIWritingAssistant.h
// Phase 26: AI Integration — editor-level writing assistance
// ============================================================================
#pragma once

#include "AITypes.h"

#include <string>
#include <utility>
#include <vector>

namespace markamp::core
{

class AIService;

/// Editor-level AI writing assistance: continue, improve, fix grammar,
/// simplify, expand, summarize, adjust tone, translate, custom prompts.
class AIWritingAssistant
{
public:
    explicit AIWritingAssistant(AIService& ai_service);

    /// Generate text continuation after cursor position.
    [[nodiscard]] auto continue_writing(const std::string& context, int cursor_pos = -1)
        -> AIResponse;

    /// Rewrite selected text with improvements.
    [[nodiscard]] auto improve_writing(const std::string& selected_text,
                                       const std::string& context = "") -> AIResponse;

    /// Fix grammar and spelling errors.
    [[nodiscard]] auto fix_grammar(const std::string& text) -> AIResponse;

    /// Simplify language to be more accessible.
    [[nodiscard]] auto simplify(const std::string& text) -> AIResponse;

    /// Expand content with more detail.
    [[nodiscard]] auto expand(const std::string& text, const std::string& context = "")
        -> AIResponse;

    /// Create a summary of the given text.
    [[nodiscard]] auto summarize(const std::string& text) -> AIResponse;

    /// Adjust writing tone (formal, casual, technical, etc.).
    [[nodiscard]] auto adjust_tone(const std::string& text, AIWritingTone tone) -> AIResponse;

    /// Translate text to another language, preserving Markdown formatting.
    [[nodiscard]] auto translate(const std::string& text, const std::string& target_language)
        -> AIResponse;

    /// Execute a user-defined custom prompt.
    [[nodiscard]] auto custom_prompt(const std::string& text, const std::string& prompt)
        -> AIResponse;

    /// Get available tone options.
    [[nodiscard]] static auto available_tones() -> std::vector<std::string>;

    /// Get available languages as (code, display_name) pairs.
    [[nodiscard]] static auto available_languages()
        -> std::vector<std::pair<std::string, std::string>>;

    /// Get the AIAction enum for a tone value.
    [[nodiscard]] static auto tone_to_string(AIWritingTone tone) -> std::string;

    /// (#201) Return the number of available writing tones.
    [[nodiscard]] static auto tone_count() -> std::size_t;

    /// (#202) Return the number of available translation languages.
    [[nodiscard]] static auto language_count() -> std::size_t;

private:
    AIService& ai_service_;
};

} // namespace markamp::core
