/// @file test_phase19_code_intelligence.cpp
/// @brief Comprehensive unit and integration tests for Phase 19 Code Intelligence providers.
///
/// Covers: SemanticTokenProvider, InlayHintProvider, FoldingRangeProvider,
///         LanguageDetector, CodeIntelligenceTypes, and Events.

#include "core/CodeIntelligenceTypes.h"
#include "core/FoldingRangeProvider.h"
#include "core/InlayHintProvider.h"
#include "core/LanguageDetector.h"
#include "core/SemanticTokenProvider.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// CodeIntelligenceTypes tests
// ============================================================================

TEST_CASE("CodeIntelligenceTypes: CompletionKindValues", "[CodeIntelligenceTypes]")
{
    CHECK(static_cast<int>(CompletionKind::kKeyword) == 0);
    CHECK(static_cast<int>(CompletionKind::kSnippet) == 1);
    CHECK(static_cast<int>(CompletionKind::kFile) == 6);
    CHECK(static_cast<int>(CompletionKind::kFolder) == 7);
}

TEST_CASE("CodeIntelligenceTypes: CompletionItemDefaults", "[CodeIntelligenceTypes]")
{
    CompletionItem item;
    CHECK(item.label.empty());
    CHECK(item.insert_text.empty());
    CHECK(item.detail.empty());
    CHECK(item.kind == CompletionKind::kKeyword);
    CHECK(item.score == Catch::Approx(0.0));
    CHECK_FALSE(item.is_snippet);
}

TEST_CASE("CodeIntelligenceTypes: CompletionListDefaults", "[CodeIntelligenceTypes]")
{
    CompletionList list;
    CHECK_FALSE(list.is_incomplete);
    CHECK(list.items.empty());
}

TEST_CASE("CodeIntelligenceTypes: HoverContentDefaults", "[CodeIntelligenceTypes]")
{
    HoverContent hover;
    CHECK(hover.markdown.empty());
    CHECK(hover.start_line == 0);
    CHECK(hover.start_char == 0);
    CHECK(hover.empty());
}

TEST_CASE("CodeIntelligenceTypes: SemanticTokenDefaults", "[CodeIntelligenceTypes]")
{
    SemanticToken token;
    CHECK(token.line == 0);
    CHECK(token.start_char == 0);
    CHECK(token.length == 0);
    CHECK(token.token_type == SemanticTokenType::kCode);
    CHECK(token.modifiers == SemanticTokenModifier::kNone);
}

TEST_CASE("CodeIntelligenceTypes: FoldingRangeDefaults", "[CodeIntelligenceTypes]")
{
    FoldingRange range;
    CHECK(range.start_line == 0);
    CHECK(range.end_line == 0);
    CHECK(range.kind == FoldingRangeKind::kHeading);
    CHECK(range.collapsed_text.empty());
}

TEST_CASE("CodeIntelligenceTypes: InlayHintDefaults", "[CodeIntelligenceTypes]")
{
    InlayHintInfo hint;
    CHECK(hint.line == 0);
    CHECK(hint.character == 0);
    CHECK(hint.label.empty());
    CHECK(hint.kind == InlayHintKind::kWordCount);
    CHECK_FALSE(hint.padding_left);
    CHECK_FALSE(hint.padding_right);
}

TEST_CASE("CodeIntelligenceTypes: CompletionSourceBitmask", "[CodeIntelligenceTypes]")
{
    // Verify bitmask OR works correctly
    auto sources = CompletionSource::kSnippets | CompletionSource::kHeadings;
    CHECK((sources & CompletionSource::kSnippets) != CompletionSource::kNone);
    CHECK((sources & CompletionSource::kHeadings) != CompletionSource::kNone);
    CHECK((sources & CompletionSource::kEmoji) == CompletionSource::kNone);
    CHECK((sources & CompletionSource::kLinks) == CompletionSource::kNone);
}

