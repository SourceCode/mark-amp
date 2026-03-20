/// @file MuiIconPipeline.h
/// @brief P10-T02: Unified MUI-aware icon rendering pipeline.
///
/// Single canonical pipeline for all icon rendering with legacy alias
/// fallback and standardized size/color/state handling.
#pragma once

#include <string>
#include <unordered_map>

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
};

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

private:
    std::unordered_map<std::string, std::string> aliases_;
};

} // namespace markamp::core
