#pragma once

#include "../core/ReviewSession.h"

#include <functional>
#include <memory>
#include <string>

namespace markamp::ui
{

/// UI panel for the flashcard review experience.
/// Displays card front/back, rating buttons, and session progress.
class FlashcardReviewPanel
{
public:
    using OnRatingCallback = std::function<void(core::Rating rating)>;
    using OnActionCallback = std::function<void()>;

    FlashcardReviewPanel() = default;

    // ── Panel lifecycle ──

    void initialize();
    void layout(float x, float y, float width, float height);
    void render();

    // ── Content display ──

    void show_card_front(const std::string& html_content);
    void show_card_back(const std::string& html_content);
    void show_empty_state(const std::string& message);
    void show_session_complete(const core::SessionStats& stats);

    // ── Rating buttons ──

    void update_interval_labels(const std::string& again_label,
                                const std::string& hard_label,
                                const std::string& good_label,
                                const std::string& easy_label);
    void set_rating_enabled(bool enabled);

    // ── Progress bar ──

    void update_progress(int current, int total);

    // ── Callbacks ──

    void set_on_rating(OnRatingCallback callback);
    void set_on_reveal(OnActionCallback callback);
    void set_on_skip(OnActionCallback callback);
    void set_on_undo(OnActionCallback callback);
    void set_on_end(OnActionCallback callback);
    void set_undo_enabled(bool enabled);

private:
    float x_{0}, y_{0}, width_{0}, height_{0};

    // Current display state
    std::string current_front_html_;
    std::string current_back_html_;
    bool showing_answer_{false};
    bool rating_enabled_{false};
    bool undo_enabled_{false};
    int progress_current_{0};
    int progress_total_{0};

    // Interval labels
    std::string again_label_{"1m"};
    std::string hard_label_{"6m"};
    std::string good_label_{"10m"};
    std::string easy_label_{"4d"};

    // Improvement 34-36: Render data for the host panel
    struct ReviewRenderData
    {
        std::string content;
        bool is_answer_side{false};
        bool rating_enabled{false};
        std::string again_label;
        std::string hard_label;
        std::string good_label;
        std::string easy_label;
        int progress_current{0};
        int progress_total{0};
        float progress_fraction{0.0F};
        bool undo_enabled{false};
    };
    ReviewRenderData render_data_;

    // Callbacks
    OnRatingCallback on_rating_;
    OnActionCallback on_reveal_;
    OnActionCallback on_skip_;
    OnActionCallback on_undo_;
    OnActionCallback on_end_;
};

} // namespace markamp::ui
