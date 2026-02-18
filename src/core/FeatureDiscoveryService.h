/// @file FeatureDiscoveryService.h
/// @brief V9 Phase 35 Task 8 — Non-intrusive feature discovery hints.
///
/// Manages contextual feature hints that suggest features the user hasn't
/// tried. Enforces a maximum of 1 hint per session, tracks dismissed hints
/// in Config, and is fully configurable (enable/disable).

#pragma once

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

// ============================================================================
// FeatureHint — a single discovery hint
// ============================================================================

/// A non-intrusive suggestion for a feature the user may not have tried.
struct FeatureHint
{
    std::string id;                 ///< Unique hint identifier
    std::string message;            ///< User-facing message
    std::string trigger_action;     ///< Action that triggers this hint
    std::string learn_more_article; ///< Help article ID for "Learn More"
};

// ============================================================================
// FeatureDiscoveryService
// ============================================================================

/// Manages feature discovery hints with per-session and persistent dismissal.
///
/// Usage:
/// ```cpp
/// FeatureDiscoveryService discovery;
/// if (auto hint = discovery.check_hint("editor.paste"))
/// {
///     show_hint(*hint);
///     discovery.dismiss_hint(hint->id);
/// }
/// ```
class FeatureDiscoveryService
{
public:
    FeatureDiscoveryService()
    {
        register_built_in_hints();
    }

    /// Check if a hint should be shown for the given user action.
    /// Returns the hint if applicable, or nullopt if no hint should be shown.
    /// Respects max-1-per-session and dismissal tracking.
    [[nodiscard]] auto check_hint(const std::string& action) -> std::optional<FeatureHint>
    {
        if (!enabled_ || hint_shown_this_session_)
        {
            return std::nullopt;
        }

        for (const auto& hint : hints_)
        {
            if (hint.trigger_action == action && !is_dismissed(hint.id))
            {
                return hint;
            }
        }
        return std::nullopt;
    }

    /// Mark a hint as dismissed (won't show again).
    void dismiss_hint(const std::string& hint_id)
    {
        dismissed_hints_.insert(hint_id);
        hint_shown_this_session_ = true;
    }

    /// Mark that a hint was shown this session (enforces max 1 per session).
    void mark_hint_shown()
    {
        hint_shown_this_session_ = true;
    }

    /// Check if a hint has been dismissed.
    [[nodiscard]] auto is_dismissed(const std::string& hint_id) const -> bool
    {
        return dismissed_hints_.count(hint_id) > 0;
    }

    /// Enable or disable feature discovery.
    void set_enabled(bool enabled)
    {
        enabled_ = enabled;
    }

    /// Check if feature discovery is enabled.
    [[nodiscard]] auto is_enabled() const -> bool
    {
        return enabled_;
    }

    /// Get all registered hints.
    [[nodiscard]] auto all_hints() const -> const std::vector<FeatureHint>&
    {
        return hints_;
    }

    /// Register a custom hint.
    void register_hint(FeatureHint hint)
    {
        hints_.push_back(std::move(hint));
    }

    /// Get the number of dismissed hints.
    [[nodiscard]] auto dismissed_count() const -> std::size_t
    {
        return dismissed_hints_.size();
    }

    /// Reset session state (for testing).
    void reset_session()
    {
        hint_shown_this_session_ = false;
    }

    /// Reset all dismissed hints (for testing).
    void reset_all()
    {
        dismissed_hints_.clear();
        hint_shown_this_session_ = false;
    }

    /// Load dismissed hints from a serialized set of IDs.
    void load_dismissed(const std::vector<std::string>& ids)
    {
        for (const auto& hint_id : ids)
        {
            dismissed_hints_.insert(hint_id);
        }
    }

    /// Get all dismissed hint IDs for persistence.
    [[nodiscard]] auto get_dismissed_ids() const -> std::vector<std::string>
    {
        return {dismissed_hints_.begin(), dismissed_hints_.end()};
    }

private:
    std::vector<FeatureHint> hints_;
    std::unordered_set<std::string> dismissed_hints_;
    bool enabled_{true};
    bool hint_shown_this_session_{false};

    void register_built_in_hints()
    {
        hints_.push_back({.id = "hint.split_view",
                          .message = "Try split view for side-by-side editing and preview",
                          .trigger_action = "editor.open",
                          .learn_more_article = "context.editor"});

        hints_.push_back({.id = "hint.canvas",
                          .message = "Use the canvas for visual brainstorming and diagramming",
                          .trigger_action = "workspace.create",
                          .learn_more_article = "context.canvas"});

        hints_.push_back({.id = "hint.notebooks",
                          .message = "Try notebooks for combining Markdown with executable code",
                          .trigger_action = "editor.code_block",
                          .learn_more_article = "context.notebook"});

        hints_.push_back(
            {.id = "hint.graph",
             .message = "View the knowledge graph to see connections between your documents",
             .trigger_action = "editor.wiki_link",
             .learn_more_article = "context.graph"});

        hints_.push_back({.id = "hint.templates",
                          .message = "Use templates to quickly create structured documents",
                          .trigger_action = "file.new",
                          .learn_more_article = "general.overview"});

        hints_.push_back({.id = "hint.shortcuts",
                          .message = "Press F1 to see all keyboard shortcuts",
                          .trigger_action = "editor.slow_mouse_navigation",
                          .learn_more_article = "general.overview"});
    }
};

} // namespace markamp::core
