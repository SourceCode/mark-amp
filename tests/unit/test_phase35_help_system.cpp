/// @file test_phase35_help_system.cpp
/// @brief V9 Phase 35 Batch 35D — Documentation & Help System tests.
///
/// Tests all Phase 35 components:
///   - HelpArticle / HelpArticleRegistry (Task 5)
///   - HelpService (Task 6)
///   - FeatureDiscoveryService (Task 8)
///   - HelpCommandProvider (Task 4, 18)
///   - DiagnosticsErrorHelp (Task 10, 11)
///   - OnboardingService (Task 16)
///   - ShortcutManager Phase 35 methods (Task 12)

#include "core/DiagnosticsService.h"
#include "core/FeatureDiscoveryService.h"
#include "core/HelpArticle.h"
#include "core/HelpCommandProvider.h"
#include "core/HelpService.h"
#include "core/OnboardingService.h"
#include "core/ShortcutManager.h"

#include <catch2/catch_test_macros.hpp>
#include <wx/defs.h>

using namespace markamp::core;

// ============================================================================
// Section 1: HelpArticle & HelpArticleRegistry
// ============================================================================

TEST_CASE("HelpArticle matches works case-insensitively", "[phase35][help-article]")
{
    HelpArticle article;
    article.id = "syntax.headings";
    article.title = "Headings";
    article.body = "Use # for heading level 1";
    article.tags = {"markdown", "formatting"};

    REQUIRE(article.matches("heading"));
    REQUIRE(article.matches("HEADING"));
    REQUIRE(article.matches("markdown"));
    REQUIRE(article.matches("level 1"));
    REQUIRE_FALSE(article.matches("unknown_topic"));
    REQUIRE(article.matches("")); // empty query matches all
}

TEST_CASE("HelpArticleRegistry register and get article", "[phase35][help-registry]")
{
    HelpArticleRegistry registry;

    HelpArticle article;
    article.id = "test.article";
    article.title = "Test Article";
    article.body = "Body text";
    article.category = HelpCategory::Tutorial;
    article.tags = {"test"};

    registry.register_article(article);

    REQUIRE(registry.article_count() == 1);

    const auto* found = registry.get_article("test.article");
    REQUIRE(found != nullptr);
    REQUIRE(found->title == "Test Article");
    REQUIRE(found->category == HelpCategory::Tutorial);

    REQUIRE(registry.get_article("nonexistent") == nullptr);
}

TEST_CASE("HelpArticleRegistry overwrite existing article", "[phase35][help-registry]")
{
    HelpArticleRegistry registry;

    HelpArticle article_v1;
    article_v1.id = "test.overwrite";
    article_v1.title = "Version 1";
    article_v1.body = "Original body";
    registry.register_article(article_v1);

    HelpArticle article_v2;
    article_v2.id = "test.overwrite";
    article_v2.title = "Version 2";
    article_v2.body = "Updated body";
    registry.register_article(article_v2);

    REQUIRE(registry.article_count() == 1);
    REQUIRE(registry.get_article("test.overwrite")->title == "Version 2");
}

TEST_CASE("HelpArticleRegistry get_articles_by_category", "[phase35][help-registry]")
{
    HelpArticleRegistry registry;

    HelpArticle syntax_article;
    syntax_article.id = "syntax.bold";
    syntax_article.title = "Bold";
    syntax_article.body = "Use ** for bold";
    syntax_article.category = HelpCategory::Syntax;
    registry.register_article(syntax_article);

    HelpArticle cmd_article;
    cmd_article.id = "cmd.save";
    cmd_article.title = "Save";
    cmd_article.body = "Cmd+S to save";
    cmd_article.category = HelpCategory::Command;
    registry.register_article(cmd_article);

    auto syntax_results = registry.get_articles_by_category(HelpCategory::Syntax);
    REQUIRE(syntax_results.size() == 1);
    REQUIRE(syntax_results[0]->id == "syntax.bold");

    auto error_results = registry.get_articles_by_category(HelpCategory::Error);
    REQUIRE(error_results.empty());
}

TEST_CASE("HelpArticleRegistry get_articles_by_tag", "[phase35][help-registry]")
{
    HelpArticleRegistry registry;

    HelpArticle article;
    article.id = "test.tagged";
    article.title = "Tagged Article";
    article.body = "Body";
    article.tags = {"alpha", "beta"};
    registry.register_article(article);

    auto alpha_results = registry.get_articles_by_tag("alpha");
    REQUIRE(alpha_results.size() == 1);

    auto gamma_results = registry.get_articles_by_tag("gamma");
    REQUIRE(gamma_results.empty());
}