TEST_CASE("CodeIntelligenceTypes: SemanticTokenTypeValues", "[CodeIntelligenceTypes]")
{
    CHECK(static_cast<int>(SemanticTokenType::kHeading) !=
          static_cast<int>(SemanticTokenType::kLink));
    CHECK(static_cast<int>(SemanticTokenType::kStrong) !=
          static_cast<int>(SemanticTokenType::kEmphasis));
}

TEST_CASE("CodeIntelligenceTypes: CodeActionKindValues", "[CodeIntelligenceTypes]")
{
    CHECK(static_cast<int>(CodeActionKind::kQuickFix) !=
          static_cast<int>(CodeActionKind::kRefactor));
    CHECK(static_cast<int>(CodeActionKind::kRefactor) != static_cast<int>(CodeActionKind::kSource));
}

// ============================================================================
// SemanticTokenProvider tests
// ============================================================================

TEST_CASE("SemanticTokenProvider: EmptyDocument", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("");
    CHECK(tokens.empty());
}

TEST_CASE("SemanticTokenProvider: HeadingDetection", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("# Hello World");
    REQUIRE_FALSE(tokens.empty());

    bool found_heading = false;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kHeading)
        {
            found_heading = true;
            CHECK(token.line == 0);
            CHECK(token.start_char == 0);
            CHECK(token.modifiers == SemanticTokenModifier::kDeclaration);
        }
    }
    CHECK(found_heading);
}

TEST_CASE("SemanticTokenProvider: MultiLevelHeadings", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("# H1\n## H2\n### H3");
    int heading_count = 0;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kHeading)
        {
            ++heading_count;
        }
    }
    CHECK(heading_count == 3);
}

TEST_CASE("SemanticTokenProvider: FrontmatterDetection", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("---\ntitle: Test\ntags: [a, b]\n---\n# Content");
    int frontmatter_count = 0;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kFrontmatter)
        {
            ++frontmatter_count;
        }
    }
    CHECK(frontmatter_count >= 3); // Opening ---, content, closing ---
}

TEST_CASE("SemanticTokenProvider: CodeFenceDetection", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("```python\nprint('hi')\n```");
    int code_count = 0;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kCode)
        {
            ++code_count;
        }
    }
    CHECK(code_count >= 3); // Opening ```, content, closing ```
}

TEST_CASE("SemanticTokenProvider: BlockquoteDetection", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("> This is a quote\n> Second line");
    int blockquote_count = 0;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kBlockquote)
        {
            ++blockquote_count;
        }
    }
    CHECK(blockquote_count == 2);
}

TEST_CASE("SemanticTokenProvider: StrongEmphasisInline", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("Some **bold** text");
    bool found_strong = false;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kStrong)
        {
            found_strong = true;
            CHECK(token.line == 0);
        }
    }
    CHECK(found_strong);
}

TEST_CASE("SemanticTokenProvider: EmphasisInline", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("Some *italic* text");
    bool found_emphasis = false;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kEmphasis)
        {
            found_emphasis = true;
        }
    }
    CHECK(found_emphasis);
}

TEST_CASE("SemanticTokenProvider: InlineCodeDetection", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("Use `code` here");
    bool found_code = false;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kCode &&
            token.modifiers == SemanticTokenModifier::kReadonly)
        {
            found_code = true;
        }
    }
    CHECK(found_code);
}

TEST_CASE("SemanticTokenProvider: LinkDetection", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("[text](url)");
    bool found_link = false;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kLink &&
            token.modifiers == SemanticTokenModifier::kNone)
        {
            found_link = true;
        }
    }
    CHECK(found_link);
}

TEST_CASE("SemanticTokenProvider: WikiLinkDetection", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("See [[target page]]");
    bool found_wikilink = false;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kLink &&
            token.modifiers == SemanticTokenModifier::kReference)
        {
            found_wikilink = true;
        }
    }
    CHECK(found_wikilink);
}

TEST_CASE("SemanticTokenProvider: ImageDetection", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("![alt text](image.png)");
    bool found_image = false;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kImage)
        {
            found_image = true;
        }
    }
    CHECK(found_image);
}

