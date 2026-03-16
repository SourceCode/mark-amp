#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <filesystem>
#include <memory>
#include <string>

namespace markamp::canvas
{

/// oEmbed metadata fetched from a video URL provider.
struct OEmbedData
{
    std::string title;
    std::string author_name;
    std::string provider_name;
    std::string thumbnail_url;
    std::filesystem::path thumbnail_path; // Local cached thumbnail
    int width{480};
    int height{270};

    // ── Round 3 Batch 10 (#100) ─────────────────────────────────

    /// (#100) Whether oEmbed title is present.
    [[nodiscard]] auto has_title() const noexcept -> bool
    {
        return !title.empty();
    }
};

/// A canvas object that embeds a video (URL or local file) with oEmbed metadata.
class VideoEmbedObject : public CanvasObject
{
public:
    VideoEmbedObject();

    [[nodiscard]] auto url() const -> const std::string&;
    auto set_url(const std::string& video_url) -> void;

    [[nodiscard]] auto oembed() const -> const OEmbedData&;
    auto set_oembed(const OEmbedData& data) -> void;

    [[nodiscard]] auto display_width() const -> double;
    [[nodiscard]] auto display_height() const -> double;
    auto set_display_dimensions(double wid, double hei) -> void;

    [[nodiscard]] auto is_local_file() const -> bool;
    auto set_local_file(const std::filesystem::path& path) -> void;
    [[nodiscard]] auto local_file() const -> const std::filesystem::path&;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string url_;
    OEmbedData oembed_;
    double display_width_{480.0};
    double display_height_{270.0};
    std::filesystem::path local_file_;
};

} // namespace markamp::canvas
