/// @file MuiIconPipeline.h
/// @brief P10-T02 + V27-P03-T01: Unified MUI-aware icon rendering pipeline.
///
/// Single canonical pipeline for all icon rendering with legacy alias
/// fallback and standardized size/color/state handling. V27 promotes this
/// pipeline to the canonical-only path for all visible UI surfaces.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Icon rendering state.
enum class IconState
{
    kNormal,
    kHover,
    kActive,
    kDisabled,
    kSelected,
    kError,
    // V27 additions
    kFocused,     ///< V27: keyboard focus state
    kDragging,    ///< V27: during drag operation
    kWarning,     ///< V27: warning severity state
};

/// Total icon states.
[[nodiscard]] constexpr auto icon_state_count() noexcept -> int { return 9; }

/// Icon render request.
struct IconRenderRequest
{
    std::string icon_id;
    int size_px{16};
    IconState state{IconState::kNormal};
    double scale_factor{1.0}; ///< High-DPI
};

/// Unified MUI-aware icon rendering pipeline.
class MuiIconPipeline
{
public:
    MuiIconPipeline();

    /// Resolve an icon ID (handles legacy aliases).
    [[nodiscard]] auto resolve(const std::string& icon_id) const -> std::string;

    /// Register a legacy alias.
    void add_alias(const std::string& legacy_id, const std::string& mui_id);

    /// Check if an icon is a legacy alias.
    [[nodiscard]] auto is_alias(const std::string& icon_id) const -> bool;

    /// Get alias count.
    [[nodiscard]] auto alias_count() const -> int
    {
        return static_cast<int>(aliases_.size());
    }

    /// Render an icon (returns resolved ID for rendering subsystem).
    [[nodiscard]] auto render(const IconRenderRequest& request) const -> std::string;

    // ── V27 additions ─────────────────────────────────────────────────────

    /// V27: This pipeline is now canonical — always returns true.
    [[nodiscard]] static constexpr auto is_canonical() noexcept -> bool { return true; }

    /// V27: Resolve enforcing canonical-only (no legacy fallback on visible surfaces).
    /// Returns empty string if icon_id is not a known MUI ID or registered alias.
    [[nodiscard]] auto v27_resolve(const std::string& icon_id) const -> std::string;

    /// V27: Register all canonical MUI aliases from the V27 icon migration.
    void register_v27_aliases();

    /// V27: Check if an icon ID is a valid MUI canonical icon.
    [[nodiscard]] auto is_canonical_mui(const std::string& icon_id) const -> bool;

    /// V27: Get the count of V27 canonical MUI icons.
    [[nodiscard]] auto v27_canonical_count() const noexcept -> int
    {
        return static_cast<int>(v27_canonical_ids_.size());
    }

private:
    std::unordered_map<std::string, std::string> aliases_;
    std::unordered_map<std::string, bool> v27_canonical_ids_; ///< V27: known MUI icons
};

} // namespace markamp::core
