/// @file TranslationCatalog.h
/// @brief V9 Phase 47 — Key-based string translation catalog.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// A single translation entry.
struct TranslationEntry
{
    std::string key;
    std::string value;
    std::string locale_id;
    std::string context; ///< Disambiguation context
};

/// Key-based translation catalog for multiple locales.
class TranslationCatalog
{
public:
    TranslationCatalog() = default;

    // ── Translation management ────────────────────────────────────────
    void add_translation(const std::string& key,
                         const std::string& value,
                         const std::string& locale_id,
                         const std::string& context = "");
    [[nodiscard]] auto translate(const std::string& key, const std::string& locale_id) const
        -> std::string;
    [[nodiscard]] auto has_translation(const std::string& key, const std::string& locale_id) const
        -> bool;
    auto remove_translation(const std::string& key, const std::string& locale_id) -> bool;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto translations_for_locale(const std::string& locale_id) const
        -> std::vector<TranslationEntry>;
    [[nodiscard]] auto missing_translations(const std::string& locale_id) const
        -> std::vector<std::string>;
    [[nodiscard]] auto all_keys() const -> std::vector<std::string>;

    // ── Defaults ──────────────────────────────────────────────────────
    void load_defaults();

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto entry_count() const -> int;
    [[nodiscard]] auto locale_count() const -> int;
    void clear();

private:
    /// Storage: locale_id -> (key -> entry)
    std::unordered_map<std::string, std::unordered_map<std::string, TranslationEntry>> catalog_;
    /// All known keys across locales
    std::vector<std::string> all_keys_;

    void register_key(const std::string& key);
};

} // namespace markamp::core
