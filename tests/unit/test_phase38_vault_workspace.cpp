/// @file test_phase38_vault_workspace.cpp
/// @brief V9 Phase 38 – Vault Style & Workspace Customization tests.

#include "core/CommandRegistry.h"
#include "core/CssSnippetLibrary.h"
#include "core/CustomFontRegistry.h"
#include "core/Events.h"
#include "core/ThemeOverrideService.h"
#include "core/VaultStyleCommandProvider.h"
#include "core/WorkspaceCustomizationCommandProvider.h"
#include "core/WorkspaceProfile.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ============================================================================
// CssSnippetLibrary
// ============================================================================

TEST_CASE("CssSnippetLibrary — add and find snippet", "[phase38][snippets]")
{
    CssSnippetLibrary library;
    REQUIRE(library.snippet_count() == 0);

    CssSnippet snippet;
    snippet.name = "Test Snippet";
    snippet.description = "A test snippet";
    snippet.category = "colors";
    snippet.css_content = ":root { --test: red; }";
    snippet.variables = {"--test"};
    library.add_snippet(snippet);

    REQUIRE(library.snippet_count() == 1);
    const auto* found = library.find_snippet("Test Snippet");
    REQUIRE(found != nullptr);
    REQUIRE(found->category == "colors");
    REQUIRE(found->variables.size() == 1);
}

TEST_CASE("CssSnippetLibrary — remove snippet", "[phase38][snippets]")
{
    CssSnippetLibrary library;
    CssSnippet snippet;
    snippet.name = "Remove Me";
    snippet.category = "typography";
    library.add_snippet(snippet);
    REQUIRE(library.snippet_count() == 1);

    REQUIRE(library.remove_snippet("Remove Me"));
    REQUIRE(library.snippet_count() == 0);
    REQUIRE_FALSE(library.remove_snippet("Does Not Exist"));
}

TEST_CASE("CssSnippetLibrary — filter by category", "[phase38][snippets]")
{
    CssSnippetLibrary library;

    CssSnippet snip_a;
    snip_a.name = "A";
    snip_a.category = "colors";
    library.add_snippet(snip_a);

    CssSnippet snip_b;
    snip_b.name = "B";
    snip_b.category = "typography";
    library.add_snippet(snip_b);

    CssSnippet snip_c;
    snip_c.name = "C";
    snip_c.category = "colors";
    library.add_snippet(snip_c);

    auto color_snippets = library.snippets_by_category("colors");
    REQUIRE(color_snippets.size() == 2);

    auto typo_snippets = library.snippets_by_category("typography");
    REQUIRE(typo_snippets.size() == 1);
}

TEST_CASE("CssSnippetLibrary — apply snippet", "[phase38][snippets]")
{
    CssSnippet snippet;
    snippet.name = "My Snippet";
    snippet.css_content = "h1 { color: blue; }";

    auto merged = CssSnippetLibrary::apply_snippet(snippet, "body { margin: 0; }");
    REQUIRE(merged.find("My Snippet") != std::string::npos);
    REQUIRE(merged.find("h1 { color: blue; }") != std::string::npos);
    REQUIRE(merged.find("body { margin: 0; }") != std::string::npos);
}

TEST_CASE("CssSnippetLibrary — preview equals apply", "[phase38][snippets]")
{
    CssSnippet snippet;
    snippet.name = "Preview";
    snippet.css_content = "p { font-size: 16px; }";

    auto preview = CssSnippetLibrary::generate_preview(snippet, "base { }");
    auto applied = CssSnippetLibrary::apply_snippet(snippet, "base { }");
    REQUIRE(preview == applied);
}

TEST_CASE("CssSnippetLibrary — load builtins", "[phase38][snippets]")
{
    CssSnippetLibrary library;
    library.load_builtins();
    REQUIRE(library.snippet_count() == 5);

    REQUIRE(library.find_snippet("Dark Headers") != nullptr);
    REQUIRE(library.find_snippet("Soft Pastels") != nullptr);
    REQUIRE(library.find_snippet("Comfortable Spacing") != nullptr);
    REQUIRE(library.find_snippet("Smooth Transitions") != nullptr);
}

TEST_CASE("CssSnippetLibrary — categories", "[phase38][snippets]")
{
    CssSnippetLibrary library;
    library.load_builtins();
    auto cats = library.categories();
    REQUIRE(cats.size() >= 3); // animations, colors, spacing, typography
}