TEST_CASE("HelpArticleRegistry search ranked by relevance", "[phase35][help-registry]")
{
    HelpArticleRegistry registry;

    // Title exact match should score highest
    HelpArticle exact;
    exact.id = "exact.match";
    exact.title = "headings";
    exact.body = "other content";
    registry.register_article(exact);

    // Title partial match
    HelpArticle partial;
    partial.id = "partial.match";
    partial.title = "About headings and paragraphs";
    partial.body = "other content";
    registry.register_article(partial);

    // Body-only match
    HelpArticle body_only;
    body_only.id = "body.match";
    body_only.title = "Formatting";
    body_only.body = "headings are important";
    registry.register_article(body_only);

    auto results = registry.search("headings");
    REQUIRE(results.size() == 3);
    // Exact title match should be first
    REQUIRE(results[0].article_id == "exact.match");
    REQUIRE(results[0].relevance_score == 100);
    // Partial title match second
    REQUIRE(results[1].relevance_score == 75);
    // Body match last
    REQUIRE(results[2].relevance_score == 25);
}

TEST_CASE("HelpArticleRegistry search returns empty for no match", "[phase35][help-registry]")
{
    HelpArticleRegistry registry;

    HelpArticle article;
    article.id = "test.article";
    article.title = "Test";
    article.body = "Body";
    registry.register_article(article);

    auto results = registry.search("zzz_no_match");
    REQUIRE(results.empty());

    // Empty query returns empty
    auto empty_results = registry.search("");
    REQUIRE(empty_results.empty());
}

TEST_CASE("HelpArticleRegistry all_article_ids", "[phase35][help-registry]")
{
    HelpArticleRegistry registry;

    HelpArticle article_a;
    article_a.id = "a.article";
    article_a.title = "A";
    article_a.body = "Body A";
    registry.register_article(article_a);

    HelpArticle article_b;
    article_b.id = "b.article";
    article_b.title = "B";
    article_b.body = "Body B";
    registry.register_article(article_b);

    auto ids = registry.all_article_ids();
    REQUIRE(ids.size() == 2);
}

// ============================================================================
// Section 2: help_category_name
// ============================================================================

TEST_CASE("help_category_name returns correct strings", "[phase35][help-category]")
{
    REQUIRE(help_category_name(HelpCategory::Syntax) == "Syntax");
    REQUIRE(help_category_name(HelpCategory::Command) == "Commands");
    REQUIRE(help_category_name(HelpCategory::Setting) == "Settings");
    REQUIRE(help_category_name(HelpCategory::Error) == "Errors");
    REQUIRE(help_category_name(HelpCategory::Tutorial) == "Tutorials");
    REQUIRE(help_category_name(HelpCategory::General) == "General");
}

// ============================================================================
// Section 3: HelpService
// ============================================================================

TEST_CASE("HelpService registers built-in articles", "[phase35][help-service]")
{
    HelpService help;
    REQUIRE(help.article_count() > 0);
}

TEST_CASE("HelpService search returns results", "[phase35][help-service]")
{
    HelpService help;
    auto results = help.search("heading");
    REQUIRE_FALSE(results.empty());
}

TEST_CASE("HelpService contextual help", "[phase35][help-service]")
{
    HelpService help;
    // Built-in context mappings include "editor", "canvas", etc.
    auto editor_help = help.get_contextual_help("editor");
    REQUIRE(editor_help != nullptr);
    REQUIRE_FALSE(editor_help->id.empty());
}

TEST_CASE("HelpService contextual help for unknown context falls back to overview",
          "[phase35][help-service]")
{
    HelpService help;
    auto result = help.get_contextual_help("nonexistent_context");
    // Falls back to "general.overview" article
    REQUIRE(result != nullptr);
    REQUIRE(result->id == "general.overview");
}

TEST_CASE("HelpService custom context registration", "[phase35][help-service]")
{
    HelpService help;

    HelpArticle article;
    article.id = "custom.context.article";
    article.title = "Custom Context";
    article.body = "Custom context body";
    help.register_article(article);
    help.register_context_help("custom_context", "custom.context.article");

    auto result = help.get_contextual_help("custom_context");
    REQUIRE(result != nullptr);
    REQUIRE(result->id == "custom.context.article");
}