TEST_CASE("SemanticTokenProvider: FootnoteReferenceDetection", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("Text [^1] more text\n[^1]: footnote body");
    int ref_count = 0;
    int def_count = 0;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kFootnoteRef)
        {
            if (token.modifiers == SemanticTokenModifier::kReference)
            {
                ++ref_count;
            }
            if (token.modifiers == SemanticTokenModifier::kDeclaration)
            {
                ++def_count;
            }
        }
    }
    CHECK(ref_count >= 1);
    CHECK(def_count >= 1);
}

TEST_CASE("SemanticTokenProvider: HtmlTagDetection", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("<details>\n<summary>Click</summary>\n</details>");
    int html_count = 0;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kHtmlTag)
        {
            ++html_count;
        }
    }
    CHECK(html_count >= 2); // At least <details> and </details>
}

TEST_CASE("SemanticTokenProvider: ListItemDetection", "[SemanticTokenProvider]")
{
    SemanticTokenProvider provider;
    auto tokens = provider.provide_tokens("- item 1\n- item 2\n  - nested");
    int list_count = 0;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kListItem)
        {
            ++list_count;
        }
    }
    CHECK(list_count >= 2);
}

// ============================================================================
// InlayHintProvider tests
// ============================================================================

TEST_CASE("InlayHintProvider: EmptyDocument", "[InlayHintProvider]")
{
    InlayHintProvider provider;
    auto hints = provider.provide_hints("", 0, 0);
    CHECK(hints.empty());
}

TEST_CASE("InlayHintProvider: HeadingWordCount", "[InlayHintProvider]")
{
    InlayHintProvider provider;
    auto hints = provider.provide_hints("# Title\nWord one two three\n\n## Second\nMore words");
    bool found_wc = false;
    bool found_on_heading_line = false;
    for (const auto& hint : hints)
    {
        if (hint.kind == InlayHintKind::kWordCount)
        {
            found_wc = true;
            // Should have a label like " (N words)"
            CHECK_FALSE(hint.label.empty());
            // Hint should be on a heading line (0 or 3)
            if (hint.line == 0 || hint.line == 3)
            {
                found_on_heading_line = true;
            }
        }
    }
    CHECK(found_wc);
    CHECK(found_on_heading_line);
}

TEST_CASE("InlayHintProvider: LinkTargetShortening", "[InlayHintProvider]")
{
    InlayHintProvider provider;
    std::string long_url(60, 'x');
    auto content = "[link](https://example.com/" + long_url + ")";
    auto hints = provider.provide_hints(content);
    bool found_link_hint = false;
    for (const auto& hint : hints)
    {
        if (hint.kind == InlayHintKind::kLinkTarget)
        {
            found_link_hint = true;
            // Shortened label should be shorter than the original URL
            CHECK(static_cast<int>(hint.label.size()) <= 50);
        }
    }
    CHECK(found_link_hint);
}

TEST_CASE("InlayHintProvider: ImageHintDetection", "[InlayHintProvider]")
{
    InlayHintProvider provider;
    auto hints = provider.provide_hints("![photo](image.png)");
    bool found_image = false;
    for (const auto& hint : hints)
    {
        if (hint.kind == InlayHintKind::kImageSize)
        {
            found_image = true;
            CHECK_FALSE(hint.label.empty());
        }
    }
    CHECK(found_image);
}

TEST_CASE("InlayHintProvider: TableColumnCount", "[InlayHintProvider]")
{
    InlayHintProvider provider;
    auto hints = provider.provide_hints("| A | B | C |\n|---|---|---|\n| 1 | 2 | 3 |");
    bool found_table = false;
    for (const auto& hint : hints)
    {
        if (hint.kind == InlayHintKind::kTableColumns)
        {
            found_table = true;
            CHECK_FALSE(hint.label.empty());
        }
    }
    CHECK(found_table);
}

TEST_CASE("InlayHintProvider: DisabledHints", "[InlayHintProvider]")
{
    InlayHintProvider custom_provider;
    custom_provider.set_word_count_enabled(false);
    custom_provider.set_link_target_enabled(false);
    custom_provider.set_image_info_enabled(false);
    custom_provider.set_table_info_enabled(false);
    auto hints = custom_provider.provide_hints(
        "# Title\nSome text\n[link](https://example.com/very/long/url/path/that/is/long)\n"
        "![img](photo.png)\n| A | B |\n|---|---|\n| 1 | 2 |");
    CHECK(hints.empty());
}

