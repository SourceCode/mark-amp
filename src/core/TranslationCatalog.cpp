/// @file TranslationCatalog.cpp
/// @brief V9 Phase 47 — TranslationCatalog implementation.

#include "TranslationCatalog.h"

#include <algorithm>

namespace markamp::core
{

void TranslationCatalog::add_translation(const std::string& key,
                                         const std::string& value,
                                         const std::string& locale_id,
                                         const std::string& context)
{
    register_key(key);
    TranslationEntry entry;
    entry.key = key;
    entry.value = value;
    entry.locale_id = locale_id;
    entry.context = context;
    catalog_[locale_id][key] = std::move(entry);
}

auto TranslationCatalog::translate(const std::string& key, const std::string& locale_id) const
    -> std::string
{
    auto locale_it = catalog_.find(locale_id);
    if (locale_it == catalog_.end())
    {
        return key; // Fallback to key
    }
    auto entry_it = locale_it->second.find(key);
    if (entry_it == locale_it->second.end())
    {
        return key; // Fallback to key
    }
    return entry_it->second.value;
}

auto TranslationCatalog::has_translation(const std::string& key, const std::string& locale_id) const
    -> bool
{
    auto locale_it = catalog_.find(locale_id);
    if (locale_it == catalog_.end())
    {
        return false;
    }
    return locale_it->second.find(key) != locale_it->second.end();
}

auto TranslationCatalog::remove_translation(const std::string& key, const std::string& locale_id)
    -> bool
{
    auto locale_it = catalog_.find(locale_id);
    if (locale_it == catalog_.end())
    {
        return false;
    }
    return locale_it->second.erase(key) > 0;
}

auto TranslationCatalog::translations_for_locale(const std::string& locale_id) const
    -> std::vector<TranslationEntry>
{
    std::vector<TranslationEntry> result;
    auto locale_it = catalog_.find(locale_id);
    if (locale_it == catalog_.end())
    {
        return result;
    }
    result.reserve(locale_it->second.size());
    for (const auto& [unused_key, entry] : locale_it->second)
    {
        result.push_back(entry);
    }
    return result;
}

auto TranslationCatalog::missing_translations(const std::string& locale_id) const
    -> std::vector<std::string>
{
    std::vector<std::string> missing;
    for (const auto& key : all_keys_)
    {
        if (!has_translation(key, locale_id))
        {
            missing.push_back(key);
        }
    }
    return missing;
}

auto TranslationCatalog::all_keys() const -> std::vector<std::string>
{
    return all_keys_;
}

void TranslationCatalog::load_defaults()
{
    // Built-in English UI strings
    add_translation("app.title", "MarkAmp", "en-US");
    add_translation("menu.file", "File", "en-US");
    add_translation("menu.edit", "Edit", "en-US");
    add_translation("menu.view", "View", "en-US");
    add_translation("menu.help", "Help", "en-US");
    add_translation("action.save", "Save", "en-US");
    add_translation("action.open", "Open", "en-US");
    add_translation("action.close", "Close", "en-US");
    add_translation("status.ready", "Ready", "en-US");
    add_translation("status.saving", "Saving...", "en-US");
}

auto TranslationCatalog::entry_count() const -> int
{
    int count = 0;
    for (const auto& [unused_locale, entries] : catalog_)
    {
        count += static_cast<int>(entries.size());
    }
    return count;
}

auto TranslationCatalog::locale_count() const -> int
{
    return static_cast<int>(catalog_.size());
}

void TranslationCatalog::clear()
{
    catalog_.clear();
    all_keys_.clear();
}

void TranslationCatalog::register_key(const std::string& key)
{
    if (std::find(all_keys_.begin(), all_keys_.end(), key) == all_keys_.end())
    {
        all_keys_.push_back(key);
    }
}

} // namespace markamp::core
