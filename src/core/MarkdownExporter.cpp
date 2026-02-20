// ============================================================================
// File: src/core/MarkdownExporter.cpp
// Phase 39: Multi-Format Export — Markdown & PlainText exporter
// ============================================================================

#include "MarkdownExporter.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ── MarkdownExporter ────────────────────────────────────────────────────────

auto MarkdownExporter::descriptor() const -> ExportFormatDescriptor
{
    return {.format = ExportFormat::Markdown,
            .name = "Markdown",
            .extension = ".md",
            .mime_type = "text/markdown",
            .description = "Standard Markdown format",
            .requires_pandoc = false,
            .supports_assets = true,
            .supports_toc = true};
}

auto MarkdownExporter::export_content(const std::string& markdown_source,
                                      const ExportOptions& options) const
    -> std::expected<std::string, std::string>
{
    std::string output = markdown_source;

    // Apply heading offset.
    if (options.heading_offset != 0)
    {
        std::string adjusted;
        std::istringstream stream(output);
        std::string line;
        while (std::getline(stream, line))
        {
            if (line.starts_with("#"))
            {
                int level = 0;
                while (level < static_cast<int>(line.size()) &&
                       line[static_cast<size_t>(level)] == '#')
                {
                    ++level;
                }
                const int new_level = std::clamp(level + options.heading_offset, 1, 6);
                adjusted += std::string(static_cast<size_t>(new_level), '#') +
                            line.substr(static_cast<size_t>(level)) + "\n";
            }
            else
            {
                adjusted += line + "\n";
            }
        }
        output = adjusted;
    }

    return output;
}

auto MarkdownExporter::check_dependencies() const -> std::expected<void, std::string>
{
    return {}; // No external dependencies.
}

// ── PlainTextExporter ───────────────────────────────────────────────────────

auto PlainTextExporter::descriptor() const -> ExportFormatDescriptor
{
    return {.format = ExportFormat::PlainText,
            .name = "Plain Text",
            .extension = ".txt",
            .mime_type = "text/plain",
            .description = "Plain text with Markdown formatting stripped",
            .requires_pandoc = false,
            .supports_assets = false,
            .supports_toc = false};
}

auto PlainTextExporter::export_content(const std::string& markdown_source,
                                       const ExportOptions& /*options*/) const
    -> std::expected<std::string, std::string>
{
    // Strip Markdown formatting.
    std::string output;
    output.reserve(markdown_source.size());

    bool in_code_block = false;
    std::istringstream stream(markdown_source);
    std::string line;
    while (std::getline(stream, line))
    {
        // Toggle code blocks.
        if (line.starts_with("```"))
        {
            in_code_block = !in_code_block;
            continue;
        }
        if (in_code_block)
        {
            output += line + "\n";
            continue;
        }

        // Strip heading markers.
        {
            auto pos = line.find_first_not_of('#');
            if (pos != std::string::npos && pos > 0 && pos < line.size() && line[pos] == ' ')
            {
                line = line.substr(pos + 1);
            }
        }

        // Strip bold/italic/code markers.
        std::string cleaned;
        for (size_t i = 0; i < line.size(); ++i)
        {
            if (line[i] == '*' || line[i] == '_' || line[i] == '`')
            {
                continue;
            }
            // Strip image/link syntax: ![text](url) or [text](url).
            if (line[i] == '!' && i + 1 < line.size() && line[i + 1] == '[')
            {
                auto close = line.find(']', i + 2);
                if (close != std::string::npos)
                {
                    cleaned += line.substr(i + 2, close - i - 2);
                    auto paren_close = line.find(')', close);
                    i = paren_close != std::string::npos ? paren_close : close;
                    continue;
                }
            }
            if (line[i] == '[')
            {
                auto close = line.find(']', i + 1);
                if (close != std::string::npos)
                {
                    cleaned += line.substr(i + 1, close - i - 1);
                    auto paren_close = line.find(')', close);
                    i = paren_close != std::string::npos ? paren_close : close;
                    continue;
                }
            }
            cleaned += line[i];
        }

        output += cleaned + "\n";
    }

    return output;
}

auto PlainTextExporter::check_dependencies() const -> std::expected<void, std::string>
{
    return {};
}

} // namespace markamp::core
