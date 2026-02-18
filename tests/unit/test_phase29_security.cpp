// ============================================================================
// File: tests/unit/test_phase29_security.cpp
// Phase 29: Security & Input Validation — Comprehensive test suite
// ============================================================================

#include "core/ClipboardSanitizer.h"
#include "core/ContentSecurityPolicy.h"
#include "core/Events.h"
#include "core/InputRateLimiter.h"
#include "core/SecurityAuditLog.h"
#include "core/SecurityCommandProvider.h"
#include "core/UrlSanitizer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ============================================================================
// UrlSanitizer Tests
// ============================================================================

TEST_CASE("UrlSanitizer: safe schemes allowed", "[security][url]")
{
    UrlSanitizer sanitizer;
    REQUIRE(sanitizer.is_safe_scheme("https://example.com"));
    REQUIRE(sanitizer.is_safe_scheme("http://example.com"));
    REQUIRE(sanitizer.is_safe_scheme("mailto:user@example.com"));
    REQUIRE(sanitizer.is_safe_scheme("file:///path/to/doc"));
    REQUIRE(sanitizer.is_safe_scheme("ftp://example.com/file"));
}

TEST_CASE("UrlSanitizer: dangerous schemes blocked", "[security][url]")
{
    UrlSanitizer sanitizer;
    REQUIRE_FALSE(sanitizer.is_safe_scheme("javascript:alert(1)"));
    REQUIRE_FALSE(sanitizer.is_safe_scheme("vbscript:msgbox"));
    REQUIRE_FALSE(sanitizer.is_safe_scheme("data:text/html,<h1>hi</h1>"));
}

TEST_CASE("UrlSanitizer: protocol-relative URLs rejected", "[security][url]")
{
    UrlSanitizer sanitizer;
    REQUIRE(UrlSanitizer::is_protocol_relative("//evil.com/payload"));
    REQUIRE_FALSE(sanitizer.is_safe_scheme("//evil.com"));
    REQUIRE_FALSE(sanitizer.is_safe_url("//evil.com/payload"));
}

TEST_CASE("UrlSanitizer: encoded dangerous schemes detected", "[security][url]")
{
    UrlSanitizer sanitizer;
    // Percent-encoded "javascript:"
    REQUIRE_FALSE(sanitizer.is_safe_scheme("%6Aavascript:alert(1)"));
    REQUIRE_FALSE(sanitizer.is_safe_url("java%73cript:alert(1)"));
}

TEST_CASE("UrlSanitizer: scheme extraction", "[security][url]")
{
    REQUIRE(UrlSanitizer::extract_scheme("https://example.com") == "https");
    REQUIRE(UrlSanitizer::extract_scheme("HTTP://EXAMPLE.COM") == "http");
    REQUIRE(UrlSanitizer::extract_scheme("relative/path") == "");
    REQUIRE(UrlSanitizer::extract_scheme("") == "");
}

TEST_CASE("UrlSanitizer: local URL detection", "[security][url]")
{
    REQUIRE(UrlSanitizer::is_local_url("http://localhost:8080"));
    REQUIRE(UrlSanitizer::is_local_url("http://127.0.0.1/path"));
    REQUIRE(UrlSanitizer::is_local_url("file:///home/user/doc.md"));
    REQUIRE_FALSE(UrlSanitizer::is_local_url("https://example.com"));
}

TEST_CASE("UrlSanitizer: normalization", "[security][url]")
{
    REQUIRE(UrlSanitizer::normalize("  HTTPS://Example.COM  ") == "https://Example.COM");
    REQUIRE(UrlSanitizer::normalize("HTTP://test.com") == "http://test.com");
}

TEST_CASE("UrlSanitizer: sanitize returns Result", "[security][url]")
{
    UrlSanitizer sanitizer;

    auto ok = sanitizer.sanitize("https://example.com");
    REQUIRE(ok.has_value());
    REQUIRE(ok.value() == "https://example.com");

    auto blocked = sanitizer.sanitize("javascript:alert(1)");
    REQUIRE_FALSE(blocked.has_value());

    auto empty = sanitizer.sanitize("");
    REQUIRE_FALSE(empty.has_value());

    auto proto_rel = sanitizer.sanitize("//evil.com");
    REQUIRE_FALSE(proto_rel.has_value());
}

