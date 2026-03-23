/// @file test_v27_p20_qa_gates.cpp
/// @brief V27 Phase 20: QA matrix, legacy removal, signoff criteria.
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
