#include "CodeLensProvider.h"

#include <sstream>

namespace markamp::ui
{

auto CodeLensProvider::GetProviderId() const -> std::string
{
    return "provider.codelens";
}

auto CodeLensProvider::GetDecorations() const -> std::vector<GutterDecoration>
{
    return decorations_;
}

void CodeLensProvider::UpdateContent(const std::string& content)
{
    decorations_.clear();

    // Simple line-by-line scanner for Markdown headings (e.g., "## Heading")
    // since MdNode AST doesn't preserve exact line numbers.
    std::istringstream stream(content);
    std::string line;
    int line_number = 0;

    while (std::getline(stream, line))
    {
        // Check for ATX headings (1 to 6 hash characters followed by a space)
        size_t hash_count = 0;
        while (hash_count < line.length() && line[hash_count] == '#' && hash_count < 6)
        {
            hash_count++;
        }

        if (hash_count > 0 && hash_count < line.length() && line[hash_count] == ' ')
        {
            // Only add code lenses to top-level or secondary headings to avoid clutter
            if (hash_count <= 2)
            {
                GutterDecoration dec;
                dec.line = line_number;
                // Add a sample dummy action indicating references or document structure
                if (hash_count == 1)
                {
                    dec.tooltip = "  \U0001F5DE\uFE0F Extract | \U0001F4DD Edit Header";
                }
                else
                {
                    dec.tooltip = "  \U0001F517 2 References";
                }

                decorations_.push_back(dec);
            }
        }

        line_number++;
    }
}

} // namespace markamp::ui
