/// @file test_latex_engine.cpp
/// @brief V4 Phase 18 – LatexEngine tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/LatexEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

namespace
{
struct TestFixture
{
    EventBus bus;
    Config config;
    LatexEngine engine{bus, config};
};
} // namespace

// ============================================================================
// Test 1: Inline render
// ============================================================================
TEST_CASE("Inline render produces Unicode output", "[latex][inline]")
{
    TestFixture f;
    auto result = f.engine.render_inline("\\alpha + \\beta");
    CHECK(result.success);
    CHECK(result.output.find("α") != std::string::npos);
    CHECK(result.output.find("β") != std::string::npos);
}

// ============================================================================
// Test 2: Display render
// ============================================================================
TEST_CASE("Display render assigns equation number", "[latex][display]")
{
    TestFixture f;
    auto result = f.engine.render_display("E = mc^2");
    CHECK(result.success);
    CHECK(result.equation_number == 1);
    CHECK(!result.output.empty());
}

// ============================================================================
// Test 3: Fraction
// ============================================================================
TEST_CASE("Fraction renders correctly", "[latex][frac]")
{
    TestFixture f;
    auto result = f.engine.render_inline("\\frac{a}{b}");
    CHECK(result.success);
    CHECK(result.output.find("a/b") != std::string::npos);
}

// ============================================================================
// Test 4: Greek letters
// ============================================================================
TEST_CASE("Greek letters map to Unicode", "[latex][greek]")
{
    TestFixture f;
    auto result = f.engine.render_inline("\\pi \\sigma \\omega");
    CHECK(result.success);
    CHECK(result.output.find("π") != std::string::npos);
    CHECK(result.output.find("σ") != std::string::npos);
    CHECK(result.output.find("ω") != std::string::npos);
}

// ============================================================================
// Test 5: Matrix environment
// ============================================================================
TEST_CASE("Matrix environment renders", "[latex][matrix]")
{
    TestFixture f;
    LatexEnvironment env;
    env.name = "matrix";
    env.content = "1 & 2 \\\\ 3 & 4";
    env.numbered = true;

    auto result = f.engine.render_environment(env);
    CHECK(result.success);
    CHECK(!result.output.empty());
    CHECK(result.equation_number > 0);
}

// ============================================================================
// Test 6: Simple macro expansion
// ============================================================================
TEST_CASE("Simple macro expansion", "[latex][macro]")
{
    TestFixture f;
    LatexMacro macro;
    macro.name = "\\R";
    macro.expansion = "\\mathbb{R}";
    macro.arg_count = 0;
    f.engine.register_macro(macro);

    auto result = f.engine.render_inline("x \\in \\R");
    CHECK(result.success);
    CHECK(result.output.find("ℝ") != std::string::npos);
}

// ============================================================================
// Test 7: Macro with arguments
// ============================================================================
TEST_CASE("Macro with arguments expands correctly", "[latex][macroarg]")
{
    TestFixture f;
    LatexMacro macro;
    macro.name = "\\vect";
    macro.expansion = "\\mathbf{#1}";
    macro.arg_count = 1;
    f.engine.register_macro(macro);

    auto expanded = f.engine.expand_macros("\\vect{x}");
    CHECK(expanded == "\\mathbf{x}");
}

// ============================================================================
// Test 8: Equation numbering increments
// ============================================================================
TEST_CASE("Equation numbering increments", "[latex][numbering]")
{
    TestFixture f;
    auto r1 = f.engine.render_display("x = 1");
    auto r2 = f.engine.render_display("y = 2");
    CHECK(r1.equation_number == 1);
    CHECK(r2.equation_number == 2);

    f.engine.reset_numbering();
    auto r3 = f.engine.render_display("z = 3");
    CHECK(r3.equation_number == 1);
}

// ============================================================================
// Test 9: Error handling (empty input)
// ============================================================================
TEST_CASE("Empty input renders successfully", "[latex][error]")
{
    TestFixture f;
    auto result = f.engine.render_inline("");
    CHECK(result.success);
    CHECK(result.output.empty());
}

// ============================================================================
// Test 10: Document processing
// ============================================================================
TEST_CASE("Process document replaces math expressions", "[latex][document]")
{
    TestFixture f;
    std::string doc = "The value $\\alpha$ is used in $$E = mc^2$$ often.";
    auto result = f.engine.process_document(doc);
    // Inline math should be replaced
    CHECK(result.find("$") == std::string::npos);
    CHECK(result.find("α") != std::string::npos);
}

// ============================================================================
// Test 11: Preamble loading
// ============================================================================
TEST_CASE("Load preamble registers macros", "[latex][preamble]")
{
    TestFixture f;
    std::string preamble = R"(
        \newcommand{\R}{\mathbb{R}}
        \newcommand{\norm}[1]{\left\|#1\right\|}
    )";
    int count = f.engine.load_preamble(preamble);
    CHECK(count == 2);
    CHECK(f.engine.macros().size() == 2);
    CHECK(f.engine.macros().count("\\R") == 1);
    CHECK(f.engine.macros().count("\\norm") == 1);
}

// ============================================================================
// Test 12: Cases environment
// ============================================================================
TEST_CASE("Cases environment renders", "[latex][cases]")
{
    TestFixture f;
    LatexEnvironment env;
    env.name = "cases";
    env.content = "x & x > 0 \\\\ -x & x \\leq 0";
    env.numbered = false;

    auto result = f.engine.render_environment(env);
    CHECK(result.success);
    CHECK(result.output.find("{") != std::string::npos);
    CHECK(result.equation_number == 0); // Unnumbered
}
