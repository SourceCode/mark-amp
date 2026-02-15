// ============================================================================
// File: src/core/PlantUMLRenderer.h
// Phase 30: Diagram Rendering Extended — PlantUML renderer
// ============================================================================
#pragma once

#include "IDiagramRenderer.h"

#include <filesystem>
#include <string>
#include <unordered_map>

namespace markamp::core
{

/// Renders PlantUML diagrams via the PlantUML CLI (jar) or server.
/// Caches rendered SVG by content hash to avoid redundant rendering.
class PlantUMLRenderer : public IDiagramRenderer
{
public:
    PlantUMLRenderer() = default;

    /// Set the path to the PlantUML jar file.
    void set_jar_path(const std::filesystem::path& jar_path);

    /// Set the PlantUML server URL (alternative to local jar).
    void set_server_url(const std::string& url);

    // IDiagramRenderer interface
    [[nodiscard]] auto render(const std::string& source) const
        -> std::expected<DiagramResult, std::string> override;
    [[nodiscard]] auto is_available() const -> bool override;
    [[nodiscard]] auto language_id() const -> std::string override;
    [[nodiscard]] auto display_name() const -> std::string override;

private:
    std::filesystem::path jar_path_;
    std::string server_url_;
    mutable std::unordered_map<std::string, std::string> cache_;

    [[nodiscard]] auto render_via_jar(const std::string& source) const
        -> std::expected<DiagramResult, std::string>;
    [[nodiscard]] auto render_via_server(const std::string& source) const
        -> std::expected<DiagramResult, std::string>;
    [[nodiscard]] static auto compute_hash(const std::string& source) -> std::string;
};

} // namespace markamp::core
