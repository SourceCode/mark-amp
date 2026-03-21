/// @file ContentStyleSystem.h
/// @brief V20 P08-T01/T04/T05: Unified content styling system.
///
/// Defines shared typography, spacing, and density tokens across editor,
/// notebook, and canvas surfaces. Provides feedback-token registration
/// for consistent diagnostics, search, and selection rendering.
#pragma once

#include "EventBus.h"
#include "ThemeEngine.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Typography scale level for content surfaces.
enum class TypeScaleLevel
{
    kCaption,    ///< Smallest readable text
    kBody,       ///< Default body text
    kSubheading, ///< Section headings
    kHeading,    ///< Major headings
    kTitle,      ///< Page titles
    kDisplay     ///< Large display text
};

/// Font family role.
enum class FontRole
{
    kMonospace,      ///< Code and terminal
    kProportional,   ///< UI labels and prose
    kEditorContent   ///< Editor body (may differ from code)
};

/// Spacing scale token.
struct SpacingToken
{
    std::string name;       ///< e.g. "spacing.xs", "spacing.md"
    int value_px{0};        ///< Pixel value at 1x scale

    [[nodiscard]] auto scaled(double dpi_factor) const noexcept -> int
    {
        return static_cast<int>(static_cast<double>(value_px) * dpi_factor);
    }
};

/// Typography token describing font metrics.
struct TypographyToken
{
    std::string name;         ///< e.g. "type.body", "type.heading"
    FontRole font_role{FontRole::kProportional};
    int font_size_px{14};
    int line_height_px{20};
    int letter_spacing{0};    ///< In hundredths of a pixel

    [[nodiscard]] auto line_height_ratio() const noexcept -> double
    {
        return font_size_px > 0
                   ? static_cast<double>(line_height_px) / static_cast<double>(font_size_px)
                   : 1.0;
    }
};

/// Feedback token for diagnostics, search, and selection rendering.
struct FeedbackStyleToken
{
    std::string scope;            ///< e.g. "feedback.error", "feedback.searchHit"
    std::string foreground;       ///< Hex color
    std::string background;       ///< Hex color (may be semi-transparent)
    std::string border_color;     ///< Underline/border color
    bool is_underline{false};
    bool is_highlight{false};
};

/// Manages shared content styling tokens across all surfaces.
class ContentStyleSystem
{
public:
    explicit ContentStyleSystem(EventBus& bus);

    /// Register a spacing token.
    void register_spacing(const SpacingToken& token);

    /// Register a typography token.
    void register_typography(const TypographyToken& token);

    /// Register a feedback style token.
    void register_feedback_style(const FeedbackStyleToken& token);

    /// Look up spacing by name.
    [[nodiscard]] auto spacing(const std::string& name) const -> const SpacingToken*;

    /// Look up typography by name.
    [[nodiscard]] auto typography(const std::string& name) const -> const TypographyToken*;

    /// Look up feedback style by scope.
    [[nodiscard]] auto feedback_style(const std::string& scope) const -> const FeedbackStyleToken*;

    /// All registered spacing tokens.
    [[nodiscard]] auto all_spacing() const -> std::vector<SpacingToken>;

    /// All registered typography tokens.
    [[nodiscard]] auto all_typography() const -> std::vector<TypographyToken>;

    /// All registered feedback styles.
    [[nodiscard]] auto all_feedback_styles() const -> std::vector<FeedbackStyleToken>;

    /// Total token count.
    [[nodiscard]] auto total_tokens() const noexcept -> int
    {
        return static_cast<int>(spacing_.size() + typography_.size() + feedback_styles_.size());
    }

    /// Populate default design tokens.
    void register_defaults();

private:
    EventBus& event_bus_;
    std::unordered_map<std::string, SpacingToken> spacing_;
    std::unordered_map<std::string, TypographyToken> typography_;
    std::unordered_map<std::string, FeedbackStyleToken> feedback_styles_;
};

} // namespace markamp::core
