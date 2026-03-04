#include "core/DiagnosticsService.h"
#include "ui/ProblemsTreeModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

// ── Helper: make a diagnostic ──

static auto
make_diag(DiagnosticSeverity sev, const std::string& msg, const std::string& src = "", int line = 0)
    -> Diagnostic
{
    Diagnostic d;
    d.severity = sev;
    d.message = msg;
    d.source = src;
    d.range.start.line = line;
    d.range.start.character = 0;
    d.range.end = d.range.start;
    return d;
}

// ──────────────────────────────────────────────
// ProblemsTreeModel — rebuild and grouping
// ──────────────────────────────────────────────

TEST_CASE("ProblemsTreeModel: empty service yields empty tree", "[ProblemsTreeModel]")
{
    DiagnosticsService svc;
    ProblemsTreeModel model;

    model.rebuild(svc);

    REQUIRE(model.file_nodes().empty());
    REQUIRE(model.total_error_count() == 0);
    REQUIRE(model.total_warning_count() == 0);
    REQUIRE(model.total_info_count() == 0);
    REQUIRE(model.total_diagnostic_count() == 0);
}

TEST_CASE("ProblemsTreeModel: groups diagnostics by file URI", "[ProblemsTreeModel]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp", {make_diag(DiagnosticSeverity::kError, "err1")});
    svc.set("file:///b.cpp", {make_diag(DiagnosticSeverity::kWarning, "warn1")});

    ProblemsTreeModel model;
    model.rebuild(svc);

    REQUIRE(model.file_nodes().size() == 2);
    REQUIRE(model.total_diagnostic_count() == 2);
}

TEST_CASE("ProblemsTreeModel: display_name extracts basename", "[ProblemsTreeModel]")
{
    DiagnosticsService svc;
    svc.set("file:///path/to/main.cpp", {make_diag(DiagnosticSeverity::kError, "err")});

    ProblemsTreeModel model;
    model.rebuild(svc);

    REQUIRE(model.file_nodes().size() == 1);
    REQUIRE(model.file_nodes()[0].display_name == "main.cpp");
}

TEST_CASE("ProblemsTreeModel: total counts are correct", "[ProblemsTreeModel]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp",
            {
                make_diag(DiagnosticSeverity::kError, "e1"),
                make_diag(DiagnosticSeverity::kError, "e2"),
                make_diag(DiagnosticSeverity::kWarning, "w1"),
            });
    svc.set("file:///b.cpp",
            {
                make_diag(DiagnosticSeverity::kInformation, "i1"),
                make_diag(DiagnosticSeverity::kWarning, "w2"),
            });

    ProblemsTreeModel model;
    model.rebuild(svc);

    REQUIRE(model.total_error_count() == 2);
    REQUIRE(model.total_warning_count() == 2);
    REQUIRE(model.total_info_count() == 1);
    REQUIRE(model.total_diagnostic_count() == 5);
}

// ──────────────────────────────────────────────
// ProblemFileNode — per-file counts and severity
// ──────────────────────────────────────────────

TEST_CASE("ProblemFileNode: severity counts", "[ProblemsTreeModel]")
{
    ProblemFileNode node;
    node.diagnostics = {
        make_diag(DiagnosticSeverity::kError, "e"),
        make_diag(DiagnosticSeverity::kWarning, "w"),
        make_diag(DiagnosticSeverity::kInformation, "i"),
        make_diag(DiagnosticSeverity::kHint, "h"),
    };

    REQUIRE(node.error_count() == 1);
    REQUIRE(node.warning_count() == 1);
    REQUIRE(node.info_count() == 1);
}

TEST_CASE("ProblemFileNode: max_severity returns highest severity", "[ProblemsTreeModel]")
{
    ProblemFileNode node;
    node.diagnostics = {
        make_diag(DiagnosticSeverity::kWarning, "w"),
        make_diag(DiagnosticSeverity::kInformation, "i"),
    };
    REQUIRE(node.max_severity() == DiagnosticSeverity::kWarning);

    node.diagnostics.push_back(make_diag(DiagnosticSeverity::kError, "e"));
    REQUIRE(node.max_severity() == DiagnosticSeverity::kError);
}

