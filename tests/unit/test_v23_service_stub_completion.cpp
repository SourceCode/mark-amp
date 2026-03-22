/// @file test_v23_service_stub_completion.cpp
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/ServiceStubCompletionAuditor.h"
using namespace markamp::core;
TEST_CASE("P17 service stub labels", "[v23][p17]") {
    REQUIRE(std::string(service_stub_label(ServiceStubArea::kDefaultReturns)) == "DefaultReturns");
    REQUIRE(std::string(service_stub_label(ServiceStubArea::kInterfaceAdoption)) == "InterfaceAdoption");
}
TEST_CASE("P17 item completeness", "[v23][p17]") {
    ServiceStubItem a; REQUIRE_FALSE(a.is_complete());
    a.is_real_service = true; a.has_error_handling = true; REQUIRE(a.is_complete());
}
TEST_CASE("P17 registration", "[v23][p17]") {
    ServiceStubCompletionAuditor aud;
    aud.add_item({ServiceStubArea::kUnsupportedState, "search-unavailable", true, true, "f.cpp", 1});
    REQUIRE(aud.item_count() == 1);
}
TEST_CASE("P17 queries", "[v23][p17]") {
    ServiceStubCompletionAuditor aud;
    aud.add_item({ServiceStubArea::kHealthReporting, "svc-health", true, true, "f.cpp", 1});
    aud.add_item({ServiceStubArea::kHealthReporting, "svc-readiness", false, false, "f.cpp", 2});
    REQUIRE(aud.complete_items().size() == 1); REQUIRE(aud.incomplete_items().size() == 1);
}
TEST_CASE("P17 report", "[v23][p17]") {
    ServiceStubCompletionAuditor aud;
    aud.add_item({ServiceStubArea::kCapabilityContracts, "search-cap", true, true, "f.cpp", 1});
    auto r = aud.report(); REQUIRE(r.total == 1); REQUIRE_FALSE(r.has_gaps());
}
TEST_CASE("P17 clear", "[v23][p17]") {
    ServiceStubCompletionAuditor aud;
    aud.add_item({ServiceStubArea::kMigrationParity, "toolbar-model", true, true, "f.cpp", 1});
    aud.clear(); REQUIRE(aud.item_count() == 0);
}
TEST_CASE("P17 export json", "[v23][p17]") {
    ServiceStubCompletionAuditor aud;
    aud.add_item({ServiceStubArea::kServiceReadiness, "readiness", true, true, "f.cpp", 1});
    REQUIRE(aud.export_json().find("\"total\": 1") != std::string::npos);
}
TEST_CASE("P17 export markdown", "[v23][p17]") {
    ServiceStubCompletionAuditor aud;
    REQUIRE(aud.export_markdown().find("Service Stub") != std::string::npos);
}
TEST_CASE("P17 integration", "[v23][p17]") {
    ServiceStubCompletionAuditor aud;
    aud.add_item({ServiceStubArea::kDefaultReturns, "marketplace-list", true, true, "f.cpp", 1});
    aud.add_item({ServiceStubArea::kRegressionCoverage, "service-regr", false, true, "f.cpp", 2});
    aud.add_item({ServiceStubArea::kInterfaceAdoption, "settings-owner", true, true, "f.cpp", 3});
    auto r = aud.report(); REQUIRE(r.complete == 2); REQUIRE(r.incomplete == 1);
}
