/// @file CellOutputRenderer.cpp
/// @brief V4 Phase 31 – Inline Output Rendering implementation.

#include "ui/CellOutputRenderer.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <regex>

namespace markamp::ui
{

// ============================================================================
// Constructor
// ============================================================================

CellOutputRenderer::CellOutputRenderer(markamp::core::EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// MIME priority selection
// ============================================================================

auto CellOutputRenderer::select_best_mime(const markamp::core::MimeBundle& bundle) -> std::string
{
    // Priority: text/html > image/png > image/svg+xml > application/json > text/plain
    static const std::vector<std::string> kPriority = {
        "text/html", "image/png", "image/svg+xml", "application/json", "text/plain"};

    for (const auto& mime : kPriority)
    {
        if (bundle.has(mime))
        {
            return mime;
        }
    }

    // Fallback: return first available.
    return bundle.best_format();
}

auto CellOutputRenderer::mime_to_output_type(const std::string& mime_type) -> OutputType
{
    if (mime_type == "text/html")
    {
        return OutputType::kHtml;
    }
    if (mime_type == "image/png")
    {
        return OutputType::kImage;
    }
    if (mime_type == "image/svg+xml")
    {
        return OutputType::kSvg;
    }
    if (mime_type == "application/json")
    {
        return OutputType::kJson;
    }
    return OutputType::kPlainText;
}

// ============================================================================
// Rendering
// ============================================================================

auto CellOutputRenderer::render_output(const markamp::core::MimeBundle& bundle) const -> CellOutput
{
    CellOutput output;
    const auto best_mime = select_best_mime(bundle);
    output.mime_type = best_mime;
    output.type = mime_to_output_type(best_mime);
    output.content = bundle.get(best_mime);
    return output;
}

auto CellOutputRenderer::render_error(const std::string& error_name,
                                      const std::string& error_value,
                                      const std::vector<std::string>& traceback) const -> CellOutput
{
    CellOutput output;
    output.type = OutputType::kError;
    output.is_error = true;
    output.mime_type = "text/plain";
    output.traceback = traceback;

    // Format: "ErrorName: error_value"
    output.content = error_name + ": " + error_value;

    // Append traceback lines.
    if (!traceback.empty())
    {
        output.content += "\n\nTraceback:\n";
        for (const auto& line : traceback)
        {
            output.content += "  " + line + "\n";
        }
    }

    return output;
}

auto CellOutputRenderer::render_stream(const std::string& stream_name,
                                       const std::string& text) const -> CellOutput
{
    CellOutput output;
    output.type = OutputType::kStream;
    output.stream_name = stream_name;
    output.mime_type = "text/plain";
    output.content = text;
    return output;
}

// ============================================================================
// ANSI escape code stripping
// ============================================================================

auto CellOutputRenderer::ansi_to_styled(const std::string& text) -> std::string
{
    // Strip ANSI escape sequences: ESC[ ... m  (CSI sequences).
    // Pattern: \033\[[\d;]*m
    std::string result;
    result.reserve(text.size());

    size_t idx = 0;
    while (idx < text.size())
    {
        // Check for ESC character (0x1B).
        if (text[idx] == '\033' && idx + 1 < text.size() && text[idx + 1] == '[')
        {
            // Skip until we find a letter (the terminator).
            idx += 2;
            while (idx < text.size() && !std::isalpha(static_cast<unsigned char>(text[idx])))
            {
                ++idx;
            }
            // Skip the terminator letter.
            if (idx < text.size())
            {
                ++idx;
            }
        }
        else
        {
            result += text[idx];
            ++idx;
        }
    }

    return result;
}

// ============================================================================
// Output state management
// ============================================================================

auto CellOutputRenderer::add_output(const std::string& cell_id, const CellOutput& output) -> void
{
    auto& state = states_[cell_id];
    state.cell_id = cell_id;
    state.cleared = false;

    CellOutput new_output = output;
    new_output.cell_id = cell_id;
    state.outputs.push_back(std::move(new_output));

    core::events::CellOutputRenderedEvent event;
    event.cell_id = cell_id;
    event_bus_.publish(event);
}

auto CellOutputRenderer::clear_outputs(const std::string& cell_id) -> void
{
    auto iter = states_.find(cell_id);
    if (iter != states_.end())
    {
        iter->second.outputs.clear();
        iter->second.cleared = true;
    }
    else
    {
        auto& state = states_[cell_id];
        state.cell_id = cell_id;
        state.cleared = true;
    }

    core::events::CellOutputClearedEvent event;
    event.cell_id = cell_id;
    event_bus_.publish(event);
}

auto CellOutputRenderer::toggle_collapsed(const std::string& cell_id) -> void
{
    auto& state = states_[cell_id];
    state.cell_id = cell_id;
    state.collapsed = !state.collapsed;

    core::events::CellOutputCollapsedEvent event;
    event.cell_id = cell_id;
    event.collapsed = state.collapsed;
    event_bus_.publish(event);
}

auto CellOutputRenderer::get_state(const std::string& cell_id) const -> const CellOutputState*
{
    auto iter = states_.find(cell_id);
    if (iter == states_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto CellOutputRenderer::all_states() const
    -> const std::unordered_map<std::string, CellOutputState>&
{
    return states_;
}

} // namespace markamp::ui
