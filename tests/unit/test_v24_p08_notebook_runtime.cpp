/// @file test_v24_p08_notebook_runtime.cpp
/// @brief V24 Phase 08 tests: Notebook runtime, kernel, cells, variable inspector.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/NotebookVariableInspector.h"
#include "../../src/core/NotebookDocumentLifecycle.h"
#include "../../src/core/KernelManager.h"
#include "../../src/core/EventBus.h"
#include "../../src/core/Config.h"

using namespace markamp::core;

// P08-T01: Notebook as first-class artifact
TEST_CASE("P08-T01 kernel state enum values", "[v24][p08]") {
    REQUIRE(KernelState::kStarting != KernelState::kIdle);
    REQUIRE(KernelState::kIdle != KernelState::kBusy);
    REQUIRE(KernelState::kBusy != KernelState::kRestarting);
    REQUIRE(KernelState::kRestarting != KernelState::kDead);
}

TEST_CASE("P08-T01 message type enum values", "[v24][p08]") {
    REQUIRE(MessageType::kExecuteRequest != MessageType::kExecuteReply);
    REQUIRE(MessageType::kStream != MessageType::kError);
}

// P08-T02: Kernel state
TEST_CASE("P08-T02 kernel spec structure", "[v24][p08]") {
    KernelSpec spec;
    spec.name = "python3";
    spec.display_name = "Python 3";
    spec.language = "python";
    REQUIRE(spec.name == "python3");
    REQUIRE(spec.display_name == "Python 3");
}

TEST_CASE("P08-T02 kernel state enum", "[v24][p08]") {
    REQUIRE(KernelState::kStarting != KernelState::kIdle);
    REQUIRE(KernelState::kBusy != KernelState::kDead);
}

// P08-T03: Execution structures
TEST_CASE("P08-T03 execution request structure", "[v24][p08]") {
    ExecutionRequest req;
    req.code = "print('hello')";
    req.cell_id = "cell-1";
    req.silent = false;
    REQUIRE(req.code == "print('hello')");
    REQUIRE(req.store_history);
}

TEST_CASE("P08-T03 execution result structure", "[v24][p08]") {
    ExecutionResult result;
    result.cell_id = "cell-1";
    result.execution_count = 1;
    result.success = true;
    REQUIRE(result.success);
    REQUIRE(result.execution_count == 1);
}

// P08-T04: Variable inspector
TEST_CASE("P08-T04 variable inspector CRUD", "[v24][p08]") {
    NotebookVariableInspector inspector;
    InspectedVariable v;
    v.name = "x";
    v.type = VariableType::kInteger;
    v.value_repr = "42";
    v.cell_index = 0;
    inspector.set_variable(v);

    REQUIRE(inspector.variable_count() == 1);
    auto* found = inspector.get_variable("x");
    REQUIRE(found != nullptr);
    REQUIRE(found->value_repr == "42");
    REQUIRE(found->type == VariableType::kInteger);
}

TEST_CASE("P08-T04 variable inspector cell filtering", "[v24][p08]") {
    NotebookVariableInspector inspector;
    InspectedVariable v1;
    v1.name = "a"; v1.cell_index = 0; v1.type = VariableType::kInteger;
    InspectedVariable v2;
    v2.name = "b"; v2.cell_index = 1; v2.type = VariableType::kString;
    InspectedVariable v3;
    v3.name = "c"; v3.cell_index = 0; v3.type = VariableType::kFloat;
    inspector.set_variable(v1);
    inspector.set_variable(v2);
    inspector.set_variable(v3);

    auto cell0 = inspector.variables_for_cell(0);
    REQUIRE(cell0.size() == 2);
    auto ints = inspector.variables_by_type(VariableType::kInteger);
    REQUIRE(ints.size() == 1);
}

// P08-T05: Staleness and cleanup
TEST_CASE("P08-T05 variable staleness marking", "[v24][p08]") {
    NotebookVariableInspector inspector;
    InspectedVariable v;
    v.name = "x"; v.cell_index = 0;
    inspector.set_variable(v);

    inspector.mark_cell_stale(0);
    auto* found = inspector.get_variable("x");
    REQUIRE(found->is_stale);

    inspector.clear();
    REQUIRE(inspector.variable_count() == 0);
}
