/// @file test_phase24_export_publishing.cpp
/// Comprehensive test suite for Phase 24: Export & Publishing
/// Tests: ExportTemplateEngine, BatchExportEngine, PrintPreparationService,
///        PublishingProfileManager, ExportValidator, ExportCommandProvider, Events

#include "core/BatchExportEngine.h"
#include "core/Events.h"
#include "core/ExportCommandProvider.h"
#include "core/ExportTemplateEngine.h"
#include "core/ExportValidator.h"
#include "core/PrintPreparationService.h"
#include "core/PublishingProfileManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// ExportTemplateEngine tests
// ============================================================================

TEST_CASE("ExportTemplateEngine — load built-in templates", "[phase24][template]")
{
    ExportTemplateEngine engine;
    engine.load_builtin_templates();
    auto templates = engine.list_templates();
    REQUIRE(templates.size() == 5);
}

TEST_CASE("ExportTemplateEngine — get builtin template by id", "[phase24][template]")
{
    ExportTemplateEngine engine;
    engine.load_builtin_templates();
    const auto* tpl = engine.get_template("builtin-article");
    REQUIRE(tpl != nullptr);
    CHECK(tpl->name == "Article");
    CHECK_FALSE(tpl->html_template.empty());
    CHECK_FALSE(tpl->css.empty());
}

TEST_CASE("ExportTemplateEngine — get non-existent template returns nullptr", "[phase24][template]")
{
    ExportTemplateEngine engine;
    const auto* tpl = engine.get_template("nonexistent");
    CHECK(tpl == nullptr);
}

TEST_CASE("ExportTemplateEngine — register custom template", "[phase24][template]")
{
    ExportTemplateEngine engine;
    ExportTemplate custom;
    custom.template_id = "custom-1";
    custom.name = "Custom Template";
    custom.html_template = "<html>{{content}}</html>";
    custom.css = "body { color: red; }";

    auto result = engine.register_template(custom);
    REQUIRE(result);

    const auto* retrieved = engine.get_template("custom-1");
    REQUIRE(retrieved != nullptr);
    CHECK(retrieved->name == "Custom Template");
}

TEST_CASE("ExportTemplateEngine — register duplicate returns false", "[phase24][template]")
{
    ExportTemplateEngine engine;
    ExportTemplate tpl_a;
    tpl_a.template_id = "dup";
    tpl_a.name = "Duplicate A";
    tpl_a.html_template = "<html></html>";

    CHECK(engine.register_template(tpl_a));

    ExportTemplate tpl_b;
    tpl_b.template_id = "dup";
    tpl_b.name = "Duplicate B";
    tpl_b.html_template = "<html></html>";

    CHECK_FALSE(engine.register_template(tpl_b));
}

TEST_CASE("ExportTemplateEngine — render with variables", "[phase24][template]")
{
    ExportTemplateEngine engine;
    engine.load_builtin_templates();

    TemplateContext ctx;
    ctx["title"] = "Test Title";
    ctx["author"] = "Jane Doe";

    auto html = engine.render("builtin-article", "<p>Hello World</p>", ctx);
    CHECK(html.find("Test Title") != std::string::npos);
    CHECK(html.find("<p>Hello World</p>") != std::string::npos);
}

TEST_CASE("ExportTemplateEngine — render missing template returns error", "[phase24][template]")
{
    ExportTemplateEngine engine;
    auto result = engine.render("nonexistent", "<p>Body</p>");
    CHECK(result.find("Error") != std::string::npos);
}

TEST_CASE("ExportTemplateEngine — validate template", "[phase24][template]")
{
    ExportTemplateEngine engine;
    ExportTemplate valid;
    valid.template_id = "valid";
    valid.name = "Valid";
    valid.html_template = "<html>{{content}}</html>";

    auto result = engine.validate_template(valid);
    CHECK(result.valid);
    CHECK(result.errors.empty());
}

