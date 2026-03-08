/// @file LanguagePack.h
/// @brief V16 Phase 13 — Language pack metadata and grammar bundle management.
///
/// A LanguagePack bundles grammar definitions, injection rules, and language
/// metadata for a programming language or markup format.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// Language category for grouping in UI and search.
enum class LanguageCategory
{
    kCore,       // C, C++, C#, Java, JavaScript, TypeScript
    kWeb,        // HTML, CSS, SCSS, JSON, YAML, TOML, Markdown
    kSystems,    // Python, Go, Rust, Ruby, PHP
    kShell,      // Bash, Zsh, Fish, PowerShell
    kInfra,      // SQL, Dockerfile, CMake, Make, Terraform
    kFunctional, // Haskell, Elixir, Erlang, Clojure, Scala
    kMobile,     // Swift, Kotlin, Dart
    kData,       // GraphQL, Protocol Buffers, TOML, INI
    kOther       // Catch-all
};

/// Metadata for a single language supported by the editor.
struct LanguageInfo
{
    std::string language_id;  ///< e.g. "cpp", "typescript"
    std::string display_name; ///< e.g. "C++", "TypeScript"
    std::string scope_name;   ///< TextMate top-level scope (e.g. "source.cpp")
    LanguageCategory category{LanguageCategory::kOther};

    /// File extensions (without dot) that trigger this language.
    std::vector<std::string> extensions;

    /// Filenames that trigger this language (e.g. "Makefile", "Dockerfile").
    std::vector<std::string> filenames;

    /// Aliases for language name resolution (e.g. "js" → "javascript").
    std::vector<std::string> aliases;

    /// Path to the grammar definition file (relative to resources/grammars/).
    std::string grammar_path;

    /// Languages that can be embedded within this language.
    std::vector<std::string> embedded_languages;

    /// Line comment prefix (e.g. "//", "#").
    std::string line_comment;

    /// Block comment start/end (e.g. "/*" / "*/").
    std::string block_comment_start;
    std::string block_comment_end;

    /// Whether this language supports folding based on indentation.
    bool indent_based_folding{false};

    /// Auto-closing bracket pairs (e.g. "()", "[]", "{}").
    std::vector<std::string> bracket_pairs;
};

/// Registry of all known language packs.
///
/// Usage:
/// ```cpp
/// LanguagePack pack;
/// auto info = pack.get_language("cpp");
/// auto scope = pack.scope_for_extension("ts");
/// ```
class LanguagePack
{
public:
    LanguagePack();

    /// Get language info by language ID.
    [[nodiscard]] auto get_language(const std::string& language_id) const -> const LanguageInfo*;

    /// Resolve a file extension to a language ID.
    [[nodiscard]] auto language_for_extension(const std::string& extension) const
        -> const LanguageInfo*;

    /// Resolve a filename to a language ID.
    [[nodiscard]] auto language_for_filename(const std::string& filename) const
        -> const LanguageInfo*;

    /// Get all languages in a category.
    [[nodiscard]] auto languages_in_category(LanguageCategory category) const
        -> std::vector<const LanguageInfo*>;

    /// Get all registered languages.
    [[nodiscard]] auto all_languages() const -> const std::vector<LanguageInfo>&;

    /// Total number of registered languages.
    [[nodiscard]] auto language_count() const -> size_t;

    /// Register a custom language.
    void register_language(LanguageInfo info);

private:
    std::vector<LanguageInfo> languages_;

    void register_builtin_languages();
};

} // namespace markamp::core
