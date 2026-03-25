/// @file test_v27_p20_qa_gates.cpp
/// @brief V27 Phase 20: QA matrix, legacy removal, signoff criteria, mark_pass.
#include <catch2/catch_test_macros.hpp>
#include "core/V27QAMatrix.h"
using namespace markamp::core;
TEST_CASE("V27 P20: QA status count", "[v27][p20]") { REQUIRE(v27_qa_status_count() == 4); }
TEST_CASE("V27 P20: QA surface entry all_pass", "[v27][p20]") {
    V27QASurfaceEntry entry{"Shell"};
    REQUIRE_FALSE(entry.all_pass());
    entry.icon_migration = V27QAStatus::kPass;
    entry.visual_redesign = V27QAStatus::kPass;
    entry.theme_parity = V27QAStatus::kPass;
    entry.accessibility = V27QAStatus::kPass;
    entry.legacy_removal = V27QAStatus::kPass;
    REQUIRE(entry.all_pass());
}
TEST_CASE("V27 P20: QA matrix populate", "[v27][p20]") {
    V27QAMatrix matrix;
    matrix.populate_v27_surfaces();
    REQUIRE(matrix.surface_count() == 18);
    REQUIRE_FALSE(matrix.all_pass());
    REQUIRE(matrix.pass_count() == 0);
}
TEST_CASE("V27 P20: QA matrix surface lookup", "[v27][p20]") {
    V27QAMatrix matrix;
    matrix.populate_v27_surfaces();
    auto* shell = matrix.surface("Shell");
    REQUIRE(shell != nullptr);
    REQUIRE(shell->surface_name == "Shell");
    REQUIRE(matrix.surface("NonExistent") == nullptr);
}
TEST_CASE("V27 P20: QA matrix incomplete surfaces", "[v27][p20]") {
    V27QAMatrix matrix;
    matrix.populate_v27_surfaces();
    auto incomplete = matrix.incomplete_surfaces();
    REQUIRE(incomplete.size() == 18);
}
TEST_CASE("V27 P20: QA matrix single surface pass-through", "[v27][p20]") {
    V27QAMatrix matrix;
    matrix.register_surface(V27QASurfaceEntry{
        "TestSurface",
        V27QAStatus::kPass,
        V27QAStatus::kPass,
        V27QAStatus::kPass,
        V27QAStatus::kPass,
        V27QAStatus::kPass});
    REQUIRE(matrix.surface_count() == 1);
    REQUIRE(matrix.all_pass());
    REQUIRE(matrix.pass_count() == 1);
    REQUIRE(matrix.incomplete_surfaces().empty());
}
TEST_CASE("V27 P20: QA matrix mixed states", "[v27][p20]") {
    V27QAMatrix matrix;
    matrix.register_surface(V27QASurfaceEntry{
        "Complete",
        V27QAStatus::kPass, V27QAStatus::kPass,
        V27QAStatus::kPass, V27QAStatus::kPass,
        V27QAStatus::kPass});
    matrix.register_surface(V27QASurfaceEntry{
        "Partial",
        V27QAStatus::kPass, V27QAStatus::kInProgress,
        V27QAStatus::kNotStarted, V27QAStatus::kPass,
        V27QAStatus::kNotStarted});
    REQUIRE(matrix.surface_count() == 2);
    REQUIRE(matrix.pass_count() == 1);
    REQUIRE_FALSE(matrix.all_pass());
    REQUIRE(matrix.incomplete_surfaces().size() == 1);
    REQUIRE(matrix.incomplete_surfaces()[0] == "Partial");
}