TEST_CASE("ExportTemplateEngine — validate empty template id fails", "[phase24][template]")
{
    ExportTemplateEngine engine;
    ExportTemplate empty;
    empty.template_id = "";
    empty.name = "Empty ID";
    empty.html_template = "<html></html>";

    auto result = engine.validate_template(empty);
    CHECK_FALSE(result.valid);
    CHECK_FALSE(result.errors.empty());
}

TEST_CASE("ExportTemplateEngine — remove template", "[phase24][template]")
{
    ExportTemplateEngine engine;
    ExportTemplate custom;
    custom.template_id = "removable";
    custom.name = "Removable";
    custom.html_template = "<html></html>";
    engine.register_template(custom);

    auto removed = engine.remove_template("removable");
    CHECK(removed);
    CHECK(engine.get_template("removable") == nullptr);
}

TEST_CASE("ExportTemplateEngine — cannot remove builtin", "[phase24][template]")
{
    ExportTemplateEngine engine;
    engine.load_builtin_templates();
    CHECK_FALSE(engine.remove_template("builtin-article"));
    CHECK(engine.get_template("builtin-article") != nullptr);
}

TEST_CASE("ExportTemplateEngine — extract variables", "[phase24][template]")
{
    auto vars = ExportTemplateEngine::extract_variables("Hello {{name}}, your {{role}} is ready");
    REQUIRE(vars.size() == 2);
    CHECK(vars[0] == "name");
    CHECK(vars[1] == "role");
}

TEST_CASE("ExportTemplateEngine — builtin_id returns expected strings", "[phase24][template]")
{
    CHECK(ExportTemplateEngine::builtin_id(BuiltinTemplate::kArticle) == "builtin-article");
    CHECK(ExportTemplateEngine::builtin_id(BuiltinTemplate::kReport) == "builtin-report");
    CHECK(ExportTemplateEngine::builtin_id(BuiltinTemplate::kSlides) == "builtin-slides");
    CHECK(ExportTemplateEngine::builtin_id(BuiltinTemplate::kResume) == "builtin-resume");
    CHECK(ExportTemplateEngine::builtin_id(BuiltinTemplate::kMinimal) == "builtin-minimal");
}

// ============================================================================
// BatchExportEngine tests
// ============================================================================

TEST_CASE("BatchExportEngine — construction", "[phase24][batch]")
{
    EventBus bus;
    BatchExportEngine engine(bus);
    CHECK_FALSE(engine.is_cancelled());
}

TEST_CASE("BatchExportEngine — execute empty batch", "[phase24][batch]")
{
    EventBus bus;
    BatchExportEngine engine(bus);

    BatchExportJob job;
    auto result = engine.execute(job);

    CHECK(result.completed);
    CHECK(result.total == 0);
    CHECK(result.succeeded == 0);
    CHECK(result.failed == 0);
}

TEST_CASE("BatchExportEngine — execute batch with documents", "[phase24][batch]")
{
    EventBus bus;
    BatchExportEngine engine(bus);

    BatchExportJob job;
    job.documents.push_back({"doc-1", "First Document", "# Hello\nContent"});
    job.documents.push_back({"doc-2", "Second Document", "# World\nMore content"});
    job.naming_pattern = "{title}";

    auto result = engine.execute(job);

    CHECK(result.completed);
    CHECK(result.total == 2);
    CHECK(result.succeeded == 2);
    CHECK(result.failed == 0);
    REQUIRE(result.items.size() == 2);
    CHECK(result.items[0].success);
    CHECK(result.items[1].success);
}

TEST_CASE("BatchExportEngine — progress callback fires", "[phase24][batch]")
{
    EventBus bus;
    BatchExportEngine engine(bus);

    BatchExportJob job;
    job.documents.push_back({"doc-1", "Doc A", "# A"});
    job.documents.push_back({"doc-2", "Doc B", "# B"});

    int callback_count = 0;
    auto result = engine.execute(job,
                                 [&](int32_t current, int32_t total, const std::string& /*title*/)
                                 {
                                     CHECK(current <= total);
                                     ++callback_count;
                                 });

    CHECK(callback_count == 2);
}

