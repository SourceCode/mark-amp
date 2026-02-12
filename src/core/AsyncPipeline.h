#pragma once

#include "CoalescingTask.h"
#include "SPSCQueue.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>

namespace markamp::core
{

/// Generic asynchronous pipeline template for parser, indexer, lint, etc.
///
/// Keeps analysis off the UI thread. Supports latest-wins coalescing
/// (via CoalescingTask) and lock-free result delivery (via SPSCQueue).
///
/// Uses std::thread + std::atomic<bool> for portability (Apple Clang
/// does not support std::jthread/std::stop_token).
///
/// Usage:
///   AsyncPipeline<std::string, ParseResult> parser([](const std::string& input, CancelToken ct) {
///       return parse(input);  // check ct.stop_requested() periodically
///   });
///   parser.submit("source code");
///   ParseResult result;
///   if (parser.try_get_result(result)) { ... }
///
/// Pattern implemented: #31 Asynchronous layout/analysis pipelines
template <typename Input, typename Output, std::size_t QueueCapacity = 16>
class AsyncPipeline
{
public:
    using Processor = std::function<std::optional<Output>(const Input&, CancelToken)>;

    explicit AsyncPipeline(Processor processor)
        : processor_(std::move(processor))
        , stop_requested_(false)
        , worker_([this] { worker_loop(); })
    {
    }

    ~AsyncPipeline()
    {
        stop();
        if (worker_.joinable())
        {
            worker_.join();
        }
    }

    AsyncPipeline(const AsyncPipeline&) = delete;
    auto operator=(const AsyncPipeline&) -> AsyncPipeline& = delete;
    AsyncPipeline(AsyncPipeline&&) = delete;
    auto operator=(AsyncPipeline&&) -> AsyncPipeline& = delete;

    /// Submit new work. Cancels any in-flight work for a prior version.
    void submit(Input input)
    {
        auto version = ++version_counter_;
        auto cancel_token = coalescer_.submit(version);

        std::lock_guard lock(input_mutex_);
        pending_input_ = std::move(input);
        pending_version_ = version;
        pending_cancel_ = cancel_token;
        has_input_ = true;
        input_cv_.notify_one();
    }

    /// Try to get the latest result (non-blocking).
    [[nodiscard]] auto try_get_result(Output& result) noexcept -> bool
    {
        return results_.try_pop(result);
    }

    /// Check if results are available.
    [[nodiscard]] auto has_result() const noexcept -> bool
    {
        return !results_.empty();
    }

    /// Stop the pipeline gracefully.
    void stop()
    {
        coalescer_.cancel();
        stop_requested_.store(true, std::memory_order_release);
        {
            std::lock_guard lock(input_mutex_);
            has_input_ = true;
            input_cv_.notify_one();
        }
    }

private:
    void worker_loop()
    {
        while (!stop_requested_.load(std::memory_order_acquire))
        {
            Input input;
            uint64_t version = 0;
            CancelToken cancel;

            {
                std::unique_lock lock(input_mutex_);
                input_cv_.wait(
                    lock,
                    [&] { return has_input_ || stop_requested_.load(std::memory_order_acquire); });

                if (stop_requested_.load(std::memory_order_acquire))
                {
                    return;
                }

                input = std::move(pending_input_);
                version = pending_version_;
                cancel = pending_cancel_;
                has_input_ = false;
            }

            // Process if still current
            if (coalescer_.is_current(version) && !cancel.stop_requested())
            {
                auto result = processor_(input, cancel);
                if (result.has_value() && coalescer_.is_current(version))
                {
                    [[maybe_unused]] auto pushed = results_.try_push(std::move(result.value()));
                }
            }
        }
    }

    Processor processor_;
    CoalescingTask coalescer_;
    SPSCQueue<Output, QueueCapacity> results_;

    std::mutex input_mutex_;
    std::condition_variable input_cv_;
    Input pending_input_;
    uint64_t pending_version_{0};
    CancelToken pending_cancel_;
    bool has_input_{false};
    uint64_t version_counter_{0};
    std::atomic<bool> stop_requested_;

    std::thread worker_;
};

} // namespace markamp::core
