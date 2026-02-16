#pragma once

/// @file ProfilerIds.h
/// @brief Compile-time profiling zone identifiers.
///
/// Phase 16: Replaces string-based profiling keys with a fixed enum.
/// O(1) lookup, zero allocation on the recording hot path.

#include <array>
#include <cstdint>
#include <string_view>

namespace markamp::core
{

/// Fixed compile-time profiling zone identifiers.
/// Used as direct array indices into Profiler's flat timing storage.
enum class ProfileId : uint16_t
{
    // ── EventBus ──
    EventBus_Publish = 0,
    EventBus_PublishFast,
    EventBus_Subscribe,

    // ── HtmlRenderer ──
    HtmlRenderer_Render,
    HtmlRenderer_RenderBlock,
    HtmlRenderer_Concatenate,

    // ── SyntaxHighlighter ──
    SyntaxHighlighter_Tokenize,
    SyntaxHighlighter_IncrementalTokenize,

    // ── MarkdownParser ──
    MarkdownParser_Parse,
    MarkdownParser_IncrementalParse,

    // ── Config ──
    Config_Get,
    Config_Set,

    // ── ThemeEngine ──
    ThemeEngine_ColorLookup,
    ThemeEngine_BrushCache,

    // ── PluginManager ──
    PluginManager_Dispatch,
    PluginManager_Activate,

    // ── Startup ──
    Startup_Total,
    Startup_ConfigLoad,
    Startup_ThemeInit,
    Startup_PluginInit,
    Startup_UIBuild,

    // ── Sentinel (must be last) ──
    kMaxProfileIds
};

/// Number of profile zones (for array sizing).
inline constexpr auto kProfileIdCount = static_cast<std::size_t>(ProfileId::kMaxProfileIds);

/// Compile-time name table for all profile IDs.
inline constexpr std::array<std::string_view, kProfileIdCount> kProfileIdNames = {{
    "EventBus::publish",
    "EventBus::publish_fast",
    "EventBus::subscribe",
    "HtmlRenderer::render",
    "HtmlRenderer::render_block",
    "HtmlRenderer::concatenate",
    "SyntaxHighlighter::tokenize",
    "SyntaxHighlighter::incremental_tokenize",
    "MarkdownParser::parse",
    "MarkdownParser::incremental_parse",
    "Config::get",
    "Config::set",
    "ThemeEngine::color_lookup",
    "ThemeEngine::brush_cache",
    "PluginManager::dispatch",
    "PluginManager::activate",
    "Startup::total",
    "Startup::config_load",
    "Startup::theme_init",
    "Startup::plugin_init",
    "Startup::ui_build",
}};

/// Look up the human-readable name for a profile ID.
[[nodiscard]] constexpr auto profile_id_name(ProfileId id) -> std::string_view
{
    auto idx = static_cast<std::size_t>(id);
    if (idx < kProfileIdCount)
    {
        return kProfileIdNames[idx];
    }
    return "unknown";
}

} // namespace markamp::core