TEST_CASE("BatchExportEngine — cancellation", "[phase24][batch]")
{
    EventBus bus;
    BatchExportEngine engine(bus);

    // Verify cancel() sets the flag.
    engine.cancel();
    CHECK(engine.is_cancelled());

    // Note: execute() resets the cancelled_ flag internally, so calling
    // execute() after cancel() will *not* produce a cancelled result.
    // This tests that the cancel API itself works correctly.
    // A real cancellation test would need multithreading.
}

TEST_CASE("BatchExportEngine — slugify", "[phase24][batch]")
{
    CHECK(BatchExportEngine::slugify("Hello World") == "hello-world");
    CHECK(BatchExportEngine::slugify("My Document (v2)") == "my-document-v2");
    CHECK(BatchExportEngine::slugify("  Leading  ") == "leading");
    CHECK(BatchExportEngine::slugify("ALL CAPS") == "all-caps");
    CHECK(BatchExportEngine::slugify("") == "");
}

TEST_CASE("BatchExportEngine — resolve_filename with tokens", "[phase24][batch]")
{
    auto result =
        BatchExportEngine::resolve_filename("{title}", "My Document", 1, ".html", "2024-01-15");
    CHECK(result == "my-document.html");

    result =
        BatchExportEngine::resolve_filename("{index}-{title}", "Test", 3, ".pdf", "2024-01-15");
    CHECK(result == "3-test.pdf");

    result = BatchExportEngine::resolve_filename("{date}-{title}", "Note", 1, ".md", "2024-01-15");
    CHECK(result == "2024-01-15-note.md");
}

TEST_CASE("BatchExportEngine — generate_index HTML", "[phase24][batch]")
{
    std::vector<BatchItemResult> items;
    items.push_back({"doc-1", "First", "first.html", true, "", 1024});
    items.push_back({"doc-2", "Second", "second.html", false, "Error writing file", 0});

    auto html = BatchExportEngine::generate_index("Test Batch", items);
    CHECK(html.find("<title>Test Batch</title>") != std::string::npos);
    CHECK(html.find("first.html") != std::string::npos);
    CHECK(html.find("second.html") != std::string::npos);
    CHECK(html.find("2 documents exported") != std::string::npos);
}

// ============================================================================
// PrintPreparationService tests
// ============================================================================

TEST_CASE("PrintPreparationService — construction", "[phase24][print]")
{
    PrintPreparationService service;
    // Should not throw.
    (void)service;
}

TEST_CASE("PrintPreparationService — prepare_for_print", "[phase24][print]")
{
    PrintPreparationService service;
    auto html = service.prepare_for_print("<h1>Title</h1><p>Body</p>");

    CHECK(html.find("<!DOCTYPE html>") != std::string::npos);
    CHECK(html.find("<h1>Title</h1>") != std::string::npos);
    CHECK(html.find("@page") != std::string::npos);
}

TEST_CASE("PrintPreparationService — generate preview", "[phase24][print]")
{
    PrintPreparationService service;
    auto preview = service.generate_preview(
        "<h1>Chapter 1</h1><p>Content</p><h1>Chapter 2</h1><p>More content</p>", "My Document");

    CHECK(preview.estimated_page_count >= 1);
    CHECK_FALSE(preview.content_html.empty());
    CHECK_FALSE(preview.print_css.empty());
    CHECK(preview.sections.size() == 2);
    CHECK(preview.sections[0] == "Chapter 1");
    CHECK(preview.sections[1] == "Chapter 2");
}

TEST_CASE("PrintPreparationService — inject page breaks", "[phase24][print]")
{
    PrintPreparationService service;
    auto html = service.inject_page_breaks("<h1>A</h1><p>Text</p><h1>B</h1><p>More</p>", 1);

    // Second h1 should have a page-break div before it.
    CHECK(html.find("page-break-before") != std::string::npos);
}