TEST_CASE("UrlSanitizer: relative URLs allowed", "[security][url]")
{
    UrlSanitizer sanitizer;
    REQUIRE(sanitizer.is_safe_url("/path/to/page"));
    REQUIRE(sanitizer.is_safe_url("relative/path"));
    REQUIRE(sanitizer.is_safe_url("#anchor"));
}

TEST_CASE("UrlSanitizer: configurable schemes", "[security][url]")
{
    UrlSanitizer sanitizer;
    REQUIRE_FALSE(sanitizer.is_safe_scheme("custom://test"));

    sanitizer.allow_scheme("custom");
    REQUIRE(sanitizer.is_safe_scheme("custom://test"));

    sanitizer.disallow_scheme("custom");
    REQUIRE_FALSE(sanitizer.is_safe_scheme("custom://test"));
}

TEST_CASE("UrlSanitizer: allowed_schemes list", "[security][url]")
{
    UrlSanitizer sanitizer;
    auto schemes = sanitizer.allowed_schemes();
    REQUIRE(schemes.size() >= 5); // http, https, mailto, file, ftp, tel, ssh
}

// ============================================================================
// ContentSecurityPolicy Tests
// ============================================================================

TEST_CASE("ContentSecurityPolicy: generate strict policy", "[security][csp]")
{
    auto config = ContentSecurityPolicy::default_strict_policy();
    auto policy = ContentSecurityPolicy::generate_policy(config);

    REQUIRE(policy.find("default-src 'self'") != std::string::npos);
    REQUIRE(policy.find("script-src 'none'") != std::string::npos);
    REQUIRE(policy.find("upgrade-insecure-requests") != std::string::npos);
}

TEST_CASE("ContentSecurityPolicy: preview policy allows inline styles", "[security][csp]")
{
    auto config = ContentSecurityPolicy::default_preview_policy();
    auto policy = ContentSecurityPolicy::generate_policy(config);

    REQUIRE(policy.find("style-src 'self' 'unsafe-inline'") != std::string::npos);
    REQUIRE(policy.find("script-src 'none'") != std::string::npos);
}

TEST_CASE("ContentSecurityPolicy: dev policy allows unsafe eval", "[security][csp]")
{
    auto config = ContentSecurityPolicy::default_dev_policy();
    auto policy = ContentSecurityPolicy::generate_policy(config);

    REQUIRE(policy.find("'unsafe-eval'") != std::string::npos);
    REQUIRE(policy.find("ws:") != std::string::npos);
}

TEST_CASE("ContentSecurityPolicy: validate directive", "[security][csp]")
{
    REQUIRE(ContentSecurityPolicy::validate_directive("'self'"));
    REQUIRE(ContentSecurityPolicy::validate_directive("https:"));
    REQUIRE(ContentSecurityPolicy::validate_directive("*.example.com"));
    REQUIRE_FALSE(ContentSecurityPolicy::validate_directive(""));
}

TEST_CASE("ContentSecurityPolicy: merge policies", "[security][csp]")
{
    auto base = ContentSecurityPolicy::default_strict_policy();
    CspConfig override_config;
    override_config.default_src = {};
    override_config.script_src = {"'self'"};
    override_config.style_src = {};

    auto merged = ContentSecurityPolicy::merge_policies(base, override_config);
    REQUIRE(merged.default_src == base.default_src);                  // Unchanged (override empty)
    REQUIRE(merged.script_src == std::vector<std::string>{"'self'"}); // Overridden
}

TEST_CASE("ContentSecurityPolicy: meta tag generation", "[security][csp]")
{
    auto config = ContentSecurityPolicy::default_strict_policy();
    auto tag = ContentSecurityPolicy::as_meta_tag(config);

    REQUIRE(tag.find("<meta http-equiv=\"Content-Security-Policy\"") != std::string::npos);
    REQUIRE(tag.find("default-src") != std::string::npos);
}

TEST_CASE("ContentSecurityPolicy: directive count", "[security][csp]")
{
    auto strict = ContentSecurityPolicy::default_strict_policy();
    REQUIRE(ContentSecurityPolicy::directive_count(strict) == 11); // 10 directives + upgrade

    CspConfig empty;
    empty.default_src = {};
    empty.script_src = {};
    empty.style_src = {};
    empty.img_src = {};
    empty.connect_src = {};
    empty.font_src = {};
    empty.frame_src = {};
    empty.object_src = {};
    empty.media_src = {};
    empty.base_uri = {};
    empty.upgrade_insecure_requests = false;
    REQUIRE(ContentSecurityPolicy::directive_count(empty) == 0);
}

