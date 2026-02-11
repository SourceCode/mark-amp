#include "MermaidRenderer.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <functional>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

namespace markamp::core
{

namespace
{

/// RAII guard that removes a file on destruction.
class TempFileGuard
{
public:
    explicit TempFileGuard(std::filesystem::path path)
        : path_(std::move(path))
    {
    }

    ~TempFileGuard()
    {
        std::error_code ec;
        std::filesystem::remove(path_, ec);
    }

    TempFileGuard(const TempFileGuard&) = delete;
    auto operator=(const TempFileGuard&) -> TempFileGuard& = delete;
    TempFileGuard(TempFileGuard&&) = delete;
    auto operator=(TempFileGuard&&) -> TempFileGuard& = delete;

    [[nodiscard]] auto path() const -> const std::filesystem::path&
    {
        return path_;
    }

private:
    std::filesystem::path path_;
};

/// Execute a command and return its exit status.
auto execute_command(const std::string& cmd) -> int
{
    return std::system(cmd.c_str());
}

/// Read entire file contents into a string.
auto read_file(const std::filesystem::path& path) -> std::expected<std::string, std::string>
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return std::unexpected(fmt::format("Failed to read file: {}", path.string()));
    }
    auto size = file.tellg();
    if (size < 0)
    {
        return std::unexpected(fmt::format("Cannot determine file size: {}", path.string()));
    }
    file.seekg(0, std::ios::beg);
    std::string content(static_cast<size_t>(size), '\0');
    if (!file.read(content.data(), size))
    {
        return std::unexpected(fmt::format("Read failed: {}", path.string()));
    }
    return content;
}

/// Write string contents to a file.
auto write_file(const std::filesystem::path& path, std::string_view content) -> bool
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }
    file.write(content.data(), static_cast<std::streamsize>(content.size()));
    return file.good();
}

/// Generate a unique temp file path with the given extension.
auto make_temp_path(const std::string& extension) -> std::filesystem::path
{
    auto tmp_dir = std::filesystem::temp_directory_path();
    auto pid = static_cast<int>(getpid());
    static int counter = 0;
    auto filename = fmt::format("markamp_mermaid_{}_{}.{}", pid, ++counter, extension);
    return tmp_dir / filename;
}

} // anonymous namespace

MermaidRenderer::MermaidRenderer()
    : mmdc_available_(detect_mmdc())
{
}

auto MermaidRenderer::is_available() const -> bool
{
    return mmdc_available_;
}

void MermaidRenderer::set_theme(const Theme& theme)
{
    mermaid_theme_ = theme.is_dark() ? "dark" : "default";
    primary_color_ = theme.colors.accent_primary.to_hex();
    primary_text_color_ = theme.colors.text_main.to_hex();
    primary_border_color_ = theme.colors.border_light.to_hex();
    line_color_ = theme.colors.text_muted.to_hex();
    secondary_color_ = theme.colors.accent_secondary.to_hex();
    tertiary_color_ = theme.colors.bg_panel.to_hex();

    // Invalidate cache — theme colors changed
    clear_cache();
}

void MermaidRenderer::set_font_family(const std::string& font)
{
    font_family_ = font;
}

auto MermaidRenderer::get_mermaid_config() const -> std::string
{
    return fmt::format(
        R"({{
  "theme": "{}",
  "themeVariables": {{
    "primaryColor": "{}",
    "primaryTextColor": "{}",
    "primaryBorderColor": "{}",
    "lineColor": "{}",
    "secondaryColor": "{}",
    "tertiaryColor": "{}"
  }},
  "fontFamily": "{}",
  "securityLevel": "strict"
}})",
        mermaid_theme_,
        primary_color_,
        primary_text_color_,
        primary_border_color_,
        line_color_,
        secondary_color_,
        tertiary_color_,
        font_family_);
}

auto MermaidRenderer::render(std::string_view mermaid_source)
    -> std::expected<std::string, std::string>
{
    if (mermaid_source.empty())
    {
        return std::unexpected(std::string("Empty Mermaid source"));
    }

    if (!mmdc_available_)
    {
        return std::unexpected(std::string("Mermaid CLI (mmdc) is not available. "
                                           "Install with: npm install -g @mermaid-js/mermaid-cli"));
    }

    // Check cache
    auto key = cache_key(mermaid_source);
    auto cache_it = svg_cache_.find(key);
    if (cache_it != svg_cache_.end())
    {
        return cache_it->second;
    }

    // Cache miss — render via CLI
    auto result = render_via_mmdc(mermaid_source);
    if (result.has_value())
    {
        // Evict oldest if at capacity
        if (svg_cache_.size() >= kMaxCacheEntries && !cache_order_.empty())
        {
            svg_cache_.erase(cache_order_.front());
            cache_order_.erase(cache_order_.begin());
        }
        svg_cache_[key] = *result;
        cache_order_.push_back(key);
    }

    return result;
}

