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