TEST_CASE("InlayHintProvider: RangeFiltering", "[InlayHintProvider]")
{
    InlayHintProvider provider;
    auto content = "# H1\ntext\n# H2\nmore text";
    auto hints = provider.provide_hints(content, 2, 3); // Only lines 2-3
    for (const auto& hint : hints)
    {
        CHECK(hint.line >= 2);
        CHECK(hint.line <= 3);
    }
}

// ============================================================================
// FoldingRangeProvider tests
// ============================================================================

TEST_CASE("FoldingRangeProvider: EmptyDocument", "[FoldingRangeProvider]")
{
    FoldingRangeProvider provider;
    auto ranges = provider.provide_ranges("");
    CHECK(ranges.empty());
}

TEST_CASE("FoldingRangeProvider: FrontmatterFolding", "[FoldingRangeProvider]")
{
    FoldingRangeProvider provider;
    auto ranges = provider.provide_ranges("---\ntitle: Test\ntags: [a]\n---\n# Content");
    bool found_fm = false;
    for (const auto& range : ranges)
    {
        if (range.kind == FoldingRangeKind::kFrontmatter)
        {
            found_fm = true;
            CHECK(range.start_line == 0);
            CHECK(range.end_line == 3);
        }
    }
    CHECK(found_fm);
}

TEST_CASE("FoldingRangeProvider: HeadingSectionFolding", "[FoldingRangeProvider]")
{
    FoldingRangeProvider provider;
    auto ranges =
        provider.provide_ranges("# Section 1\nContent here\nMore content\n# Section 2\nText");
    bool found_heading = false;
    for (const auto& range : ranges)
    {
        if (range.kind == FoldingRangeKind::kHeading && range.start_line == 0)
        {
            found_heading = true;
            CHECK(range.end_line <= 2);
        }
    }
    CHECK(found_heading);
}

TEST_CASE("FoldingRangeProvider: NestedHeadingFolding", "[FoldingRangeProvider]")
{
    FoldingRangeProvider provider;
    auto ranges = provider.provide_ranges(
        "# H1\n## H2\nText under H2\n### H3\nText under H3\n## Another H2\nMore text");
    // H1 should fold to cover everything
    bool found_h1 = false;
    for (const auto& range : ranges)
    {
        if (range.kind == FoldingRangeKind::kHeading)
        {
            if (range.collapsed_text == "# H1")
            {
                found_h1 = true;
                CHECK(range.start_line == 0);
                CHECK(range.end_line >= 5); // H1 should cover to the end
            }
        }
    }
    CHECK(found_h1);
}

TEST_CASE("FoldingRangeProvider: CodeFenceFolding", "[FoldingRangeProvider]")
{
    FoldingRangeProvider provider;
    auto ranges = provider.provide_ranges("```python\nprint('hello')\nprint('world')\n```");
    bool found_fence = false;
    for (const auto& range : ranges)
    {
        if (range.kind == FoldingRangeKind::kCodeFence)
        {
            found_fence = true;
            CHECK(range.start_line == 0);
            CHECK(range.end_line == 3);
        }
    }
    CHECK(found_fence);
}

TEST_CASE("FoldingRangeProvider: HtmlDetailsFolding", "[FoldingRangeProvider]")
{
    FoldingRangeProvider provider;
    auto ranges = provider.provide_ranges(
        "<details>\n<summary>Click me</summary>\nHidden content\n</details>");
    bool found_details = false;
    for (const auto& range : ranges)
    {
        if (range.kind == FoldingRangeKind::kCollapsible)
        {
            found_details = true;
            CHECK(range.start_line == 0);
            CHECK(range.end_line == 3);
            // Should extract summary text
            CHECK(range.collapsed_text == "Click me");
        }
    }
    CHECK(found_details);
}

