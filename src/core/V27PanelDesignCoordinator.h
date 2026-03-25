/// @file V27PanelDesignCoordinator.h
/// @brief V27 Phase 11 — Panel fleet redesign orchestration.
#pragma once
#include <string>
#include <vector>
namespace markamp::core
{
/// Classification tier for a panel surface.
enum class PanelRedesignTier : uint8_t { kPrimary, kSecondary, kStub };

/// Chrome policy for a panel class.
struct PanelChromePolicy {
    std::string panel_name;
    PanelRedesignTier tier{PanelRedesignTier::kStub};
    int header_height{32};
    bool uses_canonical_icons{false};
    bool divider_tokenized{false};
};

/// Tracks panel fleet redesign coverage and chrome consistency.
class V27PanelDesignCoordinator {
public:
    void register_panel(PanelChromePolicy policy);
    [[nodiscard]] auto panel_count() const -> int;
    [[nodiscard]] auto primary_count() const -> int;
    [[nodiscard]] auto panels_with_canonical_icons() const -> int;
    [[nodiscard]] auto all_dividers_tokenized() const -> bool;
    [[nodiscard]] auto panels() const -> const std::vector<PanelChromePolicy>&;
    [[nodiscard]] auto summary() const -> std::string;
private:
    std::vector<PanelChromePolicy> panels_;
};
} // namespace markamp::core
