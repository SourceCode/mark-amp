#pragma once

/// @file InlineImageRenderer.h
/// @brief V9 Phase 45 Task 6 – Inline image parsing and display size computation.
///
/// Header-only, pure logic. Parses ![alt](url) markdown and produces
/// InlineImageInfo for UI consumption.

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Information extracted from an inline image markdown reference.
struct InlineImageInfo
{
    std::string url;           ///< Image URL or path
    std::string alt_text;      ///< Alt text from ![alt]
    std::string title;         ///< Optional title from ![alt](url "title")
    std::string resolved_path; ///< Resolved absolute path (set by caller)
    int original_width{0};     ///< Original image width in pixels (set by caller)
    int original_height{0};    ///< Original image height in pixels (set by caller)
    int display_width{0};      ///< Computed display width
    int display_height{0};     ///< Computed display height
    int line_number{0};        ///< Source line number
    int start_col{0};          ///< Start column in the source line
    int end_col{0};            ///< End column in the source line
    bool is_valid{false};      ///< Whether the image reference is well-formed
    bool is_remote{false};     ///< Whether URL is http/https

    /// Check if this is an empty/default image info.
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return url.empty() && alt_text.empty();
    }
};

/// Parses markdown image references and computes display sizes.
class InlineImageRenderer
{
public:
    InlineImageRenderer() = default;

    /// Parse a line of markdown looking for image references.
    /// Returns all images found in the line.
    [[nodiscard]] static auto parse_images(const std::string& line, int line_number = 0)
        -> std::vector<InlineImageInfo>
    {
        std::vector<InlineImageInfo> results;
        std::size_t pos = 0;

        while (pos < line.size())
        {
            // Find ![
            auto img_start = line.find("![", pos);
            if (img_start == std::string::npos)
            {
                break;
            }

            auto info = parse_single_image(line, img_start, line_number);
            if (info.is_valid)
            {
                results.push_back(std::move(info));
                pos = static_cast<std::size_t>(results.back().end_col);
            }
            else
            {
                pos = img_start + 2;
            }
        }

        return results;
    }

    /// Parse a single image reference starting at the given position.
    [[nodiscard]] static auto parse_single_image(const std::string& line,
                                                 std::size_t start_pos,
                                                 int line_number = 0) -> InlineImageInfo
    {
        InlineImageInfo info;
        info.line_number = line_number;
        info.start_col = static_cast<int>(start_pos);

        // Must start with ![
        if (start_pos + 1 >= line.size() || line[start_pos] != '!' || line[start_pos + 1] != '[')
        {
            return info;
        }

        // Find closing ] for alt text
        auto alt_end = line.find(']', start_pos + 2);
        if (alt_end == std::string::npos)
        {
            return info;
        }

        info.alt_text = line.substr(start_pos + 2, alt_end - start_pos - 2);

        // Must be followed by (
        if (alt_end + 1 >= line.size() || line[alt_end + 1] != '(')
        {
            return info;
        }

        // Find closing ) for URL
        auto url_start = alt_end + 2;
        auto paren_close = line.find(')', url_start);
        if (paren_close == std::string::npos)
        {
            return info;
        }

        auto url_content = line.substr(url_start, paren_close - url_start);

        // Check for optional title: url "title" or url 'title'
        auto quote_pos = url_content.find('"');
        if (quote_pos == std::string::npos)
        {
            quote_pos = url_content.find('\'');
        }

        if (quote_pos != std::string::npos)
        {
            info.url = url_content.substr(0, quote_pos);
            // Trim trailing whitespace from URL
            while (!info.url.empty() && info.url.back() == ' ')
            {
                info.url.pop_back();
            }
            // Extract title between quotes
            auto title_start = quote_pos + 1;
            auto title_end = url_content.find(url_content[quote_pos], title_start);
            if (title_end != std::string::npos)
            {
                info.title = url_content.substr(title_start, title_end - title_start);
            }
        }
        else
        {
            info.url = url_content;
            // Trim whitespace
            while (!info.url.empty() && info.url.back() == ' ')
            {
                info.url.pop_back();
            }
            while (!info.url.empty() && info.url.front() == ' ')
            {
                info.url.erase(info.url.begin());
            }
        }

        info.end_col = static_cast<int>(paren_close + 1);
        info.is_valid = !info.url.empty();
        info.is_remote = (info.url.find("http://") == 0 || info.url.find("https://") == 0);

        return info;
    }

    /// Compute the display size for an image, constrained to max dimensions.
    /// Maintains aspect ratio. Sets display_width and display_height on the info.
    static void compute_display_size(InlineImageInfo& info, int max_width, int max_height)
    {
        if (info.original_width <= 0 || info.original_height <= 0)
        {
            // Unknown size: use max dimensions as-is
            info.display_width = std::min(max_width, 400);
            info.display_height = std::min(max_height, 300);
            return;
        }

        const double aspect_ratio =
            static_cast<double>(info.original_width) / static_cast<double>(info.original_height);

        int width = info.original_width;
        int height = info.original_height;

        // Scale down to fit within max dimensions
        if (width > max_width)
        {
            width = max_width;
            height = static_cast<int>(static_cast<double>(width) / aspect_ratio);
        }
        if (height > max_height)
        {
            height = max_height;
            width = static_cast<int>(static_cast<double>(height) * aspect_ratio);
        }

        info.display_width = std::max(width, 1);
        info.display_height = std::max(height, 1);
    }

    /// Check if a line contains only an image (standalone image block).
    [[nodiscard]] static auto is_standalone_image(const std::string& line) -> bool
    {
        // Trim leading whitespace
        auto start = line.find_first_not_of(" \t");
        if (start == std::string::npos)
        {
            return false;
        }

        if (line.size() - start < 5)
        {
            return false; // Minimum: ![](x)
        }

        // Must start with ![
        if (line[start] != '!' || line[start + 1] != '[')
        {
            return false;
        }

        // Find end of image reference
        auto images = parse_images(line, 0);
        if (images.size() != 1)
        {
            return false;
        }

        // Check that image spans the entire non-whitespace content
        auto end = line.find_last_not_of(" \t\r\n");
        return end != std::string::npos && static_cast<std::size_t>(images[0].end_col) > end;
    }
};

} // namespace markamp::core
