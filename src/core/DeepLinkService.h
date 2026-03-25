/// @file DeepLinkService.h
/// @brief V9 Phase 17 Tasks 11–12 — `markamp://` URI deep link scheme.
///
/// Header-only. Encodes and decodes precise cross-surface deep links using
/// a `markamp://surface/path#heading^block?line=N` URI format.

#pragma once

#include "SurfaceLink.h"

#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// DeepLinkUri — parsed deep link URI
// ============================================================================

/// Parsed components of a `markamp://` URI.
struct DeepLinkUri
{
    std::string scheme{"markamp"};
    std::string surface;   ///< e.g. "editor", "graph"
    std::string document;  ///< Document path
    std::string heading;   ///< Optional heading fragment
    std::string block_ref; ///< Optional ^block-id
    int line{-1};          ///< Optional line number (-1 = unset)
    std::unordered_map<std::string, std::string> query_params;

    /// Whether this URI has valid minimum content.
    [[nodiscard]] auto is_valid() const -> bool
    {
        return !scheme.empty() && !surface.empty();
    }
};

/// A logged deep link entry.
struct DeepLinkEntry
{
    std::string uri;
    std::string label;
    std::string created_at;
};

// ============================================================================
// DeepLinkService
// ============================================================================

/// Generates and resolves `markamp://` URI deep links for cross-surface
/// navigation.
///
/// URI format: `markamp://surface/path/to/doc#heading^block?line=N&obj=id`
///
/// Usage:
/// ```cpp
/// DeepLinkService service(event_bus);
/// auto uri = service.encode(anchor);
/// auto anchor = service.decode("markamp://editor/notes/daily.md#intro");
/// ```
class DeepLinkService
{
public:
    explicit DeepLinkService(EventBus& event_bus)
        : event_bus_(event_bus)
    {
    }

    /// Encode a LinkAnchor into a `markamp://` URI string.
    [[nodiscard]] auto encode(const LinkAnchor& anchor) const -> std::string
    {
        std::ostringstream oss;
        oss << "markamp://";

        // Surface
        oss << surface_kind_to_string(anchor.surface_kind);

        // Document path
        if (!anchor.file_path.empty())
        {
            oss << "/" << url_encode_path(anchor.file_path);
        }

        // Fragment (heading)
        if (!anchor.heading_id.empty())
        {
            oss << "#" << url_encode(anchor.heading_id);
        }

        // Query params
        bool has_query = false;
        if (anchor.line >= 0)
        {
            oss << (has_query ? "&" : "?") << "line=" << anchor.line;
        }

        return oss.str();
    }

    /// Decode a `markamp://` URI string into a LinkAnchor.
    [[nodiscard]] auto decode(const std::string& uri) const -> std::optional<LinkAnchor>
    {
        auto parsed = parse_uri(uri);
        if (!parsed.is_valid())
        {
            return std::nullopt;
        }

        LinkAnchor anchor;
        anchor.surface_kind = string_to_surface_kind(parsed.surface);
        anchor.file_path = parsed.document;

        if (!parsed.heading.empty())
        {
            anchor.heading_id = parsed.heading;
        }
        if (parsed.line >= 0)
        {
            anchor.line = parsed.line;
        }

        return anchor;
    }

