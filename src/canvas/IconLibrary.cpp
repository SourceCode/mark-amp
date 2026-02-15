#include "IconLibrary.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <set>
#include <sstream>

namespace markamp::canvas
{

auto IconLibrary::load_builtins() -> void
{
    // A representative set of built-in SVG icons.
    add_icon("arrow-right",
             "Arrow Right",
             "Arrows",
             R"(<svg viewBox="0 0 24 24"><path d="M5 12h14M12 5l7 7-7 7"/></svg>)");
    add_icon("arrow-left",
             "Arrow Left",
             "Arrows",
             R"(<svg viewBox="0 0 24 24"><path d="M19 12H5M12 19l-7-7 7-7"/></svg>)");
    add_icon("arrow-up",
             "Arrow Up",
             "Arrows",
             R"(<svg viewBox="0 0 24 24"><path d="M12 19V5M5 12l7-7 7 7"/></svg>)");
    add_icon("arrow-down",
             "Arrow Down",
             "Arrows",
             R"(<svg viewBox="0 0 24 24"><path d="M12 5v14M19 12l-7 7-7-7"/></svg>)");
    add_icon("check",
             "Checkmark",
             "Symbols",
             R"(<svg viewBox="0 0 24 24"><path d="M20 6L9 17l-5-5"/></svg>)");
    add_icon("x-mark",
             "X Mark",
             "Symbols",
             R"(<svg viewBox="0 0 24 24"><path d="M18 6L6 18M6 6l12 12"/></svg>)");
    add_icon(
        "star",
        "Star",
        "Symbols",
        R"(<svg viewBox="0 0 24 24"><path d="M12 2l3.09 6.26L22 9.27l-5 4.87 1.18 6.88L12 17.77l-6.18 3.25L7 14.14 2 9.27l6.91-1.01L12 2z"/></svg>)");
    add_icon(
        "warning",
        "Warning",
        "Symbols",
        R"(<svg viewBox="0 0 24 24"><path d="M12 9v4M12 17h.01M10.29 3.86L1.82 18a2 2 0 001.71 3h16.94a2 2 0 001.71-3L13.71 3.86a2 2 0 00-3.42 0z"/></svg>)");
    add_icon(
        "info",
        "Info",
        "Symbols",
        R"(<svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="10"/><path d="M12 16v-4M12 8h.01"/></svg>)");
    add_icon(
        "heart",
        "Heart",
        "Symbols",
        R"(<svg viewBox="0 0 24 24"><path d="M20.84 4.61a5.5 5.5 0 00-7.78 0L12 5.67l-1.06-1.06a5.5 5.5 0 00-7.78 7.78l1.06 1.06L12 21.23l7.78-7.78 1.06-1.06a5.5 5.5 0 000-7.78z"/></svg>)");

    // Common emojis for reaction badges.
    emojis_ = {"👍", "👎", "❤️", "😀", "😂", "🎉", "🔥", "⭐", "✅", "❌", "💡", "📌", "🚀", "⚠️", "💬", "👀"};
}

auto IconLibrary::import_pack(const std::filesystem::path& directory, const std::string& category)
    -> int
{
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
    {
        return 0;
    }

    int imported = 0;
    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }
        if (entry.path().extension() != ".svg")
        {
            continue;
        }

        std::ifstream file(entry.path());
        if (!file.is_open())
        {
            continue;
        }

        std::ostringstream content;
        content << file.rdbuf();

        const std::string icon_id = category + "/" + entry.path().stem().string();
        const std::string icon_name = entry.path().stem().string();

        add_icon(icon_id, icon_name, category, content.str());
        ++imported;
    }

    return imported;
}

auto IconLibrary::icons_in_category(const std::string& category) const
    -> std::vector<const IconEntry*>
{
    std::vector<const IconEntry*> result;
    for (const auto& [id, entry] : icons_)
    {
        if (entry.category == category)
        {
            result.push_back(&entry);
        }
    }
    // Sort alphabetically for consistent output.
    std::sort(result.begin(),
              result.end(),
              [](const IconEntry* lhs, const IconEntry* rhs) { return lhs->name < rhs->name; });
    return result;
}

auto IconLibrary::categories() const -> std::vector<std::string>
{
    std::set<std::string> unique_categories;
    for (const auto& [id, entry] : icons_)
    {
        unique_categories.insert(entry.category);
    }
    return {unique_categories.begin(), unique_categories.end()};
}

auto IconLibrary::search(const std::string& query) const -> std::vector<const IconEntry*>
{
    std::vector<const IconEntry*> result;
    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    for (const auto& [id, entry] : icons_)
    {
        std::string lower_name = entry.name;
        std::transform(lower_name.begin(),
                       lower_name.end(),
                       lower_name.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

        if (lower_name.find(lower_query) != std::string::npos)
        {
            result.push_back(&entry);
        }
    }
    std::sort(result.begin(),
              result.end(),
              [](const IconEntry* lhs, const IconEntry* rhs) { return lhs->name < rhs->name; });
    return result;
}

auto IconLibrary::get_icon(const std::string& icon_id) const -> const IconEntry*
{
    const auto iter = icons_.find(icon_id);
    return iter != icons_.end() ? &iter->second : nullptr;
}

auto IconLibrary::all_emojis() const -> const std::vector<std::string>&
{
    return emojis_;
}

auto IconLibrary::icon_count() const -> size_t
{
    return icons_.size();
}

auto IconLibrary::add_icon(const std::string& icon_id,
                           const std::string& icon_name,
                           const std::string& category,
                           const std::string& svg) -> void
{
    icons_[icon_id] = IconEntry{icon_id, icon_name, category, svg};
}

} // namespace markamp::canvas
