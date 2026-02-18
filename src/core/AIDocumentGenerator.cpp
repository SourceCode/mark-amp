// ============================================================================
// File: src/core/AIDocumentGenerator.cpp
// Phase 26: AI Integration — document generation implementation
// ============================================================================

#include "AIDocumentGenerator.h"

#include "AIService.h"
#include "Events.h"

#include <sstream>

namespace markamp::core
{

AIDocumentGenerator::AIDocumentGenerator(AIService& ai_service, EventBus& event_bus)
    : ai_service_(ai_service)
    , event_bus_(event_bus)
{
}

auto AIDocumentGenerator::generate(const std::string& topic, DocumentStyle style) -> std::string
{
    generating_.store(true);
    cancel_requested_.store(false);
    status_.state = GenerationState::kGenerating;
    status_.progress = 0.0;
    status_.content_so_far.clear();
    status_.error_message.clear();

    AIRequest request;
    request.action = AIAction::Custom;
    request.custom_prompt = build_style_prompt(style);
    request.context = topic;

    const auto response = ai_service_.complete(request);

    if (cancel_requested_.load())
    {
        status_.state = GenerationState::kCancelled;
        generating_.store(false);
        return status_.content_so_far;
    }

    if (response.success)
    {
        status_.state = GenerationState::kDone;
        status_.progress = 1.0;
        status_.content_so_far = response.content;

        // Count words for event.
        std::istringstream stream(response.content);
        std::string word;
        int word_count = 0;
        while (stream >> word)
        {
            ++word_count;
        }

        events::AIDocumentGeneratedEvent evt;
        evt.topic = topic;
        evt.word_count = word_count;
        event_bus_.publish(evt);
    }
    else
    {
        status_.state = GenerationState::kError;
        status_.error_message = response.error_message;
    }

    generating_.store(false);
    return status_.content_so_far;
}

auto AIDocumentGenerator::generate_streaming(const std::string& topic,
                                             DocumentStyle style,
                                             AIStreamCallback callback) -> void
{
    generating_.store(true);
    cancel_requested_.store(false);
    status_.state = GenerationState::kGenerating;
    status_.progress = 0.0;
    status_.content_so_far.clear();

    AIRequest request;
    request.action = AIAction::Custom;
    request.custom_prompt = build_style_prompt(style);
    request.context = topic;

    ai_service_.stream(request,
                       [this, callback](const std::string& partial, bool is_done)
                       {
                           if (cancel_requested_.load())
                           {
                               status_.state = GenerationState::kCancelled;
                               generating_.store(false);
                               if (callback)
                               {
                                   callback("", true);
                               }
                               return;
                           }

                           status_.content_so_far += partial;
                           if (is_done)
                           {
                               status_.state = GenerationState::kDone;
                               status_.progress = 1.0;
                               generating_.store(false);
                           }
                           if (callback)
                           {
                               callback(partial, is_done);
                           }
                       });
}

auto AIDocumentGenerator::cancel() -> void
{
    cancel_requested_.store(true);
}

auto AIDocumentGenerator::is_generating() const -> bool
{
    return generating_.load();
}

auto AIDocumentGenerator::last_status() const -> GenerationStatus
{
    return status_;
}

auto AIDocumentGenerator::style_name(DocumentStyle style) -> std::string
{
    switch (style)
    {
        case DocumentStyle::kArticle:
            return "Article";
        case DocumentStyle::kNotes:
            return "Notes";
        case DocumentStyle::kOutline:
            return "Outline";
        case DocumentStyle::kReport:
            return "Report";
    }
    return "Article";
}

auto AIDocumentGenerator::build_style_prompt(DocumentStyle style) -> std::string
{
    switch (style)
    {
        case DocumentStyle::kArticle:
            return "Generate a well-structured Markdown article with headings, paragraphs, "
                   "and clear prose. Include an introduction, body sections with h2/h3 headings, "
                   "and a conclusion. Use proper Markdown formatting throughout.";
        case DocumentStyle::kNotes:
            return "Generate concise bullet-point notes in Markdown format. Use nested lists "
                   "for sub-topics. Keep each point brief and actionable. Use checkboxes for "
                   "action items where appropriate.";
        case DocumentStyle::kOutline:
            return "Generate a hierarchical outline in Markdown format. Use heading levels "
                   "(h1-h4) to show structure. Include brief descriptions under each heading. "
                   "Focus on logical organization and flow.";
        case DocumentStyle::kReport:
            return "Generate a formal report in Markdown format. Include: title, executive "
                   "summary, introduction, methodology, findings, analysis, conclusions, and "
                   "recommendations. Use tables and lists where appropriate.";
    }
    return "Generate a Markdown document on the given topic.";
}

} // namespace markamp::core
