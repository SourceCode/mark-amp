/// @file test_code_actions_v2.cpp
/// @brief V13 Phase 29 — Unit tests for CodeActionSet, extended CodeActionProvider,
///        LightbulbWidget logic, and code action events.

#include "core/CodeActionProvider.h"
#include "core/CodeActionTypes.h"
#include "core/DiagnosticsService.h"
#include "core/Events.h"
#include "ui/LightbulbWidget.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ──────────────────────────────────────────────────────────────────────────────
// CodeActionSet
// ──────────────────────────────────────────────────────────────────────────────

TEST_CASE("CodeActionSet — add categorizes by kind", "[code_actions][phase29]")
{
    CodeActionSet set;

    CodeActionInfo quick_fix;
    quick_fix.title = "Fix trailing whitespace";
    quick_fix.kind = CodeActionKind::kQuickFix;
    set.add(quick_fix);

    CodeActionInfo refactor;
    refactor.title = "Extract heading";
    refactor.kind = CodeActionKind::kRefactorExtract;
    set.add(refactor);

    CodeActionInfo source;
    source.title = "Organize links";
    source.kind = CodeActionKind::kSource;
    set.add(source);

    REQUIRE(set.quick_fixes.size() == 1);
    REQUIRE(set.refactors.size() == 1);
    REQUIRE(set.source_actions.size() == 1);
    REQUIRE(set.total_count() == 3);
}

TEST_CASE("CodeActionSet — empty set", "[code_actions][phase29]")
{
    CodeActionSet set;
    REQUIRE(set.total_count() == 0);
    REQUIRE(set.quick_fixes.empty());
    REQUIRE(set.refactors.empty());
    REQUIRE(set.source_actions.empty());
    REQUIRE(set.preferred_action() == nullptr);
}

TEST_CASE("CodeActionSet — preferred action returns first is_preferred", "[code_actions][phase29]")
{
    CodeActionSet set;

    CodeActionInfo action1;
    action1.title = "Non-preferred";
    action1.kind = CodeActionKind::kQuickFix;
    action1.is_preferred = false;
    set.add(action1);

    CodeActionInfo action2;
    action2.title = "Preferred fix";
    action2.kind = CodeActionKind::kQuickFix;
    action2.is_preferred = true;
    set.add(action2);

    const auto* pref = set.preferred_action();
    REQUIRE(pref != nullptr);
    REQUIRE(pref->title == "Preferred fix");
}

TEST_CASE("CodeActionSet — has_preferred", "[code_actions][phase29]")
{
    CodeActionSet set;

    CodeActionInfo action;
    action.title = "Not preferred";
    action.kind = CodeActionKind::kRefactor;
    action.is_preferred = false;
    set.add(action);

    REQUIRE_FALSE(set.has_preferred());

    CodeActionInfo preferred;
    preferred.title = "Preferred";
    preferred.kind = CodeActionKind::kSource;
    preferred.is_preferred = true;
    set.add(preferred);

    REQUIRE(set.has_preferred());
}

TEST_CASE("CodeActionSet — ordered_actions puts preferred first", "[code_actions][phase29]")
{
    CodeActionSet set;

    CodeActionInfo action1;
    action1.title = "First";
    action1.kind = CodeActionKind::kQuickFix;
    set.add(action1);

    CodeActionInfo action2;
    action2.title = "Preferred";
    action2.kind = CodeActionKind::kRefactor;
    action2.is_preferred = true;
    set.add(action2);

    CodeActionInfo action3;
    action3.title = "Third";
    action3.kind = CodeActionKind::kSource;
    set.add(action3);

    auto ordered = set.all_actions();
    REQUIRE(ordered.size() == 3);
    REQUIRE(ordered[0]->title == "Preferred");
}

TEST_CASE("CodeActionSet — preview_text shows diff", "[code_actions][phase29]")
{
    CodeActionSet set;

    CodeActionInfo action;
    action.title = "Replace line";
    action.kind = CodeActionKind::kQuickFix;

    CodeActionEdit edit;
    edit.start_line = 0;
    edit.start_char = 0;
    edit.end_line = 0;
    edit.end_char = 10;
    edit.new_text = "new content";
    action.edits.push_back(edit);
    set.add(action);

    const std::string content = "old text here\nsecond line";
    auto preview = CodeActionSet::preview_text(set.quick_fixes[0], content);
    REQUIRE_FALSE(preview.empty());
    REQUIRE(preview.find("+ new content") != std::string::npos);
}

