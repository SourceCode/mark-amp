#pragma once

#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/gdicmn.h>

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

namespace markamp::ui
{

/// Composite key uniquely identifying a rendered icon parameters.
struct IconCacheKey
{
    std::string icon_name;
    int size_x{0};
    int size_y{0};
    uint32_t color_rgba{0};
    double scale{1.0};

    bool operator==(const IconCacheKey& other) const
    {
        return icon_name == other.icon_name && size_x == other.size_x && size_y == other.size_y &&
               color_rgba == other.color_rgba && scale == other.scale;
    }
};

} // namespace markamp::ui

namespace std
{
template <>
struct hash<markamp::ui::IconCacheKey>
{
    size_t operator()(const markamp::ui::IconCacheKey& key) const
    {
        // Simple hash combination
        size_t h1 = std::hash<std::string>{}(key.icon_name);
        size_t h2 = std::hash<int>{}(key.size_x) ^ (std::hash<int>{}(key.size_y) << 1);
        size_t h3 = std::hash<uint32_t>{}(key.color_rgba);
        size_t h4 = std::hash<double>{}(key.scale);

        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};
} // namespace std

namespace markamp::ui
{

/// Caches fully rendered icon bitmaps to avoid redundant rasterization.
class IconCache
{
public:
    IconCache() = default;
    ~IconCache() = default;

    /// Retrieves an icon from the cache if it exists.
    [[nodiscard]] auto get(const IconCacheKey& key) const -> std::optional<wxBitmap>;

    /// Stores a new icon in the cache.
    auto put(const IconCacheKey& key, const wxBitmap& bmp) -> void;

    /// Clears the entire cache.
    auto clear() -> void;

    /// Returns the number of cached items.
    [[nodiscard]] auto size() const -> size_t;

    /// Helper to construct a color rgba integer from wxColour
    [[nodiscard]] static auto color_to_rgba(const wxColour& c) -> uint32_t;

private:
    std::unordered_map<IconCacheKey, wxBitmap> cache_;
};

} // namespace markamp::ui
