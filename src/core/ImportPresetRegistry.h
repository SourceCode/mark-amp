/// @file ImportPresetRegistry.h
/// @brief V9 Phase 42 — Saved import presets/templates for batch document import.
#pragma once

#include "DocumentImporter.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// A saved import preset with predefined options.
struct ImportPreset
{
    std::string preset_id;
    std::string name;
    std::string description;
    ImportFormat target_format{ImportFormat::kTxt};
    ImportOptions options;
    bool is_default{false};
    int usage_count{0};
};

/// Registry for saved import presets.
///
/// Users can save and reuse import configurations for batch operations.
/// Includes built-in default presets for common import scenarios.
class ImportPresetRegistry
{
public:
    ImportPresetRegistry() = default;

    // ── Preset management ─────────────────────────────────────────────
    void register_preset(ImportPreset preset);
    auto remove_preset(const std::string& preset_id) -> bool;
    [[nodiscard]] auto find_preset(const std::string& preset_id) const -> const ImportPreset*;
    [[nodiscard]] auto preset_count() const -> int;

    // ── Defaults ──────────────────────────────────────────────────────
    void load_defaults();
    auto set_default(const std::string& preset_id) -> bool;
    [[nodiscard]] auto default_preset() const -> const ImportPreset*;

    // ── Usage tracking ────────────────────────────────────────────────
    auto mark_used(const std::string& preset_id) -> bool;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto all_presets() const -> std::vector<const ImportPreset*>;
    [[nodiscard]] auto presets_for_format(ImportFormat format) const
        -> std::vector<const ImportPreset*>;
    void clear_all();

private:
    std::vector<ImportPreset> presets_;

    auto find_mut(const std::string& preset_id) -> ImportPreset*;
};

} // namespace markamp::core
