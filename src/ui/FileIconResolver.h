/// @file FileIconResolver.h
/// @brief V13 Phase 31 Task 3 — Maps file extensions to icon identifiers.
///
/// Header-only utility that resolves file extensions to semantic icon labels
/// for rendering in the command palette's file picker mode.
#pragma once

#include <string>

namespace markamp::ui
{

/// File type categories for icon rendering.
enum class FileIconType : uint8_t
{
    kCode = 0,     ///< .cpp, .h, .c, .hpp, .cc, .cxx
    kMarkdown = 1, ///< .md, .markdown, .mdx
    kConfig = 2,   ///< .json, .yaml, .yml, .toml, .ini, .cfg
    kImage = 3,    ///< .png, .jpg, .jpeg, .gif, .svg, .webp, .bmp
    kDocument = 4, ///< .txt, .pdf, .doc, .docx, .rtf
    kScript = 5,   ///< .py, .js, .ts, .rb, .sh, .bash, .zsh
    kStyle = 6,    ///< .css, .scss, .sass, .less
    kData = 7,     ///< .csv, .tsv, .sql, .db
    kBuild = 8,    ///< CMakeLists.txt, Makefile, .cmake
    kGit = 9,      ///< .gitignore, .gitmodules, .gitattributes
    kUnknown = 10  ///< Fallback
};

/// Map a file extension to icon label string.
[[nodiscard]] inline auto file_icon_label(FileIconType type) -> const char*
{
    switch (type)
    {
        case FileIconType::kCode:
            return "code";
        case FileIconType::kMarkdown:
            return "markdown";
        case FileIconType::kConfig:
            return "config";
        case FileIconType::kImage:
            return "image";
        case FileIconType::kDocument:
            return "document";
        case FileIconType::kScript:
            return "script";
        case FileIconType::kStyle:
            return "style";
        case FileIconType::kData:
            return "data";
        case FileIconType::kBuild:
            return "build";
        case FileIconType::kGit:
            return "git";
        case FileIconType::kUnknown:
            return "file";
    }
    return "file";
}

/// Map a file extension to icon character for rendering.
[[nodiscard]] inline auto file_icon_char(FileIconType type) -> const char*
{
    switch (type)
    {
        case FileIconType::kCode:
            return "{ }";
        case FileIconType::kMarkdown:
            return "M↓";
        case FileIconType::kConfig:
            return "⚙";
        case FileIconType::kImage:
            return "🖼";
        case FileIconType::kDocument:
            return "📄";
        case FileIconType::kScript:
            return "▶";
        case FileIconType::kStyle:
            return "🎨";
        case FileIconType::kData:
            return "📊";
        case FileIconType::kBuild:
            return "🔨";
        case FileIconType::kGit:
            return "⎇";
        case FileIconType::kUnknown:
            return "📄";
    }
    return "📄";
}

/// Resolve a filename/path to a FileIconType.
[[nodiscard]] inline auto resolve_file_icon(const std::string& filename) -> FileIconType
{
    if (filename.empty())
    {
        return FileIconType::kUnknown;
    }

    // Check full filename for special cases
    auto basename = filename;
    auto last_sep = filename.find_last_of("/\\");
    if (last_sep != std::string::npos)
    {
        basename = filename.substr(last_sep + 1);
    }

    if (basename == "CMakeLists.txt" || basename == "Makefile")
    {
        return FileIconType::kBuild;
    }
    if (basename.size() > 4 && basename.substr(0, 4) == ".git")
    {
        return FileIconType::kGit;
    }

    // Extract extension
    auto dot_pos = basename.find_last_of('.');
    if (dot_pos == std::string::npos || dot_pos == basename.size() - 1)
    {
        return FileIconType::kUnknown;
    }

    std::string ext = basename.substr(dot_pos + 1);
    // Convert to lowercase
    for (auto& ch : ext)
    {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }

    // Code files
    if (ext == "cpp" || ext == "h" || ext == "c" || ext == "hpp" || ext == "cc" || ext == "cxx" ||
        ext == "hxx" || ext == "hh")
    {
        return FileIconType::kCode;
    }

    // Markdown
    if (ext == "md" || ext == "markdown" || ext == "mdx")
    {
        return FileIconType::kMarkdown;
    }

    // Config
    if (ext == "json" || ext == "yaml" || ext == "yml" || ext == "toml" || ext == "ini" ||
        ext == "cfg" || ext == "xml" || ext == "env")
    {
        return FileIconType::kConfig;
    }

    // Image
    if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "gif" || ext == "svg" ||
        ext == "webp" || ext == "bmp" || ext == "ico")
    {
        return FileIconType::kImage;
    }

    // Document
    if (ext == "txt" || ext == "pdf" || ext == "doc" || ext == "docx" || ext == "rtf")
    {
        return FileIconType::kDocument;
    }

    // Script
    if (ext == "py" || ext == "js" || ext == "ts" || ext == "rb" || ext == "sh" || ext == "bash" ||
        ext == "zsh" || ext == "lua" || ext == "pl" || ext == "php")
    {
        return FileIconType::kScript;
    }

    // Style
    if (ext == "css" || ext == "scss" || ext == "sass" || ext == "less")
    {
        return FileIconType::kStyle;
    }

    // Data
    if (ext == "csv" || ext == "tsv" || ext == "sql" || ext == "db" || ext == "sqlite")
    {
        return FileIconType::kData;
    }

    // Build
    if (ext == "cmake" || ext == "mk")
    {
        return FileIconType::kBuild;
    }

    return FileIconType::kUnknown;
}

} // namespace markamp::ui
