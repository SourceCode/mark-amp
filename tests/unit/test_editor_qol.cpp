#include "core/Config.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"
#include "ui/EditorPanel.h"

#include <catch2/catch_test_macros.hpp>
#include <wx/wx.h>

// Mock for EditorPanel dependencies
class MockThemeEngine : public markamp::core::ThemeEngine
{
public:
    MockThemeEngine(markamp::core::EventBus& bus, markamp::core::ThemeRegistry& registry)
        : markamp::core::ThemeEngine(bus, registry)
    {
    }
};

// We need a wxApp to run GUI tests, even headless
class TestApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        return true;
    }
};

// Ensure wxApp is initialized
struct WxInit
{
    WxInit()
    {
        if (!wxTheApp)
        {
            wxApp::SetInstance(new TestApp());
            int argc = 0;
            char* argv[] = {nullptr};
            wxEntryStart(argc, argv);
        }
    }
    ~WxInit()
    {
        // wxEntryCleanup(); // Causes crash in some environments if called repeatedly
    }
};

TEST_CASE("EditorPanel QoL Features", "[editor][qol]")
{
    WxInit init;

    // Setup boilerplate
    markamp::core::EventBus event_bus;
    markamp::core::ThemeRegistry registry;
    markamp::core::ThemeEngine theme_engine(event_bus, registry);

    // Create a hidden frame as parent
    wxFrame* frame = new wxFrame(nullptr, wxID_ANY, "Test Frame");
    markamp::core::Config config;

    // Use correct constructor (3 args)
    markamp::ui::EditorPanel* editor = new markamp::ui::EditorPanel(frame, theme_engine, event_bus);
    editor->LoadPreferences(config);

    SECTION("ToggleLineComment adds based on selection")
    {
        editor->SetContent("Hello World");

        // Select entire content
        editor->SetSelection(0, editor->GetContent().length());

        editor->ToggleLineComment();

        // Expected: <!-- Hello World -->
        std::string text = editor->GetContent();
        REQUIRE(text == "<!-- Hello World -->");

        // Toggle back
        editor->SetSelection(0, text.length());
        editor->ToggleLineComment();
        REQUIRE(editor->GetContent() == "Hello World");
    }

    SECTION("DuplicateLine duplicates current line")
    {
        editor->SetContent("Line 1");
        // Cursor at 0
        editor->SetSelection(0, 0);

        editor->DuplicateLine();

        // Expect: Line 1\nLine 1
        std::string text = editor->GetContent();
        REQUIRE(text.find("Line 1") != std::string::npos);
        // Should have length > original
        REQUIRE(text.length() > 6);
    }

    SECTION("InsertDateTime inserts ISO date")
    {
        editor->SetContent("");
        editor->InsertDateTime();
        std::string text = editor->GetContent();
        REQUIRE(text.length() > 0);
        // Basic check for ISO format (YYYY-MM-DD...)
        REQUIRE(text.find("-") != std::string::npos);
        REQUIRE(text.find(":") != std::string::npos);
    }

    frame->Destroy();
}

// ═══════════════════════════════════════════════════════
// V8 Phase 9: Quality-bar tests (no GUI required)
// ═══════════════════════════════════════════════════════

#include "core/Events.h"
#include "core/Theme.h"
#include "ui/SplitView.h"

namespace events = markamp::core::events;
using markamp::core::ThemeColors;
using markamp::core::ThemeColorToken;
using markamp::ui::EditorStateLayer;
using markamp::ui::SnapPreset;

TEST_CASE("ThemeColorToken includes semantic editor tokens", "[theme][editor][phase9]")
{
    REQUIRE(ThemeColorToken::EditorActiveLine != ThemeColorToken::EditorGutterError);
    REQUIRE(ThemeColorToken::EditorGutterError != ThemeColorToken::EditorGutterWarn);
    REQUIRE(ThemeColorToken::EditorGutterWarn != ThemeColorToken::EditorGutterInfo);
    REQUIRE(ThemeColorToken::EditorGutterInfo != ThemeColorToken::EditorMatchHighlight);
    REQUIRE(ThemeColorToken::EditorMatchHighlight != ThemeColorToken::EditorFindHit);
    REQUIRE(ThemeColorToken::EditorFindHit != ThemeColorToken::EditorQuickFix);

    auto quick_fix_idx = static_cast<std::size_t>(ThemeColorToken::EditorQuickFix);
    REQUIRE(markamp::core::kColorTokenCount > quick_fix_idx);
}

