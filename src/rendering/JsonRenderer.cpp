#include "JsonRenderer.h"

#include <nlohmann/json.hpp>

#include <sstream>

using json = nlohmann::json;

namespace markamp::rendering
{

[[nodiscard]] auto JsonRenderer::render(const std::string& json_content) const -> std::string
{
    std::string html = "<div class=\"json-preview\" style=\"font-family: monospace; white-space: "
                       "pre; padding: 16px; background-color: var(--bg-secondary); color: "
                       "var(--text-normal); border-radius: 4px; overflow-x: auto;\">";

    try
    {
        // Parse the JSON. nlohmann::json will throw if invalid.
        auto parsed_json = json::parse(json_content);
        render_value(parsed_json, html, 0);
    }
    catch (const json::parse_error& e)
    {
        html +=
            "<div class=\"json-error\" style=\"color: var(--text-error); margin-bottom: 12px;\">";
        html += "<strong>JSON Parse Error:</strong> " + std::string(e.what());
        html += "</div>";
        html += "<div style=\"opacity: 0.7;\">";
        html += json_content; // Fallback to raw content
        html += "</div>";
    }

    html += "</div>";
    return html;
}

void JsonRenderer::render_value(const json& value, std::string& output, int indent_level) const
{
    const std::string indent(static_cast<std::string::size_type>(indent_level * 4), ' ');

    if (value.is_object())
    {
        if (value.empty())
        {
            output += "{}";
            return;
        }

        output += "{\n";
        auto it = value.begin();
        while (it != value.end())
        {
            output += indent + "    <span style=\"color: var(--syntax-key);\">\"" + it.key() +
                      "\"</span>: ";
            render_value(it.value(), output, indent_level + 1);

            auto next_it = it;
            ++next_it;
            if (next_it != value.end())
            {
                output += ",";
            }
            output += "\n";
            ++it;
        }
        output += indent + "}";
    }
    else if (value.is_array())
    {
        if (value.empty())
        {
            output += "[]";
            return;
        }

        output += "[\n";
        for (size_t i = 0; i < value.size(); ++i)
        {
            output += indent + "    ";
            render_value(value[i], output, indent_level + 1);
            if (i < value.size() - 1)
            {
                output += ",";
            }
            output += "\n";
        }
        output += indent + "]";
    }
    else if (value.is_string())
    {
        // Minimal HTML escaping for string values
        const std::string str_val = value.get<std::string>();
        std::string escaped;
        for (const char chr : str_val)
        {
            if (chr == '<')
            {
                escaped += "&lt;";
            }
            else if (chr == '>')
            {
                escaped += "&gt;";
            }
            else if (chr == '&')
            {
                escaped += "&amp;";
            }
            else
            {
                escaped += chr;
            }
        }
        output += "<span style=\"color: var(--syntax-string);\">\"" + escaped + "\"</span>";
    }
    else if (value.is_number())
    {
        output += "<span style=\"color: var(--syntax-number);\">" + value.dump() + "</span>";
    }
    else if (value.is_boolean())
    {
        output += "<span style=\"color: var(--syntax-boolean);\">" + value.dump() + "</span>";
    }
    else if (value.is_null())
    {
        output += "<span style=\"color: var(--syntax-null);\">null</span>";
    }
}

} // namespace markamp::rendering
