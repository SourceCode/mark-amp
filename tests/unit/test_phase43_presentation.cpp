/// @file test_phase43_presentation.cpp
/// @brief Comprehensive tests for Phase 43 — Presentation & Slide System.

#include "core/CommandRegistry.h"
#include "core/Events.h"
#include "core/PresentationCommandProvider.h"
#include "core/PresentationManager.h"
#include "core/SlideCommandProvider.h"
#include "core/SlideEngine.h"
#include "core/SlideExporter.h"
#include "core/SlideThemeProvider.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ═══════════════════════════════════════════════════════════════════
// SlideEngine Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("SlideEngine: add and find slide", "[phase43][slides]")
{
    SlideEngine engine;
    Slide slide;
    slide.title = "Introduction";
    auto sid = engine.add_slide(std::move(slide));

    REQUIRE(engine.slide_count() == 1);
    const auto* found = engine.find_slide(sid);
    REQUIRE(found != nullptr);
    REQUIRE(found->title == "Introduction");
    REQUIRE(found->slide_number == 1);
}

TEST_CASE("SlideEngine: remove slide", "[phase43][slides]")
{
    SlideEngine engine;
    auto sid = engine.add_slide(Slide{});
    REQUIRE(engine.remove_slide(sid) == true);
    REQUIRE(engine.slide_count() == 0);
    REQUIRE(engine.remove_slide("nonexistent") == false);
}

TEST_CASE("SlideEngine: ordering and renumbering", "[phase43][slides]")
{
    SlideEngine engine;
    auto sid1 = engine.add_slide(Slide{});
    auto sid2 = engine.add_slide(Slide{});
    auto sid3 = engine.add_slide(Slide{});

    REQUIRE(engine.find_slide(sid1)->slide_number == 1);
    REQUIRE(engine.find_slide(sid3)->slide_number == 3);

    // Move slide 3 to position 0
    REQUIRE(engine.move_slide(sid3, 0) == true);
    REQUIRE(engine.find_slide(sid3)->slide_number == 1);
    REQUIRE(engine.find_slide(sid1)->slide_number == 2);
}

TEST_CASE("SlideEngine: swap slides", "[phase43][slides]")
{
    SlideEngine engine;
    Slide first;
    first.title = "First";
    auto sid1 = engine.add_slide(std::move(first));

    Slide second;
    second.title = "Second";
    auto sid2 = engine.add_slide(std::move(second));

    REQUIRE(engine.swap_slides(sid1, sid2) == true);
}

TEST_CASE("SlideEngine: content blocks", "[phase43][slides]")
{
    SlideEngine engine;
    auto sid = engine.add_slide(Slide{});

    SlideBlock title_block;
    title_block.block_type = SlideBlockType::kTitle;
    title_block.content = "My Presentation";
    REQUIRE(engine.add_block(sid, std::move(title_block)) == true);

    SlideBlock text_block;
    text_block.block_type = SlideBlockType::kText;
    text_block.content = "Welcome to the talk.";
    REQUIRE(engine.add_block(sid, std::move(text_block)) == true);

    REQUIRE(engine.block_count(sid) == 2);

    // Remove a block
    auto block_id = engine.find_slide(sid)->blocks[0].block_id;
    REQUIRE(engine.remove_block(sid, block_id) == true);
    REQUIRE(engine.block_count(sid) == 1);
}

TEST_CASE("SlideEngine: transitions", "[phase43][slides]")
{
    SlideEngine engine;
    auto sid = engine.add_slide(Slide{});

    REQUIRE(engine.set_transition(sid, SlideTransition::kFade) == true);
    REQUIRE(engine.find_slide(sid)->transition == SlideTransition::kFade);

    // Set all transitions
    engine.add_slide(Slide{});
    REQUIRE(engine.set_all_transitions(SlideTransition::kZoomIn) == 2);
}

TEST_CASE("SlideEngine: visibility", "[phase43][slides]")
{
    SlideEngine engine;
    auto sid1 = engine.add_slide(Slide{});
    auto sid2 = engine.add_slide(Slide{});

    REQUIRE(engine.hide_slide(sid1) == true);
    REQUIRE(engine.visible_slides().size() == 1);

    REQUIRE(engine.show_slide(sid1) == true);
    REQUIRE(engine.visible_slides().size() == 2);
}