// ──────────────────────────────────────────────────────────────────────────────
// CodeActionProvider — provide_action_set
// ──────────────────────────────────────────────────────────────────────────────

TEST_CASE("CodeActionProvider — provide_action_set returns categorized set",
          "[code_actions][phase29]")
{
    DiagnosticsService diag_service;
    CodeActionProvider provider(diag_service);

    std::string content = "# Heading\n\n### Skip level heading\n\nsome text   \n";
    auto set = provider.provide_action_set(content, 2, 0, 2, 21);

    // Should have at least the heading hierarchy fix
    REQUIRE(set.total_count() > 0);
}

TEST_CASE("CodeActionProvider — trailing whitespace action", "[code_actions][phase29]")
{
    DiagnosticsService diag_service;
    CodeActionProvider provider(diag_service);

    std::string content = "hello world   \nsecond line";
    auto set = provider.provide_action_set(content, 0, 0, 0, 14);

    bool found = false;
    for (const auto& action : set.source_actions)
    {
        if (action.title.find("trailing whitespace") != std::string::npos)
        {
            found = true;
            break;
        }
    }
    REQUIRE(found);
}

TEST_CASE("CodeActionProvider — extract heading action on selection", "[code_actions][phase29]")
{
    DiagnosticsService diag_service;
    CodeActionProvider provider(diag_service);

    std::string content = "# Main Heading\n\nSome paragraph text\n";
    auto set = provider.provide_action_set(content, 2, 0, 2, 19);

    bool found_extract = false;
    for (const auto& action : set.refactors)
    {
        if (action.title.find("Extract to H") != std::string::npos)
        {
            found_extract = true;
            break;
        }
    }
    REQUIRE(found_extract);
}

TEST_CASE("CodeActionProvider — link conversion action", "[code_actions][phase29]")
{
    DiagnosticsService diag_service;
    CodeActionProvider provider(diag_service);

    std::string content = "Click [here](https://example.com) for more info.\n";
    auto set = provider.provide_action_set(content, 0, 0, 0, 0);

    bool found_convert = false;
    for (const auto& action : set.refactors)
    {
        if (action.title.find("Convert to reference link") != std::string::npos)
        {
            found_convert = true;
            REQUIRE(action.edits.size() == 2);
            break;
        }
    }
    REQUIRE(found_convert);
}

TEST_CASE("CodeActionProvider — organize links action", "[code_actions][phase29]")
{
    DiagnosticsService diag_service;
    CodeActionProvider provider(diag_service);

    std::string content = "text\n[zebra]: http://z.com\n[alpha]: http://a.com\n";
    auto set = provider.provide_action_set(content, 0, 0, 0, 0);

    bool found_organize = false;
    for (const auto& action : set.source_actions)
    {
        if (action.title.find("Organize reference links") != std::string::npos)
        {
            found_organize = true;
            break;
        }
    }
    REQUIRE(found_organize);
}

TEST_CASE("CodeActionProvider — format document action", "[code_actions][phase29]")
{
    DiagnosticsService diag_service;
    CodeActionProvider provider(diag_service);

    std::string content = "# Title\n\n\n\nParagraph\n";
    auto set = provider.provide_action_set(content, 0, 0, 0, 0);

    bool found_format = false;
    for (const auto& action : set.source_actions)
    {
        if (action.title.find("Format document") != std::string::npos)
        {
            found_format = true;
            REQUIRE_FALSE(action.edits.empty());
            break;
        }
    }
    REQUIRE(found_format);
}

TEST_CASE("CodeActionProvider — remove unused references", "[code_actions][phase29]")
{
    DiagnosticsService diag_service;
    CodeActionProvider provider(diag_service);

    std::string content = "text [used][1] here\n[1]: http://used.com\n[2]: http://unused.com\n";
    auto set = provider.provide_action_set(content, 0, 0, 0, 0);

    bool found_remove = false;
    for (const auto& action : set.source_actions)
    {
        if (action.title.find("unused reference") != std::string::npos)
        {
            found_remove = true;
            break;
        }
    }
    REQUIRE(found_remove);
}

