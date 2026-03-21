/// @file ContentStyleSystem.cpp
/// @brief V20 P08-T01/T04/T05: Content style system implementation.

#include "ContentStyleSystem.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

ContentStyleSystem::ContentStyleSystem(EventBus& bus)
    : event_bus_(bus)
{
    register_defaults();
}

void ContentStyleSystem::register_spacing(const SpacingToken& token)
{
    spacing_[token.name] = token;
    MARKAMP_LOG_DEBUG("Spacing token registered: {} ({}px)", token.name, token.value_px);
}

void ContentStyleSystem::register_typography(const TypographyToken& token)
{
    typography_[token.name] = token;
    MARKAMP_LOG_DEBUG("Typography token registered: {} ({}px/{}px)", token.name,
                     token.font_size_px, token.line_height_px);
}

void ContentStyleSystem::register_feedback_style(const FeedbackStyleToken& token)
{
    feedback_styles_[token.scope] = token;

    events::FeedbackStyleRegisteredEvent evt;
    evt.scope = token.scope;
    evt.foreground = token.foreground;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Feedback style registered: {}", token.scope);
}

auto ContentStyleSystem::spacing(const std::string& name) const -> const SpacingToken*
{
    auto it = spacing_.find(name);
    return it != spacing_.end() ? &it->second : nullptr;
}

auto ContentStyleSystem::typography(const std::string& name) const -> const TypographyToken*
{
    auto it = typography_.find(name);
    return it != typography_.end() ? &it->second : nullptr;
}

auto ContentStyleSystem::feedback_style(const std::string& scope) const -> const FeedbackStyleToken*
{
    auto it = feedback_styles_.find(scope);
    return it != feedback_styles_.end() ? &it->second : nullptr;
}

auto ContentStyleSystem::all_spacing() const -> std::vector<SpacingToken>
{
    std::vector<SpacingToken> result;
    result.reserve(spacing_.size());
    for (const auto& [name, token] : spacing_)
    {
        result.push_back(token);
    }
    return result;
}

auto ContentStyleSystem::all_typography() const -> std::vector<TypographyToken>
{
    std::vector<TypographyToken> result;
    result.reserve(typography_.size());
    for (const auto& [name, token] : typography_)
    {
        result.push_back(token);
    }
    return result;
}

auto ContentStyleSystem::all_feedback_styles() const -> std::vector<FeedbackStyleToken>
{
    std::vector<FeedbackStyleToken> result;
    result.reserve(feedback_styles_.size());
    for (const auto& [scope, token] : feedback_styles_)
    {
        result.push_back(token);
    }
    return result;
}

void ContentStyleSystem::register_defaults()
{
    // Spacing scale
    register_spacing({"spacing.xxs", 2});
    register_spacing({"spacing.xs", 4});
    register_spacing({"spacing.sm", 8});
    register_spacing({"spacing.md", 12});
    register_spacing({"spacing.lg", 16});
    register_spacing({"spacing.xl", 24});
    register_spacing({"spacing.xxl", 32});

    // Typography scale
    register_typography({"type.caption", FontRole::kProportional, 11, 16, 0});
    register_typography({"type.body", FontRole::kProportional, 13, 20, 0});
    register_typography({"type.subheading", FontRole::kProportional, 14, 22, 0});
    register_typography({"type.heading", FontRole::kProportional, 18, 26, 0});
    register_typography({"type.title", FontRole::kProportional, 24, 32, 0});
    register_typography({"type.code", FontRole::kMonospace, 13, 20, 0});
    register_typography({"type.editor", FontRole::kEditorContent, 14, 22, 0});

    // Feedback styles
    register_feedback_style({"feedback.error", "#ff0000", "#ff000020", "#ff0000", true, false});
    register_feedback_style({"feedback.warning", "#ffa500", "#ffa50020", "#ffa500", true, false});
    register_feedback_style({"feedback.info", "#3794ff", "#3794ff20", "#3794ff", false, false});
    register_feedback_style({"feedback.searchHit", "#000000", "#ffff0060", "#ffff00", false, true});
    register_feedback_style({"feedback.selection", "#ffffff", "#264f7840", "", false, true});
    register_feedback_style({"feedback.currentLine", "", "#ffffff0a", "", false, true});

    MARKAMP_LOG_DEBUG("Content style defaults registered: {} tokens", total_tokens());
}

} // namespace markamp::core
