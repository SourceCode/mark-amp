/// @file LanguageService.cpp
/// @brief V20 P07-T01/T02: Language service implementation.

#include "LanguageService.h"

#include "Logger.h"

#include <filesystem>

namespace markamp::core
{

LanguageService::LanguageService()
{
    register_built_in_languages();
}

void LanguageService::register_built_in_languages()
{
    auto reg = [this](const std::string& id, const std::string& name,
                       std::vector<std::string> exts, bool fenced = true) {
        LanguageDefinition def;
        def.id = id;
        def.display_name = name;
        def.extensions = std::move(exts);
        def.has_custom_tokenizer = true;
        def.supports_fenced_blocks = fenced;
        register_language(def);
    };

    reg("markdown", "Markdown", {"md", "markdown", "mdown"}, false);
    reg("cpp", "C++", {"cpp", "cxx", "cc", "c", "h", "hpp", "hxx"});
    reg("python", "Python", {"py", "pyw", "pyi"});
    reg("javascript", "JavaScript", {"js", "mjs", "cjs"});
    reg("typescript", "TypeScript", {"ts", "tsx"});
    reg("json", "JSON", {"json", "jsonc"});
    reg("yaml", "YAML", {"yaml", "yml"});
    reg("html", "HTML", {"html", "htm"});
    reg("css", "CSS", {"css"});
    reg("shell", "Shell", {"sh", "bash", "zsh"});
    reg("rust", "Rust", {"rs"});
    reg("go", "Go", {"go"});
    reg("java", "Java", {"java"});
    reg("csharp", "C#", {"cs"});
    reg("sql", "SQL", {"sql"});
    reg("xml", "XML", {"xml", "xsl", "xsd"});
    reg("toml", "TOML", {"toml"});
    reg("lua", "Lua", {"lua"});
    reg("ruby", "Ruby", {"rb"});
    reg("php", "PHP", {"php"});
    reg("swift", "Swift", {"swift"});
    reg("kotlin", "Kotlin", {"kt", "kts"});
    reg("cmake", "CMake", {"cmake"});
    reg("plaintext", "Plain Text", {"txt", "text", "log"}, false);

    // Special filenames
    LanguageDefinition makefile;
    makefile.id = "makefile";
    makefile.display_name = "Makefile";
    makefile.filenames = {"Makefile", "makefile", "GNUmakefile"};
    makefile.has_custom_tokenizer = true;
    register_language(makefile);

    LanguageDefinition dockerfile;
    dockerfile.id = "dockerfile";
    dockerfile.display_name = "Dockerfile";
    dockerfile.filenames = {"Dockerfile", "dockerfile"};
    dockerfile.has_custom_tokenizer = true;
    register_language(dockerfile);
}

void LanguageService::register_language(const LanguageDefinition& definition)
{
    languages_[definition.id] = definition;

    for (const auto& ext : definition.extensions)
    {
        extension_map_[ext] = definition.id;
    }

    for (const auto& fname : definition.filenames)
    {
        filename_map_[fname] = definition.id;
    }
}

auto LanguageService::resolve_from_path(const std::string& file_path) const -> LanguageResolution
{
    if (file_path.empty())
    {
        return {"plaintext", "Plain Text", true, "fallback"};
    }

    namespace fs = std::filesystem;
    auto path = fs::path(file_path);

    // Check filename first
    auto fname = path.filename().string();
    auto fname_it = filename_map_.find(fname);
    if (fname_it != filename_map_.end())
    {
        auto def_it = languages_.find(fname_it->second);
        if (def_it != languages_.end())
        {
            return {def_it->second.id, def_it->second.display_name, false, "filename"};
        }
    }

    // Check extension
    auto ext = path.extension().string();
    if (!ext.empty() && ext[0] == '.')
    {
        ext = ext.substr(1);
    }

    // Special case for CMakeLists.txt
    if (fname == "CMakeLists.txt")
    {
        return {"cmake", "CMake", false, "filename"};
    }

    auto ext_it = extension_map_.find(ext);
    if (ext_it != extension_map_.end())
    {
        auto def_it = languages_.find(ext_it->second);
        if (def_it != languages_.end())
        {
            return {def_it->second.id, def_it->second.display_name, false, "extension"};
        }
    }

    return {"plaintext", "Plain Text", true, "fallback"};
}

auto LanguageService::resolve_from_id(const std::string& language_id) const -> LanguageResolution
{
    auto it = languages_.find(language_id);
    if (it != languages_.end())
    {
        return {it->second.id, it->second.display_name, false, "explicit"};
    }
    return {"plaintext", "Plain Text", true, "fallback"};
}

auto LanguageService::resolve_from_first_line(const std::string& first_line) const
    -> LanguageResolution
{
    if (first_line.starts_with("#!/") || first_line.starts_with("#! /"))
    {
        if (first_line.find("python") != std::string::npos)
        {
            return {"python", "Python", false, "first-line"};
        }
        if (first_line.find("bash") != std::string::npos ||
            first_line.find("/sh") != std::string::npos)
        {
            return {"shell", "Shell", false, "first-line"};
        }
        if (first_line.find("node") != std::string::npos)
        {
            return {"javascript", "JavaScript", false, "first-line"};
        }
        if (first_line.find("ruby") != std::string::npos)
        {
            return {"ruby", "Ruby", false, "first-line"};
        }
    }

    return {};
}

auto LanguageService::find_language(const std::string& language_id) const
    -> const LanguageDefinition*
{
    auto it = languages_.find(language_id);
    if (it == languages_.end())
    {
        return nullptr;
    }
    return &it->second;
}

auto LanguageService::all_languages() const -> std::vector<LanguageDefinition>
{
    std::vector<LanguageDefinition> result;
    result.reserve(languages_.size());
    for (const auto& [id, def] : languages_)
    {
        result.push_back(def);
    }
    return result;
}

auto LanguageService::fenced_block_languages() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& [id, def] : languages_)
    {
        if (def.supports_fenced_blocks)
        {
            result.push_back(def.id);
        }
    }
    return result;
}

} // namespace markamp::core
