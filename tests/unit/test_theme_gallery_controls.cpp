// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/ThemeGalleryModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_gallery() -> ThemeGalleryModel
{
    ThemeGalleryModel model;
    model.set_themes({
        {"dark-pro", "Dark Pro", "MarkAmp", ThemeStyle::kDark, true},
        {"light-clean", "Light Clean", "Community", ThemeStyle::kLight, false},
        {"high-contrast", "High Contrast", "MarkAmp", ThemeStyle::kHighContrast, true},
        {"monokai", "Monokai", "Community", ThemeStyle::kDark, false},
    });
    return model;
}

TEST_CASE("Filter by style", "[theme][filter]")
{
    auto model = make_test_gallery();
    REQUIRE(model.by_style(ThemeStyle::kDark).size() == 2);
    REQUIRE(model.by_style(ThemeStyle::kHighContrast).size() == 1);
}

TEST_CASE("Preview without commit", "[theme][preview]")
{
    auto model = make_test_gallery();
    REQUIRE_FALSE(model.is_previewing());
    model.set_preview("monokai");
    REQUIRE(model.is_previewing());
    REQUIRE(model.preview_id() == "monokai");
    model.clear_preview();
    REQUIRE_FALSE(model.is_previewing());
}

TEST_CASE("Apply and rollback", "[theme][apply]")
{
    auto model = make_test_gallery();
    model.apply("dark-pro");
    REQUIRE(model.active_id() == "dark-pro");
    REQUIRE_FALSE(model.can_rollback()); // no previous

    model.apply("monokai");
    REQUIRE(model.active_id() == "monokai");
    REQUIRE(model.can_rollback());
    REQUIRE(model.previous_id() == "dark-pro");

    model.rollback();
    REQUIRE(model.active_id() == "dark-pro");
    REQUIRE_FALSE(model.can_rollback());
}

TEST_CASE("Apply clears preview", "[theme][apply]")
{
    auto model = make_test_gallery();
    model.set_preview("monokai");
    model.apply("monokai");
    REQUIRE_FALSE(model.is_previewing());
}

TEST_CASE("Token editing with groups", "[theme][token]")
{
    ThemeGalleryModel model;
    model.set_tokens({
        {"bg", "Background", "editor", "#1E1E1E", 21.0, false},
        {"fg", "Foreground", "editor", "#D4D4D4", 12.5, false},
        {"sidebar-bg", "Sidebar BG", "sidebar", "#252526", 18.0, false},
        {"low-contrast", "Low Contrast", "editor", "#333333", 1.5, true},
    });
    REQUIRE(model.tokens_by_group("editor").size() == 3);
    REQUIRE(model.tokens_by_group("sidebar").size() == 1);
}

TEST_CASE("Contrast warnings", "[theme][token]")
{
    ThemeGalleryModel model;
    model.set_tokens({
        {"ok", "OK Token", "editor", "#FFFFFF", 21.0, false},
        {"bad", "Bad Token", "editor", "#333333", 1.5, true},
    });
    REQUIRE(model.contrast_warnings().size() == 1);
    REQUIRE(model.contrast_warnings()[0].token_id == "bad");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