    /// Generate a shareable deep link for clipboard.
    [[nodiscard]] auto generate_shareable_link(const LinkAnchor& anchor) -> std::string
    {
        auto uri = encode(anchor);

        // Record in history
        DeepLinkEntry entry;
        entry.uri = uri;
        entry.label = build_label(anchor);
        entry.created_at = []()
        {
            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);
            struct tm time_info{};
            gmtime_r(&time_t_now, &time_info);
            char buf[32]{};
            std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &time_info);
            return std::string(buf);
        }();
        history_.push_back(std::move(entry));

        // Cap history
        constexpr std::size_t kMaxHistory = 100;
        if (history_.size() > kMaxHistory)
        {
            history_.erase(history_.begin());
        }

        return uri;
    }

    /// Resolve a deep link URI back through the SurfaceLinkRouter.
    [[nodiscard]] auto resolve_deep_link(const std::string& uri) const -> std::optional<LinkAnchor>
    {
        return decode(uri);
    }

    /// Get recent deep link history.
    [[nodiscard]] auto link_history(int limit) const -> std::vector<DeepLinkEntry>
    {
        if (limit <= 0 || history_.empty())
        {
            return {};
        }

        auto start_idx = static_cast<int>(history_.size()) - limit;
        if (start_idx < 0)
        {
            start_idx = 0;
        }

        return {history_.begin() + start_idx, history_.end()};
    }

    /// Parse a URI string into its components.
    [[nodiscard]] static auto parse_uri(const std::string& uri) -> DeepLinkUri
    {
        DeepLinkUri result;

        // Check scheme
        const std::string prefix = "markamp://";
        if (uri.size() < prefix.size() || uri.substr(0, prefix.size()) != prefix)
        {
            return result; // Invalid
        }

        auto remainder = uri.substr(prefix.size());

        // Extract query string
        auto query_pos = remainder.find('?');
        std::string query_str;
        if (query_pos != std::string::npos)
        {
            query_str = remainder.substr(query_pos + 1);
            remainder = remainder.substr(0, query_pos);
        }

        // Extract fragment
        auto hash_pos = remainder.find('#');
        std::string fragment;
        if (hash_pos != std::string::npos)
        {
            fragment = remainder.substr(hash_pos + 1);
            remainder = remainder.substr(0, hash_pos);
        }

        // Parse surface (first path segment)
        auto first_slash = remainder.find('/');
        if (first_slash == std::string::npos)
        {
            result.surface = remainder;
        }
        else
        {
            result.surface = remainder.substr(0, first_slash);
            result.document = url_decode(remainder.substr(first_slash + 1));
        }

        // Parse fragment (heading and/or block ref)
        if (!fragment.empty())
        {
            auto caret_pos = fragment.find('^');
            if (caret_pos != std::string::npos)
            {
                result.heading = url_decode(fragment.substr(0, caret_pos));
                result.block_ref = fragment.substr(caret_pos + 1);
            }
            else
            {
                result.heading = url_decode(fragment);
            }
        }

        // Parse query params
        if (!query_str.empty())
        {
            parse_query_params(query_str, result);
        }

        return result;
    }