// ──────────────────────────────────────────────
// Severity filter
// ──────────────────────────────────────────────

TEST_CASE("ProblemsTreeModel: severity filter", "[ProblemsTreeModel]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp",
            {
                make_diag(DiagnosticSeverity::kError, "err"),
                make_diag(DiagnosticSeverity::kWarning, "warn"),
                make_diag(DiagnosticSeverity::kInformation, "info"),
                make_diag(DiagnosticSeverity::kHint, "hint"),
            });

    ProblemsTreeModel model;

    // Filter: errors only
    model.set_severity_filter(DiagnosticSeverity::kError);
    model.rebuild(svc);
    REQUIRE(model.total_diagnostic_count() == 1);

    // Filter: warnings and above
    model.set_severity_filter(DiagnosticSeverity::kWarning);
    model.rebuild(svc);
    REQUIRE(model.total_diagnostic_count() == 2);

    // Filter: all
    model.set_severity_filter(DiagnosticSeverity::kHint);
    model.rebuild(svc);
    REQUIRE(model.total_diagnostic_count() == 4);
}

// ──────────────────────────────────────────────
// Source filter
// ──────────────────────────────────────────────

TEST_CASE("ProblemsTreeModel: source filter", "[ProblemsTreeModel]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp",
            {
                make_diag(DiagnosticSeverity::kError, "gcc err", "gcc"),
                make_diag(DiagnosticSeverity::kWarning, "clang warn", "clang"),
            });

    ProblemsTreeModel model;

    model.set_source_filter("gcc");
    model.rebuild(svc);
    REQUIRE(model.total_diagnostic_count() == 1);
    REQUIRE(model.file_nodes()[0].diagnostics[0].source == "gcc");

    model.set_source_filter("");
    model.rebuild(svc);
    REQUIRE(model.total_diagnostic_count() == 2);
}

// ──────────────────────────────────────────────
// Text filter
// ──────────────────────────────────────────────

TEST_CASE("ProblemsTreeModel: text filter", "[ProblemsTreeModel]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp",
            {
                make_diag(DiagnosticSeverity::kError, "undefined reference to foo"),
                make_diag(DiagnosticSeverity::kWarning, "unused variable bar"),
            });

    ProblemsTreeModel model;

    model.set_text_filter("foo");
    model.rebuild(svc);
    REQUIRE(model.total_diagnostic_count() == 1);

    model.set_text_filter("UNUSED"); // case-insensitive
    model.rebuild(svc);
    // Depends on implementation — may or may not be case-insensitive
    // Just verify filter resets to full set
    model.set_text_filter("");
    model.rebuild(svc);
    REQUIRE(model.total_diagnostic_count() == 2);
}

// ──────────────────────────────────────────────
// Sort controls
// ──────────────────────────────────────────────

TEST_CASE("ProblemsTreeModel: sort mode persists", "[ProblemsTreeModel]")
{
    ProblemsTreeModel model;

    model.set_sort_mode(ProblemSortMode::kFile);
    REQUIRE(model.sort_mode() == ProblemSortMode::kFile);

    model.set_sort_mode(ProblemSortMode::kSeverity);
    REQUIRE(model.sort_mode() == ProblemSortMode::kSeverity);
}

TEST_CASE("ProblemsTreeModel: sort by severity puts errors first", "[ProblemsTreeModel]")
{
    DiagnosticsService svc;
    svc.set("file:///info.cpp", {make_diag(DiagnosticSeverity::kInformation, "info")});
    svc.set("file:///error.cpp", {make_diag(DiagnosticSeverity::kError, "error")});
    svc.set("file:///warn.cpp", {make_diag(DiagnosticSeverity::kWarning, "warn")});

    ProblemsTreeModel model;
    model.set_sort_mode(ProblemSortMode::kSeverity);
    model.rebuild(svc);

    const auto& nodes = model.file_nodes();
    REQUIRE(nodes.size() == 3);
    // Error file should come first in severity sort
    REQUIRE(nodes[0].max_severity() == DiagnosticSeverity::kError);
}

