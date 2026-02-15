/// @file EmbedResolver.cpp
/// @brief V4 Phase 10 – Note Embedding and Transclusion implementation.

#include "core/EmbedResolver.h"

#include "core/Events.h"
#include "core/VaultService.h"

#include <algorithm>
#include <regex>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// ResolvedEmbed helpers
// ============================================================================

auto ResolvedEmbed::is_text_embed() const -> bool
{
    return type == EmbedType::kFullDocument || type == EmbedType::kHeading ||
           type == EmbedType::kBlock;
}

auto ResolvedEmbed::is_media_embed() const -> bool
{
    return type == EmbedType::kImage || type == EmbedType::kPdf || type == EmbedType::kAudio ||
           type == EmbedType::kVideo;
}

// ============================================================================
// Constructor
// ============================================================================

EmbedResolver::EmbedResolver(EventBus& event_bus, VaultService& vault_service)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
{
}

// ============================================================================
// Public API
// ============================================================================

auto EmbedResolver::resolve(const WikiLink& link, int depth) const -> ResolvedEmbed
{
    if (depth >= kMaxEmbedDepth)
    {
        ResolvedEmbed result;
        result.source_link = link;
        result.error = "Maximum embed depth exceeded (" + std::to_string(kMaxEmbedDepth) + ")";
        result.depth = depth;
        return result;
    }

    auto embed_type = detect_type(link);

    switch (embed_type)
    {
        case EmbedType::kFullDocument:
            return resolve_document_embed(link, depth);
        case EmbedType::kHeading:
            return resolve_heading_embed(link, depth);
        case EmbedType::kBlock:
            return resolve_block_embed(link, depth);
        case EmbedType::kImage:
        case EmbedType::kPdf:
        case EmbedType::kAudio:
        case EmbedType::kVideo:
            return resolve_media_embed(link);
        default:
        {
            ResolvedEmbed result;
            result.source_link = link;
            result.type = EmbedType::kUnknown;
            result.error = "Unknown embed type for: " + link.target;
            return result;
        }
    }
}

auto EmbedResolver::resolve_all(const std::string& markdown,
                                const std::string& /*source_doc_id*/) const
    -> std::vector<ResolvedEmbed>
{
    std::vector<ResolvedEmbed> results;

    // Match ![[...]] patterns
    std::regex embed_regex(R"(!\[\[([^\]]+)\]\])");
    auto it = std::sregex_iterator(markdown.begin(), markdown.end(), embed_regex);
    auto end_it = std::sregex_iterator();

    for (; it != end_it; ++it)
    {
        WikiLink link;
        link.is_embed = true;

        const std::string match_content = (*it)[1].str();

        // Parse target#heading^block
        auto hash_pos = match_content.find('#');
        auto caret_pos = match_content.find('^');

        if (hash_pos != std::string::npos)
        {
            link.target = match_content.substr(0, hash_pos);
            if (caret_pos != std::string::npos && caret_pos > hash_pos)
            {
                link.heading = match_content.substr(hash_pos + 1, caret_pos - hash_pos - 1);
                link.block_ref = match_content.substr(caret_pos + 1);
            }
            else
            {
                link.heading = match_content.substr(hash_pos + 1);
            }
        }
        else if (caret_pos != std::string::npos)
        {
            link.target = match_content.substr(0, caret_pos);
            link.block_ref = match_content.substr(caret_pos + 1);
        }
        else
        {
            link.target = match_content;
        }

        results.push_back(resolve(link, 0));
    }

    return results;
}

auto EmbedResolver::expand_content(const std::string& markdown,
                                   const std::string& /*source_doc_id*/,
                                   int max_depth) const -> std::string
{
    if (max_depth <= 0)
    {
        return markdown;
    }

    std::string result = markdown;
    std::regex embed_regex(R"(!\[\[([^\]]+)\]\])");

    std::string output;
    auto it = std::sregex_iterator(result.begin(), result.end(), embed_regex);
    auto end_it = std::sregex_iterator();
    size_t last_pos = 0;

    for (; it != end_it; ++it)
    {
        // Append text before this match
        output.append(result, last_pos, static_cast<size_t>(it->position()) - last_pos);

        // Parse and resolve the embed
        WikiLink link;
        link.is_embed = true;
        const std::string match_content = (*it)[1].str();

        auto hash_pos = match_content.find('#');
        auto caret_pos = match_content.find('^');

        if (hash_pos != std::string::npos)
        {
            link.target = match_content.substr(0, hash_pos);
            link.heading = match_content.substr(hash_pos + 1);
        }
        else if (caret_pos != std::string::npos)
        {
            link.target = match_content.substr(0, caret_pos);
            link.block_ref = match_content.substr(caret_pos + 1);
        }
        else
        {
            link.target = match_content;
        }

        auto resolved = resolve(link, kMaxEmbedDepth - max_depth);

        if (resolved.is_resolved && resolved.is_text_embed())
        {
            output.append(resolved.content);
        }
        else if (resolved.is_resolved && resolved.is_media_embed())
        {
            output.append(resolved.html);
        }
        else
        {
            // Keep original unresolved embed
            output.append(it->str());
        }

        last_pos = static_cast<size_t>(it->position()) + static_cast<size_t>(it->length());
    }

    // Append remainder
    output.append(result, last_pos, result.size() - last_pos);

    return output;
}

