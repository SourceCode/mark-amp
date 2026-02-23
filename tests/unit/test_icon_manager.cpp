#include "ui/IconManager.h"

#include <catch2/catch_test_macros.hpp>
#include <wx/app.h>
#include <wx/dcmemory.h>
#include <wx/init.h>

using namespace markamp::ui;

class TestApp : public wxAppConsole
{
public:
    virtual bool OnInit() override
    {
        return true;
    }
};

TEST_CASE("IconManager: basic operations", "[icon_manager]")
{
    wxApp::SetInstance(new TestApp());
    int argc = 0;
    char** argv = nullptr;
    wxEntryStart(argc, argv);

    auto& manager = IconManager::get();
    manager.registry().clear();
    manager.cache().clear();

    std::string valid_svg = R"(
        <svg viewBox="0 0 24 24" fill="none">
            <path d="M 0 0 L 24 24" fill="red" />
        </svg>
    )";

    REQUIRE(manager.registry().register_icon("test_icon", valid_svg));

    wxSize size(24, 24);
    wxColour color(255, 0, 0); // Red

    wxBitmap bmp1 = manager.get_icon_bitmap("test_icon", size, color, 1.0);
    REQUIRE(bmp1.IsOk());
    CHECK(manager.cache().size() == 1);

    wxBitmap bmp2 = manager.get_icon_bitmap("test_icon", size, color, 1.0);
    REQUIRE(bmp2.IsOk());
    CHECK(manager.cache().size() == 1); // Uses cache, so size stays 1

    wxBitmap output(100, 100, 32);
    wxMemoryDC memDC(output);
    manager.draw_icon(memDC, "test_icon", 10, 10, size, color, 1.0);
    // Hard to verify drawn output in headless test easily without checking pixel colors,
    // but we can verify it doesn't crash.

    wxEntryCleanup();
}
