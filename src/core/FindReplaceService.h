/// @file FindReplaceService.h
/// @brief V4 Phase 16 – Regex Find and Replace service.

#pragma once

#include <expected>
#include <optional>
#include <regex>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class VaultService;

/// A single match location within a document.
struct FindMatch
{
    std::string document_id;
    std::string file_path;
    int line_number{0};
    int column{0};
    int match_length{0};
    std::string matched_text;
    std::string context_before; ///< Text before match on same line
    std::string context_after;  ///< Text after match on same line
    std::string full_line;
};

/// A preview of what a replacement would produce.
struct ReplacePreview
{
    FindMatch match;
    std::string replacement_text; ///< What the match will become
    std::string preview_line;     ///< Full line after replacement
};

/// Options controlling find/replace behavior.
struct FindReplaceOptions
{
    std::string pattern;
    std::string replacement;
    bool use_regex{false};
    bool case_sensitive{false};
    bool whole_word{false};
    bool multiline{false};
    bool preserve_case{false};

    enum class Scope : uint8_t
    {
        CurrentFile,
        OpenFiles,
        Vault
    } scope{Scope::CurrentFile};

    std::vector<std::string> include_paths;
    std::vector<std::string> exclude_paths;
};

/// Aggregate result of a find operation.
struct FindReplaceResult
{
    std::vector<FindMatch> matches;
    int total_matches{0};
    int files_searched{0};
    int files_with_matches{0};
    double elapsed_ms{0.0};
};

/// Service for searching and replacing text across documents.
class FindReplaceService
{
public:
    FindReplaceService(EventBus& event_bus, VaultService& vault_service);

    /// Find all matches across scope.
    [[nodiscard]] auto find_all(const FindReplaceOptions& options) -> FindReplaceResult;

    /// Preview replacements without applying them.
    [[nodiscard]] auto preview_replace(const FindReplaceOptions& options)
        -> std::vector<ReplacePreview>;

    /// Replace all matches. Returns count of replacements made.
    [[nodiscard]] auto replace_all(const FindReplaceOptions& options)
        -> std::expected<int, std::string>;

    /// Replace a single match. Returns the new line content.
    [[nodiscard]] auto replace_single(const FindMatch& match, const FindReplaceOptions& options)
        -> std::expected<std::string, std::string>;

    /// Validate a regex pattern. Returns error message if invalid.
    [[nodiscard]] auto validate_pattern(const std::string& pattern) const
        -> std::optional<std::string>;

    /// Expand replacement string with capture group references ($0-$9).
    [[nodiscard]] auto expand_replacement(const std::string& matched,
                                          const std::smatch& captures,
                                          const std::string& replacement) const -> std::string;

    /// Search a single file's content for matches.
    [[nodiscard]] auto search_file(const std::string& content,
                                   const std::string& document_id,
                                   const std::string& file_path,
                                   const FindReplaceOptions& options) const
        -> std::vector<FindMatch>;

    /// Test whether a file path passes include/exclude filters.
    [[nodiscard]] static auto path_matches_filter(const std::string& path,
                                                  const std::vector<std::string>& include,
                                                  const std::vector<std::string>& exclude) -> bool;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;

    [[nodiscard]] auto compile_regex(const FindReplaceOptions& options) const
        -> std::expected<std::regex, std::string>;
    [[nodiscard]] auto find_literal(const std::string& content,
                                    const std::string& document_id,
                                    const std::string& file_path,
                                    const FindReplaceOptions& options) const
        -> std::vector<FindMatch>;
};

} // namespace markamp::core