TEST_CASE("FoldingRangeProvider: MultiLineTableFolding", "[FoldingRangeProvider]")
{
    FoldingRangeProvider provider;
    auto ranges = provider.provide_ranges(
        "| H1 | H2 |\n|---|---|\n| A | B |\n| C | D |\n| E | F |\n\nAfter table");
    bool found_table = false;
    for (const auto& range : ranges)
    {
        if (range.kind == FoldingRangeKind::kTable)
        {
            found_table = true;
            CHECK(range.start_line == 0);
        }
    }
    // Table has 5 lines (>= 4 threshold)
    CHECK(found_table);
}

TEST_CASE("FoldingRangeProvider: ShortTableNoFolding", "[FoldingRangeProvider]")
{
    FoldingRangeProvider provider;
    auto ranges = provider.provide_ranges("| H1 | H2 |\n|---|---|\n| A | B |\n\nAfter table");
    bool found_table = false;
    for (const auto& range : ranges)
    {
        if (range.kind == FoldingRangeKind::kTable)
        {
            found_table = true;
        }
    }
    // Table has only 3 lines (< 4 threshold)
    CHECK_FALSE(found_table);
}

TEST_CASE("FoldingRangeProvider: NoFrontmatterInMiddle", "[FoldingRangeProvider]")
{
    FoldingRangeProvider provider;
    auto ranges = provider.provide_ranges("# Title\n---\n---");
    // Should not detect as frontmatter since first line is not ---
    bool found_fm = false;
    for (const auto& range : ranges)
    {
        if (range.kind == FoldingRangeKind::kFrontmatter)
        {
            found_fm = true;
        }
    }
    CHECK_FALSE(found_fm);
}

// ============================================================================
// LanguageDetector tests
// ============================================================================

TEST_CASE("LanguageDetector: ExtensionDetectionMarkdown", "[LanguageDetector]")
{
    CHECK(LanguageDetector::detect_from_extension("notes.md") == "markdown");
    CHECK(LanguageDetector::detect_from_extension("file.markdown") == "markdown");
}

TEST_CASE("LanguageDetector: ExtensionDetectionPython", "[LanguageDetector]")
{
    CHECK(LanguageDetector::detect_from_extension("script.py") == "python");
    CHECK(LanguageDetector::detect_from_extension("lib.pyi") == "python");
}

TEST_CASE("LanguageDetector: ExtensionDetectionTypeScript", "[LanguageDetector]")
{
    CHECK(LanguageDetector::detect_from_extension("app.ts") == "typescript");
    CHECK(LanguageDetector::detect_from_extension("component.tsx") == "typescriptreact");
}

TEST_CASE("LanguageDetector: ExtensionDetectionJavaScript", "[LanguageDetector]")
{
    CHECK(LanguageDetector::detect_from_extension("app.js") == "javascript");
    CHECK(LanguageDetector::detect_from_extension("module.mjs") == "javascript");
    CHECK(LanguageDetector::detect_from_extension("component.jsx") == "javascriptreact");
}

TEST_CASE("LanguageDetector: ExtensionDetectionCpp", "[LanguageDetector]")
{
    CHECK(LanguageDetector::detect_from_extension("main.cpp") == "cpp");
    CHECK(LanguageDetector::detect_from_extension("header.hpp") == "cpp");
    CHECK(LanguageDetector::detect_from_extension("impl.cc") == "cpp");
}

TEST_CASE("LanguageDetector: ExtensionDetectionWeb", "[LanguageDetector]")
{
    CHECK(LanguageDetector::detect_from_extension("page.html") == "html");
    CHECK(LanguageDetector::detect_from_extension("styles.css") == "css");
    CHECK(LanguageDetector::detect_from_extension("styles.scss") == "scss");
}

TEST_CASE("LanguageDetector: ExtensionDetectionData", "[LanguageDetector]")
{
    CHECK(LanguageDetector::detect_from_extension("config.json") == "json");
    CHECK(LanguageDetector::detect_from_extension("config.yaml") == "yaml");
    CHECK(LanguageDetector::detect_from_extension("config.yml") == "yaml");
    CHECK(LanguageDetector::detect_from_extension("config.toml") == "toml");
}