TEST_CASE("PrintPreparationService — estimate_page_count", "[phase24][print]")
{
    CHECK(PrintPreparationService::estimate_page_count(0) == 0);
    CHECK(PrintPreparationService::estimate_page_count(100) == 1);
    CHECK(PrintPreparationService::estimate_page_count(6000) == 2);
    CHECK(PrintPreparationService::estimate_page_count(100, PageSize::kA5) == 1);
    CHECK(PrintPreparationService::estimate_page_count(5000, PageSize::kA5) == 3);
}

TEST_CASE("PrintPreparationService — generate_print_css", "[phase24][print]")
{
    PrintLayout layout;
    layout.page_size = PageSize::kLetter;
    layout.landscape = true;

    auto css = PrintPreparationService::generate_print_css(layout);
    CHECK(css.find("letter") != std::string::npos);
    CHECK(css.find("landscape") != std::string::npos);
    CHECK(css.find("@page") != std::string::npos);
    CHECK(css.find("@media print") != std::string::npos);
}

TEST_CASE("PrintPreparationService — page_size_css", "[phase24][print]")
{
    CHECK(PrintPreparationService::page_size_css(PageSize::kA4) == "A4");
    CHECK(PrintPreparationService::page_size_css(PageSize::kA5) == "A5");
    CHECK(PrintPreparationService::page_size_css(PageSize::kLetter) == "letter");
    CHECK(PrintPreparationService::page_size_css(PageSize::kLegal) == "legal");
    CHECK(PrintPreparationService::page_size_css(PageSize::kCustom) == "A4");
}

TEST_CASE("PrintPreparationService — extract sections", "[phase24][print]")
{
    auto sections = PrintPreparationService::extract_sections(
        "<h1>Intro</h1><p>text</p><h1>Conclusion</h1><p>end</p>");
    REQUIRE(sections.size() == 2);
    CHECK(sections[0] == "Intro");
    CHECK(sections[1] == "Conclusion");
}

TEST_CASE("PrintPreparationService — add headers footers", "[phase24][print]")
{
    PrintPreparationService service;
    PrintLayout layout;
    layout.header_text = "{{title}} - Header";
    layout.footer_text = "Page {{page}}";

    auto html = service.add_headers_footers("<p>Body</p>", layout, "Test Doc");
    CHECK(html.find("Test Doc - Header") != std::string::npos);
    CHECK(html.find("print-header") != std::string::npos);
    CHECK(html.find("print-footer") != std::string::npos);
}

// ============================================================================
// PublishingProfileManager tests
// ============================================================================

TEST_CASE("PublishingProfileManager — create and retrieve profile", "[phase24][profile]")
{
    PublishingProfileManager mgr;
    PublishingProfile prof;
    prof.name = "My Profile";
    prof.output_directory = "/output";

    auto profile_id = mgr.create_profile(prof);
    CHECK_FALSE(profile_id.empty());

    auto retrieved = mgr.get_profile(profile_id);
    REQUIRE(retrieved.has_value());
    CHECK(retrieved->name == "My Profile");
    CHECK(mgr.count() == 1);
}

TEST_CASE("PublishingProfileManager — update profile", "[phase24][profile]")
{
    PublishingProfileManager mgr;
    PublishingProfile prof;
    prof.name = "Original";
    prof.output_directory = "/out";

    auto profile_id = mgr.create_profile(prof);
    auto updated = mgr.get_profile(profile_id).value();
    updated.name = "Updated";
    CHECK(mgr.update_profile(updated));

    auto result = mgr.get_profile(profile_id);
    CHECK(result->name == "Updated");
}