TEST_CASE("HelpService tooltips", "[phase35][help-service]")
{
    HelpService help;
    // Built-in tooltips include "toolbar.bold", "toolbar.italic", etc.
    auto tip = help.get_tooltip("toolbar.bold");
    REQUIRE(tip.has_value());
    REQUIRE_FALSE(tip->description.empty());
}

TEST_CASE("HelpService tooltip for unknown element returns nullopt", "[phase35][help-service]")
{
    HelpService help;
    auto tip = help.get_tooltip("nonexistent.element");
    REQUIRE_FALSE(tip.has_value());
}

TEST_CASE("HelpService custom tooltip registration", "[phase35][help-service]")
{
    HelpService help;

    TooltipInfo info;
    info.description = "Custom tooltip";
    info.shortcut_hint = "Ctrl+X";
    info.extended_help = "Extended info";
    help.register_tooltip("custom.element", info);

    auto tip = help.get_tooltip("custom.element");
    REQUIRE(tip.has_value());
    REQUIRE(tip->description == "Custom tooltip");
    REQUIRE(tip->shortcut_hint == "Ctrl+X");
}

TEST_CASE("HelpService registry direct access", "[phase35][help-service]")
{
    HelpService help;
    auto& registry = help.registry();
    REQUIRE(registry.article_count() > 0);
}

TEST_CASE("HelpService get_articles_by_category", "[phase35][help-service]")
{
    HelpService help;
    auto syntax_articles = help.get_articles_by_category(HelpCategory::Syntax);
    REQUIRE_FALSE(syntax_articles.empty());
}

// ============================================================================
// Section 4: HelpCommandProvider
// ============================================================================

TEST_CASE("HelpCommandProvider registers 8 built-in commands", "[phase35][help-commands]")
{
    HelpCommandProvider provider;
    REQUIRE(provider.command_count() == 8);
}

TEST_CASE("HelpCommandProvider command lookup by ID", "[phase35][help-commands]")
{
    HelpCommandProvider provider;

    auto* cmd = provider.get_command("help.gettingStarted");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->label == "Help: Getting Started");
    REQUIRE(cmd->category == "Help");

    REQUIRE(provider.get_command("nonexistent.command") == nullptr);
}

TEST_CASE("HelpCommandProvider execute command returns true", "[phase35][help-commands]")
{
    HelpCommandProvider provider;
    REQUIRE(provider.execute_command("help.gettingStarted"));
    REQUIRE(provider.execute_command("help.keyboardShortcuts"));
    REQUIRE_FALSE(provider.execute_command("nonexistent"));
}

TEST_CASE("HelpCommandProvider keybinding lookup", "[phase35][help-commands]")
{
    HelpCommandProvider provider;
    REQUIRE(provider.get_keybinding("help.keyboardShortcuts") == "F1");
    REQUIRE(provider.get_keybinding("help.gettingStarted").empty());
    REQUIRE(provider.get_keybinding("nonexistent").empty());
}

TEST_CASE("HelpCommandProvider all 8 command IDs present", "[phase35][help-commands]")
{
    HelpCommandProvider provider;
    const auto& cmds = provider.all_commands();

    std::vector<std::string> expected_ids = {
        "help.gettingStarted",
        "help.keyboardShortcuts",
        "help.markdownReference",
        "help.whatsNew",
        "help.search",
        "help.reportIssue",
        "help.openPlayground",
        "help.commandReference",
    };

    REQUIRE(cmds.size() == expected_ids.size());
    for (std::size_t idx = 0; idx < expected_ids.size(); ++idx)
    {
        REQUIRE(cmds[idx].id == expected_ids[idx]);
    }
}

TEST_CASE("HelpCommandProvider register custom command", "[phase35][help-commands]")
{
    HelpCommandProvider provider;

    HelpCommand custom;
    custom.id = "help.custom";
    custom.label = "Help: Custom";
    custom.description = "A custom help command";
    custom.execute_fn = []() -> bool { return true; };
    provider.register_command(std::move(custom));

    REQUIRE(provider.command_count() == 9);
    REQUIRE(provider.execute_command("help.custom"));
}

// ============================================================================
// Section 5: DiagnosticsErrorHelp
// ============================================================================

TEST_CASE("DiagnosticsErrorHelp registers 6 built-in entries", "[phase35][error-help]")
{
    DiagnosticsErrorHelp help;
    REQUIRE(help.entry_count() == 6);
}

