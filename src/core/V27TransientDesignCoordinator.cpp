/// @file V27TransientDesignCoordinator.cpp
/// @brief V27 Phase 13 — Transient surface redesign orchestration.
#include "core/V27TransientDesignCoordinator.h"
#include <sstream>
namespace markamp::core
{
void V27TransientDesignCoordinator::register_surface(TransientChromePolicy policy) {
    surfaces_.push_back(std::move(policy));
}

auto V27TransientDesignCoordinator::surface_count() const -> int {
    return static_cast<int>(surfaces_.size());
}

auto V27TransientDesignCoordinator::surfaces_with_canonical_icons() const -> int {
    int n = 0;
    for (const auto& s : surfaces_) { if (s.uses_canonical_icons) { ++n; } }
    return n;
}

auto V27TransientDesignCoordinator::count_by_class(TransientSurfaceClass cls) const -> int {
    int n = 0;
    for (const auto& s : surfaces_) { if (s.surface_class == cls) { ++n; } }
    return n;
}

auto V27TransientDesignCoordinator::all_depth_consistent() const -> bool {
    if (surfaces_.empty()) { return false; }
    const int ref = surfaces_.front().depth_blur;
    for (const auto& s : surfaces_) {
        if (s.surface_class == surfaces_.front().surface_class && s.depth_blur != ref) { return false; }
    }
    return true;
}

auto V27TransientDesignCoordinator::surfaces() const -> const std::vector<TransientChromePolicy>& {
    return surfaces_;
}

auto V27TransientDesignCoordinator::summary() const -> std::string {
    std::ostringstream os;
    os << "# V27 Transient Surface Status\n\n";
    os << "Total: " << surface_count() << " | Canonical: " << surfaces_with_canonical_icons()
       << " | Dialogs: " << count_by_class(TransientSurfaceClass::kDialog)
       << " | Tooltips: " << count_by_class(TransientSurfaceClass::kTooltip) << "\n";
    return os.str();
}
} // namespace markamp::core