TEST_CASE("LanguageDetector: ExtensionDetectionShell", "[LanguageDetector]")
{
    CHECK(LanguageDetector::detect_from_extension("script.sh") == "shellscript");
    CHECK(LanguageDetector::detect_from_extension("build.bash") == "shellscript");
    CHECK(LanguageDetector::detect_from_extension("init.zsh") == "shellscript");
}

TEST_CASE("LanguageDetector: ExtensionDetectionOther", "[LanguageDetector]")
{
    CHECK(LanguageDetector::detect_from_extension("main.rs") == "rust");
    CHECK(LanguageDetector::detect_from_extension("main.go") == "go");
    CHECK(LanguageDetector::detect_from_extension("Main.java") == "java");
    CHECK(LanguageDetector::detect_from_extension("app.rb") == "ruby");
    CHECK(LanguageDetector::detect_from_extension("app.swift") == "swift");
}

TEST_CASE("LanguageDetector: ExtensionDetectionCaseInsensitive", "[LanguageDetector]")
{
    CHECK(LanguageDetector::detect_from_extension("file.PY") == "python");
    CHECK(LanguageDetector::detect_from_extension("file.JS") == "javascript");
    CHECK(LanguageDetector::detect_from_extension("file.MD") == "markdown");
}

TEST_CASE("LanguageDetector: ExtensionDetectionUnknown", "[LanguageDetector]")
{
    CHECK(LanguageDetector::detect_from_extension("file.xyz") == "");
    CHECK(LanguageDetector::detect_from_extension("noext") == "");
    CHECK(LanguageDetector::detect_from_extension("file.") == "");
}

TEST_CASE("LanguageDetector: FrontmatterDetectionLang", "[LanguageDetector]")
{
    auto lang = LanguageDetector::detect_from_frontmatter("---\nlang: python\n---\n");
    CHECK(lang == "python");
}

TEST_CASE("LanguageDetector: FrontmatterDetectionLanguage", "[LanguageDetector]")
{
    auto lang = LanguageDetector::detect_from_frontmatter("---\nlanguage: javascript\n---\n");
    CHECK(lang == "javascript");
}

TEST_CASE("LanguageDetector: FrontmatterDetectionQuoted", "[LanguageDetector]")
{
    auto lang = LanguageDetector::detect_from_frontmatter("---\nlang: \"rust\"\n---\n");
    CHECK(lang == "rust");
}

TEST_CASE("LanguageDetector: FrontmatterDetectionSingleQuoted", "[LanguageDetector]")
{
    auto lang = LanguageDetector::detect_from_frontmatter("---\nlang: 'go'\n---\n");
    CHECK(lang == "go");
}

TEST_CASE("LanguageDetector: FrontmatterDetectionNoFrontmatter", "[LanguageDetector]")
{
    auto lang = LanguageDetector::detect_from_frontmatter("# Title\nno frontmatter");
    CHECK(lang == "");
}

TEST_CASE("LanguageDetector: FrontmatterDetectionNoLangField", "[LanguageDetector]")
{
    auto lang = LanguageDetector::detect_from_frontmatter("---\ntitle: Test\ntags: [a, b]\n---\n");
    CHECK(lang == "");
}

TEST_CASE("LanguageDetector: ShebangDetectionPython", "[LanguageDetector]")
{
    auto lang = LanguageDetector::detect_from_shebang("#!/usr/bin/python\n");
    CHECK(lang == "python");
}

TEST_CASE("LanguageDetector: ShebangDetectionEnvPython", "[LanguageDetector]")
{
    auto lang = LanguageDetector::detect_from_shebang("#!/usr/bin/env python3\n");
    CHECK(lang == "python");
}

TEST_CASE("LanguageDetector: ShebangDetectionBash", "[LanguageDetector]")
{
    auto lang = LanguageDetector::detect_from_shebang("#!/bin/bash\n");
    CHECK(lang == "shellscript");
}

