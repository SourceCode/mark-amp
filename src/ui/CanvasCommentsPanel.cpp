#include "CanvasCommentsPanel.h"

#include "core/Events.h"
#include "core/Logger.h"

#include <wx/sizer.h>
#include <wx/stattext.h>

namespace markamp::ui
{

CanvasCommentsPanel::CanvasCommentsPanel(wxWindow* parent, core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY)
    , event_bus_(event_bus)
{
    create_layout();

    // Improvement 77: Subscribe to comment events for auto-refresh
    subscriptions_.push_back(event_bus_.subscribe<core::events::AddLineCommentRequestEvent>(
        [this](const core::events::AddLineCommentRequestEvent& /*evt*/)
        {
            MARKAMP_LOG_DEBUG("Line comment added — refreshing comments panel");
            refresh_list();
        }));
}

auto CanvasCommentsPanel::load_threads(const std::vector<canvas::CommentObject*>& threads) -> void
{
    threads_ = threads;
    refresh_list();
}

auto CanvasCommentsPanel::focus_thread(const std::string& thread_id) -> void
{
    MARKAMP_LOG_DEBUG("Focusing comment thread: {}", thread_id);

    // Improvement 72: Scroll to and highlight the specified thread
    focused_thread_id_ = thread_id;
    Refresh();
}

auto CanvasCommentsPanel::create_thread(const std::string& author_id,
                                        const std::string& author_name,
                                        const std::string& body) -> void
{
    MARKAMP_LOG_INFO("New comment thread by {} ({}): {}", author_name, author_id, body);
    // Thread creation is handled upstream by the canvas workspace;
    // this panel reacts via load_threads().
}

auto CanvasCommentsPanel::reply_to_thread(const std::string& thread_id,
                                          const std::string& author_id,
                                          const std::string& author_name,
                                          const std::string& body) -> void
{
    MARKAMP_LOG_INFO("Reply to thread {} by {} ({})", thread_id, author_name, author_id);
    (void)body;
}

auto CanvasCommentsPanel::set_show_resolved(bool show) -> void
{
    show_resolved_ = show;
    refresh_list();
}

auto CanvasCommentsPanel::show_resolved() const -> bool
{
    return show_resolved_;
}

auto CanvasCommentsPanel::thread_count() const -> size_t
{
    return threads_.size();
}

auto CanvasCommentsPanel::set_mention_suggest(MentionSuggestCallback callback) -> void
{
    mention_suggest_ = std::move(callback);
}

auto CanvasCommentsPanel::create_layout() -> void
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* header = new wxStaticText(this, wxID_ANY, "Comments");
    sizer->Add(header, 0, wxEXPAND | wxALL, 8);

    // Placeholder for thread list — will be populated by refresh_list()
    sizer->AddStretchSpacer(1);

    SetSizer(sizer);
}

auto CanvasCommentsPanel::refresh_list() -> void
{
    // Improvement 71: Rebuild visible thread list based on threads_ and show_resolved_
    visible_thread_count_ = 0;
    for (const auto* thread : threads_)
    {
        if (thread == nullptr)
        {
            continue;
        }
        // Filter out resolved threads if show_resolved_ is false
        if (!show_resolved_ && thread->is_resolved())
        {
            continue;
        }
        ++visible_thread_count_;
    }

    MARKAMP_LOG_DEBUG("Refreshing comments list ({} visible of {} threads, show_resolved={})",
                      visible_thread_count_,
                      threads_.size(),
                      show_resolved_ ? "true" : "false");
    Refresh();
}

} // namespace markamp::ui