// ============================================================================
// InputRateLimiter Tests
// ============================================================================

TEST_CASE("InputRateLimiter: default actions configured", "[security][ratelimit]")
{
    InputRateLimiter limiter;
    auto actions = limiter.configured_actions();
    REQUIRE(actions.size() >= 5); // search, ai_completion, ai_chat, export, theme_preview
}

TEST_CASE("InputRateLimiter: acquire succeeds within budget", "[security][ratelimit]")
{
    InputRateLimiter limiter;
    limiter.configure_action(
        "test_action", {.max_tokens = 3, .refill_rate_per_sec = 0.0F, .refill_interval_ms = 1000});

    REQUIRE(limiter.try_acquire("test_action"));
    REQUIRE(limiter.try_acquire("test_action"));
    REQUIRE(limiter.try_acquire("test_action"));
    REQUIRE_FALSE(limiter.try_acquire("test_action")); // Exhausted
}

TEST_CASE("InputRateLimiter: remaining tokens", "[security][ratelimit]")
{
    InputRateLimiter limiter;
    limiter.configure_action(
        "test_action", {.max_tokens = 5, .refill_rate_per_sec = 0.0F, .refill_interval_ms = 1000});

    REQUIRE(limiter.remaining_tokens("test_action") == 5);
    limiter.try_acquire("test_action");
    REQUIRE(limiter.remaining_tokens("test_action") == 4);
}

TEST_CASE("InputRateLimiter: is_limited", "[security][ratelimit]")
{
    InputRateLimiter limiter;
    limiter.configure_action(
        "test_action", {.max_tokens = 1, .refill_rate_per_sec = 0.0F, .refill_interval_ms = 1000});

    REQUIRE_FALSE(limiter.is_limited("test_action"));
    limiter.try_acquire("test_action");
    REQUIRE(limiter.is_limited("test_action"));
}

TEST_CASE("InputRateLimiter: reset restores tokens", "[security][ratelimit]")
{
    InputRateLimiter limiter;
    limiter.configure_action(
        "test_action", {.max_tokens = 2, .refill_rate_per_sec = 0.0F, .refill_interval_ms = 1000});

    limiter.try_acquire("test_action");
    limiter.try_acquire("test_action");
    REQUIRE(limiter.is_limited("test_action"));

    limiter.reset("test_action");
    REQUIRE_FALSE(limiter.is_limited("test_action"));
    REQUIRE(limiter.remaining_tokens("test_action") == 2);
}

TEST_CASE("InputRateLimiter: reset_all", "[security][ratelimit]")
{
    InputRateLimiter limiter;
    limiter.configure_action(
        "a", {.max_tokens = 1, .refill_rate_per_sec = 0.0F, .refill_interval_ms = 1000});
    limiter.configure_action(
        "b", {.max_tokens = 1, .refill_rate_per_sec = 0.0F, .refill_interval_ms = 1000});

    limiter.try_acquire("a");
    limiter.try_acquire("b");

    limiter.reset_all();
    REQUIRE_FALSE(limiter.is_limited("a"));
    REQUIRE_FALSE(limiter.is_limited("b"));
}

TEST_CASE("InputRateLimiter: rejection counting", "[security][ratelimit]")
{
    InputRateLimiter limiter;
    limiter.configure_action(
        "test_action", {.max_tokens = 1, .refill_rate_per_sec = 0.0F, .refill_interval_ms = 1000});

    limiter.try_acquire("test_action"); // Success
    limiter.try_acquire("test_action"); // Rejected
    limiter.try_acquire("test_action"); // Rejected

    REQUIRE(limiter.rejections("test_action") == 2);
    REQUIRE(limiter.total_rejections() >= 2);
}

TEST_CASE("InputRateLimiter: get_config", "[security][ratelimit]")
{
    InputRateLimiter limiter;
    auto config = limiter.get_config("search");
    REQUIRE(config.max_tokens == 10);
    REQUIRE(config.refill_rate_per_sec > 0.0F);
}

