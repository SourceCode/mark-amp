// ============================================================================
// File: src/core/PublishingProfileManager.h
// Phase 24: Export & Publishing — Saved publishing profile management
// ============================================================================
#pragma once

#include "ExportTypes.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Data structures
// ============================================================================

/// A saved publishing profile for one-click export.
struct PublishingProfile
{
    std::string profile_id;       ///< Unique identifier
    std::string name;             ///< Display name
    ExportOptions export_options; ///< Stored export configuration
    std::string template_id;      ///< Associated export template (may be empty)
    std::string output_directory; ///< Default output directory
    bool auto_open{false};        ///< Open the file after export
    int64_t last_used_ms{0};      ///< Timestamp of last execution (epoch ms)
    int32_t use_count{0};         ///< Number of times executed
};

// ============================================================================
// PublishingProfileManager
// ============================================================================

/// PublishingProfileManager — CRUD manager for saved export profiles
/// with JSON persistence and execution integration.
class PublishingProfileManager
{
public:
    PublishingProfileManager();

    // ----- CRUD -------------------------------------------------------------

    /// Create a new profile.  Returns assigned profile_id, or empty on failure.
    auto create_profile(PublishingProfile profile) -> std::string;

    /// Update an existing profile.  Returns false if not found.
    auto update_profile(const PublishingProfile& profile) -> bool;

    /// Delete a profile by id.  Returns false if not found.
    auto delete_profile(const std::string& profile_id) -> bool;

    /// Retrieve a profile by id.
    [[nodiscard]] auto get_profile(const std::string& profile_id) const
        -> std::optional<PublishingProfile>;

    /// List all profiles, sorted by name.
    [[nodiscard]] auto list_profiles() const -> std::vector<PublishingProfile>;

    /// Duplicate a profile with a new name.  Returns the new profile_id.
    auto duplicate_profile(const std::string& source_id, const std::string& new_name)
        -> std::string;

    // ----- Execution --------------------------------------------------------

    /// Record that a profile was used (updates last_used_ms and use_count).
    auto mark_used(const std::string& profile_id) -> void;

    /// Get the most recently used profile, if any.
    [[nodiscard]] auto most_recent_profile() const -> std::optional<PublishingProfile>;

    // ----- Persistence ------------------------------------------------------

    /// Serialize all profiles to a JSON string.
    [[nodiscard]] auto serialize() const -> std::string;

    /// Deserialize profiles from a JSON string.  Returns number loaded.
    auto deserialize(const std::string& json_str) -> int;

    // ----- Validation -------------------------------------------------------

    /// Validate a profile's settings.
    [[nodiscard]] static auto validate(const PublishingProfile& profile)
        -> std::vector<std::string>;

    // ----- Stats ------------------------------------------------------------

    /// Total number of stored profiles.
    [[nodiscard]] auto count() const -> size_t;

private:
    std::vector<PublishingProfile> profiles_;
    int32_t next_id_{1};

    /// Find a profile by id, returning an iterator.
    auto find_iter(const std::string& profile_id) -> std::vector<PublishingProfile>::iterator;
    [[nodiscard]] auto find_iter(const std::string& profile_id) const
        -> std::vector<PublishingProfile>::const_iterator;
};

} // namespace markamp::core