TEST_CASE("LanguageDetector: ShebangDetectionNode", "[LanguageDetector]")
{
    auto lang = LanguageDetector::detect_from_shebang("#!/usr/bin/env node\n");
    CHECK(lang == "javascript");
}

TEST_CASE("LanguageDetector: ShebangDetectionNoShebang", "[LanguageDetector]")
{
    auto lang = LanguageDetector::detect_from_shebang("Hello world\nNo shebang");
    CHECK(lang == "");
}

TEST_CASE("LanguageDetector: DetectPriorityFrontmatterOverExtension", "[LanguageDetector]")
{
    LanguageDetector detector;
    auto lang = detector.detect("notes.md", "---\nlang: python\n---\n");
    CHECK(lang == "python"); // Frontmatter takes priority
}

TEST_CASE("LanguageDetector: DetectPriorityShebangOverExtension", "[LanguageDetector]")
{
    LanguageDetector detector;
    auto lang = detector.detect("script.txt", "#!/usr/bin/env ruby\n");
    CHECK(lang == "ruby"); // Shebang takes priority over extension
}

TEST_CASE("LanguageDetector: DetectFallbackToExtension", "[LanguageDetector]")
{
    LanguageDetector detector;
    auto lang = detector.detect("script.py", "");
    CHECK(lang == "python"); // Extension only
}

TEST_CASE("LanguageDetector: DetectFallbackToMarkdown", "[LanguageDetector]")
{
    LanguageDetector detector;
    auto lang = detector.detect("unknown_file", "");
    CHECK(lang == "markdown"); // Default
}

// ============================================================================
// Integration tests
// ============================================================================

TEST_CASE("CodeIntelligenceIntegration: FullDocumentPipeline", "[Integration]")
{
    // A realistic markdown document
    std::string doc =
        "---\ntitle: Test Document\ntags: [test, demo]\n---\n\n"
        "# Introduction\n\n"
        "This is a **bold** and *italic* document.\n\n"
        "## Links\n\n"
        "[Google](https://google.com)\n"
        "[[Internal Page]]\n"
        "![Image](photo.png)\n\n"
        "## Code\n\n"
        "```python\nprint('hello')\n```\n\n"
        "> A blockquote\n\n"
        "- Item 1\n- Item 2\n  - Nested\n\n"
        "| H1 | H2 | H3 |\n|---|---|---|\n| A | B | C |\n| D | E | F |\n| G | H | I |\n\n"
        "[^1]: This is a footnote\n\n"
        "<details>\n<summary>Hidden</summary>\nSecret content\n</details>";

    // Semantic tokens should detect all elements
    SemanticTokenProvider stp;
    auto tokens = stp.provide_tokens(doc);
    CHECK(static_cast<int>(tokens.size()) > 10);

    // Check for diverse token types
    bool has_frontmatter = false;
    bool has_heading = false;
    bool has_code = false;
    bool has_blockquote = false;
    bool has_link = false;
    bool has_strong = false;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kFrontmatter)
        {
            has_frontmatter = true;
        }
        if (token.token_type == SemanticTokenType::kHeading)
        {
            has_heading = true;
        }
        if (token.token_type == SemanticTokenType::kCode)
        {
            has_code = true;
        }
        if (token.token_type == SemanticTokenType::kBlockquote)
        {
            has_blockquote = true;
        }
        if (token.token_type == SemanticTokenType::kLink)
        {
            has_link = true;
        }
        if (token.token_type == SemanticTokenType::kStrong)
        {
            has_strong = true;
        }
    }
    CHECK(has_frontmatter);
    CHECK(has_heading);
    CHECK(has_code);
    CHECK(has_blockquote);
    CHECK(has_link);
    CHECK(has_strong);

    // Folding ranges should detect multiple fold types
    FoldingRangeProvider frp;
    auto folds = frp.provide_ranges(doc);
    CHECK(static_cast<int>(folds.size()) > 3);

    bool has_fm_fold = false;
    bool has_heading_fold = false;
    bool has_fence_fold = false;
    bool has_table_fold = false;
    bool has_details_fold = false;
    for (const auto& fold : folds)
    {
        if (fold.kind == FoldingRangeKind::kFrontmatter)
        {
            has_fm_fold = true;
        }
        if (fold.kind == FoldingRangeKind::kHeading)
        {
            has_heading_fold = true;
        }
        if (fold.kind == FoldingRangeKind::kCodeFence)
        {
            has_fence_fold = true;
        }
        if (fold.kind == FoldingRangeKind::kTable)
        {
            has_table_fold = true;
        }
        if (fold.kind == FoldingRangeKind::kCollapsible)
        {
            has_details_fold = true;
        }
    }
    CHECK(has_fm_fold);
    CHECK(has_heading_fold);
    CHECK(has_fence_fold);
    CHECK(has_table_fold);
    CHECK(has_details_fold);

    // Inlay hints should detect heading word count, table, etc.
    InlayHintProvider ihp;
    auto hints = ihp.provide_hints(doc);
    CHECK(static_cast<int>(hints.size()) > 2);

    // Language detection should find markdown
    LanguageDetector ld_inst;
    auto lang = ld_inst.detect("test.md", doc);
    CHECK(lang == "markdown"); // Extension, no lang: field in frontmatter
}

