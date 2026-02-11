#include "ThemeLoader.h"

#include "core/Logger.h"

#include <fstream>
#include <sstream>
#include <yaml-cpp/yaml.h>

namespace markamp::core
{

auto ThemeLoader::load_from_file(const std::filesystem::path& path)
    -> std::expected<Theme, std::string>
{
    if (!std::filesystem::exists(path))
    {
        return std::unexpected("File not found: " + path.string());
    }

    std::ifstream file(path);
    if (!file.is_open())
    {
        return std::unexpected("Could not open file: " + path.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Extract YAML frontmatter
    // Format:
    // ---
    // yaml content
    // ---
    // (optional markdown content)

    if (content.size() < 6 || content.substr(0, 3) != "---")
    {
        return std::unexpected("Invalid format: Missing starting '---'");
    }

    size_t end_pos = content.find("\n---", 3);
    if (end_pos == std::string::npos)
    {
        // Try searching without leading newline if it's the end of file (though unlikely for valid
        // frontmatter)
        end_pos = content.find("---", 3);
        if (end_pos == std::string::npos)
        {
            return std::unexpected("Invalid format: Missing ending '---'");
        }
    }

    std::string yaml_content = content.substr(3, end_pos - 3);
    return parse_yaml_content(yaml_content);
}

static auto parse_color(const YAML::Node& node, const std::string& key, Color& target) -> void
{
    if (node[key])
    {
        auto color_str = node[key].as<std::string>();
        auto result = Color::from_string(color_str);
        if (result)
        {
            target = *result;
        }
        else
        {
            MARKAMP_LOG_WARN("Invalid color for key '{}': {}", key, result.error());
        }
    }
}

auto ThemeLoader::parse_yaml_content(const std::string& yaml_content)
    -> std::expected<Theme, std::string>
{
    try
    {
        YAML::Node root = YAML::Load(yaml_content);

        if (!root["id"] || !root["name"])
        {
            return std::unexpected("Missing required fields 'id' or 'name'");
        }

        Theme theme;
        theme.id = root["id"].as<std::string>();
        theme.name = root["name"].as<std::string>();

        if (root["colors"])
        {
            const auto& colors = root["colors"];

            // Base colors
            parse_color(colors, "--bg-app", theme.colors.bg_app);
            parse_color(colors, "--bg-panel", theme.colors.bg_panel);
            parse_color(colors, "--bg-header", theme.colors.bg_header);
            parse_color(colors, "--bg-input", theme.colors.bg_input);
            parse_color(colors, "--text-main", theme.colors.text_main);
            parse_color(colors, "--text-muted", theme.colors.text_muted);
            parse_color(colors, "--accent-primary", theme.colors.accent_primary);
            parse_color(colors, "--accent-secondary", theme.colors.accent_secondary);
            parse_color(colors, "--border-light", theme.colors.border_light);
            parse_color(colors, "--border-dark", theme.colors.border_dark);

            // Editor specific - with fallbacks handled in Theme logic if needed,
            // but here we try to parse explicit logic or defaults.
            // Actually, for a loader, we just load what's there.
            // The Theme struct might need defaults initialized if we want to be safe,
            // but we'll assume the migration script populates these or we use defaults.
            // Let's implement smart defaults here if missing to be safe.

            auto resolve_color = [&](const std::string& key, const Color& fallback) -> Color
            {
                if (colors[key])
                {
                    auto res = Color::from_string(colors[key].as<std::string>());
                    if (res)
                        return *res;
                }
                return fallback;
            };

            // Use the fallback logic similar to from_json in Theme.cpp
            // To avoid duplication, verify if we can reuse from_json logic?
            // We can't easily reuse nlohmann json logic here without converting YAML to JSON first.
            // So we implement the logic here.

            // Editor
            // Editor
            theme.colors.editor_bg = resolve_color("--editor-bg", theme.colors.bg_input);
            theme.colors.editor_fg = resolve_color("--editor-fg", theme.colors.text_main);
            theme.colors.editor_selection =
                resolve_color("--editor-selection", theme.colors.accent_primary.with_alpha(0.2f));
            theme.colors.editor_line_number =
                resolve_color("--editor-line-number", theme.colors.text_muted);
            theme.colors.editor_cursor =
                resolve_color("--editor-cursor", theme.colors.accent_primary);
            theme.colors.editor_gutter = resolve_color("--editor-gutter", theme.colors.bg_input);

            // UI
            theme.colors.list_hover =
                resolve_color("--list-hover", theme.colors.accent_primary.with_alpha(0.1f));
            theme.colors.list_selected =
                resolve_color("--list-selected", theme.colors.accent_primary.with_alpha(0.2f));
            theme.colors.scrollbar_thumb =
                resolve_color("--scrollbar-thumb", theme.colors.accent_secondary);
            theme.colors.scrollbar_track =
                resolve_color("--scrollbar-track", theme.colors.bg_panel);
        }

        if (!theme.is_valid())
        {
            // List errors
            std::string error_msg = "Invalid theme:";
            for (const auto& err : theme.validation_errors())
            {
                error_msg += " " + err + ";";
            }
            return std::unexpected(error_msg);
        }

        return theme;
    }
    catch (const YAML::Exception& e)
    {
        return std::unexpected(std::string("YAML parsing error: ") + e.what());
    }
}

} // namespace markamp::core
