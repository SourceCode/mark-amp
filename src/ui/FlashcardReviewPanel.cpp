#include "FlashcardReviewPanel.h"

namespace markamp::ui
{

void FlashcardReviewPanel::initialize()
{
    showing_answer_ = false;
    rating_enabled_ = false;
    undo_enabled_ = false;
}

void FlashcardReviewPanel::layout(float x, float y, float width, float height)
{
    x_ = x;
    y_ = y;
    width_ = width;
    height_ = height;
}

void FlashcardReviewPanel::render()
{
    // Stub: actual ImGui/wxWidgets rendering deferred to UI integration phase
}

void FlashcardReviewPanel::show_card_front(const std::string& html_content)
{
    current_front_html_ = html_content;
    showing_answer_ = false;
    rating_enabled_ = false;
}

void FlashcardReviewPanel::show_card_back(const std::string& html_content)
{
    current_back_html_ = html_content;
    showing_answer_ = true;
    rating_enabled_ = true;
}

void FlashcardReviewPanel::show_empty_state(const std::string& message)
{
    current_front_html_ = message;
    current_back_html_.clear();
    showing_answer_ = false;
    rating_enabled_ = false;
}

void FlashcardReviewPanel::show_session_complete(const core::SessionStats& stats)
{
    current_front_html_ =
        "Session complete: " + std::to_string(stats.cards_reviewed) + " cards reviewed";
    showing_answer_ = false;
    rating_enabled_ = false;
}

void FlashcardReviewPanel::update_interval_labels(const std::string& again_label,
                                                  const std::string& hard_label,
                                                  const std::string& good_label,
                                                  const std::string& easy_label)
{
    again_label_ = again_label;
    hard_label_ = hard_label;
    good_label_ = good_label;
    easy_label_ = easy_label;
}

void FlashcardReviewPanel::set_rating_enabled(bool enabled)
{
    rating_enabled_ = enabled;
}

void FlashcardReviewPanel::update_progress(int current, int total)
{
    progress_current_ = current;
    progress_total_ = total;
}

void FlashcardReviewPanel::set_on_rating(OnRatingCallback callback)
{
    on_rating_ = std::move(callback);
}

void FlashcardReviewPanel::set_on_reveal(OnActionCallback callback)
{
    on_reveal_ = std::move(callback);
}

void FlashcardReviewPanel::set_on_skip(OnActionCallback callback)
{
    on_skip_ = std::move(callback);
}

void FlashcardReviewPanel::set_on_undo(OnActionCallback callback)
{
    on_undo_ = std::move(callback);
}

void FlashcardReviewPanel::set_on_end(OnActionCallback callback)
{
    on_end_ = std::move(callback);
}

void FlashcardReviewPanel::set_undo_enabled(bool enabled)
{
    undo_enabled_ = enabled;
}

} // namespace markamp::ui
