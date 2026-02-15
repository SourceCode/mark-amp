/// @file FindReplaceService.cpp
/// @brief V4 Phase 16 – Regex Find and Replace service implementation.

#include "core/FindReplaceService.h"

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/VaultService.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

FindReplaceService::FindReplaceService(EventBus& event_bus, VaultService& vault_service)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
{
}

// ============================================================================
// Compile regex from options
// ============================================================================

auto FindReplaceService::compile_regex(const FindReplaceOptions& options) const
    -> std::expected<std::regex, std::string>
{
    try
    {
        auto flags = std::regex::ECMAScript;
        if (!options.case_sensitive)
        {
            flags |= std::regex::icase;
        }
        if (options.multiline)
        {
            flags |= std::regex::multiline;
        }

        std::string pattern = options.pattern;
        if (options.whole_word && !options.use_regex)
        {
            pattern = "\\b" + pattern + "\\b";
        }

        return std::regex(pattern, flags);
    }
    catch (const std::regex_error& err)
    {
        return std::unexpected(std::string("Invalid regex: ") + err.what());
    }
}

// ============================================================================
// Validate pattern
// ============================================================================

auto FindReplaceService::validate_pattern(const std::string& pattern) const
    -> std::optional<std::string>
{
    try
    {
        std::regex test_re(pattern, std::regex::ECMAScript);
        return std::nullopt; // Valid
    }
    catch (const std::regex_error& err)
    {
        return std::string(err.what());
    }
}

// ============================================================================
// Expand replacement with capture groups ($0-$9)
// ============================================================================

auto FindReplaceService::expand_replacement(const std::string& matched,
                                            const std::smatch& captures,
                                            const std::string& replacement) const -> std::string
{
    std::string result;
    result.reserve(replacement.size());

    for (size_t idx = 0; idx < replacement.size(); ++idx)
    {
        if (replacement[idx] == '$' && idx + 1 < replacement.size())
        {
            char next = replacement[idx + 1];
            if (next >= '0' && next <= '9')
            {
                auto group = static_cast<size_t>(next - '0');
                if (group == 0)
                {
                    result += matched;
                }
                else if (group < captures.size())
                {
                    result += captures[group].str();
                }
                ++idx; // Skip digit
                continue;
            }
        }
        if (replacement[idx] == '\\' && idx + 1 < replacement.size())
        {
            char next = replacement[idx + 1];
            if (next == 'n')
            {
                result += '\n';
                ++idx;
                continue;
            }
            if (next == 't')
            {
                result += '\t';
                ++idx;
                continue;
            }
            if (next == '\\')
            {
                result += '\\';
                ++idx;
                continue;
            }
        }
        result += replacement[idx];
    }

    return result;
}

// ============================================================================
// Search a single file for matches (literal mode)
// ============================================================================

auto FindReplaceService::find_literal(const std::string& content,
                                      const std::string& document_id,
                                      const std::string& file_path,
                                      const FindReplaceOptions& options) const
    -> std::vector<FindMatch>
{
    std::vector<FindMatch> matches;

    std::string search_content = content;
    std::string search_pattern = options.pattern;

    if (!options.case_sensitive)
    {
        std::transform(
            search_content.begin(), search_content.end(), search_content.begin(), ::tolower);
        std::transform(
            search_pattern.begin(), search_pattern.end(), search_pattern.begin(), ::tolower);
    }

    // Split content into lines
    std::istringstream stream(content);
    std::string line;
    int line_num = 0;

    std::istringstream search_stream(search_content);
    std::string search_line;

    while (std::getline(stream, line) && std::getline(search_stream, search_line))
    {
        size_t pos = 0;
        while ((pos = search_line.find(search_pattern, pos)) != std::string::npos)
        {
            // Whole word check
            if (options.whole_word)
            {
                bool word_start =
                    (pos == 0) || !std::isalnum(static_cast<unsigned char>(search_line[pos - 1]));
                bool word_end = (pos + search_pattern.size() >= search_line.size()) ||
                                !std::isalnum(static_cast<unsigned char>(
                                    search_line[pos + search_pattern.size()]));
                if (!word_start || !word_end)
                {
                    ++pos;
                    continue;
                }
            }

            FindMatch match;
            match.document_id = document_id;
            match.file_path = file_path;
            match.line_number = line_num;
            match.column = static_cast<int>(pos);
            match.match_length = static_cast<int>(options.pattern.size());
            match.matched_text = line.substr(pos, options.pattern.size());
            match.context_before = line.substr(0, pos);
            match.context_after = line.substr(pos + options.pattern.size());
            match.full_line = line;
            matches.push_back(std::move(match));

            pos += options.pattern.size();
        }
        ++line_num;
    }

    return matches;
}