// ============================================================================
// ClipboardSanitizer Tests
// ============================================================================

TEST_CASE("ClipboardSanitizer: detect content type", "[security][clipboard]")
{
    REQUIRE(ClipboardSanitizer::detect_content_type("<html><body>hi</body></html>") ==
            ClipboardContentType::kHtml);
    REQUIRE(ClipboardSanitizer::detect_content_type("<div class='test'>content</div>") ==
            ClipboardContentType::kHtml);
    REQUIRE(ClipboardSanitizer::detect_content_type("# Heading\n\n**Bold text**") ==
            ClipboardContentType::kMarkdown);
    REQUIRE(ClipboardSanitizer::detect_content_type("Just plain text here") ==
            ClipboardContentType::kPlainText);
    REQUIRE(ClipboardSanitizer::detect_content_type("") == ClipboardContentType::kPlainText);
}

TEST_CASE("ClipboardSanitizer: sanitize text strips null bytes", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    std::string with_nulls = "hello";
    with_nulls += '\0';
    with_nulls += "world";

    auto result = sanitizer.sanitize_text(with_nulls);
    REQUIRE(result.content == "helloworld");
    REQUIRE(result.modifications > 0);
}

TEST_CASE("ClipboardSanitizer: sanitize text normalizes line endings", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    auto result = sanitizer.sanitize_text("line1\r\nline2\rline3\nline4");
    REQUIRE(result.content == "line1\nline2\nline3\nline4");
}

TEST_CASE("ClipboardSanitizer: sanitize text enforces max size", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    sanitizer.set_max_paste_size(10);

    std::string long_text = "This is a very long string that exceeds the limit";
    auto result = sanitizer.sanitize_text(long_text);
    REQUIRE(result.content.size() == 10);
    REQUIRE(result.was_truncated);
}

TEST_CASE("ClipboardSanitizer: sanitize HTML strips scripts", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    auto result = sanitizer.sanitize_html("<p>Hello</p><script>alert(1)</script><p>World</p>");
    REQUIRE(result.content.find("<script>") == std::string::npos);
    REQUIRE(result.content.find("</script>") == std::string::npos);
    REQUIRE(result.had_dangerous_content);
}

TEST_CASE("ClipboardSanitizer: sanitize HTML strips iframes", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    auto result = sanitizer.sanitize_html("<p>Hello</p><iframe src='evil.com'></iframe>");
    REQUIRE(result.content.find("<iframe") == std::string::npos);
    REQUIRE(result.had_dangerous_content);
}

TEST_CASE("ClipboardSanitizer: sanitize HTML strips event handlers", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    auto result = sanitizer.sanitize_html("<div onclick=\"alert(1)\">Click me</div>");
    REQUIRE(result.content.find("onclick") == std::string::npos);
    REQUIRE(result.had_dangerous_content);
}

TEST_CASE("ClipboardSanitizer: sanitize markdown with embedded HTML", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    auto result = sanitizer.sanitize_markdown("# Heading\n\n<script>alert(1)</script>\n\n**Bold**");
    REQUIRE(result.content.find("<script>") == std::string::npos);
    REQUIRE(result.content.find("# Heading") != std::string::npos);
    REQUIRE(result.content.find("**Bold**") != std::string::npos);
    REQUIRE(result.had_dangerous_content);
}

TEST_CASE("ClipboardSanitizer: auto-detect and sanitize", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    auto result = sanitizer.sanitize_auto("<div onclick=\"evil()\">HTML</div>");
    REQUIRE(result.content_type == ClipboardContentType::kHtml);
    REQUIRE(result.content.find("onclick") == std::string::npos);
}

TEST_CASE("ClipboardSanitizer: max_paste_size", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    REQUIRE(sanitizer.max_paste_size() == 1024ULL * 1024ULL);
    sanitizer.set_max_paste_size(500);
    REQUIRE(sanitizer.max_paste_size() == 500);
}

TEST_CASE("ClipboardSanitizer: content_type_name", "[security][clipboard]")
{
    REQUIRE(ClipboardSanitizer::content_type_name(ClipboardContentType::kPlainText) ==
            "Plain Text");
    REQUIRE(ClipboardSanitizer::content_type_name(ClipboardContentType::kHtml) == "HTML");
    REQUIRE(ClipboardSanitizer::content_type_name(ClipboardContentType::kMarkdown) == "Markdown");
}

