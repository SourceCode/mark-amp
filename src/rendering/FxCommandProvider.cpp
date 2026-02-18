// ============================================================================
// File: src/rendering/FxCommandProvider.cpp
// Phase 28: FX Visual Effects System — Command palette integration
// ============================================================================

#include "FxCommandProvider.h"

#include <algorithm>
#include <iterator>
#include <set>

namespace markamp::rendering
{

FxCommandProvider::FxCommandProvider()
{
    register_commands();
}

auto FxCommandProvider::register_commands() -> void
{
    commands_ = {
        {"fx.toggle_effects",
         "FX: Toggle Effects",
         "Visual Effects",
         "Ctrl+Shift+F",
         "Enable or disable all visual effects",
         false},

        {"fx.set_quality_cinematic",
         "FX: Set Quality — Cinematic",
         "Visual Effects",
         "",
         "Set FX quality to Cinematic (all effects, highest quality)",
         true},

        {"fx.set_quality_balanced",
         "FX: Set Quality — Balanced",
         "Visual Effects",
         "",
         "Set FX quality to Balanced (most effects, moderate GPU cost)",
         true},

        {"fx.set_quality_efficient",
         "FX: Set Quality — Efficient",
         "Visual Effects",
         "",
         "Set FX quality to Efficient (reduced effects, low GPU cost)",
         true},

        {"fx.set_quality_minimal",
         "FX: Set Quality — Minimal",
         "Visual Effects",
         "",
         "Set FX quality to Minimal (near-zero effects)",
         true},

        {"fx.apply_preset",
         "FX: Apply Preset",
         "Visual Effects",
         "",
         "Select and apply an FX preset from the registry",
         true},

        {"fx.toggle_reduced_motion",
         "FX: Toggle Reduced Motion",
         "FX Accessibility",
         "",
         "Toggle reduced motion mode for accessibility",
         false},

        {"fx.toggle_low_power",
         "FX: Toggle Low Power Mode",
         "FX Accessibility",
         "",
         "Toggle low power mode to conserve battery",
         false},

        {"fx.show_diagnostics",
         "FX: Show Diagnostics",
         "Visual Effects",
         "",
         "Open the FX performance diagnostics panel",
         true},

        {"fx.reset_defaults",
         "FX: Reset to Defaults",
         "Visual Effects",
         "",
         "Reset all FX settings to factory defaults",
         false},
    };
}

auto FxCommandProvider::commands() const -> const std::vector<FxCommand>&
{
    return commands_;
}

auto FxCommandProvider::find_command(const std::string& command_id) const
    -> std::optional<FxCommand>
{
    auto iter = std::find_if(commands_.begin(),
                             commands_.end(),
                             [&](const FxCommand& cmd) { return cmd.id == command_id; });
    if (iter != commands_.end())
    {
        return *iter;
    }
    return std::nullopt;
}

auto FxCommandProvider::commands_in_category(const std::string& category) const
    -> std::vector<FxCommand>
{
    std::vector<FxCommand> result;
    std::copy_if(commands_.begin(),
                 commands_.end(),
                 std::back_inserter(result),
                 [&](const FxCommand& cmd) { return cmd.category == category; });
    return result;
}

auto FxCommandProvider::categories() const -> std::vector<std::string>
{
    std::set<std::string> unique_categories;
    for (const auto& cmd : commands_)
    {
        unique_categories.insert(cmd.category);
    }
    return {unique_categories.begin(), unique_categories.end()};
}

auto FxCommandProvider::command_count() const -> int32_t
{
    return static_cast<int32_t>(commands_.size());
}

auto FxCommandProvider::available_commands(bool fx_enabled) const -> std::vector<FxCommand>
{
    std::vector<FxCommand> result;
    std::copy_if(commands_.begin(),
                 commands_.end(),
                 std::back_inserter(result),
                 [&](const FxCommand& cmd) { return !cmd.requires_fx_enabled || fx_enabled; });
    return result;
}

} // namespace markamp::rendering
