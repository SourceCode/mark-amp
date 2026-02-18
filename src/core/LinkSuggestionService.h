#pragma once

/// @file LinkSuggestionService.h
/// @brief V4 Phase 11 – Smart Linking Suggestions.

#include "EventBus.h"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace markamp::core
{

class VaultService;
class BacklinkIndex;
class SearchEngine;

// ============================================================================
// Link Suggestion
// ============================================================================

struct LinkSuggestion
{
    std::string document_id;
    std::string document_title;
    std::string file_path;
    double relevance_score{0.0};
    std::string reason;

    enum class Source : uint8_t
    {
        kTitleMatch,
        kTagOverlap,
        kGraphProximity,
        kContentSimilarity,
        kFrequentCoLink
    } source{Source::kTitleMatch};

    std::vector<std::string> shared_tags;
    int shared_links{0};
};

// ============================================================================
// Autocomplete Result
// ============================================================================

struct AutocompleteResult
{
    std::string display_text;
    std::string insert_text;
    std::string subtitle;
    double score{0.0};
};

// ============================================================================
// Link Suggestion Service
// ============================================================================

class LinkSuggestionService
{
public:
    LinkSuggestionService(EventBus& event_bus,
                          VaultService& vault_service,
                          BacklinkIndex& backlink_index,
                          SearchEngine& search_engine);

    /// Get autocomplete suggestions for [[ link prefix.
    [[nodiscard]] auto autocomplete(const std::string& prefix, int limit = 15) const
        -> std::vector<AutocompleteResult>;

    /// Get smart link suggestions for the current document.
    [[nodiscard]] auto suggest_links(const std::string& document_id, int limit = 10) const
        -> std::vector<LinkSuggestion>;

    /// Detect potential link targets in text (words matching document titles).
    [[nodiscard]] auto detect_link_targets(const std::string& text) const
        -> std::vector<std::pair<std::string, std::string>>;

    // ── Phase 17 Task 10 — Link validation ────────────────────

    /// Result of validating a single link.
    struct LinkValidationResult
    {
        std::string link_text;   ///< Original link text
        std::string link_target; ///< Resolved target
        bool is_valid{false};    ///< Whether the link resolves
        std::string message;     ///< Diagnostic message if invalid
        int line{0};             ///< Source line number
    };

    /// Validate a single link target against the vault.
    [[nodiscard]] auto validate_link(const std::string& link_target) const -> LinkValidationResult;

    /// Validate all links in a document's content.
    [[nodiscard]] auto batch_validate(const std::string& document_content) const
        -> std::vector<LinkValidationResult>;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;
    BacklinkIndex& backlink_index_;
    SearchEngine& search_engine_;

    [[nodiscard]] auto score_by_tag_overlap(const std::string& doc_a,
                                            const std::string& doc_b) const -> double;
    [[nodiscard]] auto score_by_graph_proximity(const std::string& doc_a,
                                                const std::string& doc_b) const -> double;
    [[nodiscard]] auto score_by_co_links(const std::string& doc_a, const std::string& doc_b) const
        -> double;

    [[nodiscard]] auto levenshtein_distance(const std::string& str_a,
                                            const std::string& str_b) const -> int;
};

} // namespace markamp::core