// ============================================================================
// SecurityAuditLog Tests
// ============================================================================

TEST_CASE("SecurityAuditLog: record and retrieve", "[security][audit]")
{
    SecurityAuditLog log(100);
    log.record(
        SecuritySeverity::kInfo, SecurityCategory::kInput, "Input validated", "UrlSanitizer");

    REQUIRE(log.size() == 1);
    auto events = log.recent(10);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].description == "Input validated");
    REQUIRE(events[0].source_component == "UrlSanitizer");
}

TEST_CASE("SecurityAuditLog: circular buffer overflow", "[security][audit]")
{
    SecurityAuditLog log(5);
    for (int idx = 0; idx < 10; ++idx)
    {
        log.record(
            SecuritySeverity::kInfo, SecurityCategory::kInput, "Event " + std::to_string(idx));
    }

    REQUIRE(log.size() == 5); // Capped at max
    auto recent = log.recent(5);
    REQUIRE(recent.size() == 5);
    // Oldest events should be events 5-9
    REQUIRE(recent[0].description == "Event 5");
    REQUIRE(recent[4].description == "Event 9");
}

TEST_CASE("SecurityAuditLog: query by severity", "[security][audit]")
{
    SecurityAuditLog log;
    log.record(SecuritySeverity::kInfo, SecurityCategory::kInput, "Info event");
    log.record(SecuritySeverity::kWarning, SecurityCategory::kInput, "Warning event");
    log.record(SecuritySeverity::kCritical, SecurityCategory::kInput, "Critical event");

    SecurityEventFilter filter;
    filter.min_severity = SecuritySeverity::kWarning;
    auto results = log.query(filter);

    REQUIRE(results.size() == 2);
}

TEST_CASE("SecurityAuditLog: query by category", "[security][audit]")
{
    SecurityAuditLog log;
    log.record(SecuritySeverity::kInfo, SecurityCategory::kInput, "Input event");
    log.record(SecuritySeverity::kInfo, SecurityCategory::kNetwork, "Network event");
    log.record(SecuritySeverity::kInfo, SecurityCategory::kExtension, "Extension event");

    SecurityEventFilter filter;
    filter.filter_by_category = true;
    filter.category = SecurityCategory::kNetwork;
    auto results = log.query(filter);

    REQUIRE(results.size() == 1);
    REQUIRE(results[0].description == "Network event");
}

TEST_CASE("SecurityAuditLog: summary counts", "[security][audit]")
{
    SecurityAuditLog log;
    log.record(SecuritySeverity::kInfo, SecurityCategory::kAuth, "Auth info");
    log.record(SecuritySeverity::kWarning, SecurityCategory::kAccess, "Access warning");
    log.record(SecuritySeverity::kCritical, SecurityCategory::kNetwork, "Net critical");
    log.record(SecuritySeverity::kInfo, SecurityCategory::kInput, "Input info");

    auto summary = log.summary();
    REQUIRE(summary.total_events == 4);
    REQUIRE(summary.info_count == 2);
    REQUIRE(summary.warning_count == 1);
    REQUIRE(summary.critical_count == 1);
    REQUIRE(summary.auth_events == 1);
    REQUIRE(summary.access_events == 1);
    REQUIRE(summary.network_events == 1);
    REQUIRE(summary.input_events == 1);
}

TEST_CASE("SecurityAuditLog: clear", "[security][audit]")
{
    SecurityAuditLog log;
    log.record(SecuritySeverity::kInfo, SecurityCategory::kInput, "Test");
    REQUIRE(log.size() == 1);
    log.clear();
    REQUIRE(log.size() == 0);
}

TEST_CASE("SecurityAuditLog: export JSON", "[security][audit]")
{
    SecurityAuditLog log;
    log.record(SecuritySeverity::kWarning,
               SecurityCategory::kExtension,
               "Blocked extension",
               "ExtensionSandbox",
               "ext123");

    auto json = log.export_json();
    REQUIRE(json.find("\"severity\": \"warning\"") != std::string::npos);
    REQUIRE(json.find("\"category\": \"extension\"") != std::string::npos);
    REQUIRE(json.find("Blocked extension") != std::string::npos);
}

