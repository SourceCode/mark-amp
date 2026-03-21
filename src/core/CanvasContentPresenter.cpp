/// @file CanvasContentPresenter.cpp
/// @brief V20 P08-T03: Canvas content presenter implementation.

#include "CanvasContentPresenter.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

CanvasContentPresenter::CanvasContentPresenter(EventBus& bus)
    : event_bus_(bus)
{
    register_defaults();
}

void CanvasContentPresenter::register_style(const CanvasObjectStyle& style)
{
    styles_[style.style_id] = style;
    MARKAMP_LOG_DEBUG("Canvas style registered: {}", style.style_id);
}

auto CanvasContentPresenter::style(const std::string& style_id) const -> const CanvasObjectStyle*
{
    auto it = styles_.find(style_id);
    return it != styles_.end() ? &it->second : nullptr;
}

auto CanvasContentPresenter::all_styles() const -> std::vector<CanvasObjectStyle>
{
    std::vector<CanvasObjectStyle> result;
    result.reserve(styles_.size());
    for (const auto& [id, s] : styles_)
    {
        result.push_back(s);
    }
    return result;
}

void CanvasContentPresenter::set_object_visual_state(const std::string& object_id,
                                                      CanvasObjectVisualState state)
{
    object_states_[object_id] = state;
    ++update_count_;

    events::CanvasObjectVisualStateChangedEvent evt;
    evt.object_id = object_id;
    evt.state = static_cast<int>(state);
    event_bus_.publish(evt);
}

auto CanvasContentPresenter::object_visual_state(const std::string& object_id) const
    -> CanvasObjectVisualState
{
    auto it = object_states_.find(object_id);
    return it != object_states_.end() ? it->second : CanvasObjectVisualState::kNormal;
}

void CanvasContentPresenter::register_embedded_content(const std::string& object_id,
                                                        EmbeddedContentType content_type)
{
    embedded_types_[object_id] = content_type;
    ++update_count_;
}

auto CanvasContentPresenter::embedded_content_type(const std::string& object_id) const
    -> EmbeddedContentType
{
    auto it = embedded_types_.find(object_id);
    return it != embedded_types_.end() ? it->second : EmbeddedContentType::kPlainText;
}

void CanvasContentPresenter::register_defaults()
{
    // Default sticky note style
    register_style({"sticky-note", "#e8a838", "#fef3c7", "#1e1e1e", 1, 4, 12, 1.0});

    // Default text box style
    register_style({"text-box", "#d1d5db", "#ffffff", "#1e1e1e", 1, 4, 8, 1.0});

    // Default code block style
    register_style({"code-block", "#374151", "#1e1e1e", "#d4d4d4", 1, 6, 12, 1.0});

    // Default shape style
    register_style({"shape-default", "#6366f1", "#eef2ff", "#1e1e1e", 2, 8, 8, 1.0});

    // Default frame style
    register_style({"frame", "#9ca3af", "transparent", "#1e1e1e", 1, 0, 16, 1.0});

    // Default connector style
    register_style({"connector", "#6b7280", "transparent", "#1e1e1e", 2, 0, 0, 1.0});

    // Selection state style
    register_style({"selection-ring", "#3b82f6", "transparent", "", 2, 0, 0, 0.8});

    MARKAMP_LOG_DEBUG("Canvas default styles registered: {} styles", style_count());
}

} // namespace markamp::core