TEST_CASE("PublishingProfileManager — delete profile", "[phase24][profile]")
{
    PublishingProfileManager mgr;
    PublishingProfile prof;
    prof.name = "Deletable";
    prof.output_directory = "/out";

    auto profile_id = mgr.create_profile(prof);
    CHECK(mgr.delete_profile(profile_id));
    CHECK(mgr.count() == 0);
    CHECK_FALSE(mgr.get_profile(profile_id).has_value());
}

TEST_CASE("PublishingProfileManager — delete non-existent", "[phase24][profile]")
{
    PublishingProfileManager mgr;
    CHECK_FALSE(mgr.delete_profile("nonexistent"));
}

TEST_CASE("PublishingProfileManager — list profiles sorted by name", "[phase24][profile]")
{
    PublishingProfileManager mgr;

    PublishingProfile prof_b;
    prof_b.name = "B Profile";
    prof_b.output_directory = "/out";
    mgr.create_profile(prof_b);

    PublishingProfile prof_a;
    prof_a.name = "A Profile";
    prof_a.output_directory = "/out";
    mgr.create_profile(prof_a);

    auto profiles = mgr.list_profiles();
    REQUIRE(profiles.size() == 2);
    CHECK(profiles[0].name == "A Profile");
    CHECK(profiles[1].name == "B Profile");
}

TEST_CASE("PublishingProfileManager — duplicate profile", "[phase24][profile]")
{
    PublishingProfileManager mgr;
    PublishingProfile prof;
    prof.name = "Original";
    prof.output_directory = "/out";
    prof.use_count = 5;

    auto orig_id = mgr.create_profile(prof);
    auto dup_id = mgr.duplicate_profile(orig_id, "Copy");

    CHECK_FALSE(dup_id.empty());
    CHECK(dup_id != orig_id);

    auto dup_profile = mgr.get_profile(dup_id);
    REQUIRE(dup_profile.has_value());
    CHECK(dup_profile->name == "Copy");
    CHECK(dup_profile->use_count == 0); // Reset on duplication.
    CHECK(mgr.count() == 2);
}

TEST_CASE("PublishingProfileManager — mark_used", "[phase24][profile]")
{
    PublishingProfileManager mgr;
    PublishingProfile prof;
    prof.name = "Trackable";
    prof.output_directory = "/out";

    auto profile_id = mgr.create_profile(prof);
    mgr.mark_used(profile_id);

    auto result = mgr.get_profile(profile_id);
    CHECK(result->use_count == 1);
    CHECK(result->last_used_ms > 0);
}

TEST_CASE("PublishingProfileManager — most_recent_profile", "[phase24][profile]")
{
    PublishingProfileManager mgr;

    // No profiles yet.
    CHECK_FALSE(mgr.most_recent_profile().has_value());

    PublishingProfile prof;
    prof.name = "TestProfile";
    prof.output_directory = "/out";
    auto profile_id = mgr.create_profile(prof);

    // Not yet used.
    CHECK_FALSE(mgr.most_recent_profile().has_value());

    mgr.mark_used(profile_id);
    auto recent = mgr.most_recent_profile();
    REQUIRE(recent.has_value());
    CHECK(recent->profile_id == profile_id);
}

TEST_CASE("PublishingProfileManager — serialize and deserialize", "[phase24][profile]")
{
    PublishingProfileManager mgr;

    PublishingProfile prof;
    prof.name = "Serializable";
    prof.output_directory = "/output/dir";
    prof.template_id = "article";
    prof.auto_open = true;
    mgr.create_profile(prof);

    auto json = mgr.serialize();
    CHECK_FALSE(json.empty());
    CHECK(json.find("Serializable") != std::string::npos);

    PublishingProfileManager mgr2;
    auto loaded = mgr2.deserialize(json);
    CHECK(loaded == 1);
    CHECK(mgr2.count() == 1);
}

