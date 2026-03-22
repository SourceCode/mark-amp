/// @file PanelReadinessController.h
/// @brief V25 P05: Panel readiness classification and settings host controller.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// Panel readiness classification.
enum class PanelScope { kReal, kMustFinish, kGated, kDeferred };

/// Panel classification entry.
struct PanelClassification
{
    std::string panel_id;
    std::string label;
    PanelScope scope{PanelScope::kGated};
    std::string gate_reason;
    bool has_real_factory{false};

    [[nodiscard]] auto blocks_release() const noexcept -> bool
    {
        return scope == PanelScope::kMustFinish && !has_real_factory;
    }
};

/// Settings host mode.
enum class SettingsHostMode { kStaged, kImmediate, kMixed };

/// Settings host state.
struct SettingsHostState
{
    SettingsHostMode mode{SettingsHostMode::kStaged};
    bool has_pending_changes{false};
    int pending_count{0};
    bool deep_link_active{false};
    std::string active_section;
};

/// Panel readiness controller.
class PanelReadinessController
{
public:
    PanelReadinessController() = default;

    void classify_panel(PanelClassification entry);
    [[nodiscard]] auto get_classification(const std::string& panel_id) const -> const PanelClassification*;
    [[nodiscard]] auto gated_panels() const -> std::vector<const PanelClassification*>;
    [[nodiscard]] auto blocking_panels() const -> std::vector<const PanelClassification*>;
    [[nodiscard]] auto panel_count() const noexcept -> int { return static_cast<int>(panels_.size()); }
    void clear();

private:
    std::vector<PanelClassification> panels_;
};

/// Settings host controller.
class SettingsHostController
{
public:
    SettingsHostController() = default;

    void stage_change(const std::string& key, const std::string& value);
    [[nodiscard]] auto apply_staged() -> bool;
    [[nodiscard]] auto cancel_staged() -> bool;
    [[nodiscard]] auto state() const noexcept -> const SettingsHostState& { return state_; }
    void navigate_to_section(const std::string& section);
    void activate_deep_link(const std::string& link);
    [[nodiscard]] auto staged_count() const noexcept -> int { return state_.pending_count; }

private:
    SettingsHostState state_;
    std::vector<std::pair<std::string, std::string>> staged_changes_;
};

} // namespace markamp::core
