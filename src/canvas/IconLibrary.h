#pragma once

#include "canvas/CanvasTypes.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// A single entry in the icon library.
struct IconEntry
{
    std::string id;
    std::string name;
    std::string category;
    std::string svg_content;
};

/// Manages a collection of SVG icons organized by category, with built-in
/// icons and custom pack import support.
class IconLibrary
{
public:
    /// Load built-in icons (arrows, checkmarks, stars, etc.).
    auto load_builtins() -> void;

    /// Import a custom SVG icon pack from a directory.
    /// Returns the number of icons imported.
    auto import_pack(const std::filesystem::path& directory, const std::string& category) -> int;

    /// Get all icons in a category.
    [[nodiscard]] auto icons_in_category(const std::string& category) const
        -> std::vector<const IconEntry*>;

    /// Get all category names.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Search icons by name (case-insensitive substring match).
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<const IconEntry*>;

    /// Get icon by ID.
    [[nodiscard]] auto get_icon(const std::string& icon_id) const -> const IconEntry*;

    /// Get the full emoji set.
    [[nodiscard]] auto all_emojis() const -> const std::vector<std::string>&;

    /// Total number of icons in the library.
    [[nodiscard]] auto icon_count() const -> size_t;

private:
    std::unordered_map<std::string, IconEntry> icons_;
    std::vector<std::string> emojis_;

    auto add_icon(const std::string& icon_id,
                  const std::string& icon_name,
                  const std::string& category,
                  const std::string& svg) -> void;
};

} // namespace markamp::canvas