private:
    EventBus& event_bus_;
    std::vector<DeepLinkEntry> history_;

    /// Convert SurfaceKind to string label.
    [[nodiscard]] static auto surface_kind_to_string(SurfaceKind kind) -> std::string
    {
        switch (kind)
        {
            case SurfaceKind::kEditor:
                return "editor";
            case SurfaceKind::kPreview:
                return "preview";
            case SurfaceKind::kGraph:
                return "graph";
            default:
                return "editor";
        }
    }

    /// Convert string to SurfaceKind.
    [[nodiscard]] static auto string_to_surface_kind(const std::string& str) -> SurfaceKind
    {
        if (str == "editor")
        {
            return SurfaceKind::kEditor;
        }
        if (str == "preview")
        {
            return SurfaceKind::kPreview;
        }
        if (str == "graph")
        {
            return SurfaceKind::kGraph;
        }
        return SurfaceKind::kEditor;
    }

    /// URL-encode a string (basic: spaces and special chars).
    [[nodiscard]] static auto url_encode(const std::string& str) -> std::string
    {
        std::string result;
        result.reserve(str.size());
        for (const char ch : str)
        {
            if (std::isalnum(static_cast<unsigned char>(ch)) != 0 || ch == '-' || ch == '_' ||
                ch == '.' || ch == '~')
            {
                result += ch;
            }
            else if (ch == ' ')
            {
                result += "%20";
            }
            else
            {
                result += '%';
                const auto hi = static_cast<unsigned char>(ch) >> 4;
                const auto lo = static_cast<unsigned char>(ch) & 0x0F;
                result += static_cast<char>(hi < 10 ? '0' + hi : 'A' + hi - 10);
                result += static_cast<char>(lo < 10 ? '0' + lo : 'A' + lo - 10);
            }
        }
        return result;
    }

    /// URL-encode a path (preserve slashes).
    [[nodiscard]] static auto url_encode_path(const std::string& path) -> std::string
    {
        std::string result;
        result.reserve(path.size());
        for (const char ch : path)
        {
            if (ch == '/')
            {
                result += '/';
            }
            else if (std::isalnum(static_cast<unsigned char>(ch)) != 0 || ch == '-' || ch == '_' ||
                     ch == '.' || ch == '~')
            {
                result += ch;
            }
            else if (ch == ' ')
            {
                result += "%20";
            }
            else
            {
                result += '%';
                const auto hi = static_cast<unsigned char>(ch) >> 4;
                const auto lo = static_cast<unsigned char>(ch) & 0x0F;
                result += static_cast<char>(hi < 10 ? '0' + hi : 'A' + hi - 10);
                result += static_cast<char>(lo < 10 ? '0' + lo : 'A' + lo - 10);
            }
        }
        return result;
    }

    /// URL-decode a string.
    [[nodiscard]] static auto url_decode(const std::string& str) -> std::string
    {
        std::string result;
        result.reserve(str.size());
        for (std::size_t idx = 0; idx < str.size(); ++idx)
        {
            if (str[idx] == '%' && idx + 2 < str.size())
            {
                auto hi = hex_digit(str[idx + 1]);
                auto lo = hex_digit(str[idx + 2]);
                if (hi >= 0 && lo >= 0)
                {
                    result += static_cast<char>((hi << 4) | lo);
                    idx += 2;
                    continue;
                }
            }
            if (str[idx] == '+')
            {
                result += ' ';
            }
            else
            {
                result += str[idx];
            }
        }
        return result;
    }

    /// Parse hex digit.
    [[nodiscard]] static auto hex_digit(char ch) -> int
    {
        if (ch >= '0' && ch <= '9')
        {
            return ch - '0';
        }
        if (ch >= 'A' && ch <= 'F')
        {
            return ch - 'A' + 10;
        }
        if (ch >= 'a' && ch <= 'f')
        {
            return ch - 'a' + 10;
        }
        return -1;
    }

    /// Parse query parameters from query string.
    static void parse_query_params(const std::string& query, DeepLinkUri& result)
    {
        std::size_t pos = 0;
        while (pos < query.size())
        {
            auto amp_pos = query.find('&', pos);
            if (amp_pos == std::string::npos)
            {
                amp_pos = query.size();
            }

            auto param = query.substr(pos, amp_pos - pos);
            auto eq_pos = param.find('=');
            if (eq_pos != std::string::npos)
            {
                auto key = param.substr(0, eq_pos);
                auto val = param.substr(eq_pos + 1);

                if (key == "line")
                {
                    try
                    {
                        result.line = std::stoi(val);
                    }
                    catch (...)
                    {
                        // Ignore invalid line numbers
                    }
                }
                else
                {
                    result.query_params[key] = url_decode(val);
                }
            }

            pos = amp_pos + 1;
        }
    }

    /// Build human-readable label for an anchor.
    [[nodiscard]] static auto build_label(const LinkAnchor& anchor) -> std::string
    {
        std::string label;
        label += surface_kind_to_string(anchor.surface_kind);
        if (!anchor.file_path.empty())
        {
            auto slash_pos = anchor.file_path.rfind('/');
            label += ": ";
            label += (slash_pos != std::string::npos) ? anchor.file_path.substr(slash_pos + 1)
                                                      : anchor.file_path;
        }
        if (!anchor.heading_id.empty())
        {
            label += " #" + anchor.heading_id;
        }
        return label;
    }
};

} // namespace markamp::core
