/// @file ArtifactNamingPolicy.h
/// @brief V20 P01-T05: Standardize artifact naming, template bootstrap, initial metadata.
///
/// Provides deterministic default naming for new artifacts (e.g. "Untitled-1",
/// "Notebook-2", "Board-3"), default extensions, and default language mappings.
#pragma once

#include "ArtifactRegistry.h"

#include <string>

namespace markamp::core
{

/// Provides deterministic, sequential naming for new artifacts.
class ArtifactNamingPolicy
{
public:
    ArtifactNamingPolicy() = default;

    /// Generate a default display name for the given artifact kind.
    /// Uses a global per-kind counter (e.g. "Untitled-1", "Notebook-2", "Board-3").
    [[nodiscard]] auto generate_name(ArtifactKind kind) -> std::string;

    /// Default file extension for the given artifact kind.
    [[nodiscard]] static auto default_extension(ArtifactKind kind) -> std::string;

    /// Default language identifier for the given artifact kind.
    [[nodiscard]] static auto default_language(ArtifactKind kind) -> std::string;

    /// Peek at the next sequence number for a kind (without incrementing).
    [[nodiscard]] auto next_sequence(ArtifactKind kind) const -> int;

    /// Human-readable kind label.
    [[nodiscard]] static auto kind_label(ArtifactKind kind) -> std::string;

private:
    int text_file_seq_{0};

    auto increment_sequence(ArtifactKind kind) -> int;
};

} // namespace markamp::core
