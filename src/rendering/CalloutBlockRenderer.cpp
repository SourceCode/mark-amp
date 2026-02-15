// ============================================================================
// File: src/rendering/CalloutBlockRenderer.cpp
// Phase 29: Advanced Block Types — Callout/alert block renderer implementation
// ============================================================================
#include "CalloutBlockRenderer.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::rendering
{

CalloutBlockRenderer::CalloutBlockRenderer()
{
    // Build case-insensitive lookup map
    type_map_["note"] = CalloutType::kNote;
    type_map_["tip"] = CalloutType::kTip;
    type_map_["important"] = CalloutType::kImportant;
    type_map_["warning"] = CalloutType::kWarning;
    type_map_["caution"] = CalloutType::kCaution;
}

auto CalloutBlockRenderer::detect_callout(std::string_view blockquote_content) const -> bool
{
    // Find the first non-whitespace line
    const auto first_newline = blockquote_content.find('\n');
    const auto first_line = blockquote_content.substr(0, first_newline);

    // Strip leading '>' and whitespace
    auto trimmed = first_line;
    if (!trimmed.empty() && trimmed[0] == '>')
    {
        trimmed.remove_prefix(1);
    }

    // Strip whitespace
    while (!trimmed.empty() && std::isspace(static_cast<unsigned char>(trimmed[0])))
    {
        trimmed.remove_prefix(1);
    }

    // Check for [!TYPE] pattern
    if (trimmed.size() < 4 || trimmed[0] != '[' || trimmed[1] != '!')
    {
        return false;
    }

    const auto close_bracket = trimmed.find(']', 2);
    if (close_bracket == std::string_view::npos)
    {
        return false;
    }

    auto type_str = std::string(trimmed.substr(2, close_bracket - 2));
    std::transform(type_str.begin(),
                   type_str.end(),
                   type_str.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });

    return type_map_.contains(type_str);
}

auto CalloutBlockRenderer::parse_callout_type(std::string_view first_line) const -> CalloutType
{
    // Strip leading '>' and whitespace
    auto trimmed = first_line;
    if (!trimmed.empty() && trimmed[0] == '>')
    {
        trimmed.remove_prefix(1);
    }

    while (!trimmed.empty() && std::isspace(static_cast<unsigned char>(trimmed[0])))
    {
        trimmed.remove_prefix(1);
    }

    if (trimmed.size() < 4 || trimmed[0] != '[' || trimmed[1] != '!')
    {
        return CalloutType::kNote;
    }

    const auto close_bracket = trimmed.find(']', 2);
    if (close_bracket == std::string_view::npos)
    {
        return CalloutType::kNote;
    }

    auto type_str = std::string(trimmed.substr(2, close_bracket - 2));
    std::transform(type_str.begin(),
                   type_str.end(),
                   type_str.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });

    const auto found = type_map_.find(type_str);
    if (found != type_map_.end())
    {
        return found->second;
    }

    return CalloutType::kNote;
}

auto CalloutBlockRenderer::render(std::string_view content, CalloutType type) const
    -> CalloutRenderResult
{
    CalloutRenderResult result;
    result.type = type;
    result.icon = std::string(type_icon(type));
    result.css_class = std::string(type_css_class(type));

    std::ostringstream oss;
    oss << "<div class=\"callout " << result.css_class << "\">\n"
        << "  <div class=\"callout-header\">\n"
        << "    <span class=\"callout-icon\">" << result.icon << "</span>\n"
        << "    <span class=\"callout-title\">" << type_name(type) << "</span>\n"
        << "  </div>\n"
        << "  <div class=\"callout-body\">\n"
        << "    " << content << "\n"
        << "  </div>\n"
        << "</div>\n";

    result.html = oss.str();
    return result;
}

auto CalloutBlockRenderer::type_name(CalloutType type) -> std::string_view
{
    switch (type)
    {
        case CalloutType::kNote:
            return "Note";
        case CalloutType::kTip:
            return "Tip";
        case CalloutType::kImportant:
            return "Important";
        case CalloutType::kWarning:
            return "Warning";
        case CalloutType::kCaution:
            return "Caution";
    }
    return "Note";
}

auto CalloutBlockRenderer::type_icon(CalloutType type) -> std::string_view
{
    switch (type)
    {
        case CalloutType::kNote:
            return "ℹ️";
        case CalloutType::kTip:
            return "💡";
        case CalloutType::kImportant:
            return "❗";
        case CalloutType::kWarning:
            return "⚠️";
        case CalloutType::kCaution:
            return "🔴";
    }
    return "ℹ️";
}

auto CalloutBlockRenderer::type_css_class(CalloutType type) -> std::string_view
{
    switch (type)
    {
        case CalloutType::kNote:
            return "callout-note";
        case CalloutType::kTip:
            return "callout-tip";
        case CalloutType::kImportant:
            return "callout-important";
        case CalloutType::kWarning:
            return "callout-warning";
        case CalloutType::kCaution:
            return "callout-caution";
    }
    return "callout-note";
}

} // namespace markamp::rendering
