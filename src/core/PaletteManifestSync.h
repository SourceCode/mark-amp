/// @file PaletteManifestSync.h
/// @brief V21 Phase 02 — Command palette × manifest synchronization.
///
/// Bridges the CommandPaletteModel with the canonical ControlActionManifest
/// so the palette always reflects the current set of available actions.
/// Provides:
///   - One-shot sync from manifest to palette
///   - Context-aware filtering (hide disabled/invisible actions)
///   - Shortcut consistency verification between manifest and palette
///   - Parity audit between menu and palette command sets
#pragma once

#include "core/ControlActionManifest.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// PaletteEntry — a command as seen by the palette
// ============================================================================

/// Represents a command entry synchronized to the palette.
struct PaletteEntry
{
    std::string action_id;       ///< Canonical action ID
    std::string label;           ///< Display label
    std::string category;        ///< Category grouping
    std::string shortcut;        ///< Keyboard shortcut display
    std::string description;     ///< Detailed description
    std::string source;          ///< "builtin", "extension", "user"
    bool is_enabled{true};       ///< Whether the action is currently enabled
    bool is_visible{true};       ///< Whether the action should appear in palette
    bool has_handler{false};     ///< Whether a real handler is bound
};

// ============================================================================
// ShortcutDiscrepancy — parity check result
// ============================================================================

/// Represents a discrepancy between manifest and palette shortcut displays.
struct ShortcutDiscrepancy
{
    std::string action_id;
    std::string manifest_shortcut;  ///< Shortcut as declared in manifest
    std::string palette_shortcut;   ///< Shortcut as shown in palette
    std::string menu_shortcut;      ///< Shortcut as shown in menu accelerator
};

// ============================================================================
// ParityReport — menu vs palette coverage
// ============================================================================

/// Reports parity between menu items and palette entries.
struct ParityReport
{
    std::vector<std::string> menu_only;     ///< Actions in menu but not palette
    std::vector<std::string> palette_only;  ///< Actions in palette but not menu
    std::vector<std::string> both;          ///< Actions in both
    int total_menu{0};
    int total_palette{0};
    int parity_pct{0};                      ///< Overlap percentage

    /// Whether parity is acceptable (>= 90%).
    [[nodiscard]] auto is_acceptable() const noexcept -> bool
    {
        return parity_pct >= 90;
    }
};

// ============================================================================
// PaletteManifestSync — synchronization engine
// ============================================================================

/// Syncs command palette entries from the canonical ControlActionManifest.
class PaletteManifestSync
{
public:
    PaletteManifestSync() = default;

    // ── Synchronization ──

    /// Generate palette entries from a manifest. Only includes actions that
    /// target ControlSurface::kCommandPalette (or all actions if include_all is true).
    [[nodiscard]] auto sync_from_manifest(const ControlActionManifest& manifest,
                                          bool include_all = false) const
        -> std::vector<PaletteEntry>;

    /// Generate context-aware palette entries (filtered by enablement/visibility).
    [[nodiscard]] auto sync_context_aware(const ControlActionManifest& manifest,
                                          const ContextKeyService& context) const
        -> std::vector<PaletteEntry>;

    // ── Shortcut Consistency ──

    /// Check shortcut consistency between manifest entries and palette entries.
    /// Returns discrepancies where shortcut_hint differs across surfaces.
    [[nodiscard]] auto check_shortcut_consistency(
        const ControlActionManifest& manifest,
        const std::vector<PaletteEntry>& palette_entries) const
        -> std::vector<ShortcutDiscrepancy>;

    // ── Menu-to-Palette Parity ──

    /// Compare menu-surface actions with palette-surface actions in the manifest.
    [[nodiscard]] auto check_menu_palette_parity(
        const ControlActionManifest& manifest) const
        -> ParityReport;

    // ── Queries ──

    /// Count of palette-eligible actions in a manifest.
    [[nodiscard]] auto palette_eligible_count(const ControlActionManifest& manifest) const
        -> std::size_t;

    /// Count of menu-eligible actions in a manifest.
    [[nodiscard]] auto menu_eligible_count(const ControlActionManifest& manifest) const
        -> std::size_t;

    /// Actions that appear on menus but NOT in the palette.
    [[nodiscard]] auto menu_without_palette(const ControlActionManifest& manifest) const
        -> std::vector<std::string>;

    /// Actions that appear in the palette but NOT on menus.
    [[nodiscard]] auto palette_without_menu(const ControlActionManifest& manifest) const
        -> std::vector<std::string>;
};

} // namespace markamp::core
