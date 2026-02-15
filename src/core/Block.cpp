#include "core/Block.h"

#include <algorithm>
#include <sstream>
#include <vector>

namespace markamp::core
{

auto Block::is_document() const -> bool
{
    return type == BlockType::Document;
}

auto Block::is_heading() const -> bool
{
    return type == BlockType::Heading;
}

auto Block::is_container() const -> bool
{
    switch (type)
    {
        case BlockType::Document:
        case BlockType::BlockQuote:
        case BlockType::List:
        case BlockType::ListItem:
        case BlockType::SuperBlock:
        case BlockType::Table:
        case BlockType::TableRow:
        case BlockType::CalloutBlock:
            return true;
        default:
            return false;
    }
}

auto Block::is_leaf() const -> bool
{
    return !is_container();
}

auto Block::type_abbr() const -> std::string
{
    switch (type)
    {
        case BlockType::Document:
            return "d";
        case BlockType::Paragraph:
            return "p";
        case BlockType::Heading:
            return "h";
        case BlockType::BlockQuote:
            return "b";
        case BlockType::List:
            return "l";
        case BlockType::ListItem:
            return "i";
        case BlockType::CodeBlock:
            return "c";
        case BlockType::MathBlock:
            return "m";
        case BlockType::Table:
            return "t";
        case BlockType::TableRow:
            return "tr";
        case BlockType::TableCell:
            return "td";
        case BlockType::HTMLBlock:
            return "html";
        case BlockType::ThematicBreak:
            return "tb";
        case BlockType::Audio:
            return "audio";
        case BlockType::Video:
            return "video";
        case BlockType::IFrame:
            return "iframe";
        case BlockType::Widget:
            return "widget";
        case BlockType::SuperBlock:
            return "s";
        case BlockType::EmbedBlock:
            return "e";
        case BlockType::AttributeView:
            return "av";
        case BlockType::CalloutBlock:
            return "callout";
        case BlockType::MermaidBlock:
            return "mermaid";
        case BlockType::PlantUMLBlock:
            return "plantuml";
        case BlockType::GraphvizBlock:
            return "graphviz";
        case BlockType::FlowchartBlock:
            return "flowchart";
        case BlockType::MindmapBlock:
            return "mindmap";
        case BlockType::EChartsBlock:
            return "echarts";
        case BlockType::ABCBlock:
            return "abc";
        case BlockType::Unknown:
            return "unknown";
    }
    return "unknown";
}

auto Block::sub_type_abbr() const -> std::string
{
    switch (sub_type)
    {
        case BlockSubType::None:
            return "";
        case BlockSubType::Heading1:
            return "h1";
        case BlockSubType::Heading2:
            return "h2";
        case BlockSubType::Heading3:
            return "h3";
        case BlockSubType::Heading4:
            return "h4";
        case BlockSubType::Heading5:
            return "h5";
        case BlockSubType::Heading6:
            return "h6";
        case BlockSubType::OrderedList:
            return "o";
        case BlockSubType::UnorderedList:
            return "u";
        case BlockSubType::TaskList:
            return "t";
        case BlockSubType::Math:
            return "m";
        case BlockSubType::Code:
            return "c";
    }
    return "";
}

auto Block::ial_string() const -> std::string
{
    if (ial.empty())
    {
        return "";
    }

    // Sort keys alphabetically for deterministic output
    std::vector<std::string> keys;
    keys.reserve(ial.size());
    for (const auto& [key, _] : ial)
    {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());

    std::ostringstream oss;
    oss << "{:";
    for (const auto& key : keys)
    {
        const auto& value = ial.at(key);
        oss << " " << key << "=\"";
        // Escape double quotes in values
        for (char ch : value)
        {
            if (ch == '"')
            {
                oss << "\\\"";
            }
            else
            {
                oss << ch;
            }
        }
        oss << "\"";
    }
    oss << " }";
    return oss.str();
}

auto Block::set_ial(const std::string& key, const std::string& value) -> void
{
    ial[key] = value;
}

auto Block::get_ial(const std::string& key) const -> std::optional<std::string>
{
    auto it = ial.find(key);
    if (it != ial.end())
    {
        return it->second;
    }
    return std::nullopt;
}

auto Block::remove_ial(const std::string& key) -> void
{
    ial.erase(key);
}

auto Block::set_ial_from_string(const std::string& ial_str) -> void
{
    ial.clear();
    if (ial_str.size() < 4) // minimum: "{: }"
    {
        return;
    }

    // Parse format: {: key="value" key2="value2" }
    std::size_t pos = 0;

    // Skip leading "{:" prefix
    pos = ial_str.find("{:", pos);
    if (pos == std::string::npos)
    {
        return;
    }
    pos += 2; // Skip "{:"

    while (pos < ial_str.size())
    {
        // Skip whitespace
        while (pos < ial_str.size() && (ial_str[pos] == ' ' || ial_str[pos] == '\t'))
        {
            ++pos;
        }

        // Check for end marker
        if (pos >= ial_str.size() || ial_str[pos] == '}')
        {
            break;
        }

        // Read key
        auto eq_pos = ial_str.find('=', pos);
        if (eq_pos == std::string::npos)
        {
            break;
        }
        std::string key = ial_str.substr(pos, eq_pos - pos);

        // Skip '="'
        pos = eq_pos + 1;
        if (pos >= ial_str.size() || ial_str[pos] != '"')
        {
            break;
        }
        ++pos; // skip opening quote

        // Read value, handling escaped quotes
        std::string value;
        while (pos < ial_str.size() && ial_str[pos] != '"')
        {
            if (ial_str[pos] == '\\' && pos + 1 < ial_str.size() && ial_str[pos + 1] == '"')
            {
                value += '"';
                pos += 2;
            }
            else
            {
                value += ial_str[pos];
                ++pos;
            }
        }

        if (pos < ial_str.size())
        {
            ++pos; // skip closing quote
        }

        if (!key.empty())
        {
            ial[key] = value;
        }
    }
}

auto compare_blocks_by_sort(const Block& a, const Block& b) -> bool
{
    if (a.sort != b.sort)
    {
        return a.sort < b.sort;
    }
    return a.id < b.id;
}

auto compare_blocks_by_updated(const Block& a, const Block& b) -> bool
{
    if (a.updated != b.updated)
    {
        return a.updated > b.updated;
    }
    return a.id < b.id;
}

} // namespace markamp::core
