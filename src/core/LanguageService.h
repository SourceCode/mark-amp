/// @file LanguageService.h
/// @brief V20 P07-T01/T02: Canonical language resolution and tokenization contract.
///
/// Defines one unified pipeline for language detection, token source selection,
/// and language metadata. Replaces the split between wxStyledTextCtrl lexers
/// and the custom tokenization stack.
#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Language definition with tokenization metadata.
struct LanguageDefinition
{
    std::string id;                          ///< e.g. "cpp", "python", "markdown"
    std::string display_name;                ///< Display name
    std::vector<std::string> extensions;     ///< File extensions (without dot)
    std::vector<std::string> filenames;      ///< Exact filenames (e.g. "Makefile")
    std::string first_line_pattern;          ///< First-line match (e.g. "#!" shebang)
    bool has_custom_tokenizer{false};
    bool supports_fenced_blocks{false};      ///< Can appear inside markdown fences

    [[nodiscard]] auto matches_extension(const std::string& ext) const noexcept -> bool
    {
        for (const auto& e : extensions)
        {
            if (e == ext) return true;
        }
        return false;
    }
};

/// Result of language resolution.
struct LanguageResolution
{
    std::string language_id;
    std::string display_name;
    bool is_fallback{false};              ///< Whether resolution fell back to plain text
    std::string resolution_source;        ///< "extension", "filename", "first-line", "explicit", "fallback"

    [[nodiscard]] auto ok() const noexcept -> bool { return !language_id.empty(); }
};

/// Unified language resolution service.
class LanguageService
{
public:
    LanguageService();

    /// Register a language definition.
    void register_language(const LanguageDefinition& definition);

    /// Resolve language from a file path.
    [[nodiscard]] auto resolve_from_path(const std::string& file_path) const -> LanguageResolution;

    /// Resolve language from an explicit language ID.
    [[nodiscard]] auto resolve_from_id(const std::string& language_id) const -> LanguageResolution;

    /// Resolve language from first-line content.
    [[nodiscard]] auto resolve_from_first_line(const std::string& first_line) const
        -> LanguageResolution;

    /// Get language definition by ID.
    [[nodiscard]] auto find_language(const std::string& language_id) const
        -> const LanguageDefinition*;

    /// List all registered languages.
    [[nodiscard]] auto all_languages() const -> std::vector<LanguageDefinition>;

    /// Languages supporting fenced blocks.
    [[nodiscard]] auto fenced_block_languages() const -> std::vector<std::string>;

    /// Total registrations.
    [[nodiscard]] auto language_count() const noexcept -> int
    {
        return static_cast<int>(languages_.size());
    }

private:
    void register_built_in_languages();

    std::unordered_map<std::string, LanguageDefinition> languages_;
    std::unordered_map<std::string, std::string> extension_map_;    ///< ext -> language_id
    std::unordered_map<std::string, std::string> filename_map_;     ///< filename -> language_id
};

} // namespace markamp::core
