/// @file MarkdownSymbolProvider.cpp
/// @brief V13 Phase 31 Task 4 — Markdown symbol extraction implementation.

#include "core/MarkdownSymbolProvider.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace markamp::core
{

auto MarkdownSymbolProvider::get_symbols(const std::string& document_id) const
    -> std::vector<SymbolInfo>
{
    // Read the file
    std::ifstream file(document_id);
    if (!file.is_open())
    {
        return {};
    }

    std::ostringstream content;
    content << file.rdbuf();
    return extract_from_text(content.str());
}

auto MarkdownSymbolProvider::supports(const std::string& document_id) const -> bool
{
    if (document_id.size() < 3)
    {
        return false;
    }

    // Check common markdown extensions
    auto ends_with = [](const std::string& str, const std::string& suffix) -> bool
    {
        if (suffix.size() > str.size())
            return false;
        return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    };

    return ends_with(document_id, ".md") || ends_with(document_id, ".markdown") ||
           ends_with(document_id, ".mdx");
}

auto MarkdownSymbolProvider::extract_from_text(const std::string& text) -> std::vector<SymbolInfo>
{
    std::vector<SymbolInfo> symbols;
    std::istringstream stream(text);
    std::string line;
    int line_number = 0;

    // Track parent headings for container_name
    std::vector<std::string> heading_stack; // index = level-1
    heading_stack.resize(6);                // H1-H6

    bool in_code_block = false;

    while (std::getline(stream, line))
    {
        ++line_number;

        // Skip code blocks
        if (line.size() >= 3 && line.substr(0, 3) == "```")
        {
            in_code_block = !in_code_block;
            continue;
        }
        if (in_code_block)
        {
            continue;
        }

        // Check for ATX headings (# ... ######)
        if (line.empty() || line[0] != '#')
        {
            continue;
        }

        int level = 0;
        size_t pos = 0;
        while (pos < line.size() && line[pos] == '#' && level < 6)
        {
            ++level;
            ++pos;
        }

        // Must have at least one # and a space after
        if (level == 0 || pos >= line.size() || line[pos] != ' ')
        {
            continue;
        }

        // Extract heading text (skip leading space, trim trailing #)
        std::string heading_text = line.substr(pos + 1);

        // Trim trailing # characters and spaces
        while (!heading_text.empty() && (heading_text.back() == '#' || heading_text.back() == ' '))
        {
            heading_text.pop_back();
        }

        if (heading_text.empty())
        {
            continue;
        }

        // Determine container name from parent heading
        std::string container;
        for (int i = level - 2; i >= 0; --i)
        {
            if (!heading_stack[static_cast<size_t>(i)].empty())
            {
                container = heading_stack[static_cast<size_t>(i)];
                break;
            }
        }

        // Update heading stack
        heading_stack[static_cast<size_t>(level - 1)] = heading_text;
        // Clear deeper levels
        for (size_t i = static_cast<size_t>(level); i < heading_stack.size(); ++i)
        {
            heading_stack[i].clear();
        }

        SymbolInfo info;
        info.name = heading_text;
        info.kind = SymbolKind::kHeading;
        info.line = line_number;
        info.container_name = container;
        info.level = level;

        symbols.push_back(std::move(info));
    }

    return symbols;
}

} // namespace markamp::core