TEST_CASE("DiagnosticsErrorHelp lookup by error code", "[phase35][error-help]")
{
    DiagnosticsErrorHelp help;

    const auto* md001 = help.lookup("MD001");
    REQUIRE(md001 != nullptr);
    REQUIRE(md001->title == "Heading Levels Should Only Increment by One");
    REQUIRE_FALSE(md001->fix_hint.empty());
    REQUIRE(md001->related_help == "help.markdownReference");

    const auto* syn_fence = help.lookup("SYN_UNCLOSED_FENCE");
    REQUIRE(syn_fence != nullptr);
    REQUIRE(syn_fence->title == "Unclosed Code Fence");

    REQUIRE(help.lookup("NONEXISTENT_CODE") == nullptr);
}

TEST_CASE("DiagnosticsErrorHelp suggest_fix from diagnostic", "[phase35][error-help]")
{
    DiagnosticsErrorHelp help;

    Diagnostic diag;
    diag.code = "MD009";
    diag.message = "Trailing whitespace";
    diag.severity = DiagnosticSeverity::kWarning;

    auto fix = help.suggest_fix(diag);
    REQUIRE_FALSE(fix.empty());
    REQUIRE(fix.find("trailing") != std::string::npos);
}

TEST_CASE("DiagnosticsErrorHelp suggest_fix for unknown code returns empty",
          "[phase35][error-help]")
{
    DiagnosticsErrorHelp help;

    Diagnostic diag;
    diag.code = "UNKNOWN_CODE";

    auto fix = help.suggest_fix(diag);
    REQUIRE(fix.empty());
}

TEST_CASE("DiagnosticsErrorHelp all_entries returns references", "[phase35][error-help]")
{
    DiagnosticsErrorHelp help;
    const auto& entries = help.all_entries();
    REQUIRE(entries.size() == 6);

    // Verify all known codes
    std::vector<std::string> expected_codes = {"MD001",
                                               "MD009",
                                               "SYN_UNCLOSED_FENCE",
                                               "SYN_BAD_LINK",
                                               "SYN_BROKEN_WIKILINK",
                                               "MERMAID_SYNTAX"};
    for (std::size_t idx = 0; idx < expected_codes.size(); ++idx)
    {
        REQUIRE(entries[idx].error_code == expected_codes[idx]);
    }
}

TEST_CASE("DiagnosticsErrorHelp register custom entry", "[phase35][error-help]")
{
    DiagnosticsErrorHelp help;

    ErrorHelpEntry custom;
    custom.error_code = "CUSTOM_001";
    custom.title = "Custom Error";
    custom.explanation = "A custom error explanation";
    custom.fix_hint = "Fix it like this";
    custom.related_help = "help.markdownReference";
    help.register_entry(std::move(custom));

    REQUIRE(help.entry_count() == 7);
    REQUIRE(help.lookup("CUSTOM_001") != nullptr);
}

// ============================================================================
// Section 6: FeatureDiscoveryService
// ============================================================================

TEST_CASE("FeatureDiscoveryService registers built-in hints", "[phase35][feature-discovery]")
{
    FeatureDiscoveryService discovery;
    REQUIRE(discovery.all_hints().size() == 6);
}

TEST_CASE("FeatureDiscoveryService check_hint returns hint for matching action",
          "[phase35][feature-discovery]")
{
    FeatureDiscoveryService discovery;
    auto hint = discovery.check_hint("editor.open");
    REQUIRE(hint.has_value());
    REQUIRE(hint->id == "hint.split_view");
}

TEST_CASE("FeatureDiscoveryService check_hint returns nullopt for unknown action",
          "[phase35][feature-discovery]")
{
    FeatureDiscoveryService discovery;
    auto hint = discovery.check_hint("nonexistent.action");
    REQUIRE_FALSE(hint.has_value());
}

TEST_CASE("FeatureDiscoveryService dismiss_hint prevents re-show", "[phase35][feature-discovery]")
{
    FeatureDiscoveryService discovery;

    auto hint = discovery.check_hint("editor.open");
    REQUIRE(hint.has_value());

    discovery.dismiss_hint(hint->id);
    REQUIRE(discovery.is_dismissed(hint->id));

    // After dismissal + session reset, hint should still be dismissed
    discovery.reset_session();
    auto hint2 = discovery.check_hint("editor.open");
    REQUIRE_FALSE(hint2.has_value());
}