// ============================================================================
// Search a single file for matches
// ============================================================================

auto FindReplaceService::search_file(const std::string& content,
                                     const std::string& document_id,
                                     const std::string& file_path,
                                     const FindReplaceOptions& options) const
    -> std::vector<FindMatch>
{
    if (!options.use_regex)
    {
        return find_literal(content, document_id, file_path, options);
    }

    // Regex mode
    auto regex_result = compile_regex(options);
    if (!regex_result.has_value())
    {
        return {};
    }

    const auto& compiled = *regex_result;
    std::vector<FindMatch> matches;

    std::istringstream stream(content);
    std::string line;
    int line_num = 0;

    while (std::getline(stream, line))
    {
        std::sregex_iterator iter(line.begin(), line.end(), compiled);
        const std::sregex_iterator end_iter;

        for (; iter != end_iter; ++iter)
        {
            const auto& smatch = *iter;
            FindMatch match;
            match.document_id = document_id;
            match.file_path = file_path;
            match.line_number = line_num;
            match.column = static_cast<int>(smatch.position());
            match.match_length = static_cast<int>(smatch.length());
            match.matched_text = smatch.str();

            auto match_start = static_cast<size_t>(smatch.position());
            match.context_before = line.substr(0, match_start);
            match.context_after = line.substr(match_start + static_cast<size_t>(smatch.length()));
            match.full_line = line;

            matches.push_back(std::move(match));
        }
        ++line_num;
    }

    return matches;
}

// ============================================================================
// Path matching for include/exclude filters
// ============================================================================

auto FindReplaceService::path_matches_filter(const std::string& path,
                                             const std::vector<std::string>& include,
                                             const std::vector<std::string>& exclude) -> bool
{
    // Exclude check
    for (const auto& excl : exclude)
    {
        if (path.find(excl) != std::string::npos)
        {
            return false;
        }
    }

    // Include check (empty includes = include all)
    if (include.empty())
    {
        return true;
    }

    for (const auto& incl : include)
    {
        if (path.find(incl) != std::string::npos)
        {
            return true;
        }
    }
    return false;
}

// ============================================================================
// Find all matches
// ============================================================================

auto FindReplaceService::find_all(const FindReplaceOptions& options) -> FindReplaceResult
{
    auto start_time = std::chrono::steady_clock::now();
    FindReplaceResult result;

    // Get files from vault
    auto all_docs = vault_service_.list_documents();

    for (const auto& doc : all_docs)
    {
        const auto doc_path = doc.file_path.string();

        if (!path_matches_filter(doc_path, options.include_paths, options.exclude_paths))
        {
            continue;
        }

        // Open the document to read content
        auto doc_result = vault_service_.open_document(doc.file_path);
        if (!doc_result.has_value())
        {
            continue;
        }

        const auto& doc_model = *doc_result;
        const std::string& content = doc_model->markdown();

        ++result.files_searched;

        auto file_matches = search_file(content, doc.document_id, doc_path, options);
        if (!file_matches.empty())
        {
            ++result.files_with_matches;
            result.total_matches += static_cast<int>(file_matches.size());
            result.matches.insert(result.matches.end(),
                                  std::make_move_iterator(file_matches.begin()),
                                  std::make_move_iterator(file_matches.end()));
        }
    }

    auto end_time = std::chrono::steady_clock::now();
    result.elapsed_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    // Publish event
    events::FindCompletedEvent evt;
    evt.match_count = result.total_matches;
    evt.files_with_matches = result.files_with_matches;
    evt.elapsed_ms = result.elapsed_ms;
    event_bus_.publish(evt);

    return result;
}

// ============================================================================
// Preview replace
// ============================================================================