TEST_CASE("CssSnippetLibrary — export and import", "[phase38][snippets]")
{
    CssSnippetLibrary library;
    CssSnippet snip;
    snip.name = "Export Test";
    snip.description = "For export";
    snip.category = "layout";
    snip.css_content = ".layout { display: grid; }";
    library.add_snippet(snip);

    auto json = library.export_library();
    REQUIRE(json.find("Export Test") != std::string::npos);

    CssSnippetLibrary imported;
    int count = imported.import_library(json);
    REQUIRE(count == 1);
    REQUIRE(imported.find_snippet("Export Test") != nullptr);
}

// ============================================================================
// WorkspaceProfileManager
// ============================================================================

TEST_CASE("WorkspaceProfileManager — save and get profile", "[phase38][profiles]")
{
    WorkspaceProfileManager mgr;
    REQUIRE(mgr.profile_count() == 0);

    WorkspaceProfile profile;
    profile.name = "My Profile";
    profile.description = "Test profile";
    profile.settings_json = R"({"editor.fontSize": 14})";
    mgr.save_profile(profile);

    REQUIRE(mgr.profile_count() == 1);
    REQUIRE(mgr.has_profile("My Profile"));

    const auto* found = mgr.get_profile("My Profile");
    REQUIRE(found != nullptr);
    REQUIRE(found->description == "Test profile");
}

TEST_CASE("WorkspaceProfileManager — delete profile", "[phase38][profiles]")
{
    WorkspaceProfileManager mgr;
    WorkspaceProfile profile;
    profile.name = "Delete Me";
    mgr.save_profile(profile);
    REQUIRE(mgr.delete_profile("Delete Me"));
    REQUIRE(mgr.profile_count() == 0);
}

TEST_CASE("WorkspaceProfileManager — cannot delete builtin", "[phase38][profiles]")
{
    WorkspaceProfileManager mgr;
    mgr.load_builtins();
    REQUIRE_FALSE(mgr.delete_profile("Default"));
    REQUIRE(mgr.has_profile("Default"));
}

TEST_CASE("WorkspaceProfileManager — rename profile", "[phase38][profiles]")
{
    WorkspaceProfileManager mgr;
    WorkspaceProfile profile;
    profile.name = "Old Name";
    mgr.save_profile(profile);

    REQUIRE(mgr.rename_profile("Old Name", "New Name"));
    REQUIRE_FALSE(mgr.has_profile("Old Name"));
    REQUIRE(mgr.has_profile("New Name"));
}

TEST_CASE("WorkspaceProfileManager — list profiles", "[phase38][profiles]")
{
    WorkspaceProfileManager mgr;
    mgr.load_builtins();
    auto names = mgr.list_profiles();
    REQUIRE(names.size() == 3);
}

TEST_CASE("WorkspaceProfileManager — diff profiles", "[phase38][profiles]")
{
    WorkspaceProfileManager mgr;

    WorkspaceProfile prof_a;
    prof_a.name = "Profile A";
    prof_a.settings_json = R"({"font": 14})";
    prof_a.vault_css = "body {}";
    mgr.save_profile(prof_a);

    WorkspaceProfile prof_b;
    prof_b.name = "Profile B";
    prof_b.settings_json = R"({"font": 16})";
    prof_b.vault_css = "body {}";
    mgr.save_profile(prof_b);

    auto diff = mgr.diff_profiles("Profile A", "Profile B");
    REQUIRE(diff.has_differences());
    REQUIRE(diff.diff_count() == 1); // Only settings differ, CSS is same
}

TEST_CASE("WorkspaceProfileManager — export and import", "[phase38][profiles]")
{
    WorkspaceProfileManager mgr;
    WorkspaceProfile profile;
    profile.name = "Exported";
    profile.description = "For export";
    mgr.save_profile(profile);

    auto json = mgr.export_profile("Exported");
    REQUIRE(json.find("Exported") != std::string::npos);

    WorkspaceProfileManager mgr2;
    auto imported = mgr2.import_profile(json);
    REQUIRE(imported == "Exported");
    REQUIRE(mgr2.has_profile("Exported"));
}

// ============================================================================
// CustomFontRegistry
// ============================================================================