TEST_CASE("PublishingProfileManager — validate profile", "[phase24][profile]")
{
    PublishingProfile valid;
    valid.name = "Valid";
    valid.output_directory = "/out";
    auto valid_errors = PublishingProfileManager::validate(valid);
    CHECK(valid_errors.empty());

    PublishingProfile invalid;
    // Missing name and output_directory.
    auto invalid_errors = PublishingProfileManager::validate(invalid);
    CHECK(invalid_errors.size() == 2);
}

// ============================================================================
// ExportValidator tests
// ============================================================================

TEST_CASE("ExportValidator — validate empty content reports issues", "[phase24][validator]")
{
    ExportValidator validator;
    auto issues = validator.validate("");
    auto sum = ExportValidator::summary(issues);
    CHECK(sum.total() > 0);
}

TEST_CASE("ExportValidator — valid content has no errors", "[phase24][validator]")
{
    ExportValidator validator;
    auto issues = validator.validate("# Hello World\n\nSome content here.\n");
    auto sum = ExportValidator::summary(issues);
    CHECK(sum.errors == 0);
}

TEST_CASE("ExportValidator — unclosed frontmatter", "[phase24][validator]")
{
    auto issues = ExportValidator::check_frontmatter("---\ntitle: Test\nNo closing fence");
    REQUIRE_FALSE(issues.empty());
    CHECK(issues[0].severity == ValidationSeverity::kError);
    CHECK(issues[0].category == ValidationCategory::kInvalidFrontmatter);
}

TEST_CASE("ExportValidator — valid frontmatter", "[phase24][validator]")
{
    auto issues = ExportValidator::check_frontmatter("---\ntitle: Test\n---\n# Content");
    CHECK(issues.empty());
}

TEST_CASE("ExportValidator — duplicate headings", "[phase24][validator]")
{
    auto issues = ExportValidator::check_duplicate_headings(
        "# Introduction\n\nText\n\n# Methods\n\nText\n\n# Introduction\n\nMore text");
    CHECK_FALSE(issues.empty());
    // Should report duplicates for "Introduction".
    bool found_dup = false;
    for (const auto& issue : issues)
    {
        if (issue.message.find("Introduction") != std::string::npos)
        {
            found_dup = true;
        }
    }
    CHECK(found_dup);
}

TEST_CASE("ExportValidator — large content warning", "[phase24][validator]")
{
    // Create a string larger than 1MB.
    std::string large(1024 * 1024 + 1, 'a');
    auto issues = ExportValidator::check_size(large);
    REQUIRE_FALSE(issues.empty());
    CHECK(issues[0].severity == ValidationSeverity::kWarning);
    CHECK(issues[0].category == ValidationCategory::kLargeFile);
}

TEST_CASE("ExportValidator — format compatibility LaTeX", "[phase24][validator]")
{
    auto issues = ExportValidator::check_format_compatibility(
        "# Title\n\n<div class='note'>Important</div>\n", ExportFormat::LaTeX);
    CHECK_FALSE(issues.empty());
}

TEST_CASE("ExportValidator — format compatibility PlainText", "[phase24][validator]")
{
    auto issues = ExportValidator::check_format_compatibility("# Title\n\n![Alt text](image.png)\n",
                                                              ExportFormat::PlainText);
    CHECK_FALSE(issues.empty());
}

TEST_CASE("ExportValidator — summary counts", "[phase24][validator]")
{
    std::vector<ValidationIssue> issues;
    issues.push_back({ValidationSeverity::kError, ValidationCategory::kBrokenLink, "err", 1});
    issues.push_back({ValidationSeverity::kWarning, ValidationCategory::kLargeFile, "warn", 0});
    issues.push_back({ValidationSeverity::kInfo, ValidationCategory::kDuplicateHeading, "info", 5});

    auto sum = ExportValidator::summary(issues);
    CHECK(sum.errors == 1);
    CHECK(sum.warnings == 1);
    CHECK(sum.infos == 1);
    CHECK(sum.total() == 3);
    CHECK(sum.has_errors());
}

// ============================================================================
// ExportCommandProvider tests
// ============================================================================

