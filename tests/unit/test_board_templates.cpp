#include "canvas/Board.h"
#include "canvas/BoardTemplate.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::canvas;

namespace
{

auto create_test_templates(const std::filesystem::path& dir) -> void
{
    std::filesystem::create_directories(dir);

    auto write = [&](const std::string& name, const std::string& header)
    {
        std::ofstream file(dir / (name + ".markboard"));
        file << "# " << header << "\n{}\n";
    };

    write("sprint_planning", "Sprint Planning");
    write("retrospective", "Retrospective");
    write("swot_analysis", "SWOT Analysis");
}

} // anonymous namespace

TEST_CASE("BoardTemplate: load builtins", "[board_templates]")
{
    const auto temp_dir = std::filesystem::temp_directory_path() / "markamp_tmpl_test";
    create_test_templates(temp_dir);

    BoardTemplateLibrary library;
    library.load_builtins(temp_dir);

    REQUIRE(library.template_count() == 3);
    std::filesystem::remove_all(temp_dir);
}

TEST_CASE("BoardTemplate: categories", "[board_templates]")
{
    const auto temp_dir = std::filesystem::temp_directory_path() / "markamp_tmpl_cats";
    create_test_templates(temp_dir);

    BoardTemplateLibrary library;
    library.load_builtins(temp_dir);

    auto cats = library.categories();
    REQUIRE(!cats.empty());

    std::filesystem::remove_all(temp_dir);
}

TEST_CASE("BoardTemplate: search", "[board_templates]")
{
    const auto temp_dir = std::filesystem::temp_directory_path() / "markamp_tmpl_search";
    create_test_templates(temp_dir);

    BoardTemplateLibrary library;
    library.load_builtins(temp_dir);

    auto results = library.search("sprint");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0]->id == "sprint_planning");

    std::filesystem::remove_all(temp_dir);
}

TEST_CASE("BoardTemplate: apply template", "[board_templates]")
{
    const auto temp_dir = std::filesystem::temp_directory_path() / "markamp_tmpl_apply";
    create_test_templates(temp_dir);

    BoardTemplateLibrary library;
    library.load_builtins(temp_dir);

    auto board = library.apply_template("retrospective");
    REQUIRE(board != nullptr);

    std::filesystem::remove_all(temp_dir);
}

TEST_CASE("BoardTemplate: save as template", "[board_templates]")
{
    const auto user_dir = std::filesystem::temp_directory_path() / "markamp_tmpl_save";
    std::filesystem::create_directories(user_dir);

    const Board board;

    TemplateInfo info;
    info.id = "custom_board";
    info.name = "My Custom Board";
    info.category = "Custom";
    info.author = "Test User";
    info.description = "A test template";

    BoardTemplateLibrary library;
    const bool saved = library.save_as_template(board, info, user_dir);
    REQUIRE(saved);
    REQUIRE(std::filesystem::exists(user_dir / "custom_board.markboard"));

    // Now load it back.
    library.load_user_templates(user_dir);
    REQUIRE(library.template_count() == 1);

    std::filesystem::remove_all(user_dir);
}

TEST_CASE("BoardTemplate: category filter", "[board_templates]")
{
    const auto temp_dir = std::filesystem::temp_directory_path() / "markamp_tmpl_filter";
    create_test_templates(temp_dir);

    BoardTemplateLibrary library;
    library.load_builtins(temp_dir);

    // All loaded from same directory get "General" category.
    auto general = library.templates_in_category("General");
    REQUIRE(general.size() == 3);

    auto empty = library.templates_in_category("NonExistent");
    REQUIRE(empty.empty());

    std::filesystem::remove_all(temp_dir);
}

TEST_CASE("BoardTemplate: template info", "[board_templates]")
{
    const auto temp_dir = std::filesystem::temp_directory_path() / "markamp_tmpl_info";
    create_test_templates(temp_dir);

    BoardTemplateLibrary library;
    library.load_builtins(temp_dir);

    const auto& templates = library.all_templates();
    bool found_sprint = false;
    for (const auto& tmpl : templates)
    {
        if (tmpl.id == "sprint_planning")
        {
            found_sprint = true;
            REQUIRE(tmpl.is_builtin);
            REQUIRE(tmpl.description == "Sprint Planning");
        }
    }
    REQUIRE(found_sprint);

    std::filesystem::remove_all(temp_dir);
}
