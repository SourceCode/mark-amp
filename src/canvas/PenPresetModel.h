// ============================================================================
// File: src/canvas/PenPresetModel.h
// Phase 03 W02: Pen Presets — brush presets and recent styles
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// A single pen preset definition.
struct PenPreset
{
    std::string name;
    double width{2.0};
    double opacity{1.0};
    std::string color{"#000000"};
};

/// Testable model for Pen Presets (Phase 03 W02).
///
/// Encapsulates:
/// - Named brush presets
/// - Recently used styles
/// - Active preset tracking
class PenPresetModel
{
public:
    // ── Presets ──────────────────────────────────────────────────────

    void add_preset(const PenPreset& preset);
    [[nodiscard]] auto preset_count() const noexcept -> int;
    [[nodiscard]] auto preset_at(int index) const -> const PenPreset&;

    // ── Active Preset ───────────────────────────────────────────────

    void set_active_preset(const std::string& name);
    [[nodiscard]] auto active_preset_name() const -> const std::string&;

    // ── Recent Styles ───────────────────────────────────────────────

    void push_recent(const std::string& preset_name);
    [[nodiscard]] auto recent_count() const noexcept -> int;

    void set_max_recent(int count);
    [[nodiscard]] auto max_recent() const noexcept -> int;

private:
    std::vector<PenPreset> presets_;
    std::vector<std::string> recent_;
    std::string active_preset_name_;
    int max_recent_{10};
};

} // namespace markamp::canvas
