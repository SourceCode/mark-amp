// ============================================================================
// File: src/core/ContentSecurityPolicy.cpp
// Phase 29: Security & Input Validation — CSP Header Generation
// ============================================================================

#include "ContentSecurityPolicy.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

namespace
{

auto join_sources(const std::vector<std::string>& sources) -> std::string
{
    std::ostringstream oss;
    for (size_t idx = 0; idx < sources.size(); ++idx)
    {
        if (idx > 0)
        {
            oss << ' ';
        }
        oss << sources[idx];
    }
    return oss.str();
}

auto append_directive(std::ostringstream& oss,
                      const std::string& name,
                      const std::vector<std::string>& sources) -> void
{
    if (!sources.empty())
    {
        oss << name << ' ' << join_sources(sources) << "; ";
    }
}

} // namespace

// ── Policy generation ──

auto ContentSecurityPolicy::generate_policy(const CspConfig& config) -> std::string
{
    std::ostringstream oss;

    append_directive(oss, "default-src", config.default_src);
    append_directive(oss, "script-src", config.script_src);
    append_directive(oss, "style-src", config.style_src);
    append_directive(oss, "img-src", config.img_src);
    append_directive(oss, "connect-src", config.connect_src);
    append_directive(oss, "font-src", config.font_src);
    append_directive(oss, "frame-src", config.frame_src);
    append_directive(oss, "object-src", config.object_src);
    append_directive(oss, "media-src", config.media_src);
    append_directive(oss, "base-uri", config.base_uri);

    if (config.upgrade_insecure_requests)
    {
        oss << "upgrade-insecure-requests; ";
    }

    auto result = oss.str();
    // Remove trailing space.
    if (!result.empty() && result.back() == ' ')
    {
        result.pop_back();
    }
    return result;
}

// ── Presets ──

auto ContentSecurityPolicy::default_strict_policy() -> CspConfig
{
    CspConfig config;
    config.default_src = {"'self'"};
    config.script_src = {"'none'"};
    config.style_src = {"'self'"};
    config.img_src = {"'self'", "data:"};
    config.connect_src = {"'none'"};
    config.font_src = {"'self'"};
    config.frame_src = {"'none'"};
    config.object_src = {"'none'"};
    config.media_src = {"'self'"};
    config.base_uri = {"'self'"};
    config.upgrade_insecure_requests = true;
    return config;
}

auto ContentSecurityPolicy::default_preview_policy() -> CspConfig
{
    CspConfig config;
    config.default_src = {"'self'"};
    config.script_src = {"'none'"};
    config.style_src = {"'self'", "'unsafe-inline'"};
    config.img_src = {"'self'", "data:", "https:"};
    config.connect_src = {"'self'"};
    config.font_src = {"'self'", "https:"};
    config.frame_src = {"'none'"};
    config.object_src = {"'none'"};
    config.media_src = {"'self'"};
    config.base_uri = {"'self'"};
    config.upgrade_insecure_requests = true;
    return config;
}

auto ContentSecurityPolicy::default_dev_policy() -> CspConfig
{
    CspConfig config;
    config.default_src = {"'self'"};
    config.script_src = {"'self'", "'unsafe-inline'", "'unsafe-eval'"};
    config.style_src = {"'self'", "'unsafe-inline'"};
    config.img_src = {"'self'", "data:", "https:", "http:"};
    config.connect_src = {"'self'", "ws:", "wss:"};
    config.font_src = {"'self'", "https:", "data:"};
    config.frame_src = {"'self'"};
    config.object_src = {"'none'"};
    config.media_src = {"'self'"};
    config.base_uri = {"'self'"};
    config.upgrade_insecure_requests = false;
    return config;
}

// ── Validation ──

auto ContentSecurityPolicy::validate_directive(const std::string& directive) -> bool
{
    if (directive.empty())
    {
        return false;
    }

    // A directive value must be non-empty and contain only safe characters.
    // Valid: 'self', 'none', 'unsafe-inline', https:, *.example.com, data:
    static const std::string kValid =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=%*";

    return std::all_of(directive.begin(),
                       directive.end(),
                       [&](char chr) { return kValid.find(chr) != std::string::npos; });
}

// ── Merge ──

auto ContentSecurityPolicy::merge_policies(const CspConfig& base, const CspConfig& override_config)
    -> CspConfig
{
    CspConfig result = base;

    auto merge_field = [](std::vector<std::string>& dest, const std::vector<std::string>& src)
    {
        if (!src.empty())
        {
            dest = src;
        }
    };

    merge_field(result.default_src, override_config.default_src);
    merge_field(result.script_src, override_config.script_src);
    merge_field(result.style_src, override_config.style_src);
    merge_field(result.img_src, override_config.img_src);
    merge_field(result.connect_src, override_config.connect_src);
    merge_field(result.font_src, override_config.font_src);
    merge_field(result.frame_src, override_config.frame_src);
    merge_field(result.object_src, override_config.object_src);
    merge_field(result.media_src, override_config.media_src);
    merge_field(result.base_uri, override_config.base_uri);

    return result;
}

// ── Meta tag ──

auto ContentSecurityPolicy::as_meta_tag(const CspConfig& config) -> std::string
{
    const auto policy = generate_policy(config);
    return "<meta http-equiv=\"Content-Security-Policy\" content=\"" + policy + "\">";
}

// ── Directive count ──

auto ContentSecurityPolicy::directive_count(const CspConfig& config) -> int32_t
{
    int32_t count = 0;
    if (!config.default_src.empty())
        ++count;
    if (!config.script_src.empty())
        ++count;
    if (!config.style_src.empty())
        ++count;
    if (!config.img_src.empty())
        ++count;
    if (!config.connect_src.empty())
        ++count;
    if (!config.font_src.empty())
        ++count;
    if (!config.frame_src.empty())
        ++count;
    if (!config.object_src.empty())
        ++count;
    if (!config.media_src.empty())
        ++count;
    if (!config.base_uri.empty())
        ++count;
    if (config.upgrade_insecure_requests)
        ++count;
    return count;
}

} // namespace markamp::core