TEST_CASE("ExportCommandProvider — has 8 commands", "[phase24][commands]")
{
    ExportCommandProvider provider;
    const auto& commands = provider.get_commands();
    CHECK(commands.size() == 8);
}

TEST_CASE("ExportCommandProvider — all commands enabled", "[phase24][commands]")
{
    ExportCommandProvider provider;
    for (const auto& cmd : provider.get_commands())
    {
        CHECK(provider.is_enabled(cmd.cmd_id));
    }
}

TEST_CASE("ExportCommandProvider — get specific command", "[phase24][commands]")
{
    ExportCommandProvider provider;
    auto cmd = provider.get_command("export.document");
    REQUIRE(cmd.has_value());
    CHECK(cmd->label.find("Export Document") != std::string::npos);
    CHECK(cmd->category == "Export");
}

TEST_CASE("ExportCommandProvider — keybinding for export.document", "[phase24][commands]")
{
    ExportCommandProvider provider;
    auto key = provider.get_keybinding("export.document");
    REQUIRE(key.has_value());
    CHECK(key->find("Cmd") != std::string::npos);
}

TEST_CASE("ExportCommandProvider — nonexistent command", "[phase24][commands]")
{
    ExportCommandProvider provider;
    auto cmd = provider.get_command("export.nonexistent");
    CHECK_FALSE(cmd.has_value());
    CHECK_FALSE(provider.is_enabled("export.nonexistent"));
}

TEST_CASE("ExportCommandProvider — print preview command", "[phase24][commands]")
{
    ExportCommandProvider provider;
    auto cmd = provider.get_command("export.print_preview");
    REQUIRE(cmd.has_value());
    CHECK(cmd->keybinding.has_value());
    CHECK(cmd->keybinding.value() == "Cmd+P");
}

// ============================================================================
// Events tests
// ============================================================================

TEST_CASE("Phase 24 events — ExportTemplateAppliedEvent", "[phase24][events]")
{
    using namespace markamp::core::events;
    ExportTemplateAppliedEvent evt;
    evt.template_id = "article";
    evt.format = "html";
    CHECK(evt.template_id == "article");
    CHECK(evt.format == "html");
}

TEST_CASE("Phase 24 events — BatchExportStartedEvent", "[phase24][events]")
{
    using namespace markamp::core::events;
    BatchExportStartedEvent evt;
    evt.document_count = 10;
    evt.format = "pdf";
    CHECK(evt.document_count == 10);
}

TEST_CASE("Phase 24 events — BatchExportCompletedEvent", "[phase24][events]")
{
    using namespace markamp::core::events;
    BatchExportCompletedEvent evt;
    evt.succeeded = 8;
    evt.failed = 2;
    evt.elapsed_ms = 1500;
    CHECK(evt.succeeded == 8);
    CHECK(evt.failed == 2);
    CHECK(evt.elapsed_ms == 1500);
}

TEST_CASE("Phase 24 events — PrintPreviewGeneratedEvent", "[phase24][events]")
{
    using namespace markamp::core::events;
    PrintPreviewGeneratedEvent evt;
    evt.page_count = 5;
    evt.document_id = "doc-123";
    CHECK(evt.page_count == 5);
}

TEST_CASE("Phase 24 events — PublishingProfileExecutedEvent", "[phase24][events]")
{
    using namespace markamp::core::events;
    PublishingProfileExecutedEvent evt;
    evt.profile_id = "profile-1";
    evt.profile_name = "My Profile";
    evt.success = true;
    CHECK(evt.success);
}

TEST_CASE("Phase 24 events — ExportValidationCompletedEvent", "[phase24][events]")
{
    using namespace markamp::core::events;
    ExportValidationCompletedEvent evt;
    evt.errors = 2;
    evt.warnings = 5;
    evt.document_id = "doc-456";
    CHECK(evt.errors == 2);
    CHECK(evt.warnings == 5);
}
