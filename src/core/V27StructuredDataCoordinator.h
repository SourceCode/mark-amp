/// @file V27StructuredDataCoordinator.h
/// @brief V27 Phase 14 — Structured data surface redesign orchestration.
#pragma once
#include <string>
#include <vector>
namespace markamp::core
{
/// Classification of structured data surface types.
enum class StructuredSurfaceType : uint8_t { kTree, kList, kTable, kGallery, kKanban };

/// Tracks a single structured data surface and its redesign state.
struct StructuredSurfaceEntry {
    std::string name;
    StructuredSurfaceType type{StructuredSurfaceType::kList};
    bool rows_redesigned{false};
    bool edge_states_redesigned{false};
    bool uses_canonical_icons{false};
};

/// Tracks structured data surface redesign coverage.
class V27StructuredDataCoordinator {
public:
    void register_surface(StructuredSurfaceEntry entry);
    [[nodiscard]] auto surface_count() const -> int;
    [[nodiscard]] auto rows_redesigned_count() const -> int;
    [[nodiscard]] auto edge_states_redesigned_count() const -> int;
    [[nodiscard]] auto count_by_type(StructuredSurfaceType type) const -> int;
    [[nodiscard]] auto surfaces() const -> const std::vector<StructuredSurfaceEntry>&;
    [[nodiscard]] auto summary() const -> std::string;
private:
    std::vector<StructuredSurfaceEntry> surfaces_;
};
} // namespace markamp::core