auto EmbedResolver::render_embed_html(const ResolvedEmbed& embed) const -> std::string
{
    if (embed.is_text_embed())
    {
        return "<div class=\"transclusion\" data-source=\"" + embed.document_id +
               "\">"
               "<div class=\"transclusion-header\">"
               "<a href=\"" +
               embed.resolved_path + "\">" + embed.source_link.target +
               "</a>"
               "</div>"
               "<div class=\"transclusion-content\">" +
               embed.content +
               "</div>"
               "</div>";
    }

    // Media embeds already have HTML
    return embed.html;
}

auto EmbedResolver::detect_type(const WikiLink& link) -> EmbedType
{
    // Check for heading reference
    if (link.has_heading())
    {
        return EmbedType::kHeading;
    }

    // Check for block reference
    if (link.has_block_ref())
    {
        return EmbedType::kBlock;
    }

    // Check file extension for media types
    const auto& target = link.target;
    auto dot_pos = target.rfind('.');
    if (dot_pos != std::string::npos)
    {
        std::string ext = target.substr(dot_pos + 1);
        std::transform(ext.begin(),
                       ext.end(),
                       ext.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

        if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "gif" || ext == "svg" ||
            ext == "webp" || ext == "bmp")
        {
            return EmbedType::kImage;
        }
        if (ext == "pdf")
        {
            return EmbedType::kPdf;
        }
        if (ext == "mp3" || ext == "wav" || ext == "ogg" || ext == "flac" || ext == "m4a")
        {
            return EmbedType::kAudio;
        }
        if (ext == "mp4" || ext == "webm" || ext == "mkv" || ext == "mov")
        {
            return EmbedType::kVideo;
        }
    }

    return EmbedType::kFullDocument;
}

// ============================================================================
// Private Helpers
// ============================================================================

auto EmbedResolver::resolve_document_embed(const WikiLink& link, int depth) const -> ResolvedEmbed
{
    ResolvedEmbed result;
    result.source_link = link;
    result.type = EmbedType::kFullDocument;
    result.depth = depth;

    // Try to find the document by title/path
    const auto docs = vault_service_.list_documents();
    std::string found_doc_id;
    std::string found_path;

    for (const auto& doc : docs)
    {
        if (doc.title == link.target || doc.file_path.stem().string() == link.target)
        {
            found_doc_id = doc.document_id;
            found_path = doc.file_path.string();
            break;
        }
    }

    if (found_doc_id.empty())
    {
        result.error = "Document not found: " + link.target;
        return result;
    }

    // Open and read the document
    auto open_result = vault_service_.open_document(found_path);
    if (!open_result)
    {
        result.error = open_result.error();
        return result;
    }

    result.document_id = found_doc_id;
    result.resolved_path = found_path;
    result.content = strip_frontmatter(open_result.value()->markdown());
    result.is_resolved = true;

    return result;
}

auto EmbedResolver::resolve_heading_embed(const WikiLink& link, int depth) const -> ResolvedEmbed
{
    // First resolve the full document
    WikiLink doc_link = link;
    auto doc_result = resolve_document_embed(doc_link, depth);

    if (!doc_result.is_resolved)
    {
        return doc_result;
    }

    doc_result.type = EmbedType::kHeading;

    // Extract the heading section
    auto section = extract_heading_section(doc_result.content, link.heading);
    if (!section)
    {
        doc_result.is_resolved = false;
        doc_result.error = "Heading not found: " + link.heading;
        doc_result.content.clear();
        return doc_result;
    }

    doc_result.content = *section;
    return doc_result;
}

