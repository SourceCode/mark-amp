/// @file V27SettingsDesignCoordinator.h
/// @brief V27 Phase 12 — Settings, theme gallery, control redesign orchestration.
#pragma once
#include <string>
#include <vector>
namespace markamp::core
{
/// Chrome policy for a settings category.
struct SettingsCategoryChromePolicy {
    std::string category_name;
    bool uses_canonical_icon{false};
    bool emoji_free{false};
    int row_count{0};
};

/// Tracks settings redesign coverage: emoji removal, icon migration, gallery/control family.
class V27SettingsDesignCoordinator {
public:
    void register_category(SettingsCategoryChromePolicy policy);
    [[nodiscard]] auto category_count() const -> int;
    [[nodiscard]] auto emoji_free_count() const -> int;
    [[nodiscard]] auto all_emoji_free() const -> bool;
    [[nodiscard]] auto canonical_icon_count() const -> int;
    [[nodiscard]] auto total_rows() const -> int;
    [[nodiscard]] auto categories() const -> const std::vector<SettingsCategoryChromePolicy>&;
    [[nodiscard]] auto summary() const -> std::string;
private:
    std::vector<SettingsCategoryChromePolicy> categories_;
};
} // namespace markamp::core
