#pragma once

#include "canvas/CommentObject.h"
#include "core/EventBus.h"

#include <wx/panel.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Side panel for managing comment threads on the canvas.
/// Displays threaded comments, supports resolution, filtering, and
/// mention autocomplete. Subscribes to EventBus for remote updates.
class CanvasCommentsPanel : public wxPanel
{
public:
    CanvasCommentsPanel(wxWindow* parent, core::EventBus& event_bus);
    ~CanvasCommentsPanel() override = default;

    // Rule-of-five: non-copyable, non-movable (wx panel)
    CanvasCommentsPanel(const CanvasCommentsPanel&) = delete;
    auto operator=(const CanvasCommentsPanel&) -> CanvasCommentsPanel& = delete;
    CanvasCommentsPanel(CanvasCommentsPanel&&) = delete;
    auto operator=(CanvasCommentsPanel&&) -> CanvasCommentsPanel& = delete;

    /// Load all comment threads from the board.
    auto load_threads(const std::vector<canvas::CommentObject*>& threads) -> void;

    /// Focus a specific thread, scrolling to it.
    auto focus_thread(const std::string& thread_id) -> void;

    /// Add a new root comment to the selected position.
    auto create_thread(const std::string& author_id,
                       const std::string& author_name,
                       const std::string& body) -> void;

    /// Reply to an existing thread.
    auto reply_to_thread(const std::string& thread_id,
                         const std::string& author_id,
                         const std::string& author_name,
                         const std::string& body) -> void;

    /// Toggle resolved/unresolved filter.
    auto set_show_resolved(bool show) -> void;
    [[nodiscard]] auto show_resolved() const -> bool;

    /// Thread count.
    [[nodiscard]] auto thread_count() const -> size_t;

    /// Mention autocomplete callback.
    using MentionSuggestCallback =
        std::function<std::vector<std::string>(const std::string& prefix)>;
    auto set_mention_suggest(MentionSuggestCallback callback) -> void;

private:
    core::EventBus& event_bus_;
    std::vector<canvas::CommentObject*> threads_;
    bool show_resolved_{true};
    MentionSuggestCallback mention_suggest_;
    std::vector<core::Subscription> subscriptions_;

    auto create_layout() -> void;
    auto refresh_list() -> void;
};

} // namespace markamp::ui
