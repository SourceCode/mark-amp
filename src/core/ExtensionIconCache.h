#pragma once

#include <wx/bitmap.h>

#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

namespace markamp::core
{

/// Phase 20 Task 12: Async extension icon loader with disk caching.
/// Downloads icons from manifest URLs, caches to .markamp/extension-icons/,
/// generates colored letter avatars as placeholders.
class ExtensionIconCache
{
public:
    explicit ExtensionIconCache(const std::filesystem::path& cache_dir);

    /// Get a cached icon bitmap. Returns placeholder if not yet loaded.
    /// Starts async download if icon URL is provided and not cached.
    [[nodiscard]] auto GetIcon(const std::string& extension_id,
                               const std::string& icon_url,
                               const std::string& extension_name,
                               int size = 48) -> wxBitmap;

    /// Generate a colored letter avatar for an extension.
    [[nodiscard]] static auto GenerateAvatar(const std::string& extension_name, int size = 48)
        -> wxBitmap;

    /// Check if an icon is cached on disk.
    [[nodiscard]] auto IsCached(const std::string& extension_id) const -> bool;

    /// Clear all cached icons.
    void ClearAll();

    /// Evict icons older than 7 days.
    void EvictExpired();

    /// Set callback for when an icon finishes loading (for UI refresh).
    void SetOnIconLoaded(std::function<void(const std::string& extension_id)> callback);

    /// Cache expiry: 7 days.
    static constexpr auto kCacheExpiry = std::chrono::hours(24 * 7);

private:
    std::filesystem::path cache_dir_;
    mutable std::mutex mutex_;

    // In-memory bitmap cache
    std::unordered_map<std::string, wxBitmap> bitmap_cache_;

    std::function<void(const std::string&)> on_icon_loaded_;

    /// Get the disk path for a cached icon.
    [[nodiscard]] auto CachePath(const std::string& extension_id) const -> std::filesystem::path;

    /// Derive a consistent color from extension name hash.
    [[nodiscard]] static auto ColorFromName(const std::string& name) -> wxColour;
};

} // namespace markamp::core