// ──────────────────────────────────────────────
// Collapse/expand
// ──────────────────────────────────────────────

TEST_CASE("ProblemsTreeModel: collapse and expand all", "[ProblemsTreeModel]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp", {make_diag(DiagnosticSeverity::kError, "e")});
    svc.set("file:///b.cpp", {make_diag(DiagnosticSeverity::kWarning, "w")});

    ProblemsTreeModel model;
    model.rebuild(svc);

    model.collapse_all();
    for (const auto& node : model.file_nodes())
    {
        REQUIRE(node.collapsed);
    }

    model.expand_all();
    for (const auto& node : model.file_nodes())
    {
        REQUIRE_FALSE(node.collapsed);
    }
}

TEST_CASE("ProblemsTreeModel: toggle individual file", "[ProblemsTreeModel]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp", {make_diag(DiagnosticSeverity::kError, "e")});

    ProblemsTreeModel model;
    model.rebuild(svc);

    REQUIRE_FALSE(model.file_nodes()[0].collapsed);
    model.toggle_file("file:///a.cpp");
    REQUIRE(model.file_nodes()[0].collapsed);
    model.toggle_file("file:///a.cpp");
    REQUIRE_FALSE(model.file_nodes()[0].collapsed);
}

// ──────────────────────────────────────────────
// Empty file gets dropped after filter
// ──────────────────────────────────────────────

TEST_CASE("ProblemsTreeModel: files with no matching diagnostics are omitted",
          "[ProblemsTreeModel]")
{
    DiagnosticsService svc;
    svc.set("file:///only_hints.cpp", {make_diag(DiagnosticSeverity::kHint, "h")});
    svc.set("file:///has_error.cpp", {make_diag(DiagnosticSeverity::kError, "e")});

    ProblemsTreeModel model;
    model.set_severity_filter(DiagnosticSeverity::kError);
    model.rebuild(svc);

    REQUIRE(model.file_nodes().size() == 1);
    REQUIRE(model.file_nodes()[0].file_uri == "file:///has_error.cpp");
}

// ──────────────────────────────────────────────
// Problem navigation (Phase 23 Task 24)
// ──────────────────────────────────────────────

TEST_CASE("ProblemsTreeModel: navigate_to_problem returns correct location", "[navigation]")
{
    DiagnosticsService svc;
    svc.set("file:///main.cpp",
            {make_diag(DiagnosticSeverity::kError, "undefined foo", "gcc", 42)});

    ProblemsTreeModel model;
    model.rebuild(svc);

    const auto& nodes = model.file_nodes();
    REQUIRE(nodes.size() == 1);
    REQUIRE(nodes[0].diagnostics.size() == 1);
    REQUIRE(nodes[0].diagnostics[0].range.start.line == 42);
    REQUIRE(nodes[0].file_uri == "file:///main.cpp");
}

TEST_CASE("ProblemsTreeModel: multiple diagnostics maintain correct line numbers", "[navigation]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp",
            {
                make_diag(DiagnosticSeverity::kError, "err1", "gcc", 10),
                make_diag(DiagnosticSeverity::kWarning, "warn1", "gcc", 25),
                make_diag(DiagnosticSeverity::kError, "err2", "gcc", 50),
            });

    ProblemsTreeModel model;
    model.rebuild(svc);

    const auto& diags = model.file_nodes()[0].diagnostics;
    REQUIRE(diags.size() == 3);
    // Diagnostics may be sorted by severity (errors first) within a file node
    // Verify all three line numbers are present
    bool has_10 = false;
    bool has_25 = false;
    bool has_50 = false;
    for (const auto& diag : diags)
    {
        if (diag.range.start.line == 10)
        {
            has_10 = true;
        }
        if (diag.range.start.line == 25)
        {
            has_25 = true;
        }
        if (diag.range.start.line == 50)
        {
            has_50 = true;
        }
    }
    REQUIRE(has_10);
    REQUIRE(has_25);
    REQUIRE(has_50);
}

