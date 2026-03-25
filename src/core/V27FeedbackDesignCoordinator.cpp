/// @file V27FeedbackDesignCoordinator.cpp
/// @brief V27 Phase 15 — Feedback system redesign orchestration.
#include "core/V27FeedbackDesignCoordinator.h"
#include <sstream>
namespace markamp::core
{
void V27FeedbackDesignCoordinator::register_surface(FeedbackSurfaceEntry entry) {
    surfaces_.push_back(std::move(entry));
}

auto V27FeedbackDesignCoordinator::surface_count() const -> int {
    return static_cast<int>(surfaces_.size());
}

auto V27FeedbackDesignCoordinator::severity_redesigned_count() const -> int {
    int n = 0;
    for (const auto& s : surfaces_) { if (s.severity_redesigned) { ++n; } }
    return n;
}

auto V27FeedbackDesignCoordinator::all_emoji_free() const -> bool {
    for (const auto& s : surfaces_) { if (!s.emoji_free) { return false; } }
    return !surfaces_.empty();
}

auto V27FeedbackDesignCoordinator::count_by_type(FeedbackSurfaceType type) const -> int {
    int n = 0;
    for (const auto& s : surfaces_) { if (s.type == type) { ++n; } }
    return n;
}

auto V27FeedbackDesignCoordinator::surfaces() const -> const std::vector<FeedbackSurfaceEntry>& {
    return surfaces_;
}

auto V27FeedbackDesignCoordinator::summary() const -> std::string {
    std::ostringstream os;
    os << "# V27 Feedback System Status\n\n";
    os << "Total: " << surface_count()
       << " | Severity redesigned: " << severity_redesigned_count()
       << " | Emoji-free: " << (all_emoji_free() ? "Yes" : "No") << "\n";
    return os.str();
}
} // namespace markamp::core
