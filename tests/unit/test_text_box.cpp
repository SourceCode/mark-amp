#include "canvas/CanvasTypes.h"
#include "canvas/TextBox.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("TextBox default construction", "[canvas][textbox]")
{
    TextBox box;
    REQUIRE(box.type() == CanvasObjectType::TextBox);
    REQUIRE(box.text().empty());
    REQUIRE(box.width() == Catch::Approx(200.0));
    REQUIRE(box.height() == Catch::Approx(100.0));
    REQUIRE_FALSE(box.has_border());
    REQUIRE_FALSE(box.has_fill());
}

TEST_CASE("TextBox set text and style", "[canvas][textbox]")
{
    TextBox box;
    box.set_text("Hello");
    REQUIRE(box.text() == "Hello");

    TextStyle style;
    style.font_size = 24.0;
    style.bold = true;
    style.alignment = TextAlignment::kCenter;
    box.set_style(style);

    REQUIRE(box.style().font_size == Catch::Approx(24.0));
    REQUIRE(box.style().bold);
    REQUIRE(box.style().alignment == TextAlignment::kCenter);
}

TEST_CASE("TextBox resize with min constraint", "[canvas][textbox]")
{
    TextBox box;
    box.resize(500.0, 300.0);
    REQUIRE(box.width() == Catch::Approx(500.0));
    REQUIRE(box.height() == Catch::Approx(300.0));

    // Below minimum.
    box.resize(5.0, 5.0);
    REQUIRE(box.width() == Catch::Approx(40.0));
    REQUIRE(box.height() == Catch::Approx(20.0));
}

TEST_CASE("TextBox auto_height empty text", "[canvas][textbox]")
{
    TextBox box;
    const auto height = box.auto_height();
    // 1 line * 14 * 1.4 + 10 * 2 = 19.6 + 20 = 39.6
    REQUIRE(height == Catch::Approx(39.6));
}

TEST_CASE("TextBox auto_height multi-line text", "[canvas][textbox]")
{
    TextBox box;
    box.set_text("Line 1\nLine 2\nLine 3");
    const auto height = box.auto_height();
    // 3 lines * 14 * 1.4 + 20 = 58.8 + 20 = 78.8
    REQUIRE(height == Catch::Approx(78.8));
}

TEST_CASE("TextBox border and fill", "[canvas][textbox]")
{
    TextBox box;
    REQUIRE_FALSE(box.has_border());
    REQUIRE_FALSE(box.has_fill());

    box.set_has_border(true);
    box.set_border_color(CanvasColor{255, 0, 0, 255});
    REQUIRE(box.has_border());
    REQUIRE(box.border_color().r == 255);

    box.set_has_fill(true);
    box.set_fill_color(CanvasColor{0, 0, 255, 128});
    REQUIRE(box.has_fill());
    REQUIRE(box.fill_color().b == 255);
    REQUIRE(box.fill_color().a == 128);
}

TEST_CASE("TextBox clone preserves properties", "[canvas][textbox]")
{
    TextBox box;
    box.set_text("Cloned");
    box.set_has_border(true);
    box.set_has_fill(true);
    box.resize(400.0, 200.0);

    TextStyle style;
    style.font_size = 20.0;
    style.bold = true;
    box.set_style(style);

    auto cloned = box.clone();
    const auto* cloned_box = dynamic_cast<TextBox*>(cloned.get());
    REQUIRE(cloned_box != nullptr);
    REQUIRE(cloned_box->text() == "Cloned");
    REQUIRE(cloned_box->has_border());
    REQUIRE(cloned_box->has_fill());
    REQUIRE(cloned_box->width() == Catch::Approx(400.0));
    REQUIRE(cloned_box->style().font_size == Catch::Approx(20.0));
    REQUIRE(cloned_box->style().bold);
}

TEST_CASE("TextBox to_json", "[canvas][textbox]")
{
    TextBox box;
    box.set_text("Test");
    const auto json = box.to_json();
    REQUIRE(json.find("TextBox") != std::string::npos);
    REQUIRE(json.find("Test") != std::string::npos);
}
