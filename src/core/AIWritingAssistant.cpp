// ============================================================================
// File: src/core/AIWritingAssistant.cpp
// Phase 26: AI Integration — writing assistance implementation
// ============================================================================

#include "AIWritingAssistant.h"

#include "AIService.h"

namespace markamp::core
{

AIWritingAssistant::AIWritingAssistant(AIService& ai_service)
    : ai_service_(ai_service)
{
}

auto AIWritingAssistant::continue_writing(const std::string& context, int cursor_pos) -> AIResponse
{
    std::string effective_context = context;
    if (cursor_pos >= 0 && cursor_pos < static_cast<int>(context.size()))
    {
        effective_context = context.substr(0, static_cast<size_t>(cursor_pos));
    }
    return ai_service_.execute_action(AIAction::ContinueWriting, effective_context);
}

auto AIWritingAssistant::improve_writing(const std::string& selected_text,
                                         const std::string& context) -> AIResponse
{
    return ai_service_.execute_action(AIAction::ExpandContent, selected_text, context);
}

auto AIWritingAssistant::fix_grammar(const std::string& text) -> AIResponse
{
    return ai_service_.execute_action(AIAction::FixGrammar, text);
}

auto AIWritingAssistant::simplify(const std::string& text) -> AIResponse
{
    return ai_service_.execute_action(AIAction::SimplifyContent, text);
}

auto AIWritingAssistant::expand(const std::string& text, const std::string& context) -> AIResponse
{
    return ai_service_.execute_action(AIAction::ExpandContent, text, context);
}

auto AIWritingAssistant::summarize(const std::string& text) -> AIResponse
{
    return ai_service_.execute_action(AIAction::Summarize, text);
}

auto AIWritingAssistant::adjust_tone(const std::string& text, AIWritingTone tone) -> AIResponse
{
    const std::string tone_context = "Target tone: " + tone_to_string(tone);
    return ai_service_.execute_action(AIAction::ChangeTone, text, tone_context);
}

auto AIWritingAssistant::translate(const std::string& text, const std::string& target_language)
    -> AIResponse
{
    AIRequest request;
    request.action = AIAction::Translate;
    request.selected_text = text;
    request.target_language = target_language;
    request.context = "Preserve all Markdown formatting in the translation.";
    return ai_service_.complete(request);
}

auto AIWritingAssistant::custom_prompt(const std::string& text, const std::string& prompt)
    -> AIResponse
{
    AIRequest request;
    request.action = AIAction::Custom;
    request.selected_text = text;
    request.custom_prompt = prompt;
    return ai_service_.complete(request);
}

auto AIWritingAssistant::available_tones() -> std::vector<std::string>
{
    return {"Formal", "Casual", "Technical", "Friendly", "Academic", "Concise"};
}

auto AIWritingAssistant::available_languages() -> std::vector<std::pair<std::string, std::string>>
{
    return {
        {"es", "Spanish"},    {"fr", "French"},  {"de", "German"},  {"it", "Italian"},
        {"pt", "Portuguese"}, {"nl", "Dutch"},   {"ru", "Russian"}, {"zh", "Chinese"},
        {"ja", "Japanese"},   {"ko", "Korean"},  {"ar", "Arabic"},  {"hi", "Hindi"},
        {"pl", "Polish"},     {"sv", "Swedish"}, {"da", "Danish"},  {"no", "Norwegian"},
        {"fi", "Finnish"},    {"tr", "Turkish"}, {"cs", "Czech"},   {"uk", "Ukrainian"},
        {"vi", "Vietnamese"},
    };
}

auto AIWritingAssistant::tone_to_string(AIWritingTone tone) -> std::string
{
    switch (tone)
    {
        case AIWritingTone::kFormal:
            return "Formal";
        case AIWritingTone::kCasual:
            return "Casual";
        case AIWritingTone::kTechnical:
            return "Technical";
        case AIWritingTone::kFriendly:
            return "Friendly";
        case AIWritingTone::kAcademic:
            return "Academic";
        case AIWritingTone::kConcise:
            return "Concise";
    }
    return "Formal";
}

} // namespace markamp::core
