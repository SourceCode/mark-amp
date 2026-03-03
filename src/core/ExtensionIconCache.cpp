#include "ExtensionIconCache.h"

#include "Logger.h"

#include <wx/dcmemory.h>
#include <wx/image.h>

#include <functional>

namespace markamp::core
{

ExtensionIconCache::ExtensionIconCache(const std::filesystem::path& cache_dir)
    : cache_dir_(cache_dir)
{
    std::error_code error_code;
    std::filesystem::create_directories(cache_dir_, error_code);
}

auto ExtensionIconCache::GetIcon(const std::string& extension_id,
                                 const std::string& icon_url,
                                 const std::string& extension_name,
                                 int size) -> wxBitmap
{
    std::lock_guard lock(mutex_);

    // 1. Check in-memory cache
    auto mem_it = bitmap_cache_.find(extension_id);
    if (mem_it != bitmap_cache_.end())
    {
        return mem_it->second;
    }

    // 2. Check disk cache
    const auto disk_path = CachePath(extension_id);
    if (std::filesystem::exists(disk_path))
    {
        wxImage img;
        if (img.LoadFile(disk_path.string()))
        {
            img.Rescale(size, size, wxIMAGE_QUALITY_HIGH);
            wxBitmap bmp(img);
            bitmap_cache_[extension_id] = bmp;
            return bmp;
        }
    }

    // 3. Generate placeholder avatar
    auto avatar = GenerateAvatar(extension_name, size);
    bitmap_cache_[extension_id] = avatar;

    // 4. Future: queue async download from icon_url
    if (!icon_url.empty())
    {
        MARKAMP_LOG_INFO(
            "ExtensionIconCache: would download icon for {} from {}", extension_id, icon_url);
    }

    return avatar;
}

auto ExtensionIconCache::GenerateAvatar(const std::string& extension_name, int size) -> wxBitmap
{
    wxBitmap bmp(size, size);
    wxMemoryDC memdc;
    memdc.SelectObject(bmp);

    // Background: color derived from name
    const auto bg_color = ColorFromName(extension_name);
    memdc.SetBackground(wxBrush(bg_color));
    memdc.Clear();

    // Letter: first character, white
    if (!extension_name.empty())
    {
        const std::string letter(
            1, static_cast<char>(std::toupper(static_cast<unsigned char>(extension_name[0]))));
        memdc.SetFont(wxFont(size / 2, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        memdc.SetTextForeground(*wxWHITE);

        wxSize text_size = memdc.GetTextExtent(letter);
        int text_x = (size - text_size.GetWidth()) / 2;
        int text_y = (size - text_size.GetHeight()) / 2;
        memdc.DrawText(letter, text_x, text_y);
    }

    memdc.SelectObject(wxNullBitmap);
    return bmp;
}

auto ExtensionIconCache::IsCached(const std::string& extension_id) const -> bool
{
    return std::filesystem::exists(CachePath(extension_id));
}

void ExtensionIconCache::ClearAll()
{
    std::lock_guard lock(mutex_);
    bitmap_cache_.clear();

    std::error_code error_code;
    std::filesystem::remove_all(cache_dir_, error_code);
    std::filesystem::create_directories(cache_dir_, error_code);

    MARKAMP_LOG_INFO("ExtensionIconCache: cleared all icons");
}

void ExtensionIconCache::EvictExpired()
{
    std::error_code error_code;
    const auto now = std::filesystem::file_time_type::clock::now();

    for (const auto& entry : std::filesystem::directory_iterator(cache_dir_, error_code))
    {
        if (entry.is_regular_file())
        {
            const auto age = now - entry.last_write_time();
            if (age > kCacheExpiry)
            {
                std::filesystem::remove(entry.path(), error_code);
            }
        }
    }
}

void ExtensionIconCache::SetOnIconLoaded(
    std::function<void(const std::string& extension_id)> callback)
{
    on_icon_loaded_ = std::move(callback);
}

auto ExtensionIconCache::CachePath(const std::string& extension_id) const -> std::filesystem::path
{
    return cache_dir_ / (extension_id + ".png");
}

auto ExtensionIconCache::ColorFromName(const std::string& name) -> wxColour
{
    // Simple hash to generate a consistent pleasing color
    std::size_t hash_val = std::hash<std::string>{}(name);
    auto hue = static_cast<int>(hash_val % 360);

    // HSL to RGB with S=70%, L=45% for vibrant but readable colors
    double hue_norm = static_cast<double>(hue) / 360.0;
    constexpr double kSaturation = 0.70;
    constexpr double kLightness = 0.45;

    auto hue_to_rgb = [](double p_val, double q_val, double t_val) -> double
    {
        if (t_val < 0.0)
        {
            t_val += 1.0;
        }
        if (t_val > 1.0)
        {
            t_val -= 1.0;
        }
        if (t_val < 1.0 / 6.0)
        {
            return p_val + (q_val - p_val) * 6.0 * t_val;
        }
        if (t_val < 1.0 / 2.0)
        {
            return q_val;
        }
        if (t_val < 2.0 / 3.0)
        {
            return p_val + (q_val - p_val) * (2.0 / 3.0 - t_val) * 6.0;
        }
        return p_val;
    };

    double q_factor = (kLightness < 0.5) ? kLightness * (1.0 + kSaturation)
                                         : kLightness + kSaturation - kLightness * kSaturation;
    double p_factor = 2.0 * kLightness - q_factor;

    auto red =
        static_cast<unsigned char>(hue_to_rgb(p_factor, q_factor, hue_norm + 1.0 / 3.0) * 255);
    auto green = static_cast<unsigned char>(hue_to_rgb(p_factor, q_factor, hue_norm) * 255);
    auto blue =
        static_cast<unsigned char>(hue_to_rgb(p_factor, q_factor, hue_norm - 1.0 / 3.0) * 255);

    return {red, green, blue};
}

} // namespace markamp::core
