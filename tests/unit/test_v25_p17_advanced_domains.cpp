/// @file test_v25_p17_advanced_domains.cpp
/// @brief V25 Phase 17: Advanced domain gate service tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P17: Domain classification", "[v25][p17]")
{
    AdvancedDomainGateService gate;
    AdvancedDomainEntry e;
    e.domain_id = "node-editor";
    e.triage = DomainTriage::kGated;
    e.has_placeholder_runtime = true;
    gate.classify_domain(e);
    REQUIRE(gate.domain_count() == 1);
    REQUIRE(gate.gated_domains().size() == 1);
}

TEST_CASE("V25 P17: Placeholder runtime count", "[v25][p17]")
{
    AdvancedDomainGateService gate;
    AdvancedDomainEntry e;
    e.domain_id = "av";
    e.triage = DomainTriage::kMustFinish;
    e.has_placeholder_runtime = true;
    gate.classify_domain(e);
    REQUIRE(gate.placeholder_runtime_count() == 1);
    REQUIRE(gate.must_finish_domains().size() == 1);
}

TEST_CASE("V25 P17: Domain lookup", "[v25][p17]")
{
    AdvancedDomainGateService gate;
    AdvancedDomainEntry e;
    e.domain_id = "graph";
    e.label = "Knowledge Graph";
    gate.classify_domain(e);
    auto* found = gate.get_domain("graph");
    REQUIRE(found != nullptr);
    REQUIRE(found->label == "Knowledge Graph");
}
