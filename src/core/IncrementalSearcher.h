#pragma once

#include "CoalescingTask.h"

#include <atomic>
#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace markamp::core
{

/// A single search match with location and context.
struct SearchMatch
{
    std::size_t line{0};      // 0-indexed line number
    std::size_t column{0};    // 0-indexed column number
    std::string_view context; // line containing the match (non-owning)
};

/// Configuration for an incremental search operation.
struct SearchConfig
{
    std::string needle;
    bool case_sensitive{true};
    bool whole_word{false};
};

/// Background incremental searcher with progressive result delivery.
///
/// Searches the document content on a background thread.
/// Results are delivered progressively via a callback as they are
/// found, allowing the UI to update incrementally. Uses
/// CoalescingTask so new searches cancel previous ones.
///
/// Pattern implemented: #15 Incremental search with background indexing
class IncrementalSearcher
{
public:
    /// Callback for each match found. Return false to abort the search.
    using MatchCallback =
        std::function<bool(std::size_t line, std::size_t col, std::string_view line_content)>;

    /// Callback when search is complete. Receives total match count.
    using CompleteCallback = std::function<void(std::size_t total_matches)>;

    IncrementalSearcher() = default;
    ~IncrementalSearcher();

    // Non-copyable, non-movable
    IncrementalSearcher(const IncrementalSearcher&) = delete;
    auto operator=(const IncrementalSearcher&) -> IncrementalSearcher& = delete;
    IncrementalSearcher(IncrementalSearcher&&) = delete;
    auto operator=(IncrementalSearcher&&) -> IncrementalSearcher& = delete;

    /// Start a new search. Cancels any in-flight search.
    void search(const std::string& content,
                SearchConfig config,
                MatchCallback on_match,
                CompleteCallback on_complete);

    /// Cancel the current search.
    void cancel();

    /// Check if a search is currently running.
    [[nodiscard]] auto is_searching() const noexcept -> bool;

private:
    CoalescingTask coalescing_;
    std::thread worker_;
    std::atomic<bool> searching_{false};

    void search_worker(CancelToken cancel,
                       std::string content,
                       SearchConfig config,
                       MatchCallback on_match,
                       CompleteCallback on_complete,
                       uint64_t version);
};

} // namespace markamp::core