TEST_CASE("ThemeColors editor state color defaults", "[theme][editor][phase9]")
{
    ThemeColors colors{};

    SECTION("Gutter error is reddish")
    {
        CHECK(colors.editor_gutter_error.r > 200);
        CHECK(colors.editor_gutter_error.g < 100);
    }

    SECTION("Gutter warn is yellowish")
    {
        CHECK(colors.editor_gutter_warn.r > 200);
        CHECK(colors.editor_gutter_warn.g > 150);
    }

    SECTION("Gutter info is bluish")
    {
        CHECK(colors.editor_gutter_info.b > 200);
    }

    SECTION("Quick fix is greenish")
    {
        CHECK(colors.editor_quick_fix.g > colors.editor_quick_fix.r);
    }
}

TEST_CASE("EditorStateLayer enum covers 7 layers", "[editor][phase9]")
{
    REQUIRE(EditorStateLayer::kCursor != EditorStateLayer::kSelection);
    REQUIRE(EditorStateLayer::kSelection != EditorStateLayer::kDiagnostics);
    REQUIRE(EditorStateLayer::kDiagnostics != EditorStateLayer::kActiveLine);
    REQUIRE(EditorStateLayer::kActiveLine != EditorStateLayer::kSearch);
    REQUIRE(EditorStateLayer::kSearch != EditorStateLayer::kCodeActions);
    REQUIRE(EditorStateLayer::kCodeActions != EditorStateLayer::kWriteMode);

    CHECK(static_cast<int>(EditorStateLayer::kCursor) <
          static_cast<int>(EditorStateLayer::kWriteMode));
}

TEST_CASE("SnapPreset includes workspace presets", "[splitview][phase9]")
{
    SECTION("Original presets")
    {
        CHECK(static_cast<int>(SnapPreset::Even) >= 0);
        CHECK(static_cast<int>(SnapPreset::EditorWide) >= 0);
        CHECK(static_cast<int>(SnapPreset::PreviewWide) >= 0);
    }

    SECTION("V8 Phase 9 presets")
    {
        CHECK(static_cast<int>(SnapPreset::EditorFocus) >= 0);
        CHECK(static_cast<int>(SnapPreset::Balanced) >= 0);
        CHECK(static_cast<int>(SnapPreset::Review) >= 0);
        CHECK(static_cast<int>(SnapPreset::PreviewFocus) >= 0);
    }

    SECTION("All distinct")
    {
        CHECK(SnapPreset::Even != SnapPreset::EditorWide);
        CHECK(SnapPreset::EditorWide != SnapPreset::PreviewWide);
        CHECK(SnapPreset::PreviewWide != SnapPreset::EditorFocus);
        CHECK(SnapPreset::EditorFocus != SnapPreset::Balanced);
        CHECK(SnapPreset::Balanced != SnapPreset::Review);
        CHECK(SnapPreset::Review != SnapPreset::PreviewFocus);
    }
}

TEST_CASE("ScrollSyncMode includes CursorAnchored", "[events][phase9]")
{
    REQUIRE(events::ScrollSyncMode::Proportional != events::ScrollSyncMode::CursorAnchored);
    REQUIRE(events::ScrollSyncMode::HeadingAnchor != events::ScrollSyncMode::CursorAnchored);

    events::ScrollSyncModeChangedEvent evt(events::ScrollSyncMode::CursorAnchored);
    CHECK(evt.mode == events::ScrollSyncMode::CursorAnchored);
}

TEST_CASE("kColorTokenCount integrity after Phase 9", "[theme][phase9]")
{
    auto quick_fix_idx = static_cast<std::size_t>(ThemeColorToken::EditorQuickFix);
    REQUIRE(markamp::core::kColorTokenCount > quick_fix_idx);
    CHECK(markamp::core::kColorTokenCount >= 30);
}

TEST_CASE("Minimap animation transition manager init", "[editor][minimap]")
{
    WxInit init; // Ensure wxWidgets is initialized for EditorPanel creation

    markamp::core::EventBus event_bus;
    markamp::core::ThemeRegistry theme_registry;
    markamp::core::ThemeEngine theme_engine(event_bus, theme_registry);

    auto* frame = new wxFrame(nullptr, wxID_ANY, "Test Frame");
    auto* editor = new markamp::ui::EditorPanel(frame, theme_engine, event_bus);

    // Initial state: minimap invisible
    REQUIRE(editor->GetZoomLevel() == 0); // Side effect assertion

    // Toggle minimap to trigger initialization of MinimapPanel and TransitionManager
    editor->ToggleMinimap();

    // Since transition manager is tested via the side effects inside EditorPanel (layout changes)
    // we just ensure calling ToggleMinimap multiple times doesn't crash or leak state.
    editor->ToggleMinimap();
    editor->ToggleMinimap();

    frame->Destroy();
}
