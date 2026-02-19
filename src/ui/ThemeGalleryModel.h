#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Theme style category.
enum class ThemeStyle : uint8_t
{
    kDark,
    kLight,
    kHighContrast,
};

/// Theme entry for gallery display.
struct ThemeEntry
{
    std::string theme_id;
    std::string name;
    std::string author;
    ThemeStyle style{ThemeStyle::kDark};
    bool is_builtin{false};
};

/// Theme token with contrast info.
struct ThemeToken
{
    std::string token_id;
    std::string label;
    std::string group; ///< Token group (e.g., "editor", "sidebar")
    std::string hex_value;
    double contrast_ratio{0.0}; ///< Against background
    bool has_contrast_warning{false};
};

/// Testable model for Theme Gallery Controls (Phase 28).
///
/// Encapsulates:
/// - Theme browsing with style filter
/// - Preview without commit
/// - Token editing with contrast warnings
/// - Safe apply with rollback
class ThemeGalleryModel
{
public:
    void set_themes(std::vector<ThemeEntry> themes);
    [[nodiscard]] auto all_themes() const -> const std::vector<ThemeEntry>&;

    // ── Filtering ───────────────────────────────────────────────────

    [[nodiscard]] auto by_style(ThemeStyle style) const -> std::vector<ThemeEntry>;

    // ── Preview ─────────────────────────────────────────────────────

    void set_preview(const std::string& theme_id);
    void clear_preview();
    [[nodiscard]] auto preview_id() const -> const std::string&;
    [[nodiscard]] auto is_previewing() const -> bool;

    // ── Apply / Rollback ────────────────────────────────────────────

    void apply(const std::string& theme_id);
    void rollback();
    [[nodiscard]] auto active_id() const -> const std::string&;
    [[nodiscard]] auto previous_id() const -> const std::string&;
    [[nodiscard]] auto can_rollback() const -> bool;

    // ── Token editing ───────────────────────────────────────────────

    void set_tokens(std::vector<ThemeToken> tokens);
    [[nodiscard]] auto tokens() const -> const std::vector<ThemeToken>&;
    [[nodiscard]] auto tokens_by_group(const std::string& group) const -> std::vector<ThemeToken>;
    [[nodiscard]] auto contrast_warnings() const -> std::vector<ThemeToken>;

private:
    std::vector<ThemeEntry> themes_;
    std::string preview_id_;
    std::string active_id_;
    std::string previous_id_;
    std::vector<ThemeToken> tokens_;
};

} // namespace markamp::ui
