#include "SearchService.h"

#include "Config.h"
#include "Events.h"

#include <algorithm>
#include <chrono>
#include <regex>

namespace markamp::core
{

SearchService::SearchService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

auto SearchService::search(const SearchQuery& query) -> SearchResult
{
    auto start_time = std::chrono::steady_clock::now();

    SearchResult result;
    result.query = query;

    switch (query.method)
    {
        case SearchMethod::Keyword:
            result = search_keyword(query);
            break;
        case SearchMethod::Phrase:
            result = search_phrase(query);
            break;
        case SearchMethod::Regex:
            result = search_regex(query);
            break;
        case SearchMethod::Sql:
            // SQL mode is handled via QueryService (Phase 15), not here.
            result.query = query;
            break;
    }

    // Apply filters, sort, group, paginate.
    apply_filters(result, query);
    sort_results(result, query.order_by);

    if (query.group_by == SearchGroupBy::ByDocument)
    {
        group_by_document(result);
    }

    paginate_results(result, query.page, query.page_size);

    auto end_time = std::chrono::steady_clock::now();
    result.elapsed_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    // Publish completion event.
    events::SearchCompletedEvent evt;
    evt.query_string = query.query_string;
    evt.result_count = result.total_count;
    evt.elapsed_ms = result.elapsed_ms;
    event_bus_.publish(evt);

    return result;
}

auto SearchService::search_keyword(const SearchQuery& query) -> SearchResult
{
    SearchResult result;
    result.query = query;

    // Build FTS5 MATCH expression.
    auto fts_query = query_builder_.build_keyword_query(query.query_string);

    // In a full implementation, this would execute:
    // SELECT *, bm25(blocks_fts) AS rank FROM blocks_fts
    // WHERE blocks_fts MATCH ? ORDER BY rank
    // For now, return empty results (database integration pending).
    (void)fts_query;

    return result;
}

auto SearchService::search_phrase(const SearchQuery& query) -> SearchResult
{
    SearchResult result;
    result.query = query;

    auto fts_query = query_builder_.build_phrase_query(query.query_string);
    (void)fts_query;

    return result;
}

auto SearchService::search_regex(const SearchQuery& query) -> SearchResult
{
    SearchResult result;
    result.query = query;

    if (!query_builder_.validate_regex(query.query_string))
    {
        // Invalid regex, return empty.
        return result;
    }

    // In a full implementation, this would:
    // 1. Query all blocks from the database
    // 2. Apply std::regex matching client-side
    // 3. Build highlights from match positions

    return result;
}

auto SearchService::search_ref_blocks(const std::string& query) -> std::vector<SearchHit>
{
    // Search for blocks suitable as reference targets.
    SearchQuery search_query;
    search_query.query_string = query;
    search_query.method = SearchMethod::Keyword;
    search_query.page_size = 20;

    auto result = search(search_query);
    return result.hits;
}

auto SearchService::search_embed_blocks(const std::string& query) -> std::vector<SearchHit>
{
    // Similar to ref blocks but filter to embeddable types.
    SearchQuery search_query;
    search_query.query_string = query;
    search_query.method = SearchMethod::Keyword;
    search_query.types = {SearchBlockType::Paragraph,
                          SearchBlockType::Heading,
                          SearchBlockType::CodeBlock,
                          SearchBlockType::Table};
    search_query.page_size = 20;

    auto result = search(search_query);
    return result.hits;
}

auto SearchService::search_templates(const std::string& query) -> std::vector<std::string>
{
    // Placeholder: would scan template directory for matching filenames.
    (void)query;
    return {};
}

auto SearchService::search_assets(const std::string& query) -> std::vector<std::string>
{
    // Placeholder: would scan assets directory for matching filenames.
    (void)query;
    return {};
}

void SearchService::update_fts_index(const BlockId& block_id, std::string_view content)
{
    // In full implementation: INSERT OR REPLACE into FTS5 virtual table.
    (void)block_id;
    (void)content;

    events::SearchIndexUpdatedEvent evt;
    evt.block_id = block_id.value;
    event_bus_.publish(evt);
}

void SearchService::rebuild_fts_index()
{
    auto start_time = std::chrono::steady_clock::now();

    // In full implementation: DROP and recreate FTS5 table from blocks table.

    auto end_time = std::chrono::steady_clock::now();
    const double elapsed = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    events::SearchIndexRebuiltEvent evt;
    evt.total_blocks = 0;
    evt.elapsed_ms = elapsed;
    event_bus_.publish(evt);
}

auto SearchService::get_search_statistics() -> std::pair<int, int>
{
    // Placeholder: would query FTS5 stats.
    return {0, 0};
}

void SearchService::configure_fts()
{
    // In full implementation: CREATE VIRTUAL TABLE blocks_fts USING fts5(
    //   content, type, root_id, tokenize='unicode61 remove_diacritics 2'
    // );
}

void SearchService::apply_filters(SearchResult& result, const SearchQuery& query) const
{
    if (query.types.empty() && query.boxes.empty() && query.paths.empty())
    {
        return;
    }

    auto& hits = result.hits;
    hits.erase(std::remove_if(hits.begin(),
                              hits.end(),
                              [&query](const SearchHit& hit)
                              {
                                  // Type filter
                                  if (!query.types.empty() &&
                                      query.types.find(hit.block_type) == query.types.end())
                                  {
                                      return true;
                                  }

                                  // Box filter
                                  if (!query.boxes.empty())
                                  {
                                      bool in_box = false;
                                      for (const auto& box : query.boxes)
                                      {
                                          if (hit.root_id.find(box) != std::string::npos)
                                          {
                                              in_box = true;
                                              break;
                                          }
                                      }
                                      if (!in_box)
                                      {
                                          return true;
                                      }
                                  }

                                  // Path filter
                                  if (!query.paths.empty())
                                  {
                                      bool in_path = false;
                                      for (const auto& path : query.paths)
                                      {
                                          if (hit.block_path.substr(0, path.size()) == path)
                                          {
                                              in_path = true;
                                              break;
                                          }
                                      }
                                      if (!in_path)
                                      {
                                          return true;
                                      }
                                  }

                                  return false;
                              }),
               hits.end());

    result.total_count = static_cast<int>(hits.size());
}

void SearchService::sort_results(SearchResult& result, SearchSortOrder order) const
{
    auto& hits = result.hits;

    switch (order)
    {
        case SearchSortOrder::Relevance:
            std::sort(hits.begin(),
                      hits.end(),
                      [](const SearchHit& hit_a, const SearchHit& hit_b)
                      { return hit_a.score < hit_b.score; });
            break;
        case SearchSortOrder::CreatedAsc:
            std::sort(hits.begin(),
                      hits.end(),
                      [](const SearchHit& hit_a, const SearchHit& hit_b)
                      { return hit_a.created_at < hit_b.created_at; });
            break;
        case SearchSortOrder::CreatedDesc:
            std::sort(hits.begin(),
                      hits.end(),
                      [](const SearchHit& hit_a, const SearchHit& hit_b)
                      { return hit_a.created_at > hit_b.created_at; });
            break;
        case SearchSortOrder::UpdatedAsc:
            std::sort(hits.begin(),
                      hits.end(),
                      [](const SearchHit& hit_a, const SearchHit& hit_b)
                      { return hit_a.updated_at < hit_b.updated_at; });
            break;
        case SearchSortOrder::UpdatedDesc:
            std::sort(hits.begin(),
                      hits.end(),
                      [](const SearchHit& hit_a, const SearchHit& hit_b)
                      { return hit_a.updated_at > hit_b.updated_at; });
            break;
        default:
            // BlockType, SortAsc, SortDesc: no-op for now.
            break;
    }
}

void SearchService::paginate_results(SearchResult& result, int page, int page_size) const
{
    result.total_count = static_cast<int>(result.hits.size());
    result.page = page;
    result.page_size = page_size;

    const int start = (page - 1) * page_size;
    int end = start + page_size;

    if (start >= static_cast<int>(result.hits.size()))
    {
        result.hits.clear();
        return;
    }

    end = std::min(end, static_cast<int>(result.hits.size()));
    result.hits = std::vector<SearchHit>(result.hits.begin() + start, result.hits.begin() + end);
}

void SearchService::group_by_document(SearchResult& result) const
{
    std::unordered_map<std::string, SearchGroup> groups;

    for (auto& hit : result.hits)
    {
        auto& group = groups[hit.root_id];
        if (group.root_id.empty())
        {
            group.root_id = hit.root_id;
            group.doc_title = hit.doc_title;
            group.doc_path = hit.block_path;
        }
        group.hits.push_back(std::move(hit));
        group.total_hits++;
    }

    result.groups.clear();
    for (auto& [root_id, group] : groups)
    {
        result.groups.push_back(std::move(group));
    }

    // Sort groups by document title.
    std::sort(result.groups.begin(),
              result.groups.end(),
              [](const SearchGroup& grp_a, const SearchGroup& grp_b)
              { return grp_a.doc_title < grp_b.doc_title; });
}

} // namespace markamp::core
