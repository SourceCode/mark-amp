#pragma once

/// @file LanguageDetector.h
/// @brief V9 Phase 19 — Detect document language from file extension, frontmatter, or shebang.

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>

namespace markamp::core
{

/// Header-only language detector using file extension, YAML frontmatter `lang:` field,
/// and shebang line.
///
/// Usage:
/// ```cpp
/// LanguageDetector detector;
/// auto lang = detector.detect("notes.md", "---\nlang: python\n---\n");
/// // Returns "python" (frontmatter takes priority)
///
/// auto lang2 = detector.detect("script.py", "");
/// // Returns "python" (from extension)
/// ```
class LanguageDetector
{
public:
    LanguageDetector() = default;

    /// Detect language from filename and/or content.
    /// Priority: frontmatter `lang:` > shebang > file extension > "markdown".
    /// @param filename Filename or path (used for extension detection)
    /// @param content Optional document content (for frontmatter/shebang)
    /// @return Language identifier string (e.g. "markdown", "python", "json")
    [[nodiscard]] auto detect(const std::string& filename, const std::string& content = "") const
        -> std::string
    {
        // 1. Try frontmatter `lang:` field
        if (!content.empty())
        {
            auto frontmatter_lang = detect_from_frontmatter(content);
            if (!frontmatter_lang.empty())
            {
                return frontmatter_lang;
            }
        }

        // 2. Try shebang line
        if (!content.empty())
        {
            auto shebang_lang = detect_from_shebang(content);
            if (!shebang_lang.empty())
            {
                return shebang_lang;
            }
        }

        // 3. Try file extension
        auto extension_lang = detect_from_extension(filename);
        if (!extension_lang.empty())
        {
            return extension_lang;
        }

        // 4. Default to markdown
        return "markdown";
    }

    /// Detect language from file extension only.
    [[nodiscard]] static auto detect_from_extension(const std::string& filename) -> std::string
    {
        auto dot_pos = filename.rfind('.');
        if (dot_pos == std::string::npos || dot_pos == filename.size() - 1)
        {
            return "";
        }

        auto ext = filename.substr(dot_pos + 1);
        // Lowercase the extension
        std::string ext_lower;
        ext_lower.reserve(ext.size());
        for (const char chr : ext)
        {
            ext_lower += static_cast<char>(std::tolower(static_cast<unsigned char>(chr)));
        }

        // Extension -> language mapping
        static const std::unordered_map<std::string, std::string> kExtensionMap = {
            {"md", "markdown"},
            {"markdown", "markdown"},
            {"mdx", "mdx"},
            {"txt", "plaintext"},
            {"text", "plaintext"},
            {"py", "python"},
            {"pyw", "python"},
            {"pyi", "python"},
            {"js", "javascript"},
            {"mjs", "javascript"},
            {"cjs", "javascript"},
            {"ts", "typescript"},
            {"tsx", "typescriptreact"},
            {"jsx", "javascriptreact"},
            {"json", "json"},
            {"jsonc", "jsonc"},
            {"json5", "json5"},
            {"yaml", "yaml"},
            {"yml", "yaml"},
            {"toml", "toml"},
            {"xml", "xml"},
            {"svg", "xml"},
            {"xsl", "xml"},
            {"html", "html"},
            {"htm", "html"},
            {"xhtml", "html"},
            {"css", "css"},
            {"scss", "scss"},
            {"sass", "sass"},
            {"less", "less"},
            {"c", "c"},
            {"h", "c"},
            {"cpp", "cpp"},
            {"cxx", "cpp"},
            {"cc", "cpp"},
            {"hpp", "cpp"},
            {"hxx", "cpp"},
            {"rs", "rust"},
            {"go", "go"},
            {"java", "java"},
            {"rb", "ruby"},
            {"php", "php"},
            {"swift", "swift"},
            {"kt", "kotlin"},
            {"kts", "kotlin"},
            {"sh", "shellscript"},
            {"bash", "shellscript"},
            {"zsh", "shellscript"},
            {"ps1", "powershell"},
            {"psm1", "powershell"},
            {"sql", "sql"},
            {"r", "r"},
            {"rmd", "rmarkdown"},
            {"lua", "lua"},
            {"vim", "viml"},
            {"dockerfile", "dockerfile"},
            {"makefile", "makefile"},
            {"cmake", "cmake"},
            {"ini", "ini"},
            {"cfg", "ini"},
            {"conf", "ini"},
            {"log", "log"},
            {"csv", "csv"},
            {"tsv", "csv"},
            {"tex", "latex"},
            {"latex", "latex"},
            {"bib", "bibtex"},
        };

        auto iter = kExtensionMap.find(ext_lower);
        if (iter != kExtensionMap.end())
        {
            return iter->second;
        }

        return "";
    }

