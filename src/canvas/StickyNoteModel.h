#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Sticky note size preset.
enum class StickySizePreset : uint8_t
{
    kSmall,
    kMedium,
    kLarge,
};

/// Sticky note color preset.
enum class StickyColorPreset : uint8_t
{
    kYellow,
    kPink,
    kGreen,
    kBlue,
    kOrange,
    kPurple,
};

/// Cluster suggestion.
struct ClusterSuggestion
{
    std::string cluster_id;
    std::vector<std::string> object_ids;
    std::string reason; ///< e.g., "proximity", "same-tag"

    // ── Round 5 Batch 9 (#85-86) ────────────────────────────────

    /// (#85) Whether a reason is set.
    [[nodiscard]] auto has_reason() const noexcept -> bool
    {
        return !reason.empty();
    }

    /// (#86) Number of objects in this cluster.
    [[nodiscard]] auto object_count() const noexcept -> size_t
    {
        return object_ids.size();
    }
};

/// Testable model for Sticky Notes & Quick Capture (Phase 52).
///
/// Encapsulates:
/// - Quick creation (position, immediate edit)
/// - Color and size presets
/// - Text content with overflow detection
/// - Clustering suggestions
class StickyNoteModel
{
public:
    // ── Creation ────────────────────────────────────────────────────

    void set_text(const std::string& text);
    [[nodiscard]] auto text() const -> const std::string&;

    void set_editing(bool editing);
    [[nodiscard]] auto is_editing() const -> bool;

    // ── Presets ─────────────────────────────────────────────────────

    void set_color_preset(StickyColorPreset preset);
    [[nodiscard]] auto color_preset() const -> StickyColorPreset;
    [[nodiscard]] static auto color_for(StickyColorPreset preset) -> std::string;

    void set_size_preset(StickySizePreset preset);
    [[nodiscard]] auto size_preset() const -> StickySizePreset;
    [[nodiscard]] static auto dimensions_for(StickySizePreset preset) -> std::pair<double, double>;

    // ── Overflow ────────────────────────────────────────────────────

    void set_max_chars(int max_chars);
    [[nodiscard]] auto is_overflowing() const -> bool;

    // ── Clustering ──────────────────────────────────────────────────

    void set_suggestions(std::vector<ClusterSuggestion> suggestions);
    [[nodiscard]] auto suggestions() const -> const std::vector<ClusterSuggestion>&;

private:
    std::string text_;
    bool editing_{false};
    StickyColorPreset color_preset_{StickyColorPreset::kYellow};
    StickySizePreset size_preset_{StickySizePreset::kMedium};
    int max_chars_{200};
    std::vector<ClusterSuggestion> suggestions_;

    // ── Round 5 Batch 9 (#87-90) ────────────────────────────────

    /// (#87) Whether note has text content.
    [[nodiscard]] auto has_text() const noexcept -> bool
    {
        return !text_.empty();
    }

    /// (#88) Whether preset is yellow.
    [[nodiscard]] auto is_yellow() const noexcept -> bool
    {
        return color_preset_ == StickyColorPreset::kYellow;
    }

    /// (#89) Whether size is small.
    [[nodiscard]] auto is_small() const noexcept -> bool
    {
        return size_preset_ == StickySizePreset::kSmall;
    }

    /// (#90) Whether cluster suggestions exist.
    [[nodiscard]] auto has_suggestions() const noexcept -> bool
    {
        return !suggestions_.empty();
    }
};

} // namespace markamp::canvas
