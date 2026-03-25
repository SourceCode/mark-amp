/// @file V27TransientDesignCoordinator.h
/// @brief V27 Phase 13 — Transient surface redesign orchestration.
#pragma once
#include <string>
#include <vector>
namespace markamp::core
{
/// Classification of transient surface types.
enum class TransientSurfaceClass : uint8_t { kDialog, kTooltip, kPopover, kNotification };

/// Chrome policy for a transient surface class.
struct TransientChromePolicy {
    std::string surface_name;
    TransientSurfaceClass surface_class{TransientSurfaceClass::kDialog};
    int min_width{0};
    int max_width{0};
    int severity_icon_size{16};
    int depth_blur{0};
    bool uses_canonical_icons{false};
};

/// Tracks transient surface redesign: dialog, tooltip, popover, notification families.
class V27TransientDesignCoordinator {
public:
    void register_surface(TransientChromePolicy policy);
    [[nodiscard]] auto surface_count() const -> int;
    [[nodiscard]] auto surfaces_with_canonical_icons() const -> int;
    [[nodiscard]] auto count_by_class(TransientSurfaceClass cls) const -> int;
    [[nodiscard]] auto all_depth_consistent() const -> bool;
    [[nodiscard]] auto surfaces() const -> const std::vector<TransientChromePolicy>&;
    [[nodiscard]] auto summary() const -> std::string;
private:
    std::vector<TransientChromePolicy> surfaces_;
};
} // namespace markamp::core
