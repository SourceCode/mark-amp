/// @file LanguagePack.cpp
/// @brief V16 Phase 13 — Language pack registry with 30+ built-in languages.

#include "LanguagePack.h"

#include <algorithm>

namespace markamp::core
{

LanguagePack::LanguagePack()
{
    register_builtin_languages();
}

auto LanguagePack::get_language(const std::string& language_id) const -> const LanguageInfo*
{
    for (const auto& lang : languages_)
    {
        if (lang.language_id == language_id)
        {
            return &lang;
        }
        for (const auto& alias : lang.aliases)
        {
            if (alias == language_id)
            {
                return &lang;
            }
        }
    }
    return nullptr;
}

auto LanguagePack::language_for_extension(const std::string& extension) const -> const LanguageInfo*
{
    for (const auto& lang : languages_)
    {
        for (const auto& ext : lang.extensions)
        {
            if (ext == extension)
            {
                return &lang;
            }
        }
    }
    return nullptr;
}

auto LanguagePack::language_for_filename(const std::string& filename) const -> const LanguageInfo*
{
    // Lowercase the filename for comparison
    std::string lower_name = filename;
    std::transform(lower_name.begin(),
                   lower_name.end(),
                   lower_name.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    for (const auto& lang : languages_)
    {
        for (const auto& fname : lang.filenames)
        {
            std::string lower_fname = fname;
            std::transform(lower_fname.begin(),
                           lower_fname.end(),
                           lower_fname.begin(),
                           [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
            if (lower_fname == lower_name)
            {
                return &lang;
            }
        }
    }
    return nullptr;
}

auto LanguagePack::languages_in_category(LanguageCategory category) const
    -> std::vector<const LanguageInfo*>
{
    std::vector<const LanguageInfo*> result;
    for (const auto& lang : languages_)
    {
        if (lang.category == category)
        {
            result.push_back(&lang);
        }
    }
    return result;
}

auto LanguagePack::all_languages() const -> const std::vector<LanguageInfo>&
{
    return languages_;
}

auto LanguagePack::language_count() const -> size_t
{
    return languages_.size();
}

void LanguagePack::register_language(LanguageInfo info)
{
    languages_.push_back(std::move(info));
}

void LanguagePack::register_builtin_languages()
{
    // Core languages
    languages_.push_back({"cpp",
                          "C++",
                          "source.cpp",
                          LanguageCategory::kCore,
                          {"cpp", "cc", "cxx", "c++", "c", "h", "hpp", "hh", "hxx", "inl"},
                          {"CMakeLists.txt"},
                          {"c++"},
                          "grammars/cpp.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}"}});
    languages_.push_back({"csharp",
                          "C#",
                          "source.cs",
                          LanguageCategory::kCore,
                          {"cs"},
                          {},
                          {"c#"},
                          "grammars/csharp.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}"}});
    languages_.push_back({"java",
                          "Java",
                          "source.java",
                          LanguageCategory::kCore,
                          {"java"},
                          {},
                          {},
                          "grammars/java.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}"}});
    languages_.push_back({"javascript",
                          "JavaScript",
                          "source.js",
                          LanguageCategory::kCore,
                          {"js", "mjs", "cjs"},
                          {},
                          {"js"},
                          "grammars/javascript.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}", "``"}});
    languages_.push_back({"typescript",
                          "TypeScript",
                          "source.ts",
                          LanguageCategory::kCore,
                          {"ts", "mts", "cts"},
                          {},
                          {"ts"},
                          "grammars/typescript.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}", "``"}});
    languages_.push_back({"jsx",
                          "JSX",
                          "source.js.jsx",
                          LanguageCategory::kCore,
                          {"jsx"},
                          {},
                          {},
                          "grammars/javascriptreact.tmLanguage.json",
                          {"javascript", "html"},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}", "<>"}});
    languages_.push_back({"tsx",
                          "TSX",
                          "source.tsx",
                          LanguageCategory::kCore,
                          {"tsx"},
                          {},
                          {},
                          "grammars/typescriptreact.tmLanguage.json",
                          {"typescript", "html"},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}", "<>"}});

    // Web languages
    languages_.push_back({"html",
                          "HTML",
                          "text.html.basic",
                          LanguageCategory::kWeb,
                          {"html", "htm", "xhtml"},
                          {},
                          {},
                          "grammars/html.tmLanguage.json",
                          {"javascript", "css"},
                          "",
                          "<!--",
                          "-->",
                          false,
                          {"<>"}});
    languages_.push_back({"css",
                          "CSS",
                          "source.css",
                          LanguageCategory::kWeb,
                          {"css"},
                          {},
                          {},
                          "grammars/css.tmLanguage.json",
                          {},
                          "",
                          "/*",
                          "*/",
                          false,
                          {"()", "{}", "[]"}});
    languages_.push_back({"scss",
                          "SCSS",
                          "source.css.scss",
                          LanguageCategory::kWeb,
                          {"scss"},
                          {},
                          {},
                          "grammars/scss.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "{}", "[]"}});
    languages_.push_back({"json",
                          "JSON",
                          "source.json",
                          LanguageCategory::kWeb,
                          {"json", "json5"},
                          {},
                          {},
                          "grammars/json.tmLanguage.json",
                          {},
                          "",
                          "",
                          "",
                          false,
                          {"[]", "{}"}});
    languages_.push_back({"yaml",
                          "YAML",
                          "source.yaml",
                          LanguageCategory::kWeb,
                          {"yaml", "yml"},
                          {},
                          {},
                          "grammars/yaml.tmLanguage.json",
                          {},
                          "#",
                          "",
                          "",
                          false,
                          {}});
    languages_.push_back({"toml",
                          "TOML",
                          "source.toml",
                          LanguageCategory::kWeb,
                          {"toml"},
                          {},
                          {},
                          "grammars/toml.tmLanguage.json",
                          {},
                          "#",
                          "",
                          "",
                          false,
                          {"[]", "{}"}});
    languages_.push_back(
        {"markdown",
         "Markdown",
         "text.html.markdown",
         LanguageCategory::kWeb,
         {"md", "mdx", "markdown"},
         {},
         {"md"},
         "grammars/markdown.tmLanguage.json",
         {"javascript", "typescript", "python", "cpp", "rust", "go", "ruby", "java"},
         "",
         "",
         "",
         false,
         {}});

    // Systems languages
    languages_.push_back({"python",
                          "Python",
                          "source.python",
                          LanguageCategory::kSystems,
                          {"py", "pyw", "pyi", "pyx"},
                          {},
                          {"py"},
                          "grammars/python.tmLanguage.json",
                          {},
                          "#",
                          "\"\"\"",
                          "\"\"\"",
                          true,
                          {"()", "[]", "{}"}});
    languages_.push_back({"go",
                          "Go",
                          "source.go",
                          LanguageCategory::kSystems,
                          {"go"},
                          {},
                          {},
                          "grammars/go.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}"}});
    languages_.push_back({"rust",
                          "Rust",
                          "source.rust",
                          LanguageCategory::kSystems,
                          {"rs"},
                          {},
                          {},
                          "grammars/rust.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}", "<>"}});
    languages_.push_back({"ruby",
                          "Ruby",
                          "source.ruby",
                          LanguageCategory::kSystems,
                          {"rb", "erb", "gemspec"},
                          {"Gemfile", "Rakefile"},
                          {},
                          "grammars/ruby.tmLanguage.json",
                          {},
                          "#",
                          "=begin",
                          "=end",
                          false,
                          {"()", "[]", "{}"}});
    languages_.push_back({"php",
                          "PHP",
                          "source.php",
                          LanguageCategory::kSystems,
                          {"php", "phtml"},
                          {},
                          {},
                          "grammars/php.tmLanguage.json",
                          {"html"},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}"}});

    // Shell languages
    languages_.push_back({"bash",
                          "Bash",
                          "source.shell",
                          LanguageCategory::kShell,
                          {"sh", "bash", "zsh", "fish", "ksh"},
                          {},
                          {"shell"},
                          "grammars/shellscript.tmLanguage.json",
                          {},
                          "#",
                          "",
                          "",
                          false,
                          {"()", "[]", "{}"}});
    languages_.push_back({"powershell",
                          "PowerShell",
                          "source.powershell",
                          LanguageCategory::kShell,
                          {"ps1", "psm1", "psd1"},
                          {},
                          {},
                          "grammars/powershell.tmLanguage.json",
                          {},
                          "#",
                          "<#",
                          "#>",
                          false,
                          {"()", "[]", "{}"}});

    // Infrastructure languages
    languages_.push_back({"sql",
                          "SQL",
                          "source.sql",
                          LanguageCategory::kInfra,
                          {"sql", "mysql", "pgsql"},
                          {},
                          {},
                          "grammars/sql.tmLanguage.json",
                          {},
                          "--",
                          "/*",
                          "*/",
                          false,
                          {"()"}});
    languages_.push_back({"dockerfile",
                          "Dockerfile",
                          "source.dockerfile",
                          LanguageCategory::kInfra,
                          {},
                          {"Dockerfile", "Dockerfile.*"},
                          {"docker"},
                          "grammars/dockerfile.tmLanguage.json",
                          {"bash"},
                          "#",
                          "",
                          "",
                          false,
                          {}});
    languages_.push_back({"cmake",
                          "CMake",
                          "source.cmake",
                          LanguageCategory::kInfra,
                          {"cmake"},
                          {"CMakeLists.txt"},
                          {},
                          "grammars/cmake.tmLanguage.json",
                          {},
                          "#",
                          "",
                          "",
                          false,
                          {"()"}});
    languages_.push_back({"makefile",
                          "Makefile",
                          "source.makefile",
                          LanguageCategory::kInfra,
                          {"mk"},
                          {"Makefile", "GNUmakefile"},
                          {"make"},
                          "grammars/makefile.tmLanguage.json",
                          {"bash"},
                          "#",
                          "",
                          "",
                          false,
                          {}});
    languages_.push_back({"terraform",
                          "Terraform",
                          "source.terraform",
                          LanguageCategory::kInfra,
                          {"tf", "tfvars"},
                          {},
                          {"hcl"},
                          "grammars/terraform.tmLanguage.json",
                          {},
                          "#",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}"}});

    // Functional languages
    languages_.push_back({"haskell",
                          "Haskell",
                          "source.haskell",
                          LanguageCategory::kFunctional,
                          {"hs", "lhs"},
                          {},
                          {},
                          "grammars/haskell.tmLanguage.json",
                          {},
                          "--",
                          "{-",
                          "-}",
                          true,
                          {"()", "[]"}});
    languages_.push_back({"scala",
                          "Scala",
                          "source.scala",
                          LanguageCategory::kFunctional,
                          {"scala", "sbt"},
                          {},
                          {},
                          "grammars/scala.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}"}});

    // Mobile languages
    languages_.push_back({"swift",
                          "Swift",
                          "source.swift",
                          LanguageCategory::kMobile,
                          {"swift"},
                          {},
                          {},
                          "grammars/swift.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}", "<>"}});
    languages_.push_back({"kotlin",
                          "Kotlin",
                          "source.kotlin",
                          LanguageCategory::kMobile,
                          {"kt", "kts"},
                          {},
                          {},
                          "grammars/kotlin.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}", "<>"}});
    languages_.push_back({"dart",
                          "Dart",
                          "source.dart",
                          LanguageCategory::kMobile,
                          {"dart"},
                          {},
                          {},
                          "grammars/dart.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "[]", "{}", "<>"}});

    // Data languages
    languages_.push_back({"graphql",
                          "GraphQL",
                          "source.graphql",
                          LanguageCategory::kData,
                          {"graphql", "gql"},
                          {},
                          {},
                          "grammars/graphql.tmLanguage.json",
                          {},
                          "#",
                          "",
                          "",
                          false,
                          {"()", "{}", "[]"}});
    languages_.push_back({"proto",
                          "Protocol Buffers",
                          "source.proto",
                          LanguageCategory::kData,
                          {"proto"},
                          {},
                          {"protobuf"},
                          "grammars/proto.tmLanguage.json",
                          {},
                          "//",
                          "/*",
                          "*/",
                          false,
                          {"()", "{}", "[]"}});
}

} // namespace markamp::core
