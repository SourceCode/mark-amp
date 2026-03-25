/// @file V27PanelDesignCoordinator.cpp
/// @brief V27 Phase 11 — Panel fleet redesign orchestration.
#include "core/V27PanelDesignCoordinator.h"
#include <sstream>
namespace markamp::core
{
void V27PanelDesignCoordinator::register_panel(PanelChromePolicy policy) {
    panels_.push_back(std::move(policy));
}

auto V27PanelDesignCoordinator::panel_count() const -> int {
    return static_cast<int>(panels_.size());
}

auto V27PanelDesignCoordinator::primary_count() const -> int {
    int n = 0;
    for (const auto& p : panels_) { if (p.tier == PanelRedesignTier::kPrimary) { ++n; } }
    return n;
}

auto V27PanelDesignCoordinator::panels_with_canonical_icons() const -> int {
    int n = 0;
    for (const auto& p : panels_) { if (p.uses_canonical_icons) { ++n; } }
    return n;
}

auto V27PanelDesignCoordinator::all_dividers_tokenized() const -> bool {
    for (const auto& p : panels_) { if (!p.divider_tokenized) { return false; } }
    return !panels_.empty();
}

auto V27PanelDesignCoordinator::panels() const -> const std::vector<PanelChromePolicy>& {
    return panels_;
}

auto V27PanelDesignCoordinator::summary() const -> std::string {
    std::ostringstream os;
    os << "# V27 Panel Fleet Status\n\n";
    os << "Total: " << panel_count() << " | Primary: " << primary_count()
       << " | Canonical icons: " << panels_with_canonical_icons() << "\n";
    return os.str();
}
} // namespace markamp::core