    /// Detect language from YAML frontmatter `lang:` or `language:` field.
    [[nodiscard]] static auto detect_from_frontmatter(const std::string& content) -> std::string
    {
        // Check for frontmatter start
        if (content.size() < 4 || content.substr(0, 4) != "---\n")
        {
            return "";
        }

        // Find frontmatter end
        auto end_pos = content.find("\n---", 4);
        if (end_pos == std::string::npos)
        {
            return "";
        }

        auto frontmatter = content.substr(4, end_pos - 4);

        // Look for `lang:` or `language:` field
        std::string lang_value;

        auto lang_pos = frontmatter.find("lang:");
        if (lang_pos == std::string::npos)
        {
            lang_pos = frontmatter.find("language:");
        }

        if (lang_pos == std::string::npos)
        {
            return "";
        }

        // Make sure it's at the start of a line
        if (lang_pos > 0 && frontmatter[lang_pos - 1] != '\n')
        {
            return "";
        }

        auto colon_pos = frontmatter.find(':', lang_pos);
        auto value_start = colon_pos + 1;
        // Skip whitespace
        while (value_start < frontmatter.size() &&
               (frontmatter[value_start] == ' ' || frontmatter[value_start] == '\t'))
        {
            ++value_start;
        }

        auto value_end = frontmatter.find('\n', value_start);
        if (value_end == std::string::npos)
        {
            value_end = frontmatter.size();
        }

        lang_value = frontmatter.substr(value_start, value_end - value_start);

        // Trim trailing whitespace
        while (!lang_value.empty() &&
               (lang_value.back() == ' ' || lang_value.back() == '\t' || lang_value.back() == '\r'))
        {
            lang_value.pop_back();
        }

        // Remove quotes if present
        if (lang_value.size() >= 2 && ((lang_value.front() == '"' && lang_value.back() == '"') ||
                                       (lang_value.front() == '\'' && lang_value.back() == '\'')))
        {
            lang_value = lang_value.substr(1, lang_value.size() - 2);
        }

        return lang_value;
    }

    /// Detect language from shebang line (#!).
    [[nodiscard]] static auto detect_from_shebang(const std::string& content) -> std::string
    {
        if (content.size() < 2 || content[0] != '#' || content[1] != '!')
        {
            return "";
        }

        auto line_end = content.find('\n');
        auto shebang =
            content.substr(2, (line_end == std::string::npos) ? std::string::npos : line_end - 2);

        // Extract interpreter name
        // Common patterns: #!/usr/bin/python, #!/usr/bin/env python3
        static const std::unordered_map<std::string, std::string> kInterpreterMap = {
            {"python", "python"},
            {"python3", "python"},
            {"node", "javascript"},
            {"nodejs", "javascript"},
            {"bash", "shellscript"},
            {"sh", "shellscript"},
            {"zsh", "shellscript"},
            {"ruby", "ruby"},
            {"perl", "perl"},
            {"php", "php"},
            {"lua", "lua"},
            {"Rscript", "r"},
        };

        // Check for env-style shebang
        auto env_pos = shebang.find("env ");
        std::string interpreter;
        if (env_pos != std::string::npos)
        {
            interpreter = shebang.substr(env_pos + 4);
        }
        else
        {
            auto last_slash = shebang.rfind('/');
            interpreter =
                (last_slash != std::string::npos) ? shebang.substr(last_slash + 1) : shebang;
        }

        // Trim whitespace and version numbers
        auto space_pos = interpreter.find(' ');
        if (space_pos != std::string::npos)
        {
            interpreter = interpreter.substr(0, space_pos);
        }

        // Remove version suffix (e.g., python3.9 → python3)
        while (!interpreter.empty() && interpreter.back() == '.')
        {
            interpreter.pop_back();
        }

        auto iter = kInterpreterMap.find(interpreter);
        if (iter != kInterpreterMap.end())
        {
            return iter->second;
        }

        return "";
    }
};

} // namespace markamp::core
