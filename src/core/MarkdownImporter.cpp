// ============================================================================
// File: src/core/MarkdownImporter.cpp
// Phase 40: Import — MarkdownImporter & PlainTextImporter implementation
// ============================================================================

#include "MarkdownImporter.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace markamp::core
{

// ── MarkdownImporter ────────────────────────────────────────────────────────

auto MarkdownImporter::descriptor() const -> ImportFormatDescriptor
{
    return {.format = ImportFormat::Markdown,
            .name = "Markdown",
            .extension = ".md",
            .all_extensions = {".md", ".markdown", ".mdown", ".mkd"},
            .description = "Standard Markdown files",
            .requires_pandoc = false};
}

auto MarkdownImporter::import_file(const std::filesystem::path& source_path,
                                   const ImportOptions& options) const
    -> std::expected<std::string, std::string>
{
    std::ifstream file(source_path);
    if (!file)
        return std::unexpected("Cannot open: " + source_path.string());

    std::ostringstream ss;
    ss << file.rdbuf();
    auto content = ss.str();

    // Optionally strip YAML frontmatter (--- ... ---).
    if (!options.extract_frontmatter && content.starts_with("---\n"))
    {
        auto end = content.find("\n---\n", 4);
        if (end != std::string::npos)
        {
            content = content.substr(end + 5);
        }
    }

    return content;
}

auto MarkdownImporter::check_dependencies() const -> std::expected<void, std::string>
{
    return {};
}

auto MarkdownImporter::can_import(const std::filesystem::path& file_path) const -> bool
{
    auto ext = file_path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".md" || ext == ".markdown" || ext == ".mdown" || ext == ".mkd";
}

// ── PlainTextImporter ───────────────────────────────────────────────────────

auto PlainTextImporter::descriptor() const -> ImportFormatDescriptor
{
    return {.format = ImportFormat::PlainText,
            .name = "Plain Text",
            .extension = ".txt",
            .all_extensions = {".txt", ".text", ".log"},
            .description = "Plain text files (wrapped as Markdown)",
            .requires_pandoc = false};
}

auto PlainTextImporter::import_file(const std::filesystem::path& source_path,
                                    const ImportOptions& /*options*/) const
    -> std::expected<std::string, std::string>
{
    std::ifstream file(source_path);
    if (!file)
        return std::unexpected("Cannot open: " + source_path.string());

    std::ostringstream ss;
    auto title = source_path.stem().string();
    ss << "# " << title << "\n\n";
    ss << file.rdbuf();

    return ss.str();
}

auto PlainTextImporter::check_dependencies() const -> std::expected<void, std::string>
{
    return {};
}

auto PlainTextImporter::can_import(const std::filesystem::path& file_path) const -> bool
{
    auto ext = file_path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".txt" || ext == ".text" || ext == ".log";
}

} // namespace markamp::core
