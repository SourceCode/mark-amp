#include "ScriptRenderer.h"

#include <regex>
#include <sstream>

namespace markamp::rendering
{

[[nodiscard]] auto ScriptRenderer::render(const std::string& content,
                                          const std::string& extension) const -> std::string
{
    ScriptStructure structure = extract_structure(content, extension);

    std::string html = "<div class=\"script-preview\" style=\"padding: 16px; font-family: "
                       "var(--font-ui); color: var(--text-normal);\">";

    html += "<h2 style=\"margin-top: 0; border-bottom: 1px solid var(--border-subtle); "
            "padding-bottom: 8px;\">Script Structure (" +
            extension + ")</h2>";

    if (!structure.includes.empty())
    {
        render_section(html, "Dependencies & Includes", structure.includes, "📦");
    }

    if (!structure.classes.empty())
    {
        render_section(html, "Classes & Interfaces", structure.classes, "🧩");
    }

    if (!structure.functions.empty())
    {
        render_section(html, "Functions & Methods", structure.functions, "⚡");
    }

    if (structure.includes.empty() && structure.classes.empty() && structure.functions.empty())
    {
        html += "<div style=\"opacity: 0.7; font-style: italic;\">No structural components "
                "found.</div>";
    }

    html += "</div>";
    return html;
}

[[nodiscard]] auto ScriptRenderer::extract_structure(const std::string& content,
                                                     const std::string& extension) const
    -> ScriptStructure
{
    ScriptStructure structure;
    std::istringstream stream(content);
    std::string line;
    int line_number = 1;

    // Simple Regex patterns for extraction
    std::regex cpp_include(R"s(^\s*#include\s*[<"]([^>"]+)[>"])s");
    std::regex cpp_class(R"s(^\s*(?:class|struct)\s+([A-Za-z0-9_]+))s");
    std::regex cpp_func(R"s(^\s*(?:[\w:]+\s+)+([A-Za-z0-9_~]+)\s*\()s");

    std::regex py_import(R"s(^\s*(?:import|from)\s+([A-Za-z0-9_\.]+))s");
    std::regex py_class(R"s(^\s*class\s+([A-Za-z0-9_]+))s");
    std::regex py_func(R"s(^\s*def\s+([A-Za-z0-9_]+))s");

    std::regex js_import(R"s(^\s*import\s+.*from\s+['"]([^'"]+)['"])s");
    std::regex js_class(R"s(^\s*(?:export\s+)?(?:default\s+)?class\s+([A-Za-z0-9_]+))s");
    std::regex js_func(
        R"s(^\s*(?:export\s+)?(?:default\s+)?(?:async\s+)?function\s+([A-Za-z0-9_]+))s");
    std::regex js_arrow_func(
        R"s(^\s*(?:export\s+)?(?:const|let|var)\s+([A-Za-z0-9_]+)\s*=\s*(?:async\s*)?(?:\([^\)]*\)|[A-Za-z0-9_]+)\s*=>)s");

    bool is_cpp = (extension == ".cpp" || extension == ".c" || extension == ".h" ||
                   extension == ".hpp" || extension == ".cc");
    bool is_py = (extension == ".py");
    bool is_js =
        (extension == ".js" || extension == ".ts" || extension == ".jsx" || extension == ".tsx");

    while (std::getline(stream, line))
    {
        std::smatch match;

        if (is_cpp)
        {
            if (std::regex_search(line, match, cpp_include))
            {
                structure.includes.push_back({match[1], line, line_number});
            }
            else if (std::regex_search(line, match, cpp_class))
            {
                structure.classes.push_back({match[1], line, line_number});
            }
            else if (std::regex_search(line, match, cpp_func))
            {
                // Heuristic: filter out control flow like 'if (', 'while ('
                std::string name = match[1];
                if (name != "if" && name != "while" && name != "for" && name != "switch" &&
                    name != "catch")
                {
                    structure.functions.push_back({name, line, line_number});
                }
            }
        }
        else if (is_py)
        {
            if (std::regex_search(line, match, py_import))
            {
                structure.includes.push_back({match[1], line, line_number});
            }
            else if (std::regex_search(line, match, py_class))
            {
                structure.classes.push_back({match[1], line, line_number});
            }
            else if (std::regex_search(line, match, py_func))
            {
                structure.functions.push_back({match[1], line, line_number});
            }
        }
        else if (is_js)
        {
            if (std::regex_search(line, match, js_import))
            {
                structure.includes.push_back({match[1], line, line_number});
            }
            else if (std::regex_search(line, match, js_class))
            {
                structure.classes.push_back({match[1], line, line_number});
            }
            else if (std::regex_search(line, match, js_func) ||
                     std::regex_search(line, match, js_arrow_func))
            {
                structure.functions.push_back({match[1], line, line_number});
            }
        }

        line_number++;
    }

    return structure;
}

void ScriptRenderer::render_section(std::string& output,
                                    const std::string& title,
                                    const std::vector<Component>& components,
                                    const std::string& icon) const
{
    output += "<details open style=\"margin-bottom: 12px; border: 1px solid var(--border-subtle); "
              "border-radius: 4px; overflow: hidden;\">";
    output += "<summary style=\"padding: 8px 12px; background-color: var(--bg-secondary); cursor: "
              "pointer; font-weight: bold; user-select: none;\">";
    output += icon + " " + title +
              " <span style=\"opacity: 0.6; font-size: 0.9em; font-weight: normal;\">(" +
              std::to_string(components.size()) + ")</span>";
    output += "</summary>";

    output += "<div style=\"padding: 8px 0; background-color: var(--bg-primary);\">";

    for (const auto& comp : components)
    {
        // Encode basic HTML entities in declaration
        std::string safe_decl;
        for (const char chr : comp.declaration)
        {
            if (chr == '<')
                safe_decl += "&lt;";
            else if (chr == '>')
                safe_decl += "&gt;";
            else if (chr == '&')
                safe_decl += "&amp;";
            else
                safe_decl += chr;
        }

        output +=
            "<div style=\"padding: 4px 12px; display: flex; align-items: baseline; gap: 8px;\">";
        output += "<span style=\"color: var(--text-muted); font-size: 0.8em; min-width: 30px; "
                  "text-align: right;\">" +
                  std::to_string(comp.line_number) + "</span>";
        output += "<span style=\"color: var(--syntax-entity); font-weight: bold;\">" + comp.name +
                  "</span>";
        output += "<span style=\"color: var(--text-muted); font-size: 0.9em; white-space: nowrap; "
                  "overflow: hidden; text-overflow: ellipsis; max-width: 400px;\" title=\"" +
                  safe_decl + "\">" + safe_decl + "</span>";
        output += "</div>";
    }

    output += "</div></details>";
}

} // namespace markamp::rendering
