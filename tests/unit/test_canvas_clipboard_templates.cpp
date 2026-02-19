// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/ClipboardModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Clipboard copy and clear", "[clipboard][basic]")
{
    ClipboardModel model;
    REQUIRE_FALSE(model.has_clipboard());
    model.copy({{"obj-1", "shape", "{}"}, {"obj-2", "text", "{}"}});
    REQUIRE(model.has_clipboard());
    REQUIRE(model.clipboard().size() == 2);
    model.clear_clipboard();
    REQUIRE_FALSE(model.has_clipboard());
}

TEST_CASE("Paste mode variants", "[clipboard][paste]")
{
    ClipboardModel model;
    REQUIRE(model.paste_mode() == PasteMode::kFull);
    model.set_paste_mode(PasteMode::kStyleOnly);
    REQUIRE(model.paste_mode() == PasteMode::kStyleOnly);
    model.set_paste_mode(PasteMode::kContentOnly);
    REQUIRE(model.paste_mode() == PasteMode::kContentOnly);
}

TEST_CASE("Duplicate mode and offset", "[clipboard][duplicate]")
{
    ClipboardModel model;
    REQUIRE(model.duplicate_mode() == DuplicateMode::kOffset);
    model.set_duplicate_mode(DuplicateMode::kInPlace);
    REQUIRE(model.duplicate_mode() == DuplicateMode::kInPlace);
    model.set_duplicate_offset(30.0, 15.0);
    REQUIRE(model.offset_x() == 30.0);
    REQUIRE(model.offset_y() == 15.0);
}

TEST_CASE("Board template save and remove", "[clipboard][template]")
{
    ClipboardModel model;
    model.save_template({"t1", "My Template", 5, "{}"});
    REQUIRE(model.templates().size() == 1);
    model.remove_template("t1");
    REQUIRE(model.templates().empty());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