auto EmbedResolver::resolve_block_embed(const WikiLink& link, int depth) const -> ResolvedEmbed
{
    WikiLink doc_link = link;
    auto doc_result = resolve_document_embed(doc_link, depth);

    if (!doc_result.is_resolved)
    {
        return doc_result;
    }

    doc_result.type = EmbedType::kBlock;

    auto block = extract_block(doc_result.content, link.block_ref);
    if (!block)
    {
        doc_result.is_resolved = false;
        doc_result.error = "Block not found: ^" + link.block_ref;
        doc_result.content.clear();
        return doc_result;
    }

    doc_result.content = *block;
    return doc_result;
}

auto EmbedResolver::resolve_media_embed(const WikiLink& link) const -> ResolvedEmbed
{
    ResolvedEmbed result;
    result.source_link = link;
    result.type = detect_type(link);
    result.resolved_path = link.target;
    result.is_resolved = true;

    switch (result.type)
    {
        case EmbedType::kImage:
            result.html = "<img src=\"" + link.target + "\" alt=\"" + link.target + "\" />";
            break;
        case EmbedType::kPdf:
            result.html = "<iframe src=\"" + link.target + "\" class=\"embed-pdf\"></iframe>";
            break;
        case EmbedType::kAudio:
            result.html = "<audio controls src=\"" + link.target + "\"></audio>";
            break;
        case EmbedType::kVideo:
            result.html = "<video controls src=\"" + link.target + "\"></video>";
            break;
        default:
            result.is_resolved = false;
            result.error = "Unsupported media type";
            break;
    }

    return result;
}

auto EmbedResolver::extract_heading_section(const std::string& markdown,
                                            const std::string& heading) const
    -> std::optional<std::string>
{
    std::istringstream stream(markdown);
    std::string line;
    bool found = false;
    int target_level = 0;
    std::string section;

    while (std::getline(stream, line))
    {
        // Check if this line is a heading
        int level = 0;
        size_t pos = 0;
        while (pos < line.size() && line[pos] == '#')
        {
            ++level;
            ++pos;
        }

        if (level > 0 && pos < line.size() && line[pos] == ' ')
        {
            std::string heading_text = line.substr(pos + 1);
            // Trim trailing whitespace
            while (!heading_text.empty() &&
                   std::isspace(static_cast<unsigned char>(heading_text.back())))
            {
                heading_text.pop_back();
            }

            if (found)
            {
                // If we hit a heading of same or higher level, stop
                if (level <= target_level)
                {
                    break;
                }
            }
            else
            {
                // Case-insensitive heading comparison
                std::string lower_heading = heading_text;
                std::string lower_target = heading;
                std::transform(lower_heading.begin(),
                               lower_heading.end(),
                               lower_heading.begin(),
                               [](unsigned char chr)
                               { return static_cast<char>(std::tolower(chr)); });
                std::transform(lower_target.begin(),
                               lower_target.end(),
                               lower_target.begin(),
                               [](unsigned char chr)
                               { return static_cast<char>(std::tolower(chr)); });

                if (lower_heading == lower_target)
                {
                    found = true;
                    target_level = level;
                    section = line + "\n";
                    continue;
                }
            }
        }

        if (found)
        {
            section += line + "\n";
        }
    }

    if (!found)
    {
        return std::nullopt;
    }

    return section;
}

auto EmbedResolver::extract_block(const std::string& markdown, const std::string& block_id) const
    -> std::optional<std::string>
{
    const std::string marker = "^" + block_id;
    std::istringstream stream(markdown);
    std::string line;
    std::string paragraph;
    bool found = false;

    while (std::getline(stream, line))
    {
        if (line.empty())
        {
            if (found)
            {
                break;
            }
            paragraph.clear();
            continue;
        }

        paragraph += line + "\n";

        if (line.find(marker) != std::string::npos)
        {
            found = true;
        }
    }

    if (!found)
    {
        return std::nullopt;
    }

    return paragraph;
}

auto EmbedResolver::strip_frontmatter(const std::string& markdown) const -> std::string
{
    if (markdown.size() < 3 || markdown.substr(0, 3) != "---")
    {
        return markdown;
    }

    auto end_pos = markdown.find("---", 3);
    if (end_pos == std::string::npos)
    {
        return markdown;
    }

    // Skip past the closing --- and any immediately following newline
    auto content_start = end_pos + 3;
    if (content_start < markdown.size() && markdown[content_start] == '\n')
    {
        ++content_start;
    }

    return markdown.substr(content_start);
}

} // namespace markamp::core