auto MermaidRenderer::render_via_mmdc(std::string_view source)
    -> std::expected<std::string, std::string>
{
    // Create temp files
    auto input_path = make_temp_path("mmd");
    auto output_path = make_temp_path("svg");
    auto config_path = make_temp_path("json");

    // RAII guards ensure cleanup even on error
    TempFileGuard input_guard(input_path);
    TempFileGuard output_guard(output_path);
    TempFileGuard config_guard(config_path);

    // Write Mermaid source to temp file
    if (!write_file(input_path, source))
    {
        return std::unexpected(std::string("Failed to write Mermaid source to temp file"));
    }

    // Write config to temp file
    auto config = get_mermaid_config();
    if (!write_file(config_path, config))
    {
        return std::unexpected(std::string("Failed to write Mermaid config to temp file"));
    }

    // Execute mmdc
    auto cmd = fmt::format("mmdc -i \"{}\" -o \"{}\" -c \"{}\" --quiet 2>&1",
                           input_path.string(),
                           output_path.string(),
                           config_path.string());

    auto exit_code = execute_command(cmd);
    if (exit_code != 0)
    {
        // Read stderr/stdout from the command output
        return std::unexpected(fmt::format("Mermaid rendering failed (exit code {}). "
                                           "Check your Mermaid syntax.",
                                           exit_code));
    }

    // Read SVG output
    auto svg_result = read_file(output_path);
    if (!svg_result)
    {
        return std::unexpected(svg_result.error());
    }

    if (svg_result->empty())
    {
        return std::unexpected(std::string("Mermaid rendered empty SVG output"));
    }

    // Sanitize SVG output (strip script, foreignObject, event handlers)
    return sanitize_svg(*svg_result);
}

auto MermaidRenderer::detect_mmdc() -> bool
{
#ifdef _WIN32
    auto exit_code = execute_command("where mmdc >NUL 2>&1");
#else
    auto exit_code = execute_command("which mmdc >/dev/null 2>&1");
#endif
    return exit_code == 0;
}

auto MermaidRenderer::cache_key(std::string_view source) const -> size_t
{
    // Improvement 23: proper hash_combine with all theme variables
    auto hash_combine = [](size_t seed, size_t value) -> size_t
    {
        // Golden ratio constant for hash distribution (boost::hash_combine)
        constexpr size_t kGoldenRatio = 0x9e3779b9;
        seed ^= value + kGoldenRatio + (seed << 6) + (seed >> 2);
        return seed;
    };

    const std::hash<std::string_view> sv_hash{};
    const std::hash<std::string> str_hash{};

    size_t seed = sv_hash(source);
    seed = hash_combine(seed, str_hash(mermaid_theme_));
    seed = hash_combine(seed, str_hash(primary_color_));
    seed = hash_combine(seed, str_hash(primary_text_color_));
    seed = hash_combine(seed, str_hash(primary_border_color_));
    seed = hash_combine(seed, str_hash(line_color_));
    seed = hash_combine(seed, str_hash(secondary_color_));
    seed = hash_combine(seed, str_hash(tertiary_color_));
    return seed;
}

void MermaidRenderer::clear_cache()
{
    svg_cache_.clear();
    cache_order_.clear();
}

auto MermaidRenderer::sanitize_svg(const std::string& svg) -> std::string
{
    std::string result;
    result.reserve(svg.size());

    size_t pos = 0;
    while (pos < svg.size())
    {
        auto tag_start = svg.find('<', pos);
        if (tag_start == std::string::npos)
        {
            result.append(svg, pos);
            break;
        }

        // Copy text before tag
        result.append(svg, pos, tag_start - pos);

        auto tag_end = svg.find('>', tag_start);
        if (tag_end == std::string::npos)
        {
            result.append(svg, tag_start);
            break;
        }

        const std::string_view tag_content =
            std::string_view{svg}.substr(tag_start, tag_end - tag_start + 1);

        // Convert to lowercase for matching (only the lowered copy allocates)
        std::string lower_tag{tag_content};
        std::transform(lower_tag.begin(),
                       lower_tag.end(),
                       lower_tag.begin(),
                       [](unsigned char chr) { return std::tolower(chr); });

        // Strip dangerous SVG elements
        const bool is_dangerous =
            lower_tag.starts_with("<script") || lower_tag.starts_with("</script") ||
            lower_tag.starts_with("<foreignobject") || lower_tag.starts_with("</foreignobject");

        if (is_dangerous)
        {
            // Skip this tag entirely
            pos = tag_end + 1;
            continue;
        }

        // Strip on* event handler attributes from the tag
        // Simple approach: remove on[a-z]+="..." patterns
        std::string clean_tag;
        clean_tag.reserve(tag_content.size());
        size_t tpos = 0;
        while (tpos < tag_content.size())
        {
            // Check for on* attribute pattern (space followed by on)
            if (tpos > 0 && std::isspace(static_cast<unsigned char>(tag_content[tpos - 1])) != 0 &&
                tpos + 2 < tag_content.size() && tag_content[tpos] == 'o' &&
                tag_content[tpos + 1] == 'n' &&
                std::isalpha(static_cast<unsigned char>(tag_content[tpos + 2])) != 0)
            {
                // Found on* attribute — skip to end of attribute value
                auto eq_pos = tag_content.find('=', tpos);
                if (eq_pos != std::string::npos && eq_pos + 1 < tag_content.size())
                {
                    auto quote_char = tag_content[eq_pos + 1];
                    if (quote_char == '"' || quote_char == '\'')
                    {
                        auto end_quote = tag_content.find(quote_char, eq_pos + 2);
                        if (end_quote != std::string::npos)
                        {
                            tpos = end_quote + 1;
                            continue;
                        }
                    }
                }
            }
            clean_tag += tag_content[tpos];
            ++tpos;
        }

        result.append(clean_tag);
        pos = tag_end + 1;
    }

    return result;
}

} // namespace markamp::core
