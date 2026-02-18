/// @file SlideEngine.h
/// @brief V9 Phase 43 — Slide creation, ordering, content blocks, and transitions.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Types of content blocks within a slide.
enum class SlideBlockType : uint8_t
{
    kTitle = 0,
    kSubtitle = 1,
    kText = 2,
    kBulletList = 3,
    kCodeBlock = 4,
    kImage = 5,
    kTable = 6,
    kQuote = 7,
    kDivider = 8,
};

/// Slide transition effects.
enum class SlideTransition : uint8_t
{
    kNone = 0,
    kFade = 1,
    kSlideLeft = 2,
    kSlideRight = 3,
    kSlideUp = 4,
    kZoomIn = 5,
    kDissolve = 6,
};

/// A single content block within a slide.
struct SlideBlock
{
    std::string block_id;
    SlideBlockType block_type{SlideBlockType::kText};
    std::string content;
    int sort_order{0};
    std::string style_class; ///< CSS class for custom styling
};

/// A single slide within a presentation.
struct Slide
{
    std::string slide_id;
    std::string title;
    int slide_number{0};
    std::vector<SlideBlock> blocks;
    SlideTransition transition{SlideTransition::kNone};
    std::string layout; ///< e.g. "title-only", "two-column", "blank"
    std::string speaker_notes;
    std::string background_color;
    std::string background_image;
    bool hidden{false};
};

/// Engine for creating and managing slides.
///
/// Handles slide CRUD, content block management, ordering,
/// and transition assignment.
class SlideEngine
{
public:
    SlideEngine() = default;

    // ── Slide management ──────────────────────────────────────────────
    auto add_slide(Slide slide) -> std::string;
    auto remove_slide(const std::string& slide_id) -> bool;
    [[nodiscard]] auto find_slide(const std::string& slide_id) const -> const Slide*;
    [[nodiscard]] auto slide_count() const -> int;

    // ── Ordering ──────────────────────────────────────────────────────
    auto move_slide(const std::string& slide_id, int new_position) -> bool;
    auto swap_slides(const std::string& slide_id_a, const std::string& slide_id_b) -> bool;
    [[nodiscard]] auto ordered_slides() const -> std::vector<const Slide*>;

    // ── Content blocks ────────────────────────────────────────────────
    auto add_block(const std::string& slide_id, SlideBlock block) -> bool;
    auto remove_block(const std::string& slide_id, const std::string& block_id) -> bool;
    [[nodiscard]] auto block_count(const std::string& slide_id) const -> int;

    // ── Transitions ───────────────────────────────────────────────────
    auto set_transition(const std::string& slide_id, SlideTransition transition) -> bool;
    auto set_all_transitions(SlideTransition transition) -> int;

    // ── Visibility ────────────────────────────────────────────────────
    auto hide_slide(const std::string& slide_id) -> bool;
    auto show_slide(const std::string& slide_id) -> bool;
    [[nodiscard]] auto visible_slides() const -> std::vector<const Slide*>;

    // ── Utilities ─────────────────────────────────────────────────────
    auto set_speaker_notes(const std::string& slide_id, const std::string& notes) -> bool;
    void clear_all();

private:
    std::vector<Slide> slides_;
    int next_id_{1};

    auto find_mut(const std::string& slide_id) -> Slide*;
    void renumber_slides();
};

} // namespace markamp::core