TEST_CASE("SlideEngine: speaker notes", "[phase43][slides]")
{
    SlideEngine engine;
    auto sid = engine.add_slide(Slide{});

    REQUIRE(engine.set_speaker_notes(sid, "Remember to pause here") == true);
    REQUIRE(engine.find_slide(sid)->speaker_notes == "Remember to pause here");
}

TEST_CASE("SlideEngine: clear all", "[phase43][slides]")
{
    SlideEngine engine;
    engine.add_slide(Slide{});
    engine.add_slide(Slide{});
    engine.clear_all();
    REQUIRE(engine.slide_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// PresentationManager Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("PresentationManager: create and find", "[phase43][presentations]")
{
    PresentationManager mgr;
    auto pid = mgr.create_presentation("Team Update", 10);

    REQUIRE(mgr.presentation_count() == 1);
    const auto* pres = mgr.find_presentation(pid);
    REQUIRE(pres != nullptr);
    REQUIRE(pres->title == "Team Update");
    REQUIRE(pres->slide_count == 10);
    REQUIRE(pres->state == PresentationState::kIdle);
}

TEST_CASE("PresentationManager: close", "[phase43][presentations]")
{
    PresentationManager mgr;
    auto pid = mgr.create_presentation("Demo", 5);
    REQUIRE(mgr.close_presentation(pid) == true);
    REQUIRE(mgr.presentation_count() == 0);
}

TEST_CASE("PresentationManager: playback control", "[phase43][presentations]")
{
    PresentationManager mgr;
    auto pid = mgr.create_presentation("Talk", 8);

    REQUIRE(mgr.start_presentation(pid) == true);
    REQUIRE(mgr.find_presentation(pid)->state == PresentationState::kPlaying);

    REQUIRE(mgr.pause_presentation(pid) == true);
    REQUIRE(mgr.find_presentation(pid)->state == PresentationState::kPaused);

    REQUIRE(mgr.start_presentation(pid) == true);
    REQUIRE(mgr.stop_presentation(pid) == true);
    REQUIRE(mgr.find_presentation(pid)->state == PresentationState::kIdle);
    REQUIRE(mgr.find_presentation(pid)->current_slide == 1);
}

TEST_CASE("PresentationManager: navigation", "[phase43][presentations]")
{
    PresentationManager mgr;
    auto pid = mgr.create_presentation("Nav Test", 5);

    REQUIRE(mgr.go_to_slide(pid, 3) == true);
    REQUIRE(mgr.find_presentation(pid)->current_slide == 3);

    REQUIRE(mgr.next_slide(pid) == true);
    REQUIRE(mgr.find_presentation(pid)->current_slide == 4);

    REQUIRE(mgr.previous_slide(pid) == true);
    REQUIRE(mgr.find_presentation(pid)->current_slide == 3);

    // Bounds
    REQUIRE(mgr.go_to_slide(pid, 5) == true);
    REQUIRE(mgr.next_slide(pid) == false);
    REQUIRE(mgr.go_to_slide(pid, 1) == true);
    REQUIRE(mgr.previous_slide(pid) == false);
    REQUIRE(mgr.go_to_slide(pid, 0) == false);
    REQUIRE(mgr.go_to_slide(pid, 6) == false);
}

TEST_CASE("PresentationManager: loop wrap-around", "[phase43][presentations]")
{
    PresentationManager mgr;
    auto pid = mgr.create_presentation("Loop Test", 3);
    mgr.set_loop(pid, true);
    mgr.go_to_slide(pid, 3);

    REQUIRE(mgr.next_slide(pid) == true);
    REQUIRE(mgr.find_presentation(pid)->current_slide == 1);
}

TEST_CASE("PresentationManager: presenter mode", "[phase43][presentations]")
{
    PresentationManager mgr;
    auto pid = mgr.create_presentation("Pres Mode", 5);

    REQUIRE(mgr.toggle_presenter_mode(pid) == true);
    REQUIRE(mgr.find_presentation(pid)->presenter_mode == true);

    REQUIRE(mgr.toggle_presenter_mode(pid) == true);
    REQUIRE(mgr.find_presentation(pid)->presenter_mode == false);
}

TEST_CASE("PresentationManager: auto advance", "[phase43][presentations]")
{
    PresentationManager mgr;
    auto pid = mgr.create_presentation("Auto", 5);

    REQUIRE(mgr.set_auto_advance(pid, 5) == true);
    REQUIRE(mgr.find_presentation(pid)->auto_advance_seconds == 5);
    REQUIRE(mgr.set_auto_advance(pid, -1) == false);
}

TEST_CASE("PresentationManager: active presentations", "[phase43][presentations]")
{
    PresentationManager mgr;
    auto pid1 = mgr.create_presentation("A", 5);
    mgr.create_presentation("B", 3);
    mgr.start_presentation(pid1);

    REQUIRE(mgr.active_presentations().size() == 1);
    REQUIRE(mgr.all_presentations().size() == 2);
}

TEST_CASE("PresentationManager: close all", "[phase43][presentations]")
{
    PresentationManager mgr;
    mgr.create_presentation("A", 5);
    mgr.create_presentation("B", 3);
    mgr.close_all();
    REQUIRE(mgr.presentation_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// SlideThemeProvider Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("SlideThemeProvider: register and find", "[phase43][themes]")
{
    SlideThemeProvider provider;
    SlideTheme theme;
    theme.theme_id = "custom-1";
    theme.name = "Custom Theme";
    provider.register_theme(std::move(theme));

    REQUIRE(provider.theme_count() == 1);
    REQUIRE(provider.find_theme("custom-1")->name == "Custom Theme");
}

TEST_CASE("SlideThemeProvider: remove", "[phase43][themes]")
{
    SlideThemeProvider provider;
    SlideTheme theme;
    theme.theme_id = "rm-theme";
    provider.register_theme(std::move(theme));

    REQUIRE(provider.remove_theme("rm-theme") == true);
    REQUIRE(provider.theme_count() == 0);
}

TEST_CASE("SlideThemeProvider: load_defaults", "[phase43][themes]")
{
    SlideThemeProvider provider;
    provider.load_defaults();

    REQUIRE(provider.theme_count() == 3);
    REQUIRE(provider.find_theme("light-default") != nullptr);
    REQUIRE(provider.find_theme("dark-default") != nullptr);
    REQUIRE(provider.find_theme("minimal") != nullptr);
}

TEST_CASE("SlideThemeProvider: dark and light filter", "[phase43][themes]")
{
    SlideThemeProvider provider;
    provider.load_defaults();

    REQUIRE(provider.dark_themes().size() == 1);
    REQUIRE(provider.light_themes().size() == 2);
    REQUIRE(provider.builtin_themes().size() == 3);
}

TEST_CASE("SlideThemeProvider: clear", "[phase43][themes]")
{
    SlideThemeProvider provider;
    provider.load_defaults();
    provider.clear_all();
    REQUIRE(provider.theme_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// SlideExporter Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("SlideExporter: export presentation", "[phase43][export]")
{
    SlideExporter exporter;
    SlideExportOptions opts;
    opts.format = SlideExportFormat::kPdf;
    opts.output_path = "/output/slides.pdf";

    auto eid = exporter.export_presentation("pres_1", opts);
    REQUIRE(exporter.export_count() == 1);

    const auto* result = exporter.find_export(eid);
    REQUIRE(result != nullptr);
    REQUIRE(result->status == ExportStatus::kCompleted);
    REQUIRE(result->slides_exported == 10);
}

TEST_CASE("SlideExporter: export format name", "[phase43][export]")
{
    REQUIRE(slide_export_format_name(SlideExportFormat::kPdf) == "pdf");
    REQUIRE(slide_export_format_name(SlideExportFormat::kHtml) == "html");
    REQUIRE(slide_export_format_name(SlideExportFormat::kPng) == "png");
    REQUIRE(slide_export_format_name(SlideExportFormat::kSvg) == "svg");
    REQUIRE(slide_export_format_name(SlideExportFormat::kPptx) == "pptx");
}

TEST_CASE("SlideExporter: exports by format", "[phase43][export]")
{
    SlideExporter exporter;
    SlideExportOptions pdf_opts;
    pdf_opts.format = SlideExportFormat::kPdf;
    exporter.export_presentation("p1", pdf_opts);

    SlideExportOptions html_opts;
    html_opts.format = SlideExportFormat::kHtml;
    exporter.export_presentation("p2", html_opts);

    REQUIRE(exporter.exports_by_format(SlideExportFormat::kPdf).size() == 1);
    REQUIRE(exporter.exports_by_format(SlideExportFormat::kHtml).size() == 1);
}

TEST_CASE("SlideExporter: clear history", "[phase43][export]")
{
    SlideExporter exporter;
    SlideExportOptions opts;
    exporter.export_presentation("p1", opts);
    exporter.clear_history();
    REQUIRE(exporter.export_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// Command Provider Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("PresentationCommandProvider: count and ids", "[phase43][pres-commands]")
{
    REQUIRE(PresentationCommandProvider::command_count() == 8);
    auto ids = PresentationCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "pres.create");
}

TEST_CASE("PresentationCommandProvider: register to registry", "[phase43][pres-commands]")
{
    CommandRegistry registry;
    const PresentationCommandProvider provider;
    provider.register_commands(registry);

    REQUIRE(registry.get_command("pres.create") != nullptr);
    REQUIRE(registry.get_command("pres.create")->category == "Presentation");
}

TEST_CASE("SlideCommandProvider: count and ids", "[phase43][slide-commands]")
{
    REQUIRE(SlideCommandProvider::command_count() == 8);
    auto ids = SlideCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "slide.add");
}

TEST_CASE("SlideCommandProvider: register to registry", "[phase43][slide-commands]")
{
    CommandRegistry registry;
    const SlideCommandProvider provider;
    provider.register_commands(registry);

    REQUIRE(registry.get_command("slide.add") != nullptr);
    REQUIRE(registry.get_command("slide.add")->category == "Slides");
}

// ═══════════════════════════════════════════════════════════════════
// Phase 43 Events Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Phase 43 Events: PresentationCreatedEvent", "[phase43][events]")
{
    PresentationCreatedEvent event;
    event.presentation_id = "pres_1";
    event.title = "My Talk";
    event.slide_count = 20;
    REQUIRE(event.slide_count == 20);
}

TEST_CASE("Phase 43 Events: SlideshowStateChangedEvent", "[phase43][events]")
{
    SlideshowStateChangedEvent event;
    event.presentation_id = "pres_1";
    event.new_state = "playing";
    event.current_slide = 5;
    REQUIRE(event.new_state == "playing");
}

TEST_CASE("Phase 43 Events: PresentationSlideChangedEvent", "[phase43][events]")
{
    PresentationSlideChangedEvent event;
    event.slide_id = "s_1";
    event.action = "added";
    event.slide_number = 3;
    REQUIRE(event.action == "added");
}

TEST_CASE("Phase 43 Events: SlideThemeAppliedEvent", "[phase43][events]")
{
    SlideThemeAppliedEvent event;
    event.presentation_id = "pres_1";
    event.theme_id = "dark-default";
    event.theme_name = "Dark Default";
    REQUIRE(event.theme_name == "Dark Default");
}

TEST_CASE("Phase 43 Events: SlideExportCompletedEvent", "[phase43][events]")
{
    SlideExportCompletedEvent event;
    event.export_id = "exp_1";
    event.presentation_id = "pres_1";
    event.format = "pdf";
    event.success = true;
    event.slides_exported = 15;
    REQUIRE(event.success == true);
}

TEST_CASE("Phase 43 Events: SlideBlockChangedEvent", "[phase43][events]")
{
    SlideBlockChangedEvent event;
    event.slide_id = "s_1";
    event.block_id = "b_1";
    event.action = "added";
    REQUIRE(event.action == "added");
}
