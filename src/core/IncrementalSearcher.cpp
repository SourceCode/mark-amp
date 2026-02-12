#include "IncrementalSearcher.h"

#include <algorithm>
#include <cctype>

namespace markamp::core
{

IncrementalSearcher::~IncrementalSearcher()
{
    cancel();
}

void IncrementalSearcher::search(const std::string& content,
                                 SearchConfig config,
                                 MatchCallback on_match,
                                 CompleteCallback on_complete)
{
    // Cancel previous search
    cancel();

    auto version = coalescing_.current_version() + 1;
    auto cancel_token = coalescing_.submit(version);

    searching_.store(true, std::memory_order_release);
    worker_ = std::thread(
        [this,
         content_copy = content,
         cfg = std::move(config),
         match_cb = std::move(on_match),
         complete_cb = std::move(on_complete),
         ver = version,
         cancel_tok = cancel_token]() mutable
        {
            search_worker(cancel_tok,
                          std::move(content_copy),
                          std::move(cfg),
                          std::move(match_cb),
                          std::move(complete_cb),
                          ver);
        });
}

void IncrementalSearcher::cancel()
{
    coalescing_.cancel();
    if (worker_.joinable())
    {
        worker_.join();
    }
    searching_.store(false, std::memory_order_release);
}

auto IncrementalSearcher::is_searching() const noexcept -> bool
{
    return searching_.load(std::memory_order_acquire);
}

void IncrementalSearcher::search_worker(CancelToken cancel,
                                        std::string content,
                                        SearchConfig config,
                                        MatchCallback on_match,
                                        CompleteCallback on_complete,
                                        uint64_t version)
{
    std::size_t total_matches = 0;
    std::size_t line_number = 0;
    std::size_t line_start = 0;

    // Prepare needle for case-insensitive search
    std::string needle = config.needle;
    std::string search_content = content;
    if (!config.case_sensitive)
    {
        std::transform(needle.begin(),
                       needle.end(),
                       needle.begin(),
                       [](unsigned char character)
                       { return static_cast<char>(std::tolower(character)); });
        std::transform(search_content.begin(),
                       search_content.end(),
                       search_content.begin(),
                       [](unsigned char character)
                       { return static_cast<char>(std::tolower(character)); });
    }

    for (std::size_t idx = 0; idx < search_content.size(); ++idx)
    {
        if (cancel.stop_requested() || !coalescing_.is_current(version))
        {
            searching_.store(false, std::memory_order_release);
            return;
        }

        // Track line boundaries
        if (idx > 0 && content[idx - 1] == '\n')
        {
            ++line_number;
            line_start = idx;
        }

        // Check for match at current position
        if (idx + needle.size() <= search_content.size() &&
            search_content.compare(idx, needle.size(), needle) == 0)
        {
            // Check whole-word boundary if needed
            if (config.whole_word)
            {
                bool left_boundary =
                    (idx == 0) || !std::isalnum(static_cast<unsigned char>(content[idx - 1]));
                bool right_boundary =
                    (idx + needle.size() >= content.size()) ||
                    !std::isalnum(static_cast<unsigned char>(content[idx + needle.size()]));
                if (!left_boundary || !right_boundary)
                {
                    continue;
                }
            }

            ++total_matches;

            // Find end of current line for context
            auto line_end = content.find('\n', line_start);
            if (line_end == std::string::npos)
            {
                line_end = content.size();
            }
            std::string_view line_context(content.data() + line_start, line_end - line_start);

            if (on_match)
            {
                bool should_continue = on_match(line_number, idx - line_start, line_context);
                if (!should_continue)
                {
                    break;
                }
            }
        }
    }

    if (on_complete && coalescing_.is_current(version))
    {
        on_complete(total_matches);
    }

    searching_.store(false, std::memory_order_release);
}

} // namespace markamp::core
