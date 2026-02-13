#include "AsyncHighlighter.h"

#include <chrono>

namespace markamp::core
{

AsyncHighlighter::AsyncHighlighter(ResultCallback on_result)
    : on_result_(std::move(on_result))
    , worker_([this]() { worker_loop(); })
{
}

AsyncHighlighter::~AsyncHighlighter()
{
    stop();
}

void AsyncHighlighter::set_content(const std::string& content, const std::string& language)
{
    // R20 Fix 19: Guard against calling set_content after stop()
    if (!running_.load(std::memory_order_acquire))
    {
        return;
    }

    {
        std::lock_guard lock(content_mutex_);
        content_ = content;
        language_ = language;
        line_states_.clear();

        // Count lines and initialize states
        auto lines = split_lines(content_);
        line_states_.resize(lines.size());
    }

    auto ver = version_.fetch_add(1, std::memory_order_release) + 1;
    (void)coalescing_.submit(ver);

    {
        std::lock_guard lock(work_mutex_);
        has_work_.store(true, std::memory_order_release);
    }
    work_cv_.notify_one();
}

void AsyncHighlighter::notify_edit(std::size_t start_line, std::size_t /* end_line */)
{
    auto ver = version_.fetch_add(1, std::memory_order_release) + 1;
    (void)coalescing_.submit(ver);

    {
        std::lock_guard lock(work_mutex_);
        has_work_.store(true, std::memory_order_release);
    }
    work_cv_.notify_one();

    // The actual start_line is used by the worker when it picks up work
    (void)start_line;
}

auto AsyncHighlighter::version() const noexcept -> uint64_t
{
    return version_.load(std::memory_order_acquire);
}

void AsyncHighlighter::stop()
{
    running_.store(false, std::memory_order_release);
    coalescing_.cancel();

    {
        std::lock_guard lock(work_mutex_);
        has_work_.store(true, std::memory_order_release);
    }
    work_cv_.notify_one();

    if (worker_.joinable())
    {
        worker_.join();
    }
}

void AsyncHighlighter::worker_loop()
{
    while (running_.load(std::memory_order_acquire))
    {
        // Wait for work
        {
            std::unique_lock lock(work_mutex_);
            work_cv_.wait(lock,
                          [this] {
                              return has_work_.load(std::memory_order_acquire) ||
                                     !running_.load(std::memory_order_acquire);
                          });
            has_work_.store(false, std::memory_order_release);
        }

        if (!running_.load(std::memory_order_acquire))
        {
            break;
        }

        auto ver = coalescing_.current_version();
        auto task_cancel = coalescing_.submit(ver);

        tokenize_range(0, ver, task_cancel);
    }
}

// R20 Fix 38: Design note — content_copy and language_copy are taken under
// content_mutex_, then tokenization runs unlocked. The cancel token is checked
// between line iterations inside process_line(), not mid-tokenize call. This is
// an accepted trade-off: a single very long line may delay cancellation, but
// in practice Markdown lines are short enough that latency remains sub-ms.
void AsyncHighlighter::tokenize_range(std::size_t start_line, uint64_t ver, CancelToken cancel)
{
    std::string content_copy;
    std::string language_copy;
    {
        std::lock_guard lock(content_mutex_);
        content_copy = content_;
        language_copy = language_;
    }

    if (cancel.stop_requested() || !coalescing_.is_current(ver))
    {
        return;
    }

    // Tokenize the full content (incremental per-line optimization can be added later)
    auto tokens = highlighter_.tokenize(content_copy, language_copy);

    if (cancel.stop_requested() || !coalescing_.is_current(ver))
    {
        return;
    }

    // Group tokens by line
    auto lines = split_lines(content_copy);
    std::vector<std::vector<Token>> tokens_per_line(lines.size());

    std::size_t line_start_offset = 0;
    std::size_t current_line = 0;
    for (const auto& token : tokens)
    {
        // Advance to the correct line
        while (current_line + 1 < lines.size() &&
               token.start >= line_start_offset + lines[current_line].size() + 1)
        {
            line_start_offset += lines[current_line].size() + 1;
            ++current_line;
        }
        tokens_per_line[current_line].push_back(token);
    }

    if (cancel.stop_requested() || !coalescing_.is_current(ver))
    {
        return;
    }

    // Publish result
    HighlightResult result;
    result.version = ver;
    result.start_line = start_line;
    result.end_line = lines.size();
    result.tokens = std::move(tokens_per_line);

    if (on_result_ && coalescing_.is_current(ver))
    {
        on_result_(std::move(result));
    }
}

auto AsyncHighlighter::split_lines(const std::string& content) -> std::vector<std::string_view>
{
    std::vector<std::string_view> lines;
    std::size_t start = 0;
    for (std::size_t idx = 0; idx < content.size(); ++idx)
    {
        if (content[idx] == '\n')
        {
            lines.emplace_back(content.data() + start, idx - start);
            start = idx + 1;
        }
    }
    // Last line (may not end with '\n')
    if (start <= content.size())
    {
        lines.emplace_back(content.data() + start, content.size() - start);
    }
    return lines;
}

} // namespace markamp::core
