/// @file PresentationEngine.h
/// @brief V4 Phase 27 – Slide/Presentation Mode (pure-logic engine).
/// Parses Markdown into slides, handles navigation, extracts speaker notes.
/// No wxWidgets dependency — UI rendering is separate.

#pragma once

#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;
}

namespace markamp::ui
{

/// A single slide in a presentation.
struct Slide
{
    int index{0};
    std::string title;
    std::string markdown_content;
    std::string speaker_notes; ///< Content after <!-- notes --> marker
    bool has_code{false};
    bool has_image{false};
};

/// Configuration for presentation parsing and display.
struct PresentationConfig
{
    std::string slide_separator{"---"};
    bool use_heading_breaks{true};  ///< H1 starts a new slide
    int heading_break_level{1};     ///< 1 = H1 only, 2 = H1 and H2
    std::string transition{"fade"}; ///< "fade", "slide", "none"
    int transition_duration_ms{300};
    bool show_slide_numbers{true};
    bool show_progress_bar{true};
    double aspect_ratio{16.0 / 9.0};
};

/// Pure-logic presentation engine.
/// Parses Markdown into slides, manages navigation, and tracks state.
class PresentationEngine
{
public:
    explicit PresentationEngine(markamp::core::EventBus& event_bus);

    /// Parse markdown into slides.
    auto load_document(const std::string& markdown) -> void;

    /// Parse slides with a custom config.
    auto load_document(const std::string& markdown, const PresentationConfig& config) -> void;

    /// Enter presentation mode.
    auto start() -> void;

    /// Exit presentation mode.
    auto stop() -> void;

    /// Navigate to a specific slide.
    auto go_to_slide(int index) -> void;

    /// Next slide (bounds-checked).
    auto next_slide() -> void;

    /// Previous slide (bounds-checked).
    auto previous_slide() -> void;

    /// Total slide count.
    [[nodiscard]] auto slide_count() const -> int;

    /// Current slide index.
    [[nodiscard]] auto current_slide() const -> int;

    /// Get speaker notes for the current slide.
    [[nodiscard]] auto speaker_notes() const -> const std::string&;

    /// Get a specific slide.
    [[nodiscard]] auto get_slide(int index) const -> const Slide&;

    /// Get all slides.
    [[nodiscard]] auto slides() const -> const std::vector<Slide>&;

    /// Progress through the deck (0.0–1.0).
    [[nodiscard]] auto progress() const -> double;

    /// Whether we are in presentation mode.
    [[nodiscard]] auto is_presenting() const -> bool;

    /// Get config.
    [[nodiscard]] auto config() const -> const PresentationConfig&;

    /// Set config.
    auto set_config(const PresentationConfig& config) -> void;

private:
    markamp::core::EventBus& event_bus_;

    std::vector<Slide> slides_;
    int current_index_{0};
    PresentationConfig config_;
    bool is_presenting_{false};

    /// Internal slide parser.
    [[nodiscard]] auto parse_slides(const std::string& markdown,
                                    const PresentationConfig& cfg) const -> std::vector<Slide>;

    /// Extract speaker notes from slide content.
    static auto extract_speaker_notes(std::string& content) -> std::string;

    /// Detect code blocks and images in content.
    static auto detect_content_types(Slide& slide) -> void;

    /// Extract title from slide content.
    static auto extract_title(const std::string& content) -> std::string;

    static inline const std::string kEmptyString;
};

} // namespace markamp::ui
