#pragma once

#include "core/Theme.h"

#include <expected>
#include <filesystem>
#include <string>

namespace markamp::core
{

class ThemeLoader
{
public:
    /// Load a theme from a Markdown file with YAML frontmatter.
    /// Returns the parsed Theme or an error message.
    static auto load_from_file(const std::filesystem::path& path)
        -> std::expected<Theme, std::string>;

    /// Parse YAML frontmatter content into a Theme object.
    static auto parse_yaml_content(const std::string& yaml_content)
        -> std::expected<Theme, std::string>;
};

} // namespace markamp::core
