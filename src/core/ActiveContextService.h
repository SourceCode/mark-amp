/// @file ActiveContextService.h
/// @brief V20 P06-T02/T04: Active context propagation across all surfaces.
///
/// Provides a single authoritative answer to "what is active?" at all times.
/// Tracks the active artifact, active surface, active selection, and focus
/// target consistently across editor, notebook, canvas, and shell panels.
#pragma once

#include "ArtifactRegistry.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Surface types in the workbench.
enum class ActiveSurfaceKind
{
    kEditor,
    kNotebook,
    kCanvas,
    kExplorer,
    kSearch,
    kOutput,
    kTerminal,
    kSettings,
    kWelcome,
    kNone
};

/// Current active context snapshot.
struct ActiveContext
{
    ArtifactId active_artifact;
    ActiveSurfaceKind active_surface{ActiveSurfaceKind::kNone};
    std::string focus_target;                   ///< Widget/panel with keyboard focus
    int selection_count{0};                     ///< Selected items in the active surface
    std::optional<std::string> active_language; ///< Language of active artifact
    bool is_editing{false};                     ///< Whether user is actively editing

    [[nodiscard]] auto has_artifact() const noexcept -> bool
    {
        return !active_artifact.empty();
    }

    [[nodiscard]] auto is_editor() const noexcept -> bool
    {
        return active_surface == ActiveSurfaceKind::kEditor;
    }

    [[nodiscard]] auto is_canvas() const noexcept -> bool
    {
        return active_surface == ActiveSurfaceKind::kCanvas;
    }

    [[nodiscard]] auto is_notebook() const noexcept -> bool
    {
        return active_surface == ActiveSurfaceKind::kNotebook;
    }
};

/// Context key for conditional enablement (VS Code-style "when" clauses).
struct ContextKey
{
    std::string key;
    std::string value;
};

/// Manages the active context chain across all surfaces.
class ActiveContextService
{
public:
    /// Update the active context.
    void set_context(const ActiveContext& context);

    /// Get the current active context.
    [[nodiscard]] auto context() const -> const ActiveContext& { return context_; }

    /// Set a context key value (for when-clause evaluation).
    void set_context_key(const std::string& key, const std::string& value);

    /// Get a context key value.
    [[nodiscard]] auto get_context_key(const std::string& key) const
        -> std::optional<std::string>;

    /// Clear a context key.
    void clear_context_key(const std::string& key);

    /// List all context keys.
    [[nodiscard]] auto all_context_keys() const -> std::vector<ContextKey>;

    /// Evaluate a simple when-clause (key == value).
    [[nodiscard]] auto evaluate_when(const std::string& when_clause) const -> bool;

    /// Total context updates.
    [[nodiscard]] auto update_count() const noexcept -> int { return update_count_; }

private:
    ActiveContext context_;
    std::unordered_map<std::string, std::string> context_keys_;
    int update_count_{0};
};

} // namespace markamp::core