TEST_CASE("SecurityAuditLog: max_entries", "[security][audit]")
{
    SecurityAuditLog log(200);
    REQUIRE(log.max_entries() == 200);
}

// ============================================================================
// SecurityCommandProvider Tests
// ============================================================================

TEST_CASE("SecurityCommandProvider: registers commands", "[security][commands]")
{
    SecurityCommandProvider provider;
    REQUIRE(provider.command_count() == 8);
}

TEST_CASE("SecurityCommandProvider: find command by ID", "[security][commands]")
{
    SecurityCommandProvider provider;

    auto cmd = provider.find_command("security.viewAuditLog");
    REQUIRE(cmd.has_value());
    REQUIRE(cmd->title == "Security: View Audit Log");
    REQUIRE(cmd->category == "Security");

    auto missing = provider.find_command("nonexistent.command");
    REQUIRE_FALSE(missing.has_value());
}

TEST_CASE("SecurityCommandProvider: commands in category", "[security][commands]")
{
    SecurityCommandProvider provider;

    auto security_cmds = provider.commands_in_category("Security");
    REQUIRE(security_cmds.size() >= 4);

    auto rate_cmds = provider.commands_in_category("Rate Limiting");
    REQUIRE(rate_cmds.size() == 2);
}

TEST_CASE("SecurityCommandProvider: categories", "[security][commands]")
{
    SecurityCommandProvider provider;
    auto cats = provider.categories();
    REQUIRE(cats.size() == 3); // Security, Content Security, Rate Limiting
}

TEST_CASE("SecurityCommandProvider: available commands", "[security][commands]")
{
    SecurityCommandProvider provider;
    auto available = provider.available_commands();
    REQUIRE(available.size() == 8);
}

TEST_CASE("SecurityCommandProvider: command IDs", "[security][commands]")
{
    SecurityCommandProvider provider;

    // Verify all expected command IDs exist
    REQUIRE(provider.find_command("security.viewAuditLog").has_value());
    REQUIRE(provider.find_command("security.clearAuditLog").has_value());
    REQUIRE(provider.find_command("security.viewExtensionPermissions").has_value());
    REQUIRE(provider.find_command("security.toggleStrictCsp").has_value());
    REQUIRE(provider.find_command("security.viewRateLimitStatus").has_value());
    REQUIRE(provider.find_command("security.resetRateLimits").has_value());
    REQUIRE(provider.find_command("security.exportReport").has_value());
    REQUIRE(provider.find_command("security.toggleAuditLogging").has_value());
}

// ============================================================================
// Event Tests
// ============================================================================

TEST_CASE("SecurityUrlBlockedEvent: fields", "[security][events]")
{
    SecurityUrlBlockedEvent event;
    event.url = "javascript:alert(1)";
    event.reason = "Blocked scheme";
    REQUIRE(event.url == "javascript:alert(1)");
    REQUIRE(event.reason == "Blocked scheme");
}

TEST_CASE("SecurityCspViolationEvent: fields", "[security][events]")
{
    SecurityCspViolationEvent event;
    event.directive = "script-src";
    event.blocked_uri = "https://evil.com/script.js";
    REQUIRE(event.directive == "script-src");
    REQUIRE(event.blocked_uri == "https://evil.com/script.js");
}

TEST_CASE("SecurityRateLimitedEvent: fields", "[security][events]")
{
    SecurityRateLimitedEvent event;
    event.action_name = "search";
    event.remaining_tokens = 0;
    REQUIRE(event.action_name == "search");
    REQUIRE(event.remaining_tokens == 0);
}

TEST_CASE("SecurityClipboardSanitizedEvent: fields", "[security][events]")
{
    SecurityClipboardSanitizedEvent event;
    event.content_type = "HTML";
    event.bytes_removed = 42;
    REQUIRE(event.content_type == "HTML");
    REQUIRE(event.bytes_removed == 42);
}

TEST_CASE("SecurityAuditEvent: fields", "[security][events]")
{
    SecurityAuditEvent event;
    event.severity = "warning";
    event.category = "network";
    event.message = "Unauthorized network access";
    REQUIRE(event.severity == "warning");
    REQUIRE(event.category == "network");
    REQUIRE(event.message == "Unauthorized network access");
}
