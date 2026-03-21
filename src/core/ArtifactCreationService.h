/// @file ArtifactCreationService.h
/// @brief V20 P01-T02: Shell-owned artifact creation service.
///
/// Provides a single entry point for creating artifacts of any kind.
/// All creation paths (menu, palette, shortcut, context menu, empty state)
/// route through this service.
#pragma once

#include "ArtifactRegistry.h"
#include "EventBus.h"

#include <optional>
#include <string>

namespace markamp::core
{
class Config;

/// Request descriptor for artifact creation.
struct ArtifactCreationRequest
{
    ArtifactKind kind{ArtifactKind::kTextFile};
    std::string display_name;                       ///< Optional explicit name
    std::string language_id;                        ///< Language (e.g. "markdown", "python")
    std::optional<std::string> target_directory;    ///< Where to place on disk (if known)
    std::optional<std::string> template_id;         ///< Template to apply content from
    std::string source;                             ///< Entry point that triggered creation
};

/// Result of an artifact creation attempt.
struct ArtifactCreationResult
{
    bool success{false};
    ArtifactId id;
    std::string error_message;

    [[nodiscard]] auto ok() const noexcept -> bool { return success; }
};

/// Central creation service for all artifact types.
///
/// Validates the request, generates a default name if none provided,
/// registers the artifact in the ArtifactRegistry, publishes
/// ArtifactCreatedEvent, and returns the result.
class ArtifactCreationService
{
public:
    ArtifactCreationService(EventBus& bus, ArtifactRegistry& registry, Config& cfg);

    /// Create an artifact from a request descriptor.
    [[nodiscard]] auto create(const ArtifactCreationRequest& request) -> ArtifactCreationResult;

    /// Convenience: create a new text file.
    [[nodiscard]] auto create_text_file(const std::string& name = {},
                                         const std::string& language = "markdown",
                                         const std::string& source = "command")
        -> ArtifactCreationResult;

    /// Convenience: create a new notebook.
    [[nodiscard]] auto create_notebook(const std::string& name = {},
                                        const std::string& source = "command")
        -> ArtifactCreationResult;

    /// Convenience: create a new canvas.
    [[nodiscard]] auto create_canvas(const std::string& name = {},
                                      const std::string& source = "command")
        -> ArtifactCreationResult;

    // ── Tracking ──

    /// Total number of successful creations.
    [[nodiscard]] auto creation_count() const noexcept -> int { return creation_count_; }

    /// Count by kind.
    [[nodiscard]] auto creation_count_by_kind(ArtifactKind kind) const noexcept -> int;

private:
    EventBus& event_bus_;
    ArtifactRegistry& registry_;
    Config& config_;

    int creation_count_{0};
    int text_file_count_{0};
    int notebook_count_{0};
    int canvas_count_{0};
};

} // namespace markamp::core