TEST_CASE("CodeIntelligenceIntegration: EmptyDocumentAllProviders", "[Integration]")
{
    std::string empty;

    SemanticTokenProvider stp;
    CHECK(stp.provide_tokens(empty).empty());

    FoldingRangeProvider frp;
    CHECK(frp.provide_ranges(empty).empty());

    InlayHintProvider ihp;
    CHECK(ihp.provide_hints(empty).empty());

    LanguageDetector ld_inst;
    CHECK(ld_inst.detect("", empty) == "markdown"); // Default
}

TEST_CASE("CodeIntelligenceIntegration: UnicodeContent", "[Integration]")
{
    std::string unicode_doc = "# Héllo Wörld\n\n"
                              "Some **bold** and *italic*\n\n"
                              "[[LinkTarget]]\n"
                              "[External](https://example.jp)";

    SemanticTokenProvider stp;
    auto tokens = stp.provide_tokens(unicode_doc);
    CHECK_FALSE(tokens.empty());

    // Should still detect heading
    bool found_heading = false;
    for (const auto& token : tokens)
    {
        if (token.token_type == SemanticTokenType::kHeading)
        {
            found_heading = true;
        }
    }
    CHECK(found_heading);
}

TEST_CASE("CodeIntelligenceIntegration: LargeDocument", "[Integration]")
{
    // Generate a large document
    std::string large_doc;
    for (int idx = 0; idx < 100; ++idx)
    {
        large_doc += "# Section " + std::to_string(idx) + "\n";
        large_doc += "Some content for section " + std::to_string(idx) + ".\n";
        large_doc += "**Bold** and *italic* text.\n\n";
    }

    SemanticTokenProvider stp;
    auto tokens = stp.provide_tokens(large_doc);
    CHECK(static_cast<int>(tokens.size()) > 200);

    FoldingRangeProvider frp;
    auto folds = frp.provide_ranges(large_doc);
    CHECK(static_cast<int>(folds.size()) >= 99); // 100 headings, last one folds to end

    LanguageDetector ld_inst;
    CHECK(ld_inst.detect("large.md", large_doc) == "markdown");
}

TEST_CASE("CodeIntelligenceIntegration: MalformedMarkdown", "[Integration]")
{
    // Edge cases: unclosed fences, invalid headings, etc.
    std::string malformed = "```\nunclosed fence\n"
                            "# Not a heading (inside unclosed fence)\n"
                            "####### Invalid heading level\n"
                            "[unclosed link(url\n"
                            "![unclosed image(file.png";

    SemanticTokenProvider stp;
    auto tokens = stp.provide_tokens(malformed);
    // Should not crash, just produce whatever tokens it can
    CHECK_FALSE(tokens.empty()); // At least the ``` opener

    FoldingRangeProvider frp;
    auto folds = frp.provide_ranges(malformed);
    // Should not crash with malformed input
    // No specific assertion on results since behavior for malformed is best-effort
    (void)folds;
}
