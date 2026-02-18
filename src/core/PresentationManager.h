/// @file PresentationManager.h
/// @brief V9 Phase 43 — Presentation lifecycle, slide navigation, and presenter mode.
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Presentation playback state.
enum class PresentationState : uint8_t
{
    kIdle = 0,    ///< Not presenting
    kPlaying = 1, ///< Slideshow active
    kPaused = 2,  ///< Slideshow paused
};

/// A presentation definition.
struct Presentation
{
    std::string presentation_id;
    std::string title;
    std::string author;
    std::string description;
    int slide_count{0};
    int current_slide{1};
    PresentationState state{PresentationState::kIdle};
    bool presenter_mode{false};
    bool loop{false};
    int auto_advance_seconds{0}; ///< 0 = manual advance
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point started_at;
};

/// Manages presentation lifecycle and playback.
///
/// Handles creation, navigation, presenter mode, and slideshow control.
class PresentationManager
{
public:
    PresentationManager() = default;

    // ── Presentation lifecycle ────────────────────────────────────────
    auto create_presentation(const std::string& title, int slide_count) -> std::string;
    auto close_presentation(const std::string& presentation_id) -> bool;
    [[nodiscard]] auto find_presentation(const std::string& presentation_id) const
        -> const Presentation*;
    [[nodiscard]] auto presentation_count() const -> int;

    // ── Playback control ──────────────────────────────────────────────
    auto start_presentation(const std::string& presentation_id) -> bool;
    auto pause_presentation(const std::string& presentation_id) -> bool;
    auto stop_presentation(const std::string& presentation_id) -> bool;

    // ── Navigation ────────────────────────────────────────────────────
    auto next_slide(const std::string& presentation_id) -> bool;
    auto previous_slide(const std::string& presentation_id) -> bool;
    auto go_to_slide(const std::string& presentation_id, int slide_num) -> bool;

    // ── Presenter mode ────────────────────────────────────────────────
    auto toggle_presenter_mode(const std::string& presentation_id) -> bool;
    auto set_loop(const std::string& presentation_id, bool loop_enabled) -> bool;
    auto set_auto_advance(const std::string& presentation_id, int seconds) -> bool;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto active_presentations() const -> std::vector<const Presentation*>;
    [[nodiscard]] auto all_presentations() const -> std::vector<const Presentation*>;
    void close_all();

private:
    std::vector<Presentation> presentations_;
    int next_id_{1};

    auto find_mut(const std::string& presentation_id) -> Presentation*;
};

} // namespace markamp::core
