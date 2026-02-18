/// RenderingSafety.cpp — V7 Phase 27: Rendering pipeline safety

#include "RenderingSafety.h"

namespace markamp::rendering
{

auto RenderingSafety::safe_render(std::function<std::string()> render_fn,
                                  std::function<std::string()> fallback_fn) -> RenderOutcome
{
    auto start_time = std::chrono::steady_clock::now();

    try
    {
        auto output = render_fn();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - start_time);

        // Check output size
        if (output.size() > kMaxRenderOutputBytes)
        {
            logger_.warn("Render output too large (" + std::to_string(output.size()) +
                             " bytes), using fallback",
                         {{"size", std::to_string(output.size())}});
            fallback_count_++;

            auto fallback_output = fallback_fn();
            return RenderOutcome{
                .output = std::move(fallback_output),
                .used_fallback = true,
                .elapsed = elapsed,
            };
        }

        success_count_++;
        return RenderOutcome{
            .output = std::move(output),
            .used_fallback = false,
            .elapsed = elapsed,
        };
    }
    catch (const std::exception& exception)
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - start_time);

        logger_.error("Render failed: " + std::string(exception.what()) + ", using fallback",
                      {{"exception", exception.what()}});
        fallback_count_++;

        try
        {
            auto fallback_output = fallback_fn();
            return RenderOutcome{
                .output = std::move(fallback_output),
                .used_fallback = true,
                .elapsed = elapsed,
            };
        }
        catch (const std::exception& fallback_err)
        {
            logger_.error("Fallback render also failed: " + std::string(fallback_err.what()));
            return RenderOutcome{
                .output = "[Rendering error]",
                .used_fallback = true,
                .elapsed = elapsed,
            };
        }
    }
}

auto RenderingSafety::validate_output_size(const std::string& output) -> Result<void>
{
    if (output.size() > kMaxRenderOutputBytes)
    {
        return std::unexpected(core::make_render_error(
            "Render output exceeds maximum size: " + std::to_string(output.size()) + " > " +
                std::to_string(kMaxRenderOutputBytes),
            ErrorCode::FrameBudgetExceeded));
    }
    return {};
}

void RenderingSafety::reset_counters()
{
    fallback_count_ = 0;
    success_count_ = 0;
}

} // namespace markamp::rendering
