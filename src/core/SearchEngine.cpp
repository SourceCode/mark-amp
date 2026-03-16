/// @file SearchEngine.cpp
/// @brief V4 Phase 05 – Full-Text Search Engine with inverted index and BM25 scoring.

#include "SearchEngine.h"

#include "EventBus.h"
#include "Events.h"
#include "VaultService.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <numeric>
#include <regex>
#include <sstream>
#include <unordered_set>

namespace markamp::core
{

// ============================================================================
// Stop words (common English words excluded from indexing)
// ============================================================================
namespace
{

auto is_stop_word(const std::string& word) -> bool
{
    static const std::unordered_set<std::string> kStopWords = {
        "a",   "an",    "and",  "are",   "as",    "at",   "be",   "but", "by",  "for",  "if",
        "in",  "into",  "is",   "it",    "no",    "not",  "of",   "on",  "or",  "such", "that",
        "the", "their", "then", "there", "these", "they", "this", "to",  "was", "will", "with"};
    return kStopWords.contains(word);
}

// BM25 parameters
constexpr double kBM25_K1 = 1.2;
constexpr double kBM25_B = 0.75;

} // anonymous namespace

// ============================================================================
// Constructor
// ============================================================================
SearchEngine::SearchEngine(EventBus& event_bus, VaultService& vault_service)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
{
    vault_opened_sub_ = event_bus_.subscribe<events::VaultOpenedEvent>(
        [this](const events::VaultOpenedEvent& /*event*/) { rebuild_index(); });

    doc_created_sub_ = event_bus_.subscribe<events::VaultDocumentCreatedEvent>(
        [this](const events::VaultDocumentCreatedEvent& event)
        { index_document(event.document_id); });

    doc_deleted_sub_ = event_bus_.subscribe<events::VaultDocumentDeletedEvent>(
        [this](const events::VaultDocumentDeletedEvent& event)
        { remove_document(event.document_id); });
}

// ============================================================================
// Tokenization
// ============================================================================
auto SearchEngine::normalize_token(std::string_view token) const -> std::string
{
    std::string result;
    result.reserve(token.size());
    for (char chr : token)
    {
        if (std::isalnum(static_cast<unsigned char>(chr)))
        {
            result += static_cast<char>(std::tolower(static_cast<unsigned char>(chr)));
        }
    }
    return result;
}

auto SearchEngine::tokenize(std::string_view text) const -> std::vector<std::string>
{
    std::vector<std::string> tokens;
    size_t idx = 0;

    while (idx < text.size())
    {
        // Skip non-alphanumeric
        while (idx < text.size() && !std::isalnum(static_cast<unsigned char>(text[idx])))
        {
            ++idx;
        }
        if (idx >= text.size())
        {
            break;
        }

        // Collect word characters
        size_t start = idx;
        while (idx < text.size() && (std::isalnum(static_cast<unsigned char>(text[idx])) ||
                                     text[idx] == '_' || text[idx] == '-'))
        {
            ++idx;
        }

        auto normalized = normalize_token(text.substr(start, idx - start));
        if (!normalized.empty() && normalized.size() >= 2 && !is_stop_word(normalized))
        {
            tokens.push_back(std::move(normalized));
        }
    }

    return tokens;
}

// ============================================================================
// Index building
// ============================================================================
auto SearchEngine::rebuild_index() -> void
{
    const auto start_time = std::chrono::steady_clock::now();

    const std::lock_guard index_lock(mutex_);

    inverted_index_.clear();
    doc_meta_.clear();
    total_documents_ = 0;
    avg_doc_length_ = 0.0;

    const auto documents = vault_service_.list_documents();

    for (const auto& entry : documents)
    {
        auto doc_result = vault_service_.open_document(entry.file_path);
        if (!doc_result.has_value())
        {
            continue;
        }

        auto doc_ptr = doc_result.value();
        const auto& content = doc_ptr->markdown();

        // Store document metadata
        DocMeta meta;
        meta.title = entry.title;
        meta.file_path =
            std::filesystem::relative(entry.file_path, vault_service_.vault_path()).string();
        meta.content = content;
        meta.modified_time = entry.modified_time;

        auto tokens = tokenize(content);
        meta.total_terms = static_cast<int>(tokens.size());

        // Build posting lists
        for (int pos = 0; pos < static_cast<int>(tokens.size()); ++pos)
        {
            const auto& token = tokens[static_cast<size_t>(pos)];
            auto& posting = inverted_index_[token];
            posting.doc_positions[entry.document_id].push_back(pos);
            posting.total_frequency++;
        }

        doc_meta_[entry.document_id] = std::move(meta);
    }

    total_documents_ = static_cast<int>(doc_meta_.size());
    recompute_avg_doc_length();

    const auto elapsed = std::chrono::steady_clock::now() - start_time;
    const double elapsed_ms = std::chrono::duration<double, std::milli>(elapsed).count();

    events::VaultSearchIndexRebuiltEvent evt;
    evt.document_count = total_documents_;
    evt.term_count = static_cast<int>(inverted_index_.size());
    evt.elapsed_ms = elapsed_ms;
    event_bus_.publish(evt);
}

auto SearchEngine::recompute_avg_doc_length() -> void
{
    if (doc_meta_.empty())
    {
        avg_doc_length_ = 0.0;
        return;
    }

    double total = 0.0;
    for (const auto& [doc_id, meta] : doc_meta_)
    {
        total += meta.total_terms;
    }
    avg_doc_length_ = total / static_cast<double>(doc_meta_.size());
}

auto SearchEngine::index_document(const std::string& document_id) -> void
{
    const std::lock_guard index_lock(mutex_);

    // Remove old entries first
    for (auto& [term, posting] : inverted_index_)
    {
        posting.doc_positions.erase(document_id);
    }
    doc_meta_.erase(document_id);

    // Find the document entry from vault
    const auto documents = vault_service_.list_documents();
    for (const auto& entry : documents)
    {
        if (entry.document_id != document_id)
        {
            continue;
        }

        auto doc_result = vault_service_.open_document(entry.file_path);
        if (!doc_result.has_value())
        {
            break;
        }

        auto doc_ptr = doc_result.value();
        const auto& content = doc_ptr->markdown();

        DocMeta meta;
        meta.title = entry.title;
        meta.file_path =
            std::filesystem::relative(entry.file_path, vault_service_.vault_path()).string();
        meta.content = content;
        meta.modified_time = entry.modified_time;

        auto tokens = tokenize(content);
        meta.total_terms = static_cast<int>(tokens.size());

        for (int pos = 0; pos < static_cast<int>(tokens.size()); ++pos)
        {
            const auto& token = tokens[static_cast<size_t>(pos)];
            auto& posting = inverted_index_[token];
            posting.doc_positions[document_id].push_back(pos);
            posting.total_frequency++;
        }

        doc_meta_[document_id] = std::move(meta);
        break;
    }

    total_documents_ = static_cast<int>(doc_meta_.size());
    recompute_avg_doc_length();
}

auto SearchEngine::remove_document(const std::string& document_id) -> void
{
    const std::lock_guard index_lock(mutex_);

    // Remove from posting lists
    std::vector<std::string> empty_terms;
    for (auto& [term, posting] : inverted_index_)
    {
        auto doc_iter = posting.doc_positions.find(document_id);
        if (doc_iter != posting.doc_positions.end())
        {
            posting.total_frequency -= static_cast<int>(doc_iter->second.size());
            posting.doc_positions.erase(doc_iter);
            if (posting.doc_positions.empty())
            {
                empty_terms.push_back(term);
            }
        }
    }

    // Clean up empty terms
    for (const auto& term : empty_terms)
    {
        inverted_index_.erase(term);
    }

    doc_meta_.erase(document_id);
    total_documents_ = static_cast<int>(doc_meta_.size());
    recompute_avg_doc_length();
}

// ============================================================================
// BM25 Scoring
// ============================================================================
auto SearchEngine::bm25_score(const std::string& term, const std::string& doc_id) const -> double
{
    auto term_iter = inverted_index_.find(term);
    if (term_iter == inverted_index_.end())
    {
        return 0.0;
    }

    const auto& posting = term_iter->second;
    auto doc_iter = posting.doc_positions.find(doc_id);
    if (doc_iter == posting.doc_positions.end())
    {
        return 0.0;
    }

    auto meta_iter = doc_meta_.find(doc_id);
    if (meta_iter == doc_meta_.end())
    {
        return 0.0;
    }

    const double term_freq = static_cast<double>(doc_iter->second.size());
    const double doc_len = static_cast<double>(meta_iter->second.total_terms);
    const int docs_with_term = static_cast<int>(posting.doc_positions.size());

    // IDF component
    const double idf = std::log(
        (static_cast<double>(total_documents_) - static_cast<double>(docs_with_term) + 0.5) /
            (static_cast<double>(docs_with_term) + 0.5) +
        1.0);

    // TF component with length normalization
    const double normalized_tf =
        (term_freq * (kBM25_K1 + 1.0)) /
        (term_freq +
         kBM25_K1 * (1.0 - kBM25_B + kBM25_B * doc_len / std::max(avg_doc_length_, 1.0)));

    return idf * normalized_tf;
}

// ============================================================================
// Snippet extraction
// ============================================================================
auto SearchEngine::extract_snippet(const std::string& doc_id,
                                   const std::vector<std::string>& terms,
                                   int context_chars) const -> std::string
{
    auto meta_iter = doc_meta_.find(doc_id);
    if (meta_iter == doc_meta_.end() || meta_iter->second.content.empty())
    {
        return "";
    }

    const auto& content = meta_iter->second.content;
    std::string lower_content = content;
    std::transform(lower_content.begin(),
                   lower_content.end(),
                   lower_content.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    // Find the first occurrence of any search term
    size_t best_pos = std::string::npos;
    for (const auto& term : terms)
    {
        size_t found_pos = lower_content.find(term);
        if (found_pos != std::string::npos &&
            (best_pos == std::string::npos || found_pos < best_pos))
        {
            best_pos = found_pos;
        }
    }

    if (best_pos == std::string::npos)
    {
        // No match found — return start of document
        size_t snippet_len = std::min(content.size(), static_cast<size_t>(context_chars * 2));
        return content.substr(0, snippet_len) + "...";
    }

    // Extract surrounding context
    size_t snippet_start = (best_pos > static_cast<size_t>(context_chars))
                               ? best_pos - static_cast<size_t>(context_chars)
                               : 0;
    size_t snippet_end = std::min(content.size(), best_pos + static_cast<size_t>(context_chars));

    std::string snippet = content.substr(snippet_start, snippet_end - snippet_start);

    // Add ellipsis if truncated
    if (snippet_start > 0)
    {
        snippet = "..." + snippet;
    }
    if (snippet_end < content.size())
    {
        snippet += "...";
    }

    return snippet;
}

// ============================================================================
// Edit distance for fuzzy matching
// ============================================================================
auto SearchEngine::levenshtein_distance(std::string_view str_a, std::string_view str_b) -> int
{
    const size_t len_a = str_a.size();
    const size_t len_b = str_b.size();

    std::vector<std::vector<int>> dp_matrix(len_a + 1, std::vector<int>(len_b + 1));

    for (size_t row = 0; row <= len_a; ++row)
    {
        dp_matrix[row][0] = static_cast<int>(row);
    }
    for (size_t col = 0; col <= len_b; ++col)
    {
        dp_matrix[0][col] = static_cast<int>(col);
    }

    for (size_t row = 1; row <= len_a; ++row)
    {
        for (size_t col = 1; col <= len_b; ++col)
        {
            int cost = (str_a[row - 1] == str_b[col - 1]) ? 0 : 1;
            dp_matrix[row][col] = std::min({
                dp_matrix[row - 1][col] + 1,       // deletion
                dp_matrix[row][col - 1] + 1,       // insertion
                dp_matrix[row - 1][col - 1] + cost // substitution
            });
        }
    }

    return dp_matrix[len_a][len_b];
}

// ============================================================================
// Scope filtering
// ============================================================================
auto SearchEngine::apply_scope(std::vector<SearchHit>& hits, const SearchScope& scope) const -> void
{
    if (scope.folders.empty() && scope.exclude_folders.empty() && scope.tags.empty() &&
        scope.frontmatter_field.empty())
    {
        return; // No scope filters
    }

    std::erase_if(hits,
                  [&](const SearchHit& hit) -> bool
                  {
                      // Folder inclusion filter
                      if (!scope.folders.empty())
                      {
                          bool in_folder = false;
                          for (const auto& folder : scope.folders)
                          {
                              if (hit.file_path.starts_with(folder))
                              {
                                  in_folder = true;
                                  break;
                              }
                          }
                          if (!in_folder)
                          {
                              return true;
                          }
                      }

                      // Folder exclusion filter
                      for (const auto& excluded : scope.exclude_folders)
                      {
                          if (hit.file_path.starts_with(excluded))
                          {
                              return true;
                          }
                      }

                      // Tag filter: check if document has required tags
                      if (!scope.tags.empty())
                      {
                          const auto documents = vault_service_.list_documents();
                          for (const auto& entry : documents)
                          {
                              if (entry.document_id == hit.document_id)
                              {
                                  for (const auto& required_tag : scope.tags)
                                  {
                                      bool has_tag = false;
                                      for (const auto& doc_tag : entry.tags)
                                      {
                                          if (doc_tag == required_tag)
                                          {
                                              has_tag = true;
                                              break;
                                          }
                                      }
                                      if (!has_tag)
                                      {
                                          return true;
                                      }
                                  }
                                  break;
                              }
                          }
                      }

                      // Frontmatter field filter
                      if (!scope.frontmatter_field.empty())
                      {
                          auto doc_result = vault_service_.open_document(hit.file_path);
                          if (doc_result.has_value())
                          {
                              const auto& frontmatter = doc_result.value()->frontmatter();
                              auto field_iter =
                                  frontmatter.string_fields.find(scope.frontmatter_field);
                              if (field_iter == frontmatter.string_fields.end() ||
                                  field_iter->second != scope.frontmatter_value)
                              {
                                  return true;
                              }
                          }
                      }

                      return false;
                  });
}

// ============================================================================
// Search methods
// ============================================================================
auto SearchEngine::search_keyword(const SearchQuery& query) -> std::vector<SearchHit>
{
    auto query_tokens = tokenize(query.query_string);
    if (query_tokens.empty())
    {
        return {};
    }

    // Find documents containing ALL query terms (AND semantics)
    std::unordered_set<std::string> candidate_docs;
    bool first_term = true;

    for (const auto& term : query_tokens)
    {
        auto term_iter = inverted_index_.find(term);
        if (term_iter == inverted_index_.end())
        {
            return {}; // One term not found -> no AND results
        }

        if (first_term)
        {
            for (const auto& [doc_id, positions] : term_iter->second.doc_positions)
            {
                candidate_docs.insert(doc_id);
            }
            first_term = false;
        }
        else
        {
            std::unordered_set<std::string> intersection;
            for (const auto& [doc_id, positions] : term_iter->second.doc_positions)
            {
                if (candidate_docs.contains(doc_id))
                {
                    intersection.insert(doc_id);
                }
            }
            candidate_docs = std::move(intersection);
        }

        if (candidate_docs.empty())
        {
            return {};
        }
    }

    // Score each matching document
    std::vector<SearchHit> hits;
    hits.reserve(candidate_docs.size());

    for (const auto& doc_id : candidate_docs)
    {
        SearchHit hit;
        hit.document_id = doc_id;

        auto meta_iter = doc_meta_.find(doc_id);
        if (meta_iter != doc_meta_.end())
        {
            hit.document_title = meta_iter->second.title;
            hit.file_path = meta_iter->second.file_path;
        }

        // Sum BM25 scores for all query terms
        double total_score = 0.0;
        int total_matches = 0;
        for (const auto& term : query_tokens)
        {
            total_score += bm25_score(term, doc_id);
            auto term_iter = inverted_index_.find(term);
            if (term_iter != inverted_index_.end())
            {
                auto doc_iter = term_iter->second.doc_positions.find(doc_id);
                if (doc_iter != term_iter->second.doc_positions.end())
                {
                    total_matches += static_cast<int>(doc_iter->second.size());
                }
            }
        }

        hit.score = total_score;
        hit.match_count = total_matches;
        hit.snippet = extract_snippet(doc_id, query_tokens);

        hits.push_back(std::move(hit));
    }

    return hits;
}

auto SearchEngine::search_phrase(const SearchQuery& query) -> std::vector<SearchHit>
{
    auto query_tokens = tokenize(query.query_string);
    if (query_tokens.empty())
    {
        return {};
    }

    // First get docs containing ALL terms
    std::unordered_set<std::string> candidate_docs;
    bool first_term = true;

    for (const auto& term : query_tokens)
    {
        auto term_iter = inverted_index_.find(term);
        if (term_iter == inverted_index_.end())
        {
            return {};
        }

        if (first_term)
        {
            for (const auto& [doc_id, positions] : term_iter->second.doc_positions)
            {
                candidate_docs.insert(doc_id);
            }
            first_term = false;
        }
        else
        {
            std::unordered_set<std::string> intersection;
            for (const auto& [doc_id, positions] : term_iter->second.doc_positions)
            {
                if (candidate_docs.contains(doc_id))
                {
                    intersection.insert(doc_id);
                }
            }
            candidate_docs = std::move(intersection);
        }
    }

    // Check for consecutive positions (phrase matching)
    std::vector<SearchHit> hits;

    for (const auto& doc_id : candidate_docs)
    {
        // Get positions of the first term
        auto first_iter = inverted_index_.find(query_tokens[0]);
        if (first_iter == inverted_index_.end())
        {
            continue;
        }
        auto first_pos_iter = first_iter->second.doc_positions.find(doc_id);
        if (first_pos_iter == first_iter->second.doc_positions.end())
        {
            continue;
        }

        int phrase_matches = 0;

        for (int start_pos : first_pos_iter->second)
        {
            bool match = true;
            for (size_t term_idx = 1; term_idx < query_tokens.size(); ++term_idx)
            {
                auto next_iter = inverted_index_.find(query_tokens[term_idx]);
                if (next_iter == inverted_index_.end())
                {
                    match = false;
                    break;
                }
                auto next_doc_iter = next_iter->second.doc_positions.find(doc_id);
                if (next_doc_iter == next_iter->second.doc_positions.end())
                {
                    match = false;
                    break;
                }

                int expected_pos = start_pos + static_cast<int>(term_idx);
                const auto& positions = next_doc_iter->second;
                if (!std::binary_search(positions.begin(), positions.end(), expected_pos))
                {
                    match = false;
                    break;
                }
            }
            if (match)
            {
                ++phrase_matches;
            }
        }

        if (phrase_matches > 0)
        {
            SearchHit hit;
            hit.document_id = doc_id;

            auto meta_iter = doc_meta_.find(doc_id);
            if (meta_iter != doc_meta_.end())
            {
                hit.document_title = meta_iter->second.title;
                hit.file_path = meta_iter->second.file_path;
            }

            hit.match_count = phrase_matches;
            hit.score = static_cast<double>(phrase_matches);
            for (const auto& term : query_tokens)
            {
                hit.score += bm25_score(term, doc_id);
            }
            hit.snippet = extract_snippet(doc_id, query_tokens);

            hits.push_back(std::move(hit));
        }
    }

    return hits;
}

auto SearchEngine::search_regex(const SearchQuery& query) -> std::vector<SearchHit>
{
    std::regex pattern;
    try
    {
        auto flags = std::regex::ECMAScript;
        if (!query.case_sensitive)
        {
            flags |= std::regex::icase;
        }
        pattern = std::regex(query.query_string, flags);
    }
    catch (const std::regex_error&)
    {
        return {}; // Invalid regex
    }

    std::vector<SearchHit> hits;

    for (const auto& [doc_id, meta] : doc_meta_)
    {
        const auto& content = meta.content;
        auto words_begin = std::sregex_iterator(content.begin(), content.end(), pattern);
        auto words_end = std::sregex_iterator();

        int match_count = static_cast<int>(std::distance(words_begin, words_end));

        if (match_count > 0)
        {
            SearchHit hit;
            hit.document_id = doc_id;
            hit.document_title = meta.title;
            hit.file_path = meta.file_path;
            hit.match_count = match_count;
            hit.score = static_cast<double>(match_count);

            // Extract snippet around first match
            auto first_match = std::sregex_iterator(content.begin(), content.end(), pattern);
            if (first_match != words_end)
            {
                size_t match_pos = static_cast<size_t>(first_match->position());
                size_t snippet_start = (match_pos > 80) ? match_pos - 80 : 0;
                size_t snippet_end = std::min(content.size(), match_pos + 80);
                hit.snippet = content.substr(snippet_start, snippet_end - snippet_start);
            }

            hits.push_back(std::move(hit));
        }
    }

    return hits;
}

auto SearchEngine::search_fuzzy(const SearchQuery& query) -> std::vector<SearchHit>
{
    auto query_tokens = tokenize(query.query_string);
    if (query_tokens.empty())
    {
        return {};
    }

    // For each query term, find similar terms in the index
    std::unordered_set<std::string> expanded_terms;
    for (const auto& query_term : query_tokens)
    {
        expanded_terms.insert(query_term); // Always include exact match

        for (const auto& [index_term, posting] : inverted_index_)
        {
            // Only compare terms of similar length
            int len_diff =
                std::abs(static_cast<int>(index_term.size()) - static_cast<int>(query_term.size()));
            if (len_diff > 2)
            {
                continue;
            }

            int distance = levenshtein_distance(query_term, index_term);
            if (distance <= 2 && distance > 0)
            {
                expanded_terms.insert(index_term);
            }
        }
    }

    // Find documents containing any expanded term
    std::unordered_map<std::string, double> doc_scores;
    std::unordered_map<std::string, int> doc_match_counts;

    for (const auto& term : expanded_terms)
    {
        auto term_iter = inverted_index_.find(term);
        if (term_iter == inverted_index_.end())
        {
            continue;
        }

        for (const auto& [doc_id, positions] : term_iter->second.doc_positions)
        {
            double term_score = bm25_score(term, doc_id);

            // Apply penalty for fuzzy (non-exact) matches
            bool is_exact = false;
            for (const auto& qt : query_tokens)
            {
                if (qt == term)
                {
                    is_exact = true;
                    break;
                }
            }
            if (!is_exact)
            {
                term_score *= 0.6; // 40% penalty for fuzzy
            }

            doc_scores[doc_id] += term_score;
            doc_match_counts[doc_id] += static_cast<int>(positions.size());
        }
    }

    std::vector<SearchHit> hits;
    std::vector<std::string> expanded_vec(expanded_terms.begin(), expanded_terms.end());

    for (const auto& [doc_id, score] : doc_scores)
    {
        SearchHit hit;
        hit.document_id = doc_id;
        hit.score = score;
        hit.match_count = doc_match_counts[doc_id];

        auto meta_iter = doc_meta_.find(doc_id);
        if (meta_iter != doc_meta_.end())
        {
            hit.document_title = meta_iter->second.title;
            hit.file_path = meta_iter->second.file_path;
        }

        hit.snippet = extract_snippet(doc_id, expanded_vec);
        hits.push_back(std::move(hit));
    }

    return hits;
}

// ============================================================================
// Main search entry point
// ============================================================================
auto SearchEngine::search(const SearchQuery& query) -> SearchResult
{
    const auto start_time = std::chrono::steady_clock::now();
    const std::lock_guard index_lock(mutex_);

    SearchResult result;
    result.query = query;

    // Dispatch to appropriate search method
    std::vector<SearchHit> hits;
    switch (query.method)
    {
        case SearchMethod::Keyword:
            hits = search_keyword(query);
            break;
        case SearchMethod::Phrase:
            hits = search_phrase(query);
            break;
        case SearchMethod::Regex:
            hits = search_regex(query);
            break;
        case SearchMethod::Fuzzy:
            hits = search_fuzzy(query);
            break;
    }

    // Apply scope filters
    apply_scope(hits, query.scope);

    // Sort results
    switch (query.sort)
    {
        case SearchSortOrder::Relevance:
            std::sort(hits.begin(),
                      hits.end(),
                      [](const SearchHit& lhs, const SearchHit& rhs)
                      { return lhs.score > rhs.score; });
            break;
        case SearchSortOrder::ModifiedDesc:
        case SearchSortOrder::ModifiedAsc:
        {
            bool desc = (query.sort == SearchSortOrder::ModifiedDesc);
            std::sort(hits.begin(),
                      hits.end(),
                      [&](const SearchHit& lhs, const SearchHit& rhs)
                      {
                          auto lhs_it = doc_meta_.find(lhs.document_id);
                          auto rhs_it = doc_meta_.find(rhs.document_id);
                          int64_t lhs_time =
                              (lhs_it != doc_meta_.end()) ? lhs_it->second.modified_time : 0;
                          int64_t rhs_time =
                              (rhs_it != doc_meta_.end()) ? rhs_it->second.modified_time : 0;
                          return desc ? lhs_time > rhs_time : lhs_time < rhs_time;
                      });
            break;
        }
        case SearchSortOrder::CreatedDesc:
        case SearchSortOrder::CreatedAsc:
            // Fall through to relevance (created time not tracked separately)
            break;
        case SearchSortOrder::Alphabetical:
            std::sort(hits.begin(),
                      hits.end(),
                      [](const SearchHit& lhs, const SearchHit& rhs)
                      { return lhs.document_title < rhs.document_title; });
            break;
    }

    // Pagination
    result.total_count = static_cast<int>(hits.size());
    result.page = query.page;
    result.page_size = query.page_size;

    int start_index = (query.page - 1) * query.page_size;
    int end_index = std::min(start_index + query.page_size, static_cast<int>(hits.size()));

    if (start_index < static_cast<int>(hits.size()))
    {
        result.hits.assign(hits.begin() + start_index, hits.begin() + end_index);
    }

    const auto elapsed = std::chrono::steady_clock::now() - start_time;
    result.elapsed_ms = std::chrono::duration<double, std::milli>(elapsed).count();

    // Publish search completed event
    events::SearchCompletedEvent evt;
    evt.query_string = query.query_string;
    evt.result_count = result.total_count;
    evt.elapsed_ms = result.elapsed_ms;
    event_bus_.publish(evt);

    return result;
}

// ============================================================================
// Suggestions / autocomplete
// ============================================================================
auto SearchEngine::suggest(const std::string& prefix, int limit) const -> std::vector<std::string>
{
    const std::lock_guard index_lock(mutex_);

    auto normalized_prefix = normalize_token(prefix);
    if (normalized_prefix.empty())
    {
        return {};
    }

    std::vector<std::pair<std::string, int>> matches;

    for (const auto& [term, posting] : inverted_index_)
    {
        if (term.starts_with(normalized_prefix))
        {
            matches.emplace_back(term, posting.total_frequency);
        }
    }

    // Sort by frequency descending
    std::sort(matches.begin(),
              matches.end(),
              [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });

    std::vector<std::string> suggestions;
    for (int idx = 0; idx < std::min(limit, static_cast<int>(matches.size())); ++idx)
    {
        suggestions.push_back(matches[static_cast<size_t>(idx)].first);
    }

    return suggestions;
}

// ============================================================================
// Index statistics
// ============================================================================
auto SearchEngine::index_stats() const -> std::pair<int, int>
{
    const std::lock_guard index_lock(mutex_);
    return {total_documents_, static_cast<int>(inverted_index_.size())};
}

// ── Batch 19-22 improvements (#135-136) ──

auto SearchEngine::document_count() const -> int
{
    const std::lock_guard index_lock(mutex_);
    return total_documents_;
}

auto SearchEngine::term_count() const -> int
{
    const std::lock_guard index_lock(mutex_);
    return static_cast<int>(inverted_index_.size());
}

// (#171) Return the average document length used for BM25 scoring.
auto SearchEngine::avg_doc_length() const -> double
{
    const std::lock_guard index_lock(mutex_);
    return avg_doc_length_;
}

// (#172) Check if a specific document is present in the index.
auto SearchEngine::has_document(const std::string& document_id) const -> bool
{
    const std::lock_guard index_lock(mutex_);
    return doc_meta_.contains(document_id);
}

// (#173) Check if the search index contains any documents.
auto SearchEngine::is_indexed() const -> bool
{
    const std::lock_guard index_lock(mutex_);
    return total_documents_ > 0;
}

} // namespace markamp::core
