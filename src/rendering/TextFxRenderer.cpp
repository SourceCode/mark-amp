/// TextFxRenderer.cpp — Phase 48: Text FX Renderer Implementation

#include "TextFxRenderer.h"

#include <spdlog/spdlog.h>

namespace markamp::rendering
{

TextFxRenderer::TextFxRenderer() = default;

void TextFxRenderer::set_profile(TextFxChannel channel, const TextFxProfile& profile)
{
    profiles_[channel] = profile;
    spdlog::debug("TextFxRenderer: set profile for channel '{}'", channel_name(channel));
}

auto TextFxRenderer::get_profile(TextFxChannel channel) const -> TextFxProfile
{
    auto iter = profiles_.find(channel);
    if (iter != profiles_.end())
    {
        return iter->second;
    }
    return TextFxProfile{};
}

auto TextFxRenderer::has_active_profiles() const -> bool
{
    if (clean_mode_)
    {
        return false;
    }
    for (const auto& [channel, profile] : profiles_)
    {
        if (profile.enabled)
        {
            return true;
        }
    }
    return false;
}

auto TextFxRenderer::render_text_fx(TextFxChannel channel) const -> bool
{
    if (clean_mode_)
    {
        return false;
    }

    auto iter = profiles_.find(channel);
    if (iter == profiles_.end() || !iter->second.enabled)
    {
        return false;
    }

    // FX rendering would happen here — currently returns profile active state
    return true;
}

void TextFxRenderer::set_clean_mode(bool enabled) noexcept
{
    clean_mode_ = enabled;
    spdlog::info("TextFxRenderer: clean mode {}", enabled ? "enabled" : "disabled");
}

auto TextFxRenderer::is_clean_mode() const noexcept -> bool
{
    return clean_mode_;
}

void TextFxRenderer::apply_theme_text_effects(const core::TextEffects& text_effects)
{
    // Apply theme-level text effects to the default channel
    TextFxProfile default_profile;
    default_profile.enabled = text_effects.stroke_enabled || text_effects.shadow_enabled ||
                              text_effects.outer_glow_enabled ||
                              text_effects.gradient_fill_enabled || text_effects.bloom_enabled;

    default_profile.stroke_width = text_effects.stroke_width;
    default_profile.stroke_color = text_effects.stroke_color;
    default_profile.shadow_offset_x = text_effects.shadow_offset_x;
    default_profile.shadow_offset_y = text_effects.shadow_offset_y;
    default_profile.shadow_blur = text_effects.shadow_blur;
    default_profile.shadow_alpha = text_effects.shadow_alpha;
    default_profile.glow_radius = text_effects.outer_glow_radius;
    default_profile.glow_color = text_effects.outer_glow_color;
    default_profile.glow_alpha = text_effects.outer_glow_alpha;
    default_profile.gradient_enabled = text_effects.gradient_fill_enabled;
    default_profile.gradient_start = text_effects.gradient_start;
    default_profile.gradient_end = text_effects.gradient_end;
    default_profile.bloom_intensity = text_effects.bloom_intensity;

    profiles_[TextFxChannel::kDefault] = default_profile;
    spdlog::debug("TextFxRenderer: applied theme text effects to default channel");
}

auto TextFxRenderer::channel_count() const noexcept -> std::size_t
{
    return profiles_.size();
}

auto TextFxRenderer::channel_name(TextFxChannel channel) -> std::string_view
{
    switch (channel)
    {
        case TextFxChannel::kDefault:
            return "Default";
        case TextFxChannel::kKeyword:
            return "Keyword";
        case TextFxChannel::kString:
            return "String";
        case TextFxChannel::kComment:
            return "Comment";
        case TextFxChannel::kType:
            return "Type";
        case TextFxChannel::kFunction:
            return "Function";
        case TextFxChannel::kNumber:
            return "Number";
        case TextFxChannel::kOperator:
            return "Operator";
        case TextFxChannel::kHeading:
            return "Heading";
        case TextFxChannel::kDiagnostic:
            return "Diagnostic";
    }
    return "Unknown";
}

} // namespace markamp::rendering
