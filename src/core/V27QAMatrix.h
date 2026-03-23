/// @file V27QAMatrix.h
/// @brief V27 Phase 20 — Cleanup, legacy removal, visual QA gates.
#pragma once
#include <string>
#include <vector>
namespace markamp::core
{
enum class V27QAStatus { kNotStarted, kInProgress, kPass, kFail };
[[nodiscard]] constexpr auto v27_qa_status_count() noexcept -> int { return 4; }
struct V27QASurfaceEntry {
    std::string surface_name;
    V27QAStatus icon_migration{V27QAStatus::kNotStarted};
    V27QAStatus visual_redesign{V27QAStatus::kNotStarted};
    V27QAStatus theme_parity{V27QAStatus::kNotStarted};
    V27QAStatus accessibility{V27QAStatus::kNotStarted};
    V27QAStatus legacy_removal{V27QAStatus::kNotStarted};
    [[nodiscard]] auto all_pass() const noexcept -> bool {
        return icon_migration == V27QAStatus::kPass
            && visual_redesign == V27QAStatus::kPass
            && theme_parity == V27QAStatus::kPass
            && accessibility == V27QAStatus::kPass
            && legacy_removal == V27QAStatus::kPass;
    }
};
class V27QAMatrix {
public:
    V27QAMatrix() = default;
    void register_surface(const V27QASurfaceEntry& entry);
    [[nodiscard]] auto surface(const std::string& name) const -> const V27QASurfaceEntry*;
    [[nodiscard]] auto all_surfaces() const -> const std::vector<V27QASurfaceEntry>& { return entries_; }
    [[nodiscard]] auto surface_count() const noexcept -> int { return static_cast<int>(entries_.size()); }
    [[nodiscard]] auto all_pass() const noexcept -> bool;
    [[nodiscard]] auto pass_count() const noexcept -> int;
    [[nodiscard]] auto incomplete_surfaces() const -> std::vector<std::string>;
    void populate_v27_surfaces();
private:
    std::vector<V27QASurfaceEntry> entries_;
};
} // namespace markamp::core