// ──────────────────────────────────────────────────────────────────────────────
// Extension Provider Registration
// ──────────────────────────────────────────────────────────────────────────────

TEST_CASE("CodeActionProvider — register and unregister extension providers",
          "[code_actions][phase29]")
{
    DiagnosticsService diag_service;
    CodeActionProvider provider(diag_service);

    REQUIRE(provider.extension_provider_count() == 0);

    provider.register_provider("test-ext",
                               [](const std::string& /*content*/,
                                  int /*start_line*/,
                                  int /*start_char*/,
                                  int /*end_line*/,
                                  int /*end_char*/,
                                  const std::string& /*uri*/) -> std::vector<CodeActionInfo>
                               {
                                   CodeActionInfo action;
                                   action.title = "Extension action";
                                   action.kind = CodeActionKind::kQuickFix;
                                   return {action};
                               });

    REQUIRE(provider.extension_provider_count() == 1);

    auto set = provider.provide_action_set("hello", 0, 0, 0, 0);
    bool found_ext = false;
    for (const auto& action : set.quick_fixes)
    {
        if (action.title == "Extension action")
        {
            found_ext = true;
            break;
        }
    }
    REQUIRE(found_ext);

    provider.unregister_provider("test-ext");
    REQUIRE(provider.extension_provider_count() == 0);
}

// ──────────────────────────────────────────────────────────────────────────────
// LightbulbWidget
// ──────────────────────────────────────────────────────────────────────────────

TEST_CASE("LightbulbWidget — initial state", "[code_actions][phase29]")
{
    markamp::ui::LightbulbWidget widget;
    REQUIRE_FALSE(widget.IsVisible());
    REQUIRE(widget.GetLine() == -1);
}

TEST_CASE("LightbulbWidget — show and hide", "[code_actions][phase29]")
{
    markamp::ui::LightbulbWidget widget;

    widget.SetVisible(true, 5, CodeActionKind::kRefactor);
    REQUIRE(widget.IsVisible());
    REQUIRE(widget.GetLine() == 5);
    REQUIRE(widget.GetKind() == CodeActionKind::kRefactor);

    widget.Hide();
    REQUIRE_FALSE(widget.IsVisible());
    REQUIRE(widget.GetLine() == -1);
}

TEST_CASE("LightbulbWidget — hit test when hidden returns false", "[code_actions][phase29]")
{
    markamp::ui::LightbulbWidget widget;
    REQUIRE_FALSE(widget.HitTest(10, 10));
}

// ──────────────────────────────────────────────────────────────────────────────
// Code Action Events
// ──────────────────────────────────────────────────────────────────────────────

TEST_CASE("CodeActionsComputedEvent fields", "[code_actions][phase29]")
{
    markamp::core::events::CodeActionsComputedEvent event;
    event.document_uri = "file://test.md";
    event.action_count = 3;
    event.has_preferred = true;

    REQUIRE(event.document_uri == "file://test.md");
    REQUIRE(event.action_count == 3);
    REQUIRE(event.has_preferred);
}

TEST_CASE("CodeActionAppliedEvent fields", "[code_actions][phase29]")
{
    markamp::core::events::CodeActionAppliedEvent event;
    event.document_uri = "file://test.md";
    event.action_title = "Fix whitespace";
    event.action_kind = 0;
    event.edits_applied = 1;

    REQUIRE(event.action_title == "Fix whitespace");
    REQUIRE(event.edits_applied == 1);
}

TEST_CASE("LightbulbVisibilityChangedEvent fields", "[code_actions][phase29]")
{
    markamp::core::events::LightbulbVisibilityChangedEvent event;
    event.line = 10;
    event.visible = true;
    event.primary_kind = 0;

    REQUIRE(event.line == 10);
    REQUIRE(event.visible);
}

TEST_CASE("ExtensionActionProviderRegisteredEvent fields", "[code_actions][phase29]")
{
    markamp::core::events::ExtensionActionProviderRegisteredEvent event;
    event.provider_id = "my-extension";

    REQUIRE(event.provider_id == "my-extension");
}
