/// @file V27StructuredDataCoordinator.cpp
/// @brief V27 Phase 14 — Structured data surface redesign orchestration.
#include "core/V27StructuredDataCoordinator.h"
#include <sstream>
namespace markamp::core
{
void V27StructuredDataCoordinator::register_surface(StructuredSurfaceEntry entry) {
    surfaces_.push_back(std::move(entry));
}

auto V27StructuredDataCoordinator::surface_count() const -> int {
    return static_cast<int>(surfaces_.size());
}

auto V27StructuredDataCoordinator::rows_redesigned_count() const -> int {
    int n = 0;
    for (const auto& s : surfaces_) { if (s.rows_redesigned) { ++n; } }
    return n;
}

auto V27StructuredDataCoordinator::edge_states_redesigned_count() const -> int {
    int n = 0;
    for (const auto& s : surfaces_) { if (s.edge_states_redesigned) { ++n; } }
    return n;
}

auto V27StructuredDataCoordinator::count_by_type(StructuredSurfaceType type) const -> int {
    int n = 0;
    for (const auto& s : surfaces_) { if (s.type == type) { ++n; } }
    return n;
}

auto V27StructuredDataCoordinator::surfaces() const -> const std::vector<StructuredSurfaceEntry>& {
    return surfaces_;
}

auto V27StructuredDataCoordinator::summary() const -> std::string {
    std::ostringstream os;
    os << "# V27 Structured Data Status\n\n";
    os << "Total: " << surface_count()
       << " | Rows redesigned: " << rows_redesigned_count()
       << " | Edge states: " << edge_states_redesigned_count() << "\n";
    return os.str();
}
} // namespace markamp::core
