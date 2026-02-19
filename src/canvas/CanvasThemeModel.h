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
};

} // namespace markamp::canvas
