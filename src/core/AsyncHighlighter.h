#pragma once

#include "CoalescingTask.h"
#include "SyntaxHighlighter.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace markamp::core
{

/// Per-line lexer state for incremental re-tokenization.
/// When a line's end-state matches the previously stored state,
/// re-lexing can stop (convergence).
struct LineState
{
    uint32_t state_hash{0}; // hash of lexer state at end of this line
    uint64_t version{0};    // document version when this line was last tokenized
};

/// Result of an async highlighting pass.
struct HighlightResult
{
    uint64_t version{0};                    // document version this result corresponds to
    std::size_t start_line{0};              // first line re-tokenized
    std::size_t end_line{0};                // last line re-tokenized (exclusive)
    std::vector<std::vector<Token>> tokens; // tokens per line for the affected range
};

/// Background incremental syntax highlighter.
///
/// Uses a dedicated std::thread to re-tokenize only the affected
/// region after an edit. The worker checks a CancelToken
/// to abandon stale work when the user types faster than we can parse.
///
/// On completion, calls a user-provided callback with a versioned
/// HighlightResult. The UI thread can then apply the tokens if the
/// version still matches the current document.
///
/// Patterns implemented:
///   #5  Asynchronous syntax highlighting with incremental tokenization
///   #8  Work coalescing and cancellation (latest-wins)
class AsyncHighlighter
{
public:
    using ResultCallback = std::function<void(HighlightResult)>;

    /// Construct with a callback that receives highlight results.
    explicit AsyncHighlighter(ResultCallback on_result);
    ~AsyncHighlighter();

    // Non-copyable, non-movable
    AsyncHighlighter(const AsyncHighlighter&) = delete;
    auto operator=(const AsyncHighlighter&) -> AsyncHighlighter& = delete;
    AsyncHighlighter(AsyncHighlighter&&) = delete;
    auto operator=(AsyncHighlighter&&) -> AsyncHighlighter& = delete;

    /// Set the full document content and language. Triggers a full re-lex.
    void set_content(const std::string& content, const std::string& language);

    /// Notify the highlighter that lines [start_line, end_line) were edited.
    /// Triggers an incremental re-lex from start_line.
    void notify_edit(std::size_t start_line, std::size_t end_line);

    /// Get the current document version.
    [[nodiscard]] auto version() const noexcept -> uint64_t;

    /// Stop the background worker.
    void stop();

private:
    ResultCallback on_result_;
    SyntaxHighlighter highlighter_;
    CoalescingTask coalescing_;

    std::string content_;
    std::string language_;
    std::vector<LineState> line_states_;
    mutable std::mutex content_mutex_;

    std::thread worker_;
    std::mutex work_mutex_;
    std::condition_variable work_cv_;
    std::atomic<uint64_t> version_{0};
    std::atomic<bool> has_work_{false};
    std::atomic<bool> running_{true};

    /// Background worker entry point.
    void worker_loop();

    /// Perform incremental tokenization from start_line.
    void tokenize_range(std::size_t start_line, uint64_t ver, CancelToken cancel);

    /// Split content into lines.
    [[nodiscard]] static auto split_lines(const std::string& content)
        -> std::vector<std::string_view>;
};

} // namespace markamp::core
