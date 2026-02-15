// ============================================================================
// File: src/rendering/IFrameBlockRenderer.cpp
// Phase 29: Advanced Block Types — IFrame block renderer implementation
// ============================================================================
#include "IFrameBlockRenderer.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::rendering
{

auto IFrameBlockRenderer::render(const IFrameConfig& config) const -> std::string
{
    if (!validate_url(config.url))
    {
        return R"(<div class="iframe-block iframe-blocked"><p class="iframe-error">)"
               R"(URL blocked for security reasons: )" +
               config.url + "</p></div>\n";
    }

    std::ostringstream oss;
    oss << R"(<div class="iframe-block">)"
        << "\n"
        << R"(  <iframe src=")" << config.url << R"(")"
        << R"( width=")" << config.width << R"(")"
        << R"( height=")" << config.height << R"(")";

    if (config.sandbox)
    {
        oss << " sandbox=\"";
        if (config.allow_scripts)
        {
            oss << "allow-scripts allow-same-origin";
        }
        oss << "\"";
    }

    oss << R"( style="border:)" << config.border_style << R"(")"
        << " loading=\"lazy\""
        << " referrerpolicy=\"no-referrer\""
        << "></iframe>\n"
        << "</div>\n";

    return oss.str();
}

auto IFrameBlockRenderer::validate_url(std::string_view url) -> bool
{
    if (url.empty())
    {
        return false;
    }

    // Normalize to lowercase for scheme check
    auto url_lower = std::string(url.substr(0, 20)); // Only need prefix
    std::transform(url_lower.begin(),
                   url_lower.end(),
                   url_lower.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });

    // Block dangerous schemes
    if (url_lower.starts_with("javascript:"))
        return false;
    if (url_lower.starts_with("data:"))
        return false;
    if (url_lower.starts_with("file:"))
        return false;
    if (url_lower.starts_with("vbscript:"))
        return false;

    // Allow only http(s) schemes
    if (url_lower.starts_with("https://") || url_lower.starts_with("http://"))
    {
        return true;
    }

    // Reject anything else (e.g., relative paths without scheme)
    return false;
}

auto IFrameBlockRenderer::parse_dimensions(std::string_view info_string) -> std::pair<int, int>
{
    int width = 640;
    int height = 480;

    // Parse "iframe WIDTHxHEIGHT" or "iframe WIDTH HEIGHT"
    auto pos = info_string.find_first_of("0123456789");
    if (pos != std::string_view::npos)
    {
        auto remaining = info_string.substr(pos);
        // Try WIDTHxHEIGHT format
        const auto x_pos = remaining.find('x');
        if (x_pos != std::string_view::npos)
        {
            width = std::atoi(std::string(remaining.substr(0, x_pos)).c_str());
            height = std::atoi(std::string(remaining.substr(x_pos + 1)).c_str());
        }
    }

    // Clamp to sane ranges
    width = std::clamp(width, 100, 1920);
    height = std::clamp(height, 100, 1080);

    return {width, height};
}

} // namespace markamp::rendering
