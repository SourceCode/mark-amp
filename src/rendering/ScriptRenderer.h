#pragma once

#include <string>
#include <vector>

namespace markamp::rendering
{

/// Renders source code scripts (C++, Python, JS/TS, etc.) into a structural HTML view
/// consisting of logical components like Includes/Imports, Classes, and Functions.
class ScriptRenderer
{
public:
    ScriptRenderer() = default;

    /// Renders the given script content to an HTML representation.
    [[nodiscard]] auto render(const std::string& content, const std::string& extension) const
        -> std::string;

private:
    struct Component
    {
        std::string name;
        std::string declaration;
        int line_number{0};
    };

    struct ScriptStructure
    {
        std::vector<Component> includes;
        std::vector<Component> classes;
        std::vector<Component> functions;
    };

    /// Extracts structural components based on the file extension
    [[nodiscard]] auto extract_structure(const std::string& content,
                                         const std::string& extension) const -> ScriptStructure;

    /// Helper to generate HTML for a section
    void render_section(std::string& output,
                        const std::string& title,
                        const std::vector<Component>& components,
                        const std::string& icon) const;
};

} // namespace markamp::rendering
