/// @file PresentationEngine.cpp
/// @brief V4 Phase 27 – Slide/Presentation Mode implementation.

#include "ui/PresentationEngine.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <sstream>

namespace markamp::ui
{

// ============================================================================
// Constructor
// ============================================================================

PresentationEngine::PresentationEngine(markamp::core::EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// Loading
// ============================================================================

auto PresentationEngine::load_document(const std::string& markdown) -> void
{
    slides_ = parse_slides(markdown, config_);
    current_index_ = 0;
}

auto PresentationEngine::load_document(const std::string& markdown,
                                       const PresentationConfig& config) -> void
{
    config_ = config;
    slides_ = parse_slides(markdown, config_);
    current_index_ = 0;
}

// ============================================================================
// Presentation lifecycle
// ============================================================================

auto PresentationEngine::start() -> void
{
    is_presenting_ = true;
    current_index_ = 0;
    core::events::PresentationStartedEvent start_event;
    start_event.slide_count = static_cast<int>(slides_.size());
    event_bus_.publish(start_event);
}

auto PresentationEngine::stop() -> void
{
    is_presenting_ = false;
    event_bus_.publish(core::events::PresentationEndedEvent{});
}

// ============================================================================
// Navigation
// ============================================================================

auto PresentationEngine::go_to_slide(int index) -> void
{
    if (slides_.empty())
    {
        return;
    }
    current_index_ = std::clamp(index, 0, static_cast<int>(slides_.size()) - 1);
    core::events::SlideChangedEvent goto_event;
    goto_event.slide_index = current_index_;
    goto_event.total_slides = static_cast<int>(slides_.size());
    event_bus_.publish(goto_event);
}

auto PresentationEngine::next_slide() -> void
{
    if (current_index_ < static_cast<int>(slides_.size()) - 1)
    {
        ++current_index_;
        core::events::SlideChangedEvent next_event;
        next_event.slide_index = current_index_;
        next_event.total_slides = static_cast<int>(slides_.size());
        event_bus_.publish(next_event);
    }
}

auto PresentationEngine::previous_slide() -> void
{
    if (current_index_ > 0)
    {
        --current_index_;
        core::events::SlideChangedEvent prev_event;
        prev_event.slide_index = current_index_;
        prev_event.total_slides = static_cast<int>(slides_.size());
        event_bus_.publish(prev_event);
    }
}

// ============================================================================
// Accessors
// ============================================================================

auto PresentationEngine::slide_count() const -> int
{
    return static_cast<int>(slides_.size());
}

auto PresentationEngine::current_slide() const -> int
{
    return current_index_;
}

auto PresentationEngine::speaker_notes() const -> const std::string&
{
    if (slides_.empty())
    {
        return kEmptyString;
    }
    return slides_[static_cast<size_t>(current_index_)].speaker_notes;
}

auto PresentationEngine::get_slide(int index) const -> const Slide&
{
    return slides_.at(static_cast<size_t>(index));
}

auto PresentationEngine::slides() const -> const std::vector<Slide>&
{
    return slides_;
}

auto PresentationEngine::progress() const -> double
{
    if (slides_.size() <= 1)
    {
        return slides_.empty() ? 0.0 : 1.0;
    }
    return static_cast<double>(current_index_) / static_cast<double>(slides_.size() - 1);
}

auto PresentationEngine::is_presenting() const -> bool
{
    return is_presenting_;
}

auto PresentationEngine::config() const -> const PresentationConfig&
{
    return config_;
}

auto PresentationEngine::set_config(const PresentationConfig& config) -> void
{
    config_ = config;
}

// ============================================================================
// Slide parsing
// ============================================================================

auto PresentationEngine::parse_slides(const std::string& markdown,
                                      const PresentationConfig& cfg) const -> std::vector<Slide>
{
    std::vector<Slide> result;

    if (markdown.empty())
    {
        return result;
    }

    // Split into lines.
    std::vector<std::string> lines;
    std::istringstream stream(markdown);
    std::string line;
    while (std::getline(stream, line))
    {
        lines.push_back(line);
    }

    // Accumulate content for current slide.
    std::string current_content;
    bool has_content = false;

    auto flush_slide = [&]()
    {
        if (!has_content)
        {
            return;
        }
        // Trim trailing newlines.
        while (!current_content.empty() && current_content.back() == '\n')
        {
            current_content.pop_back();
        }

        Slide slide;
        slide.index = static_cast<int>(result.size());
        slide.markdown_content = current_content;

        // Extract speaker notes.
        slide.speaker_notes = extract_speaker_notes(slide.markdown_content);

        // Extract title.
        slide.title = extract_title(slide.markdown_content);

        // Detect content types.
        detect_content_types(slide);

        result.push_back(std::move(slide));
        current_content.clear();
        has_content = false;
    };

    for (const auto& cur_line : lines)
    {
        // Check for --- separator.
        if (cur_line == cfg.slide_separator)
        {
            flush_slide();
            continue;
        }

        // Check for heading break.
        if (cfg.use_heading_breaks)
        {
            bool is_heading_break = false;
            if (cfg.heading_break_level >= 1 && cur_line.size() >= 2 &&
                cur_line.substr(0, 2) == "# " && (cur_line.size() < 3 || cur_line[2] != '#'))
            {
                is_heading_break = true;
            }
            if (cfg.heading_break_level >= 2 && cur_line.size() >= 3 &&
                cur_line.substr(0, 3) == "## " && (cur_line.size() < 4 || cur_line[3] != '#'))
            {
                is_heading_break = true;
            }

            if (is_heading_break && has_content)
            {
                flush_slide();
            }
        }

        if (!current_content.empty())
        {
            current_content += '\n';
        }
        current_content += cur_line;
        has_content = true;
    }

    // Flush final slide.
    flush_slide();

    return result;
}

// ============================================================================
// Speaker notes extraction
// ============================================================================

auto PresentationEngine::extract_speaker_notes(std::string& content) -> std::string
{
    const std::string marker = "<!-- notes -->";
    auto pos = content.find(marker);
    if (pos == std::string::npos)
    {
        return "";
    }

    std::string notes = content.substr(pos + marker.size());
    content = content.substr(0, pos);

    // Trim leading/trailing whitespace from notes.
    while (!notes.empty() && (notes.front() == '\n' || notes.front() == ' '))
    {
        notes.erase(notes.begin());
    }
    while (!notes.empty() && (notes.back() == '\n' || notes.back() == ' '))
    {
        notes.pop_back();
    }

    // Trim trailing whitespace from content.
    while (!content.empty() && (content.back() == '\n' || content.back() == ' '))
    {
        content.pop_back();
    }

    return notes;
}

// ============================================================================
// Content type detection
// ============================================================================

auto PresentationEngine::detect_content_types(Slide& slide) -> void
{
    slide.has_code = slide.markdown_content.find("```") != std::string::npos;
    slide.has_image = slide.markdown_content.find("![") != std::string::npos;
}

// ============================================================================
// Title extraction
// ============================================================================

auto PresentationEngine::extract_title(const std::string& content) -> std::string
{
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line))
    {
        // Find first heading (any level).
        if (line.size() >= 2 && line[0] == '#')
        {
            size_t hash_end = line.find_first_not_of('#');
            if (hash_end != std::string::npos && hash_end < line.size() && line[hash_end] == ' ')
            {
                return line.substr(hash_end + 1);
            }
        }
    }
    return "";
}

} // namespace markamp::ui
