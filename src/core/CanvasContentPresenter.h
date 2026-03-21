/// @file CanvasContentPresenter.h
/// @brief V20 P08-T03: Canvas content presentation and embedded styling.
///
/// Standardizes canvas object styling, text rendering, inspector chrome,
/// and embedded code-block presentation for visual parity.
#pragma once

#include "EventBus.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Canvas object visual state for chrome rendering.
enum class CanvasObjectVisualState
{
    kNormal,     ///< Default state
    kHovered,    ///< Mouse hover
    kSelected,   ///< Selected
    kEditing,    ///< In-place editing
    kLocked,     ///< Locked (non-interactive)
    kDragging    ///< Being dragged
};

/// Style preset for canvas objects.
struct CanvasObjectStyle
{
    std::string style_id;              ///< Unique style identifier
    std::string border_color;          ///< Border/stroke color
    std::string fill_color;            ///< Background fill
    std::string text_color;            ///< Text foreground
    int border_width{1};               ///< Border thickness (px)
    int corner_radius{4};              ///< Corner rounding (px)
    int padding{8};                    ///< Internal padding (px)
    double opacity{1.0};               ///< Object opacity (0.0-1.0)

    [[nodiscard]] auto has_border() const noexcept -> bool { return border_width > 0; }
    [[nodiscard]] auto has_fill() const noexcept -> bool { return !fill_color.empty(); }
};

/// Embedded content type within canvas objects.
enum class EmbeddedContentType
{
    kPlainText,     ///< Plain text
    kRichText,      ///< Markdown/rich text
    kCodeBlock,     ///< Syntax-highlighted code
    kImage,         ///< Image content
    kDiagram        ///< Mermaid/diagram content
};

/// Manages canvas content presentation rules and object styling.
class CanvasContentPresenter
{
public:
    explicit CanvasContentPresenter(EventBus& bus);

    /// Register a canvas object style preset.
    void register_style(const CanvasObjectStyle& style);

    /// Look up a style by ID.
    [[nodiscard]] auto style(const std::string& style_id) const -> const CanvasObjectStyle*;

    /// All registered styles.
    [[nodiscard]] auto all_styles() const -> std::vector<CanvasObjectStyle>;

    /// Set visual state for an object.
    void set_object_visual_state(const std::string& object_id, CanvasObjectVisualState state);

    /// Get visual state for an object.
    [[nodiscard]] auto object_visual_state(const std::string& object_id) const -> CanvasObjectVisualState;

    /// Register an embedded content type for styling.
    void register_embedded_content(const std::string& object_id, EmbeddedContentType content_type);

    /// Get embedded content type for an object.
    [[nodiscard]] auto embedded_content_type(const std::string& object_id) const -> EmbeddedContentType;

    /// Total registered styles.
    [[nodiscard]] auto style_count() const noexcept -> int
    {
        return static_cast<int>(styles_.size());
    }

    /// Total state updates.
    [[nodiscard]] auto update_count() const noexcept -> int { return update_count_; }

    /// Populate default canvas styles.
    void register_defaults();

private:
    EventBus& event_bus_;
    std::unordered_map<std::string, CanvasObjectStyle> styles_;
    std::unordered_map<std::string, CanvasObjectVisualState> object_states_;
    std::unordered_map<std::string, EmbeddedContentType> embedded_types_;
    int update_count_{0};
};

} // namespace markamp::core
