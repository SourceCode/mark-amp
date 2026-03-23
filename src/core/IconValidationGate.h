/// @file IconValidationGate.h
/// @brief P10-T05 + V27-P01-T03: Legacy icon debt removal and migration validation.
///
/// Static checks for banned legacy icon IDs, migration completeness
/// validation, and extension compatibility boundaries. V27 adds emoji
/// pattern banning, per-surface validation, and pattern-based scanning.
#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// A validation result entry.
struct IconValidationResult
{
    std::string icon_id;
    std::string issue; ///< "banned_legacy", "unmapped", "missing_asset", "v27_emoji", "v27_glyph"
    std::string location;
};

/// V27: A banned icon pattern entry.
struct V27BannedPattern
{
    std::string pattern;       ///< The banned string or pattern
    std::string category;      ///< "emoji", "legacy_char", "glyph_fallback"
    std::string replacement;   ///< Suggested MUI replacement
    std::string description;   ///< Why this is banned
};

/// Validates icon usage against migration rules.
class IconValidationGate
{
public:
    IconValidationGate();

    /// Add a banned legacy icon ID.
    void ban_legacy_id(const std::string& legacy_id);

    /// Check if an icon ID is banned.
    [[nodiscard]] auto is_banned(const std::string& icon_id) const -> bool;

    /// Validate an icon usage.
    [[nodiscard]] auto validate(const std::string& icon_id,
                                const std::string& location) const -> IconValidationResult;

    /// Get all banned IDs.
    [[nodiscard]] auto banned_ids() const -> const std::unordered_set<std::string>&
    {
        return banned_ids_;
    }

    /// Get banned ID count.
    [[nodiscard]] auto banned_count() const -> int
    {
        return static_cast<int>(banned_ids_.size());
    }

    /// Check if migration is complete (no banned IDs in use).
    [[nodiscard]] auto migration_complete() const -> bool { return violations_ == 0; }

    /// Get violation count.
    [[nodiscard]] auto violation_count() const -> int { return violations_; }

    /// Record a violation.
    void record_violation() { ++violations_; }

    // ── V27 additions ─────────────────────────────────────────────────────

    /// V27: Register all V27 banned patterns (emoji, legacy chars, glyph fallbacks).
    void register_v27_banned_patterns();

    /// V27: Get all banned patterns.
    [[nodiscard]] auto v27_banned_patterns() const -> const std::vector<V27BannedPattern>&
    {
        return v27_patterns_;
    }

    /// V27: Count of V27 banned patterns.
    [[nodiscard]] auto v27_banned_pattern_count() const noexcept -> int
    {
        return static_cast<int>(v27_patterns_.size());
    }

    /// V27: Validate a surface for banned icon usage.
    [[nodiscard]] auto validate_surface(const std::string& surface_name,
                                         const std::vector<std::string>& icon_ids) const
        -> std::vector<IconValidationResult>;

    /// V27: Check if a string contains any banned emoji pattern.
    [[nodiscard]] auto contains_banned_emoji(const std::string& text) const -> bool;

    /// V27: Check if a string contains any banned legacy char pattern.
    [[nodiscard]] auto contains_banned_legacy_char(const std::string& text) const -> bool;

private:
    void register_default_bans();

    std::unordered_set<std::string> banned_ids_;
    int violations_{0};
    std::vector<V27BannedPattern> v27_patterns_;       ///< V27: structured banned patterns
    std::unordered_set<std::string> v27_banned_emojis_; ///< V27: quick lookup for emoji patterns
};

} // namespace markamp::core
