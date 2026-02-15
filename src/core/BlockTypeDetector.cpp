// ============================================================================
// File: src/core/BlockTypeDetector.cpp
// Phase 29: Advanced Block Types — BlockTypeDetector implementation
// ============================================================================
#include "BlockTypeDetector.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>

namespace markamp::core
{

namespace
{

auto split_lines(std::string_view text) -> std::vector<std::string_view>
{
    std::vector<std::string_view> lines;
    std::size_t start = 0;

    while (start < text.size())
    {
        const auto newline_pos = text.find('\n', start);
        if (newline_pos == std::string_view::npos)
        {
            lines.push_back(text.substr(start));
            break;
        }
        lines.push_back(text.substr(start, newline_pos - start));
        start = newline_pos + 1;
    }

    return lines;
}

auto trim(std::string_view str_view) -> std::string_view
{
    while (!str_view.empty() && std::isspace(static_cast<unsigned char>(str_view.front())))
    {
        str_view.remove_prefix(1);
    }
    while (!str_view.empty() && std::isspace(static_cast<unsigned char>(str_view.back())))
    {
        str_view.remove_suffix(1);
    }
    return str_view;
}

} // anonymous namespace

auto BlockTypeDetector::detect(std::string_view markdown) const -> std::vector<BlockDetection>
{
    std::vector<BlockDetection> results;

    auto callouts = detect_callouts(markdown);
    auto embeds = detect_embeds(markdown);
    auto media = detect_media(markdown);
    auto iframes = detect_iframes(markdown);
    auto super_blocks = detect_super_blocks(markdown);

    results.insert(results.end(),
                   std::make_move_iterator(callouts.begin()),
                   std::make_move_iterator(callouts.end()));
    results.insert(results.end(),
                   std::make_move_iterator(embeds.begin()),
                   std::make_move_iterator(embeds.end()));
    results.insert(results.end(),
                   std::make_move_iterator(media.begin()),
                   std::make_move_iterator(media.end()));
    results.insert(results.end(),
                   std::make_move_iterator(iframes.begin()),
                   std::make_move_iterator(iframes.end()));
    results.insert(results.end(),
                   std::make_move_iterator(super_blocks.begin()),
                   std::make_move_iterator(super_blocks.end()));

    // Sort by start_line
    std::sort(results.begin(),
              results.end(),
              [](const BlockDetection& left, const BlockDetection& right)
              { return left.start_line < right.start_line; });

    return results;
}

auto BlockTypeDetector::detect_callouts(std::string_view markdown) const
    -> std::vector<BlockDetection>
{
    std::vector<BlockDetection> results;
    const auto lines = split_lines(markdown);

    for (int idx = 0; idx < static_cast<int>(lines.size()); ++idx)
    {
        auto line = trim(lines[idx]);

        // Check for > [!TYPE] pattern
        if (line.size() < 5 || line[0] != '>')
        {
            continue;
        }

        auto after_gt = trim(line.substr(1));
        if (after_gt.size() < 4 || after_gt[0] != '[' || after_gt[1] != '!')
        {
            continue;
        }

        const auto close_bracket = after_gt.find(']', 2);
        if (close_bracket == std::string_view::npos)
        {
            continue;
        }

        auto type_str = std::string(after_gt.substr(2, close_bracket - 2));
        std::transform(type_str.begin(),
                       type_str.end(),
                       type_str.begin(),
                       [](unsigned char chr) { return std::tolower(chr); });

        // Validate type
        if (type_str != "note" && type_str != "tip" && type_str != "important" &&
            type_str != "warning" && type_str != "caution")
        {
            continue;
        }

        // Collect blockquote lines
        int end_line = idx;
        std::ostringstream content_oss;
        for (int jdx = idx + 1; jdx < static_cast<int>(lines.size()); ++jdx)
        {
            auto bq_line = trim(lines[jdx]);
            if (bq_line.empty() || bq_line[0] != '>')
            {
                break;
            }
            content_oss << bq_line.substr(1) << "\n";
            end_line = jdx;
        }

        BlockDetection detection;
        detection.type = AdvancedBlockType::kCallout;
        detection.metadata = type_str;
        detection.content = content_oss.str();
        detection.start_line = idx;
        detection.end_line = end_line;
        results.push_back(std::move(detection));
    }

    return results;
}

auto BlockTypeDetector::detect_embeds(std::string_view markdown) const
    -> std::vector<BlockDetection>
{
    std::vector<BlockDetection> results;
    const auto lines = split_lines(markdown);

    // Pattern: ((alphanumeric-with-hyphens)) on its own line
    const std::regex embed_re(R"(^\s*\(\(([a-zA-Z0-9_-]+)\)\)\s*$)");

    for (int idx = 0; idx < static_cast<int>(lines.size()); ++idx)
    {
        std::string line_str(lines[idx]);
        std::smatch match;

        if (std::regex_match(line_str, match, embed_re))
        {
            BlockDetection detection;
            detection.type = AdvancedBlockType::kEmbed;
            detection.metadata = match[1].str();
            detection.start_line = idx;
            detection.end_line = idx;
            results.push_back(std::move(detection));
        }
    }

    return results;
}

auto BlockTypeDetector::detect_media(std::string_view markdown) const -> std::vector<BlockDetection>
{
    std::vector<BlockDetection> results;
    const auto lines = split_lines(markdown);

    // Pattern: !audio[alt](path) or !video[alt](path)
    const std::regex media_re(R"(^\s*!(audio|video)\[([^\]]*)\]\(([^)]+)\)\s*$)");

    for (int idx = 0; idx < static_cast<int>(lines.size()); ++idx)
    {
        std::string line_str(lines[idx]);
        std::smatch match;

        if (std::regex_match(line_str, match, media_re))
        {
            BlockDetection detection;
            detection.type =
                (match[1].str() == "audio") ? AdvancedBlockType::kAudio : AdvancedBlockType::kVideo;
            detection.metadata = match[3].str(); // file path
            detection.content = match[2].str();  // alt text
            detection.start_line = idx;
            detection.end_line = idx;
            results.push_back(std::move(detection));
        }
    }

    return results;
}

auto BlockTypeDetector::detect_iframes(std::string_view markdown) const
    -> std::vector<BlockDetection>
{
    std::vector<BlockDetection> results;
    const auto lines = split_lines(markdown);

    for (int idx = 0; idx < static_cast<int>(lines.size()); ++idx)
    {
        auto line = trim(lines[idx]);

        // Check for ```iframe
        if (!line.starts_with("```iframe"))
        {
            continue;
        }

        // Collect content until closing ```
        std::ostringstream content_oss;
        int end_line = idx;

        for (int jdx = idx + 1; jdx < static_cast<int>(lines.size()); ++jdx)
        {
            auto inner_line = trim(lines[jdx]);
            if (inner_line == "```")
            {
                end_line = jdx;
                break;
            }
            content_oss << lines[jdx] << "\n";
            end_line = jdx;
        }

        BlockDetection detection;
        detection.type = AdvancedBlockType::kIFrame;
        detection.content = content_oss.str();
        detection.metadata = std::string(line.substr(9)); // info after "```iframe"
        detection.start_line = idx;
        detection.end_line = end_line;
        results.push_back(std::move(detection));
    }

    return results;
}

auto BlockTypeDetector::detect_super_blocks(std::string_view markdown) const
    -> std::vector<BlockDetection>
{
    std::vector<BlockDetection> results;
    const auto lines = split_lines(markdown);

    for (int idx = 0; idx < static_cast<int>(lines.size()); ++idx)
    {
        auto line = trim(lines[idx]);

        // Check for {{{row or {{{col
        if (!line.starts_with("{{{"))
        {
            continue;
        }

        auto layout_marker = line.substr(3);

        // Collect content until closing }}}
        std::ostringstream content_oss;
        int end_line = idx;

        for (int jdx = idx + 1; jdx < static_cast<int>(lines.size()); ++jdx)
        {
            auto inner_line = trim(lines[jdx]);
            if (inner_line == "}}}")
            {
                end_line = jdx;
                break;
            }
            content_oss << lines[jdx] << "\n";
            end_line = jdx;
        }

        BlockDetection detection;
        detection.type = AdvancedBlockType::kSuperBlock;
        detection.content = content_oss.str();
        detection.metadata = std::string(layout_marker);
        detection.start_line = idx;
        detection.end_line = end_line;
        results.push_back(std::move(detection));
    }

    return results;
}

} // namespace markamp::core
