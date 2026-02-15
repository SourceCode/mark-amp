/// @file test_note_template_engine.cpp
/// @brief V4 Phase 14 – Note Template Engine tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/NoteTemplateEngine.h"
#include "core/VaultService.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace
{

struct NoteTemplateTestFixture
{
    NoteTemplateTestFixture()
        : vault_dir_(fs::temp_directory_path() /
                     ("markamp_tmpl_test_" +
                      std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
        , vault_service_(event_bus_, config_)
        , engine_(event_bus_, config_, vault_service_)
    {
        fs::create_directories(vault_dir_);
        fs::create_directories(vault_dir_ / "_templates");
    }

    ~NoteTemplateTestFixture()
    {
        fs::remove_all(vault_dir_);
    }

    void create_template_file(const std::string& name, const std::string& content) const
    {
        const auto path = vault_dir_ / "_templates" / (name + ".md");
        std::ofstream out(path);
        out << content;
    }

    markamp::core::EventBus event_bus_;
    markamp::core::Config config_;
    fs::path vault_dir_;
    markamp::core::VaultService vault_service_;
    markamp::core::NoteTemplateEngine engine_;
};

} // anonymous namespace

// ============================================================================
// Test 1: Load templates from folder
// ============================================================================
TEST_CASE_METHOD(NoteTemplateTestFixture, "Templates loaded from folder", "[template][load]")
{
    create_template_file("meeting", "# {{title}}\nDate: {{date}}");
    create_template_file("daily", "# Daily Note\n{{date}}");
    create_template_file("project", "# Project: {{title}}");

    (void)vault_service_.open_vault(vault_dir_);
    engine_.load_templates();

    auto templates = engine_.list_templates();
    CHECK(templates.size() == 3);
}

// ============================================================================
// Test 2: Apply simple variables
// ============================================================================
TEST_CASE_METHOD(NoteTemplateTestFixture, "Apply replaces title variable", "[template][apply]")
{
    create_template_file("simple", "# {{title}}\nBody text.");

    (void)vault_service_.open_vault(vault_dir_);
    engine_.load_templates();

    auto tmpl = engine_.get_template("simple");
    REQUIRE(tmpl.has_value());

    markamp::core::NoteTemplateContext ctx;
    ctx.document_title = "My Document";

    auto result = engine_.apply(*tmpl, ctx);
    CHECK(result == "# My Document\nBody text.");
}

// ============================================================================
// Test 3: Built-in date variable
// ============================================================================
TEST_CASE_METHOD(NoteTemplateTestFixture, "Built-in date variable resolves", "[template][builtin]")
{
    create_template_file("dated", "Date: {{date}}");

    (void)vault_service_.open_vault(vault_dir_);
    engine_.load_templates();

    auto tmpl = engine_.get_template("dated");
    REQUIRE(tmpl.has_value());

    markamp::core::NoteTemplateContext ctx;
    auto result = engine_.apply(*tmpl, ctx);

    // Should contain a date in YYYY-MM-DD format
    CHECK(result.find("Date: ") == 0);
    CHECK(result.size() > 6); // "Date: " + date
}

// ============================================================================
// Test 4: Built-in time variable
// ============================================================================
TEST_CASE_METHOD(NoteTemplateTestFixture, "Built-in time variable resolves", "[template][builtin]")
{
    create_template_file("timed", "Time: {{time}}");

    (void)vault_service_.open_vault(vault_dir_);
    engine_.load_templates();

    auto tmpl = engine_.get_template("timed");
    REQUIRE(tmpl.has_value());

    markamp::core::NoteTemplateContext ctx;
    auto result = engine_.apply(*tmpl, ctx);

    CHECK(result.find("Time: ") == 0);
    // Should have at least HH:MM format
    CHECK(result.size() >= 11); // "Time: HH:MM"
}

// ============================================================================
// Test 5: Default values
// ============================================================================
TEST_CASE_METHOD(NoteTemplateTestFixture,
                 "Default value used when variable not in context",
                 "[template][default]")
{
    create_template_file("defaults", "Status: {{status:draft}}");

    (void)vault_service_.open_vault(vault_dir_);
    engine_.load_templates();

    auto tmpl = engine_.get_template("defaults");
    REQUIRE(tmpl.has_value());

    markamp::core::NoteTemplateContext ctx;
    auto result = engine_.apply(*tmpl, ctx);

    CHECK(result == "Status: draft");
}

// ============================================================================
// Test 6: Custom variable provider
// ============================================================================
TEST_CASE_METHOD(NoteTemplateTestFixture, "Custom variable provider resolves", "[template][custom]")
{
    create_template_file("custom", "Project: {{project}}");

    (void)vault_service_.open_vault(vault_dir_);
    engine_.load_templates();
    engine_.register_variable("project", []() { return "MarkAmp"; });

    auto tmpl = engine_.get_template("custom");
    REQUIRE(tmpl.has_value());

    markamp::core::NoteTemplateContext ctx;
    auto result = engine_.apply(*tmpl, ctx);

    CHECK(result == "Project: MarkAmp");
}

// ============================================================================
// Test 7: Cursor marker removal
// ============================================================================
TEST_CASE_METHOD(NoteTemplateTestFixture, "Cursor marker removed from output", "[template][cursor]")
{
    create_template_file("cursor", "# Title\n{{cursor}}\nFooter");

    (void)vault_service_.open_vault(vault_dir_);
    engine_.load_templates();

    auto tmpl = engine_.get_template("cursor");
    REQUIRE(tmpl.has_value());

    markamp::core::NoteTemplateContext ctx;
    auto result = engine_.apply(*tmpl, ctx);

    CHECK(result == "# Title\n\nFooter");
    CHECK(result.find("{{cursor}}") == std::string::npos);
}

// ============================================================================
// Test 8: Parse variables
// ============================================================================
TEST_CASE_METHOD(NoteTemplateTestFixture,
                 "parse_variables extracts all variables",
                 "[template][parse]")
{
    std::string content = "# {{title}}\nDate: {{date}}\nStatus: {{status:draft}}\n"
                          "Author: {{author}}\n{{cursor}}";

    auto vars = engine_.parse_variables(content);

    // Should have 4 variables (cursor excluded)
    CHECK(vars.size() == 4);

    // Check that title and date are detected
    bool has_title = false;
    bool has_date = false;
    bool has_status = false;
    for (const auto& var : vars)
    {
        if (var.name == "title")
        {
            has_title = true;
        }
        if (var.name == "date")
        {
            has_date = true;
            CHECK(var.is_builtin);
        }
        if (var.name == "status")
        {
            has_status = true;
            CHECK(var.default_value == "draft");
        }
    }
    CHECK(has_title);
    CHECK(has_date);
    CHECK(has_status);
}

// ============================================================================
// Test 9: Input variable detection
// ============================================================================
TEST_CASE_METHOD(NoteTemplateTestFixture,
                 "Input variable parsed with requires_input",
                 "[template][input]")
{
    std::string content = "{{input:Enter description}}";
    auto vars = engine_.parse_variables(content);

    REQUIRE(vars.size() == 1);
    CHECK(vars[0].name == "input");
    CHECK(vars[0].requires_input);
}

// ============================================================================
// Test 10: Choice variable detection
// ============================================================================
TEST_CASE_METHOD(NoteTemplateTestFixture,
                 "Choice variable parsed with options",
                 "[template][choice]")
{
    std::string content = "{{status:draft|published|archived}}";
    auto vars = engine_.parse_variables(content);

    REQUIRE(vars.size() == 1);
    CHECK(vars[0].type == markamp::core::NoteTemplateVariable::Type::Choice);
    CHECK(vars[0].choices.size() == 3);
    CHECK(vars[0].choices[0] == "draft");
    CHECK(vars[0].choices[1] == "published");
    CHECK(vars[0].choices[2] == "archived");
}

// ============================================================================
// Test 11: Create from template
// ============================================================================
TEST_CASE_METHOD(NoteTemplateTestFixture,
                 "create_from_template creates document",
                 "[template][create]")
{
    create_template_file("note", "# {{title}}\nCreated: {{date}}");

    (void)vault_service_.open_vault(vault_dir_);
    engine_.load_templates();

    markamp::core::NoteTemplateContext ctx;
    auto result = engine_.create_from_template("note", "Test Note", ctx);

    CHECK(result.has_value());
}

// ============================================================================
// Test 12: Missing variable kept as empty
// ============================================================================
TEST_CASE_METHOD(NoteTemplateTestFixture,
                 "Missing variable resolves to empty",
                 "[template][missing]")
{
    create_template_file("missing", "Value: {{unknown_var}}.");

    (void)vault_service_.open_vault(vault_dir_);
    engine_.load_templates();

    auto tmpl = engine_.get_template("missing");
    REQUIRE(tmpl.has_value());

    markamp::core::NoteTemplateContext ctx;
    auto result = engine_.apply(*tmpl, ctx);

    CHECK(result == "Value: .");
}