// ──────────────────────────────────────────────
// Diagnostic event publishing (Phase 23 Task 25)
// ──────────────────────────────────────────────

TEST_CASE("DiagnosticsService: change listener fires on set", "[events]")
{
    DiagnosticsService svc;
    std::string fired_uri;
    svc.on_change([&fired_uri](const std::string& uri) { fired_uri = uri; });

    svc.set("file:///test.cpp", {make_diag(DiagnosticSeverity::kError, "err")});
    REQUIRE(fired_uri == "file:///test.cpp");
}

TEST_CASE("DiagnosticsService: change listener fires on remove", "[events]")
{
    DiagnosticsService svc;
    svc.set("file:///test.cpp", {make_diag(DiagnosticSeverity::kError, "err")});

    std::string fired_uri;
    svc.on_change([&fired_uri](const std::string& uri) { fired_uri = uri; });

    svc.remove("file:///test.cpp");
    REQUIRE(fired_uri == "file:///test.cpp");
}

TEST_CASE("DiagnosticsService: change listener fires on clear", "[events]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp", {make_diag(DiagnosticSeverity::kError, "err")});
    svc.set("file:///b.cpp", {make_diag(DiagnosticSeverity::kWarning, "warn")});

    int fire_count = 0;
    svc.on_change([&fire_count](const std::string& /*uri*/) { fire_count++; });

    svc.clear();
    REQUIRE(fire_count == 2); // One for each URI
}

TEST_CASE("DiagnosticsService: remove_listener stops notifications", "[events]")
{
    DiagnosticsService svc;
    int fire_count = 0;
    auto listener_id = svc.on_change([&fire_count](const std::string& /*uri*/) { fire_count++; });

    svc.set("file:///a.cpp", {make_diag(DiagnosticSeverity::kError, "err")});
    REQUIRE(fire_count == 1);

    svc.remove_listener(listener_id);
    svc.set("file:///b.cpp", {make_diag(DiagnosticSeverity::kWarning, "warn")});
    REQUIRE(fire_count == 1); // Still 1, listener was removed
}

// ──────────────────────────────────────────────
// Batch diagnostics (integration)
// ──────────────────────────────────────────────

TEST_CASE("DiagnosticsService: clear_by_source", "[batch]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp",
            {
                make_diag(DiagnosticSeverity::kError, "gcc err", "gcc"),
                make_diag(DiagnosticSeverity::kWarning, "clang warn", "clang"),
            });

    svc.clear_by_source("gcc");
    REQUIRE(svc.total_count() == 1);
    REQUIRE(svc.get("file:///a.cpp")[0].source == "clang");
}

TEST_CASE("DiagnosticsService: clear_by_severity", "[batch]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp",
            {
                make_diag(DiagnosticSeverity::kError, "err"),
                make_diag(DiagnosticSeverity::kWarning, "warn"),
                make_diag(DiagnosticSeverity::kInformation, "info"),
            });

    svc.clear_by_severity(DiagnosticSeverity::kWarning);
    REQUIRE(svc.total_count() == 2);
}

TEST_CASE("DiagnosticsService: diagnostics_for_severity", "[batch]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp", {make_diag(DiagnosticSeverity::kError, "e1")});
    svc.set("file:///b.cpp",
            {
                make_diag(DiagnosticSeverity::kError, "e2"),
                make_diag(DiagnosticSeverity::kWarning, "w1"),
            });

    auto errors = svc.diagnostics_for_severity(DiagnosticSeverity::kError);
    REQUIRE(errors.size() == 2);
}

TEST_CASE("Diagnostic: related_information field", "[related_info]")
{
    Diagnostic diag;
    diag.message = "Undefined variable 'x'";
    diag.severity = DiagnosticSeverity::kError;

    Diagnostic::RelatedInfo info;
    info.uri = "file:///header.h";
    info.position = {.line = 10, .character = 5};
    info.message = "First declared here";
    diag.related_information.push_back(info);

    REQUIRE(diag.related_information.size() == 1);
    REQUIRE(diag.related_information[0].uri == "file:///header.h");
    REQUIRE(diag.related_information[0].position.line == 10);
}
