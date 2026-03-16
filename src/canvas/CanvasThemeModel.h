#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Canvas semantic token.
struct CanvasToken
{
    std::string token_name;
    std::string value;    ///< Current resolved value (hex color, etc.)
    std::string fallback; ///< Fallback if token missing

    // ── Round 4 Batch 4 (#38-40) ────────────────────────────────

    /// (#38) Whether a resolved value is set.
    [[nodiscard]] auto has_value() const noexcept -> bool
    {
        return !value.empty();
    }

    /// (#39) Whether a fallback is configured.
    [[nodiscard]] auto has_fallback() const noexcept -> bool
    {
        return !fallback.empty();
    }

    /// (#40) Whether a token name is set.
    [[nodiscard]] auto has_name() const noexcept -> bool
    {
        return !token_name.empty();
    }
};

/// Board background preset.
enum class BoardBackground : uint8_t
{
    kWhite,
    kLightGray,
    kDarkGray,
    kDotGrid,
    kLineGrid,
    kTransparent,
};

/// Testable model for Canvas Theming (Phase 74).
///
/// Encapsulates:
/// - Semantic token registry for canvas elements
/// - Token resolution with fallback
/// - Board background presets
/// - Legibility guardrail (minimum contrast check)
class CanvasThemeModel
{
public:
    // ── Tokens ──────────────────────────────────────────────────────

    void set_tokens(std::vector<CanvasToken> tokens);
    [[nodiscard]] auto tokens() const -> const std::vector<CanvasToken>&;
    [[nodiscard]] auto resolve(const std::string& token_name) const -> std::string;
    [[nodiscard]] auto missing_tokens() const -> std::vector<std::string>;

    // ── Background ──────────────────────────────────────────────────

    void set_background(BoardBackground preset);
    [[nodiscard]] auto background() const -> BoardBackground;

    // ── Legibility ──────────────────────────────────────────────────

    void set_min_contrast(double ratio);
    [[nodiscard]] auto min_contrast() const -> double;
    [[nodiscard]] auto passes_contrast(double ratio) const -> bool;

private:
    std::vector<CanvasToken> tokens_;
    BoardBackground background_{BoardBackground::kWhite};
    double min_contrast_{3.0};

    // ── Round 4 Batch 5 (#41-45) ────────────────────────────────

    /// (#41) Whether tokens are loaded.
    [[nodiscard]] auto has_tokens() const noexcept -> bool
    {
        return !tokens_.empty();
    }

    /// (#42) Number of registered tokens.
    [[nodiscard]] auto token_count() const noexcept -> size_t
    {
        return tokens_.size();
    }

    /// (#43) Whether the background is dot grid.
    [[nodiscard]] auto is_dot_grid() const noexcept -> bool
    {
        return background_ == BoardBackground::kDotGrid;
    }

    /// (#44) Whether the background is line grid.
    [[nodiscard]] auto is_line_grid() const noexcept -> bool
    {
        return background_ == BoardBackground::kLineGrid;
    }

    /// (#45) Whether the background is transparent.
    [[nodiscard]] auto is_transparent() const noexcept -> bool
    {
        return background_ == BoardBackground::kTransparent;
    }
};

} // namespace markamp::canvas
