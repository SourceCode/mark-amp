#pragma once

#include <nlohmann/json.hpp>

#include <string>

namespace markamp::rendering
{

/// Renders JSON content to HTML with pretty-printing and syntax highlighting.
class JsonRenderer
{
public:
    JsonRenderer() = default;

    /// Renders the given JSON string to an HTML representation.
    /// If the JSON is invalid, it outputs a formatted error message and the raw content.
    [[nodiscard]] auto render(const std::string& json_content) const -> std::string;

private:
    /// Recursive helper to render a JSON values to HTML
    void render_value(const nlohmann::json& value, std::string& output, int indent_level) const;
};

} // namespace markamp::rendering