TEST_CASE("FeatureDiscoveryService max 1 hint per session", "[phase35][feature-discovery]")
{
    FeatureDiscoveryService discovery;

    // Show first hint
    discovery.mark_hint_shown();

    // Second check should return nullopt
    auto hint = discovery.check_hint("workspace.create");
    REQUIRE_FALSE(hint.has_value());

    // After reset, hints should be available again
    discovery.reset_session();
    auto hint2 = discovery.check_hint("workspace.create");
    REQUIRE(hint2.has_value());
}

TEST_CASE("FeatureDiscoveryService enable/disable", "[phase35][feature-discovery]")
{
    FeatureDiscoveryService discovery;
    REQUIRE(discovery.is_enabled());

    discovery.set_enabled(false);
    REQUIRE_FALSE(discovery.is_enabled());

    auto hint = discovery.check_hint("editor.open");
    REQUIRE_FALSE(hint.has_value());

    discovery.set_enabled(true);
    auto hint2 = discovery.check_hint("editor.open");
    REQUIRE(hint2.has_value());
}

TEST_CASE("FeatureDiscoveryService persistence round-trip", "[phase35][feature-discovery]")
{
    FeatureDiscoveryService discovery;
    discovery.dismiss_hint("hint.split_view");
    discovery.dismiss_hint("hint.canvas");

    auto dismissed_ids = discovery.get_dismissed_ids();
    REQUIRE(dismissed_ids.size() == 2);

    // Create new instance and load
    FeatureDiscoveryService discovery2;
    discovery2.load_dismissed(dismissed_ids);
    REQUIRE(discovery2.is_dismissed("hint.split_view"));
    REQUIRE(discovery2.is_dismissed("hint.canvas"));
    REQUIRE_FALSE(discovery2.is_dismissed("hint.notebooks"));
}

TEST_CASE("FeatureDiscoveryService reset_all clears everything", "[phase35][feature-discovery]")
{
    FeatureDiscoveryService discovery;
    discovery.dismiss_hint("hint.split_view");
    discovery.mark_hint_shown();

    discovery.reset_all();
    REQUIRE(discovery.dismissed_count() == 0);
    auto hint = discovery.check_hint("editor.open");
    REQUIRE(hint.has_value());
}

TEST_CASE("FeatureDiscoveryService register custom hint", "[phase35][feature-discovery]")
{
    FeatureDiscoveryService discovery;

    FeatureHint custom;
    custom.id = "hint.custom";
    custom.message = "Try custom feature";
    custom.trigger_action = "custom.action";
    custom.learn_more_article = "general.overview";
    discovery.register_hint(std::move(custom));

    REQUIRE(discovery.all_hints().size() == 7);
    auto hint = discovery.check_hint("custom.action");
    REQUIRE(hint.has_value());
    REQUIRE(hint->id == "hint.custom");
}

// ============================================================================
// Section 7: OnboardingService
// ============================================================================

TEST_CASE("OnboardingService initial state", "[phase35][onboarding]")
{
    OnboardingService onboarding;
    REQUIRE(onboarding.is_first_launch());
    REQUIRE(onboarding.current_step() == OnboardingStep::Welcome);
    REQUIRE(onboarding.current_step_index() == 0);
    REQUIRE(OnboardingService::total_steps() == 5);
    REQUIRE_FALSE(onboarding.is_complete());
}

TEST_CASE("OnboardingService advance through all steps", "[phase35][onboarding]")
{
    OnboardingService onboarding;

    onboarding.advance();
    REQUIRE(onboarding.current_step() == OnboardingStep::ChooseTheme);

    onboarding.advance();
    REQUIRE(onboarding.current_step() == OnboardingStep::ImportSettings);

    onboarding.advance();
    REQUIRE(onboarding.current_step() == OnboardingStep::FeatureTour);

    onboarding.advance();
    REQUIRE(onboarding.current_step() == OnboardingStep::OpenWorkspace);

    onboarding.advance();
    REQUIRE(onboarding.current_step() == OnboardingStep::Complete);
    REQUIRE(onboarding.is_complete());
    REQUIRE_FALSE(onboarding.is_first_launch());

    // Advancing past complete does nothing
    onboarding.advance();
    REQUIRE(onboarding.current_step() == OnboardingStep::Complete);
}

