#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Layout preset.
struct LayoutPreset
{
    std::string preset_id;
    std::string name;
    bool sidebar_visible{true};
    bool toolbar_visible{true};
    bool status_bar_visible{true};
    bool panel_visible{true};
};

/// User profile type.
enum class ProfileType : uint8_t
{
    kWriting,
    kCoding,
    kCanvas,
    kCustom,
};

/// Control visibility toggle.
struct ControlVisibility
{
    std::string control_id;
    std::string label;
    bool is_visible{true};
};

/// Testable model for Personalization & Layout Presets (Phase 37).
///
/// Encapsulates:
/// - Layout preset CRUD (save/load/rename/delete)
/// - Control visibility preferences
/// - Profile-aware preset linking
/// - One-click reset to defaults
class LayoutPresetModel
{
public:
    // ── Presets ──────────────────────────────────────────────────────

    void set_presets(std::vector<LayoutPreset> presets);
    [[nodiscard]] auto presets() const -> const std::vector<LayoutPreset>&;
    void add_preset(LayoutPreset preset);
    void remove_preset(const std::string& preset_id);
    [[nodiscard]] auto preset_by_id(const std::string& preset_id) const -> const LayoutPreset*;

    void set_active_preset(const std::string& preset_id);
    [[nodiscard]] auto active_preset_id() const -> const std::string&;

    // ── Control visibility ──────────────────────────────────────────

    void set_controls(std::vector<ControlVisibility> controls);
    [[nodiscard]] auto controls() const -> const std::vector<ControlVisibility>&;
    void toggle_control(const std::string& control_id);
    [[nodiscard]] auto is_control_visible(const std::string& control_id) const -> bool;

    // ── Profile ─────────────────────────────────────────────────────

    void set_profile(ProfileType profile);
    [[nodiscard]] auto profile() const -> ProfileType;

    /// Default preset ID for a profile.
    [[nodiscard]] static auto default_preset_for(ProfileType profile) -> std::string;

    // ── Reset ───────────────────────────────────────────────────────

    void reset_to_defaults();

private:
    std::vector<LayoutPreset> presets_;
    std::string active_preset_id_;
    std::vector<ControlVisibility> controls_;
    ProfileType profile_{ProfileType::kWriting};
};

} // namespace markamp::ui
