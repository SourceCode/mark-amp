/// @file V27IconAuditReport.h
/// @brief V27 Phase 01 — Comprehensive icon audit report and migration tracking.
///
/// Code-backed inventory of all icon call sites grouped by surface and source
/// type. Tracks migration from legacy icon systems (emoji, icon-char, inline
/// SVG) to the canonical MUI pipeline.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Icon source classification — enumerates all coexisting icon systems
// ============================================================================

/// Source system from which an icon originates.
enum class IconSource
{
    kMui,       ///< Material UI icon — the canonical V27 target
    kLegacyChar,///< Legacy icon-char constants (e.g. "▪", "●")
    kEmoji,     ///< Emoji/unicode used as UI chrome (e.g. "📄", "🔍")
    kInlineSvg, ///< Inline SVG markup embedded in source
    kLucide,    ///< Lucide icon set (resources/icons/lucide/)
    kManifest,  ///< Icon manifest system (IconManifest.h)
};

/// Returns human-readable label for an IconSource.
[[nodiscard]] constexpr auto icon_source_label(IconSource src) noexcept -> const char*
{
    switch (src)
    {
    case IconSource::kMui:       return "MUI";
    case IconSource::kLegacyChar:return "LegacyChar";
    case IconSource::kEmoji:     return "Emoji";
    case IconSource::kInlineSvg: return "InlineSVG";
    case IconSource::kLucide:    return "Lucide";
    case IconSource::kManifest:  return "Manifest";
    }
    return "Unknown";
}

/// Total number of icon source types.
[[nodiscard]] constexpr auto icon_source_count() noexcept -> int { return 6; }

// ============================================================================
// Surface classification — all UI surfaces that display icons
// ============================================================================

/// UI surface where icons appear.
enum class IconSurface
{
    kActivityBar,
    kToolbar,
    kTabBar,
    kBreadcrumb,
    kCommandPalette,
    kContextMenu,
    kFileTree,
    kPanelHeader,
    kStatusBar,
    kSettingsCategory,
    kSettingsRow,
    kDialog,
    kNotification,
    kEmptyState,
    kThemeGallery,
    kStartup,
};

/// Returns human-readable label for an IconSurface.
[[nodiscard]] constexpr auto icon_surface_label(IconSurface surface) noexcept -> const char*
{
    switch (surface)
    {
    case IconSurface::kActivityBar:     return "ActivityBar";
    case IconSurface::kToolbar:         return "Toolbar";
    case IconSurface::kTabBar:          return "TabBar";
    case IconSurface::kBreadcrumb:      return "Breadcrumb";
    case IconSurface::kCommandPalette:  return "CommandPalette";
    case IconSurface::kContextMenu:     return "ContextMenu";
    case IconSurface::kFileTree:        return "FileTree";
    case IconSurface::kPanelHeader:     return "PanelHeader";
    case IconSurface::kStatusBar:       return "StatusBar";
    case IconSurface::kSettingsCategory:return "SettingsCategory";
    case IconSurface::kSettingsRow:     return "SettingsRow";
    case IconSurface::kDialog:          return "Dialog";
    case IconSurface::kNotification:    return "Notification";
    case IconSurface::kEmptyState:      return "EmptyState";
    case IconSurface::kThemeGallery:    return "ThemeGallery";
    case IconSurface::kStartup:         return "Startup";
    }
    return "Unknown";
}

/// Total number of icon surface types.
[[nodiscard]] constexpr auto icon_surface_count() noexcept -> int { return 16; }

// ============================================================================
// Audit report entries
// ============================================================================

/// A single icon usage record found during audit.
struct IconAuditEntry
{
    std::string icon_id;          ///< The icon identifier as used in source
    IconSource source;            ///< Which icon system it comes from
    IconSurface surface;          ///< Which UI surface displays it
    std::string file_path;        ///< Source file where usage was found
    std::string mui_replacement;  ///< Target MUI icon ID (empty if not yet mapped)
    bool is_migrated{false};      ///< True if already converted to MUI
};

/// Per-surface migration progress summary.
struct SurfaceMigrationSummary
{
    IconSurface surface;
    int total_icons{0};
    int migrated_icons{0};
    int legacy_remaining{0};
    int emoji_remaining{0};

    /// Migration percentage (0-100).
    [[nodiscard]] auto progress_percent() const noexcept -> int
    {
        return total_icons > 0 ? (migrated_icons * 100) / total_icons : 100;
    }
};

// ============================================================================
// V27 Icon Audit Report
// ============================================================================

/// Comprehensive V27 icon audit: collects all icon usages across the product,
/// tracks per-surface migration progress, and reports legacy residue.
class V27IconAuditReport
{
public:
    V27IconAuditReport() = default;

    /// Record an icon usage.
    void record(const IconAuditEntry& entry);

    /// All recorded entries.
    [[nodiscard]] auto entries() const -> const std::vector<IconAuditEntry>&
    {
        return entries_;
    }

    /// Total entries recorded.
    [[nodiscard]] auto total_count() const noexcept -> int
    {
        return static_cast<int>(entries_.size());
    }

    /// Count entries by source type.
    [[nodiscard]] auto count_by_source(IconSource source) const -> int;

    /// Count entries by surface.
    [[nodiscard]] auto count_by_surface(IconSurface surface) const -> int;

    /// Count migrated entries.
    [[nodiscard]] auto migrated_count() const -> int;

    /// Count legacy (non-MUI) entries still on visible surfaces.
    [[nodiscard]] auto legacy_count() const -> int;

    /// Overall migration progress (0-100).
    [[nodiscard]] auto overall_progress_percent() const noexcept -> int
    {
        auto total = total_count();
        return total > 0 ? (migrated_count() * 100) / total : 100;
    }

    /// Per-surface migration summary.
    [[nodiscard]] auto surface_summary(IconSurface surface) const -> SurfaceMigrationSummary;

    /// All surfaces with remaining legacy icons.
    [[nodiscard]] auto surfaces_with_legacy() const -> std::vector<IconSurface>;

    /// Populate the report with the V27 baseline audit data.
    void populate_v27_baseline();

private:
    std::vector<IconAuditEntry> entries_;
};

} // namespace markamp::core
