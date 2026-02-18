// ============================================================================
// File: src/rendering/FxProfilePersistence.h
// Phase 28: FX Visual Effects System — FX Profile Serialization
// ============================================================================
#pragma once

#include "FxPresetRegistry.h"

#include <optional>
#include <string>
#include <vector>

namespace markamp::rendering
{

/// Serializes and deserializes FX presets to/from JSON.
/// Manages a directory of user-created FX profiles.
class FxProfilePersistence
{
public:
    FxProfilePersistence() = default;

    // ── Serialization ──

    /// Serialize an FxPreset to a JSON string.
    [[nodiscard]] auto serialize(const FxPreset& preset) const -> std::string;

    /// Deserialize a JSON string to an FxPreset. Returns nullopt on failure.
    [[nodiscard]] auto deserialize(const std::string& json_data) const -> std::optional<FxPreset>;

    // ── Profile management ──

    /// Save a preset under a profile name (stored in-memory).
    auto save_user_profile(const std::string& profile_name, const FxPreset& preset) -> void;

    /// Load a previously saved profile by name.
    [[nodiscard]] auto load_user_profile(const std::string& profile_name) const
        -> std::optional<FxPreset>;

    /// List all saved profile names.
    [[nodiscard]] auto list_user_profiles() const -> std::vector<std::string>;

    /// Delete a profile by name. Returns true if found and deleted.
    auto delete_user_profile(const std::string& profile_name) -> bool;

    /// Check if a profile exists.
    [[nodiscard]] auto has_profile(const std::string& profile_name) const -> bool;

    /// Total number of saved profiles.
    [[nodiscard]] auto profile_count() const noexcept -> std::size_t;

    // ── Bulk operations ──

    /// Export all profiles to a single JSON string.
    [[nodiscard]] auto export_all() const -> std::string;

    /// Import profiles from a JSON string. Returns number imported.
    auto import_all(const std::string& json_data) -> int32_t;

    /// Clear all profiles.
    auto clear_all() -> void;

private:
    std::vector<std::pair<std::string, FxPreset>> profiles_;
};

} // namespace markamp::rendering
