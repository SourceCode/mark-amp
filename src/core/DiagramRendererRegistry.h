// ============================================================================
// File: src/core/DiagramRendererRegistry.h
// Phase 30: Diagram Rendering Extended — Renderer registry
// ============================================================================
#pragma once

#include "IDiagramRenderer.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Thread-safe registry that maps diagram language identifiers
/// to their renderer implementations.
class DiagramRendererRegistry
{
public:
    DiagramRendererRegistry() = default;

    /// Register a renderer for a diagram language.
    void register_renderer(std::unique_ptr<IDiagramRenderer> renderer);

    /// Look up a renderer by language ID (e.g., "plantuml", "graphviz").
    [[nodiscard]] auto get_renderer(const std::string& language_id) const -> IDiagramRenderer*;

    /// Get all registered language IDs.
    [[nodiscard]] auto registered_languages() const -> std::vector<std::string>;

    /// Check if a language ID has a registered renderer.
    [[nodiscard]] auto has_renderer(const std::string& language_id) const -> bool;

    /// Get all renderers and their availability status.
    [[nodiscard]] auto renderer_status() const -> std::vector<std::pair<std::string, bool>>;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::unique_ptr<IDiagramRenderer>> renderers_;
};

} // namespace markamp::core