TEST_CASE("CustomFontRegistry — register and find font", "[phase38][fonts]")
{
    CustomFontRegistry registry;
    REQUIRE(registry.font_count() == 0);

    FontEntry font;
    font.name = "JetBrains Mono";
    font.family = "JetBrains Mono";
    font.file_path = "/fonts/JetBrainsMono.ttf";
    font.format = FontFormat::kTTF;
    font.weight = FontWeight::kNormal;
    registry.register_font(font);

    REQUIRE(registry.font_count() == 1);
    const auto* found = registry.find_font("JetBrains Mono");
    REQUIRE(found != nullptr);
    REQUIRE(found->family == "JetBrains Mono");
}

TEST_CASE("CustomFontRegistry — unregister font", "[phase38][fonts]")
{
    CustomFontRegistry registry;
    FontEntry font;
    font.name = "Remove Me";
    font.family = "X";
    registry.register_font(font);

    REQUIRE(registry.unregister_font("Remove Me"));
    REQUIRE(registry.font_count() == 0);
    REQUIRE_FALSE(registry.unregister_font("Not Found"));
}

TEST_CASE("CustomFontRegistry — fonts by family", "[phase38][fonts]")
{
    CustomFontRegistry registry;

    FontEntry regular;
    regular.name = "Inter Regular";
    regular.family = "Inter";
    regular.weight = FontWeight::kNormal;
    registry.register_font(regular);

    FontEntry bold;
    bold.name = "Inter Bold";
    bold.family = "Inter";
    bold.weight = FontWeight::kBold;
    registry.register_font(bold);

    FontEntry mono;
    mono.name = "Fira Code";
    mono.family = "Fira Code";
    registry.register_font(mono);

    auto inter_fonts = registry.fonts_by_family("Inter");
    REQUIRE(inter_fonts.size() == 2);

    auto families = registry.families();
    REQUIRE(families.size() == 2);
}

TEST_CASE("CustomFontRegistry — generate font-face CSS", "[phase38][fonts]")
{
    CustomFontRegistry registry;
    FontEntry font;
    font.name = "My Font";
    font.family = "MyFont";
    font.file_path = "/path/to/font.woff2";
    font.format = FontFormat::kWOFF2;
    font.weight = FontWeight::kBold;
    font.style = FontStyle::kItalic;
    registry.register_font(font);

    auto css = registry.generate_font_face_css();
    REQUIRE(css.find("@font-face") != std::string::npos);
    REQUIRE(css.find("MyFont") != std::string::npos);
    REQUIRE(css.find("woff2") != std::string::npos);
    REQUIRE(css.find("700") != std::string::npos);
    REQUIRE(css.find("italic") != std::string::npos);
}

TEST_CASE("CustomFontRegistry — validate font", "[phase38][fonts]")
{
    FontEntry valid;
    valid.name = "Valid";
    valid.family = "Valid";
    valid.file_path = "/path.ttf";
    valid.format = FontFormat::kTTF;
    auto [ok, err] = CustomFontRegistry::validate_font(valid);
    REQUIRE(ok);
    REQUIRE(err.empty());

    FontEntry invalid;
    auto [ok2, err2] = CustomFontRegistry::validate_font(invalid);
    REQUIRE_FALSE(ok2);
}

TEST_CASE("CustomFontRegistry — detect format", "[phase38][fonts]")
{
    REQUIRE(CustomFontRegistry::detect_format("/font.ttf") == FontFormat::kTTF);
    REQUIRE(CustomFontRegistry::detect_format("/font.otf") == FontFormat::kOTF);
    REQUIRE(CustomFontRegistry::detect_format("/font.woff") == FontFormat::kWOFF);
    REQUIRE(CustomFontRegistry::detect_format("/font.woff2") == FontFormat::kWOFF2);
    REQUIRE(CustomFontRegistry::detect_format("/font.xyz") == FontFormat::kUnknown);
}

// ============================================================================
// ThemeOverrideService
// ============================================================================

TEST_CASE("ThemeOverrideService — set and get override", "[phase38][overrides]")
{
    ThemeOverrideService service;
    REQUIRE(service.override_count() == 0);

    service.set_override("editor.background", "#1e1e1e", "#ffffff", OverrideScope::kVault);

    REQUIRE(service.override_count() == 1);
    const auto* ovr = service.get_override("editor.background", OverrideScope::kVault);
    REQUIRE(ovr != nullptr);
    REQUIRE(ovr->override_value == "#ffffff");
    REQUIRE(ovr->original_value == "#1e1e1e");
}