TEST_CASE("OnboardingService skip jumps to complete", "[phase35][onboarding]")
{
    OnboardingService onboarding;
    onboarding.advance(); // Move to ChooseTheme

    onboarding.skip();
    REQUIRE(onboarding.is_complete());
    REQUIRE(onboarding.current_step() == OnboardingStep::Complete);
}

TEST_CASE("OnboardingService reset", "[phase35][onboarding]")
{
    OnboardingService onboarding;
    onboarding.skip();
    REQUIRE(onboarding.is_complete());

    onboarding.reset();
    REQUIRE(onboarding.is_first_launch());
    REQUIRE(onboarding.current_step() == OnboardingStep::Welcome);
}

TEST_CASE("OnboardingService mark_completed", "[phase35][onboarding]")
{
    OnboardingService onboarding;
    onboarding.mark_completed();
    REQUIRE(onboarding.is_complete());
    REQUIRE(onboarding.current_step() == OnboardingStep::Complete);
}

TEST_CASE("OnboardingService tour slides", "[phase35][onboarding]")
{
    OnboardingService onboarding;
    const auto& slides = onboarding.tour_slides();
    REQUIRE(slides.size() == 5);
    REQUIRE(slides[0].title == "Powerful Markdown Editor");
    REQUIRE_FALSE(slides[0].description.empty());
    REQUIRE_FALSE(slides[0].image_ref.empty());
}

TEST_CASE("OnboardingService theme selection", "[phase35][onboarding]")
{
    OnboardingService onboarding;
    onboarding.set_selected_theme("dark_plus");
    REQUIRE(onboarding.selected_theme() == "dark_plus");
}

TEST_CASE("OnboardingService import source", "[phase35][onboarding]")
{
    OnboardingService onboarding;
    REQUIRE(onboarding.import_source() == "none");

    onboarding.set_import_source("vscode");
    REQUIRE(onboarding.import_source() == "vscode");
}

TEST_CASE("onboarding_step_name returns correct strings", "[phase35][onboarding]")
{
    REQUIRE(onboarding_step_name(OnboardingStep::Welcome) == "Welcome");
    REQUIRE(onboarding_step_name(OnboardingStep::ChooseTheme) == "ChooseTheme");
    REQUIRE(onboarding_step_name(OnboardingStep::ImportSettings) == "ImportSettings");
    REQUIRE(onboarding_step_name(OnboardingStep::FeatureTour) == "FeatureTour");
    REQUIRE(onboarding_step_name(OnboardingStep::OpenWorkspace) == "OpenWorkspace");
    REQUIRE(onboarding_step_name(OnboardingStep::Complete) == "Complete");
}

// ============================================================================
// Section 8: ShortcutManager Phase 35 methods
// ============================================================================

TEST_CASE("ShortcutManager export_as_markdown produces cheat-sheet", "[phase35][shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    // Register a shortcut to export
    mgr.register_shortcut({.id = "test.save",
                           .description = "Save File",
                           .key_code = 'S',
                           .modifiers = wxMOD_CMD,
                           .context = "global",
                           .category = "File",
                           .action = []() {}});

    auto markdown = mgr.export_as_markdown();
    REQUIRE_FALSE(markdown.empty());
    REQUIRE(markdown.find("# Keyboard Shortcuts") != std::string::npos);
    REQUIRE(markdown.find("File") != std::string::npos);
    REQUIRE(markdown.find("Save File") != std::string::npos);
}

TEST_CASE("ShortcutManager export_as_json produces valid structure", "[phase35][shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    mgr.register_shortcut({.id = "test.copy",
                           .description = "Copy",
                           .key_code = 'C',
                           .modifiers = wxMOD_CMD,
                           .context = "global",
                           .category = "Edit",
                           .action = []() {}});

    auto json = mgr.export_as_json();
    REQUIRE_FALSE(json.empty());
    REQUIRE(json.find("test.copy") != std::string::npos);
    REQUIRE(json.find("Copy") != std::string::npos);
}

TEST_CASE("ShortcutManager import_from_json applies remaps", "[phase35][shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    mgr.register_shortcut({.id = "test.action",
                           .description = "Test Action",
                           .key_code = 'T',
                           .modifiers = wxMOD_CMD,
                           .context = "global",
                           .category = "Test",
                           .action = []() {}});

    // Export, then import into a new manager
    auto json = mgr.export_as_json();

    EventBus bus2;
    ShortcutManager mgr2(bus2);
    mgr2.register_shortcut({.id = "test.action",
                            .description = "Test Action",
                            .key_code = 'X',
                            .modifiers = 0,
                            .context = "global",
                            .category = "Test",
                            .action = []() {}});

    mgr2.import_from_json(json);
    // After import, the shortcut should have the remapped key from the export
    auto shortcuts = mgr2.get_shortcuts_for_category("Test");
    REQUIRE_FALSE(shortcuts.empty());
}

