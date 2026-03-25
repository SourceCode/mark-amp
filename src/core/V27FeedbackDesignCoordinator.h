/// @file V27FeedbackDesignCoordinator.h
/// @brief V27 Phase 15 — Feedback system redesign orchestration.
#pragma once
#include <string>
#include <vector>
namespace markamp::core
{
/// Classification of feedback surface types.
enum class FeedbackSurfaceType : uint8_t { kInlineValidation, kStatusBar, kBanner, kProgressBar };

/// Tracks a single feedback surface and its redesign state.
struct FeedbackSurfaceEntry {
    std::string name;
    FeedbackSurfaceType type{FeedbackSurfaceType::kInlineValidation};
    bool severity_redesigned{false};
    bool uses_canonical_icons{false};
    bool emoji_free{false};
};

/// Tracks feedback system redesign coverage.
class V27FeedbackDesignCoordinator {
public:
    void register_surface(FeedbackSurfaceEntry entry);
    [[nodiscard]] auto surface_count() const -> int;
    [[nodiscard]] auto severity_redesigned_count() const -> int;
    [[nodiscard]] auto all_emoji_free() const -> bool;
    [[nodiscard]] auto count_by_type(FeedbackSurfaceType type) const -> int;
    [[nodiscard]] auto surfaces() const -> const std::vector<FeedbackSurfaceEntry>&;
    [[nodiscard]] auto summary() const -> std::string;
private:
    std::vector<FeedbackSurfaceEntry> surfaces_;
};
} // namespace markamp::core