TEST_CASE("ThemeOverrideService — remove override", "[phase38][overrides]")
{
    ThemeOverrideService service;
    service.set_override("token.a", "old", "new", OverrideScope::kWorkspace);

    REQUIRE(service.remove_override("token.a", OverrideScope::kWorkspace));
    REQUIRE(service.override_count() == 0);
    REQUIRE_FALSE(service.remove_override("token.a", OverrideScope::kWorkspace));
}

TEST_CASE("ThemeOverrideService — overrides for scope", "[phase38][overrides]")
{
    ThemeOverrideService service;
    service.set_override("a", "", "1", OverrideScope::kVault);
    service.set_override("b", "", "2", OverrideScope::kWorkspace);
    service.set_override("c", "", "3", OverrideScope::kVault);

    auto vault = service.overrides_for_scope(OverrideScope::kVault);
    REQUIRE(vault.size() == 2);

    auto workspace = service.overrides_for_scope(OverrideScope::kWorkspace);
    REQUIRE(workspace.size() == 1);
}

TEST_CASE("ThemeOverrideService — apply overrides", "[phase38][overrides]")
{
    ThemeOverrideService service;
    service.set_override("editor.background", "#1e1e1e", "#ffffff");
    service.set_override("editor.foreground", "#d4d4d4", "#000000");

    std::vector<std::pair<std::string, std::string>> base = {
        {"editor.background", "#1e1e1e"},
        {"editor.foreground", "#d4d4d4"},
        {"editor.cursor", "#ff0000"},
    };

    auto merged = service.apply_overrides(base);
    REQUIRE(merged.size() == 3);

    // Check overrides were applied
    bool bg_found = false;
    bool fg_found = false;
    for (const auto& [key, val] : merged)
    {
        if (key == "editor.background")
        {
            REQUIRE(val == "#ffffff");
            bg_found = true;
        }
        if (key == "editor.foreground")
        {
            REQUIRE(val == "#000000");
            fg_found = true;
        }
    }
    REQUIRE(bg_found);
    REQUIRE(fg_found);
}

TEST_CASE("ThemeOverrideService — reset by scope", "[phase38][overrides]")
{
    ThemeOverrideService service;
    service.set_override("a", "", "1", OverrideScope::kVault);
    service.set_override("b", "", "2", OverrideScope::kWorkspace);
    service.set_override("c", "", "3", OverrideScope::kVault);

    service.reset_overrides(OverrideScope::kVault);
    REQUIRE(service.override_count() == 1); // Only workspace remains
}

TEST_CASE("ThemeOverrideService — export and import", "[phase38][overrides]")
{
    ThemeOverrideService service;
    service.set_override("token.x", "old", "new", OverrideScope::kVault);

    auto json = service.export_overrides();
    REQUIRE(json.find("token.x") != std::string::npos);

    ThemeOverrideService service2;
    int count = service2.import_overrides(json);
    REQUIRE(count == 1);
    REQUIRE(service2.override_count() == 1);
}

// ============================================================================
// VaultStyleCommandProvider
// ============================================================================

TEST_CASE("VaultStyleCommandProvider — provides 8 commands", "[phase38][commands]")
{
    REQUIRE(VaultStyleCommandProvider::command_count() == 8);
    auto ids = VaultStyleCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "vaultStyle.editCss");
}

TEST_CASE("VaultStyleCommandProvider — register commands", "[phase38][commands]")
{
    VaultStyleCommandProvider provider;
    CommandRegistry registry;
    provider.register_commands(registry);

    auto* cmd = registry.get_command("vaultStyle.editCss");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->title == "Edit Vault CSS");
    REQUIRE(cmd->category == "Vault Style");
    REQUIRE(cmd->shortcut == "Cmd+Shift+V");
}

TEST_CASE("VaultStyleCommandProvider — get_command", "[phase38][commands]")
{
    VaultStyleCommandProvider provider;
    auto cmd = provider.get_command("vaultStyle.addSnippet");
    REQUIRE(cmd.id == "vaultStyle.addSnippet");
    REQUIRE(cmd.title == "Add CSS Snippet");
}

// ============================================================================
// WorkspaceCustomizationCommandProvider
// ============================================================================

TEST_CASE("WorkspaceCustomizationCommandProvider — provides 8 commands", "[phase38][commands]")
{
    REQUIRE(WorkspaceCustomizationCommandProvider::command_count() == 8);
    auto ids = WorkspaceCustomizationCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "workspace.saveProfile");
}