auto FindReplaceService::preview_replace(const FindReplaceOptions& options)
    -> std::vector<ReplacePreview>
{
    auto found = find_all(options);
    std::vector<ReplacePreview> previews;
    previews.reserve(found.matches.size());

    std::optional<std::regex> compiled;
    if (options.use_regex)
    {
        auto regex_result = compile_regex(options);
        if (regex_result.has_value())
        {
            compiled = std::move(*regex_result);
        }
    }

    for (const auto& match : found.matches)
    {
        ReplacePreview preview;
        preview.match = match;

        if (options.use_regex && compiled.has_value())
        {
            std::smatch captures;
            std::regex_search(match.matched_text, captures, *compiled);
            preview.replacement_text =
                expand_replacement(match.matched_text, captures, options.replacement);
        }
        else
        {
            preview.replacement_text = options.replacement;
        }

        // Build preview line
        preview.preview_line =
            match.context_before + preview.replacement_text + match.context_after;
        previews.push_back(std::move(preview));
    }

    return previews;
}

// ============================================================================
// Replace all
// ============================================================================

auto FindReplaceService::replace_all(const FindReplaceOptions& options)
    -> std::expected<int, std::string>
{
    auto found = find_all(options);
    if (found.matches.empty())
    {
        return 0;
    }

    // Group matches by document_id
    std::unordered_map<std::string, std::vector<const FindMatch*>> by_document;
    for (const auto& match : found.matches)
    {
        by_document[match.document_id].push_back(&match);
    }

    std::optional<std::regex> compiled;
    if (options.use_regex)
    {
        auto regex_result = compile_regex(options);
        if (!regex_result.has_value())
        {
            return std::unexpected(regex_result.error());
        }
        compiled = std::move(*regex_result);
    }

    int total_replacements = 0;
    int files_modified = 0;

    for (auto& [doc_id, doc_matches] : by_document)
    {
        // Find the file path from first match
        if (doc_matches.empty())
        {
            continue;
        }
        std::filesystem::path file_path(doc_matches[0]->file_path);

        auto doc_result = vault_service_.open_document(file_path);
        if (!doc_result.has_value())
        {
            continue;
        }

        const auto& doc_model = *doc_result;
        const std::string& content = doc_model->markdown();

        // Split into lines
        std::vector<std::string> lines;
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line))
        {
            lines.push_back(line);
        }

        // Sort matches by line (desc) then column (desc) for safe replacement
        std::sort(doc_matches.begin(),
                  doc_matches.end(),
                  [](const FindMatch* lhs, const FindMatch* rhs)
                  {
                      if (lhs->line_number != rhs->line_number)
                      {
                          return lhs->line_number > rhs->line_number;
                      }
                      return lhs->column > rhs->column;
                  });

        for (const auto* match_ptr : doc_matches)
        {
            auto line_idx = static_cast<size_t>(match_ptr->line_number);
            if (line_idx >= lines.size())
            {
                continue;
            }

            std::string replacement_text;
            if (options.use_regex && compiled.has_value())
            {
                std::smatch captures;
                std::regex_search(match_ptr->matched_text, captures, *compiled);
                replacement_text =
                    expand_replacement(match_ptr->matched_text, captures, options.replacement);
            }
            else
            {
                replacement_text = options.replacement;
            }

            auto col = static_cast<size_t>(match_ptr->column);
            auto len = static_cast<size_t>(match_ptr->match_length);
            lines[line_idx].replace(col, len, replacement_text);
            ++total_replacements;
        }

        // Rejoin lines and update document
        std::string new_content;
        for (size_t line_idx = 0; line_idx < lines.size(); ++line_idx)
        {
            if (line_idx > 0)
            {
                new_content += '\n';
            }
            new_content += lines[line_idx];
        }

        doc_model->set_markdown(new_content);
        auto save_result = vault_service_.save_document(doc_id);
        static_cast<void>(save_result); // fire-and-forget in this context
        ++files_modified;
    }

    // Publish event
    events::ReplaceCompletedEvent evt;
    evt.replacements = total_replacements;
    evt.files_modified = files_modified;
    event_bus_.publish(evt);

    return total_replacements;
}

// ============================================================================
// Replace single match
// ============================================================================

auto FindReplaceService::replace_single(const FindMatch& match, const FindReplaceOptions& options)
    -> std::expected<std::string, std::string>
{
    std::string replacement_text;

    if (options.use_regex)
    {
        auto regex_result = compile_regex(options);
        if (!regex_result.has_value())
        {
            return std::unexpected(regex_result.error());
        }

        std::smatch captures;
        std::regex_search(match.matched_text, captures, *regex_result);
        replacement_text = expand_replacement(match.matched_text, captures, options.replacement);
    }
    else
    {
        replacement_text = options.replacement;
    }

    // Build the new line
    std::string new_line = match.context_before + replacement_text + match.context_after;
    return new_line;
}

} // namespace markamp::core
