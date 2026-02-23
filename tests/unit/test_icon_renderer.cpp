#include "ui/IconRenderer.h"
#include "ui/SvgDocument.h"

#include <catch2/catch_test_macros.hpp>
#include <wx/app.h>
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

TEST_CASE("IconRenderer: render basic document", "[icon_renderer]")
{
    wxApp::SetInstance(new TestApp());
    int argc = 0;
    char** argv = nullptr;
    wxEntryStart(argc, argv);

    std::string xml = R"(
        <svg viewBox="0 0 24 24" fill="none">
            <path d="M 0 0 L 24 24" fill="red" stroke="blue" stroke-width="2" />
        </svg>
    )";

    SvgDocument doc;
    REQUIRE(doc.parse(xml));

    wxSize targetSize(24, 24);
    wxColour color(255, 0, 0); // Red

    wxBitmap bmp = IconRenderer::RenderIcon(doc, targetSize, color, 1.0);

    REQUIRE(bmp.IsOk());
    CHECK(bmp.GetWidth() == 24);
    CHECK(bmp.GetHeight() == 24);

    wxEntryCleanup();
}