TEST_CASE("WorkspaceCustomizationCommandProvider — register commands", "[phase38][commands]")
{
    WorkspaceCustomizationCommandProvider provider;
    CommandRegistry registry;
    provider.register_commands(registry);

    auto* cmd = registry.get_command("workspace.saveProfile");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->title == "Save Workspace Profile");
    REQUIRE(cmd->shortcut == "Cmd+Shift+W");
}

TEST_CASE("WorkspaceCustomizationCommandProvider — get_command", "[phase38][commands]")
{
    WorkspaceCustomizationCommandProvider provider;
    auto cmd = provider.get_command("workspace.themeOverrides");
    REQUIRE(cmd.id == "workspace.themeOverrides");
    REQUIRE(cmd.title == "Theme Token Overrides");
}

// ============================================================================
// Phase 38 Events
// ============================================================================

TEST_CASE("CssSnippetAppliedEvent data", "[phase38][events]")
{
    CssSnippetAppliedEvent evt;
    evt.snippet_name = "Dark Headers";
    evt.category = "typography";
    REQUIRE(evt.snippet_name == "Dark Headers");
    REQUIRE(evt.category == "typography");
}

TEST_CASE("WorkspaceProfileSavedEvent data", "[phase38][events]")
{
    WorkspaceProfileSavedEvent evt;
    evt.profile_name = "Writing Focus";
    evt.is_new = true;
    REQUIRE(evt.profile_name == "Writing Focus");
    REQUIRE(evt.is_new);
}

TEST_CASE("WorkspaceProfileLoadedEvent data", "[phase38][events]")
{
    WorkspaceProfileLoadedEvent evt;
    evt.profile_name = "Research Mode";
    REQUIRE(evt.profile_name == "Research Mode");
}

TEST_CASE("CustomFontRegisteredEvent data", "[phase38][events]")
{
    CustomFontRegisteredEvent evt;
    evt.font_name = "Inter";
    evt.font_family = "Inter";
    evt.file_path = "/fonts/Inter.ttf";
    REQUIRE(evt.font_name == "Inter");
    REQUIRE(evt.font_family == "Inter");
}

TEST_CASE("ThemeOverrideChangedEvent data", "[phase38][events]")
{
    ThemeOverrideChangedEvent evt;
    evt.token_name = "editor.background";
    evt.new_value = "#ffffff";
    evt.scope = "vault";
    REQUIRE(evt.token_name == "editor.background");
    REQUIRE(evt.new_value == "#ffffff");
}

TEST_CASE("WorkspaceCustomizationResetEvent data", "[phase38][events]")
{
    WorkspaceCustomizationResetEvent evt;
    evt.scope = "vault";
    evt.settings_cleared = 5;
    REQUIRE(evt.scope == "vault");
    REQUIRE(evt.settings_cleared == 5);
}

// ============================================================================
// snippet_category_name utility
// ============================================================================

TEST_CASE("snippet_category_name", "[phase38][snippets]")
{
    REQUIRE(snippet_category_name(SnippetCategory::kTypography) == "typography");
    REQUIRE(snippet_category_name(SnippetCategory::kColors) == "colors");
    REQUIRE(snippet_category_name(SnippetCategory::kSpacing) == "spacing");
    REQUIRE(snippet_category_name(SnippetCategory::kAnimations) == "animations");
    REQUIRE(snippet_category_name(SnippetCategory::kLayout) == "layout");
    REQUIRE(snippet_category_name(SnippetCategory::kCustom) == "custom");
}

// ============================================================================
// ThemeOverrideService::scope_name utility
// ============================================================================

TEST_CASE("ThemeOverrideService scope_name", "[phase38][overrides]")
{
    REQUIRE(ThemeOverrideService::scope_name(OverrideScope::kVault) == "vault");
    REQUIRE(ThemeOverrideService::scope_name(OverrideScope::kWorkspace) == "workspace");
    REQUIRE(ThemeOverrideService::scope_name(OverrideScope::kGlobal) == "global");
}

// ============================================================================
// FontEntry helpers
// ============================================================================

TEST_CASE("FontEntry style_string and format_string", "[phase38][fonts]")
{
    FontEntry font;
    font.name = "Test";
    font.family = "Test";
    font.file_path = "/test.otf";
    font.format = FontFormat::kOTF;
    font.style = FontStyle::kOblique;
    font.weight = FontWeight::kSemiBold;

    REQUIRE(font.style_string() == "oblique");
    REQUIRE(font.format_string() == "opentype");
    REQUIRE(font.weight_value() == 600);
}
