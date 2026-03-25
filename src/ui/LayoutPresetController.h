#pragma once

/**
 * @file LayoutPresetController.h
 * @brief Phase 37 Task 1: Named layout preset CRUD and profile support.
 *
 * Save/load named layout presets, tie presets to user profiles
 * (coding, writing), and provide quick reset to defaults.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// User profile type.
enum class ProfileType : uint8_t
{
    kCoding,
    kWriting,
    kCustom,
};

/// A named layout preset.
struct LayoutPreset
{
    std::string preset_id;
    std::string name;
    ProfileType profile{ProfileType::kCustom};
    bool is_default{false};
    std::vector<std::string> visible_panels;
    std::vector<std::string> panel_order;

    /// Get the profile name.
    [[nodiscard]] auto profile_name() const -> std::string;
};

/**
 * @brief Named layout preset CRUD and profile management.
 */
class LayoutPresetController
{
public:
    LayoutPresetController() = default;

    // ── Preset CRUD ────────────────────────────────────────────────

    /// Create a new preset.
    void create_preset(const LayoutPreset& preset);

    /// Rename a preset.
    void rename_preset(const std::string& preset_id, const std::string& new_name);

    /// Delete a preset.
    void delete_preset(const std::string& preset_id);

    /// Get a preset by ID.
    [[nodiscard]] auto find_preset(const std::string& preset_id) const -> const LayoutPreset*;

    /// Get all presets.
    [[nodiscard]] auto all_presets() const -> const std::vector<LayoutPreset>&;

    /// Get presets for a profile.
    [[nodiscard]] auto presets_for_profile(ProfileType profile) const -> std::vector<LayoutPreset>;

    /// Get preset count.
    [[nodiscard]] auto preset_count() const -> int;

    // ── Active preset ──────────────────────────────────────────────

    /// Apply a preset (sets it as active).
    void apply_preset(const std::string& preset_id);

    /// Get the active preset ID.
    [[nodiscard]] auto active_preset_id() const -> const std::string&;

    // ── Default and reset ──────────────────────────────────────────

    /// Reset to the default preset for the current profile.
    void reset_to_default();

    /// Get the default preset for a profile.
    [[nodiscard]] auto default_for_profile(ProfileType profile) const -> const LayoutPreset*;

    // ── Profile ────────────────────────────────────────────────────

    /// Set the current profile.
    void set_profile(ProfileType profile);

    /// Get the current profile.
    [[nodiscard]] auto current_profile() const -> ProfileType;

    // ── Standard presets ───────────────────────────────────────────

    /// Register built-in presets for all profiles.
    void register_standard_presets();

private:
    std::vector<LayoutPreset> presets_;
    std::string active_preset_id_;
    ProfileType current_profile_{ProfileType::kCoding};
};

} // namespace markamp::ui