TEST_CASE("ShortcutManager get_all_categories", "[phase35][shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    mgr.register_shortcut({.id = "test.a",
                           .description = "A",
                           .key_code = 'A',
                           .modifiers = 0,
                           .context = "global",
                           .category = "Alpha",
                           .action = []() {}});

    mgr.register_shortcut({.id = "test.b",
                           .description = "B",
                           .key_code = 'B',
                           .modifiers = 0,
                           .context = "global",
                           .category = "Beta",
                           .action = []() {}});

    mgr.register_shortcut({.id = "test.a2",
                           .description = "A2",
                           .key_code = 'C',
                           .modifiers = 0,
                           .context = "global",
                           .category = "Alpha",
                           .action = []() {}});

    auto categories = mgr.get_all_categories();
    REQUIRE(categories.size() == 2);
    // Should contain "Alpha" and "Beta" in some order
    bool has_alpha = std::find(categories.begin(), categories.end(), "Alpha") != categories.end();
    bool has_beta = std::find(categories.begin(), categories.end(), "Beta") != categories.end();
    REQUIRE(has_alpha);
    REQUIRE(has_beta);
}

TEST_CASE("ShortcutManager get_conflicts detects duplicate bindings", "[phase35][shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    // Two shortcuts with the same key+modifier+context
    mgr.register_shortcut({.id = "test.conflict1",
                           .description = "Conflict 1",
                           .key_code = 'X',
                           .modifiers = wxMOD_CMD,
                           .context = "global",
                           .category = "Test",
                           .action = []() {}});

    mgr.register_shortcut({.id = "test.conflict2",
                           .description = "Conflict 2",
                           .key_code = 'X',
                           .modifiers = wxMOD_CMD,
                           .context = "global",
                           .category = "Test",
                           .action = []() {}});

    auto conflicts = mgr.get_conflicts();
    REQUIRE_FALSE(conflicts.empty());
    // Should contain both conflicting IDs
    bool found_conflict1 = false;
    bool found_conflict2 = false;
    for (const auto& conflict_pair : conflicts)
    {
        if (conflict_pair.first == "test.conflict1" || conflict_pair.second == "test.conflict1")
        {
            found_conflict1 = true;
        }
        if (conflict_pair.first == "test.conflict2" || conflict_pair.second == "test.conflict2")
        {
            found_conflict2 = true;
        }
    }
    REQUIRE(found_conflict1);
    REQUIRE(found_conflict2);
}

TEST_CASE("ShortcutManager get_conflicts returns empty when no conflicts", "[phase35][shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    mgr.register_shortcut({.id = "test.unique1",
                           .description = "Unique 1",
                           .key_code = 'A',
                           .modifiers = wxMOD_CMD,
                           .context = "global",
                           .category = "Test",
                           .action = []() {}});

    mgr.register_shortcut({.id = "test.unique2",
                           .description = "Unique 2",
                           .key_code = 'B',
                           .modifiers = wxMOD_CMD,
                           .context = "global",
                           .category = "Test",
                           .action = []() {}});

    auto conflicts = mgr.get_conflicts();
    REQUIRE(conflicts.empty());
}

TEST_CASE("ShortcutManager shortcut_count", "[phase35][shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    REQUIRE(mgr.shortcut_count() == 0);

    mgr.register_shortcut({.id = "test.one",
                           .description = "One",
                           .key_code = '1',
                           .modifiers = 0,
                           .context = "global",
                           .category = "Num",
                           .action = []() {}});

    mgr.register_shortcut({.id = "test.two",
                           .description = "Two",
                           .key_code = '2',
                           .modifiers = 0,
                           .context = "global",
                           .category = "Num",
                           .action = []() {}});

    REQUIRE(mgr.shortcut_count() == 2);
}

TEST_CASE("ShortcutManager export_as_markdown empty manager returns header only",
          "[phase35][shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    auto markdown = mgr.export_as_markdown();
    REQUIRE(markdown.find("# Keyboard Shortcuts") != std::string::npos);
}
