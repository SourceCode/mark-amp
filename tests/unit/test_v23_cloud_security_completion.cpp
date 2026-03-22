/// @file test_v23_cloud_security_completion.cpp
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/CloudSecurityCompletionAuditor.h"
using namespace markamp::core;

TEST_CASE("P12 cloud security labels", "[v23][p12]") {
    REQUIRE(std::string(cloud_security_label(CloudSecurityArea::kEncryption)) == "Encryption");
    REQUIRE(std::string(cloud_security_label(CloudSecurityArea::kSyncTransport)) == "SyncTransport");
    REQUIRE(std::string(cloud_security_label(CloudSecurityArea::kSecurityHarness)) == "SecurityHarness");
}
TEST_CASE("P12 item completeness", "[v23][p12]") {
    CloudSecurityItem a; REQUIRE_FALSE(a.is_complete());
    a.is_real_crypto = true; a.has_error_handling = true; REQUIRE(a.is_complete());
}
TEST_CASE("P12 auditor registration", "[v23][p12]") {
    CloudSecurityCompletionAuditor aud;
    aud.add_item({CloudSecurityArea::kEncryption, "aes-gcm", true, true, "f.cpp", 1});
    REQUIRE(aud.item_count() == 1);
}
TEST_CASE("P12 auditor queries", "[v23][p12]") {
    CloudSecurityCompletionAuditor aud;
    aud.add_item({CloudSecurityArea::kVaultWatching, "vault-watch", true, true, "f.cpp", 1});
    aud.add_item({CloudSecurityArea::kVaultWatching, "vault-notify", false, false, "f.cpp", 2});
    REQUIRE(aud.items_by_area(CloudSecurityArea::kVaultWatching).size() == 2);
    REQUIRE(aud.complete_items().size() == 1);
}
TEST_CASE("P12 auditor report", "[v23][p12]") {
    CloudSecurityCompletionAuditor aud;
    aud.add_item({CloudSecurityArea::kSyncTransport, "s3-upload", true, true, "f.cpp", 1});
    aud.add_item({CloudSecurityArea::kRetryRecovery, "retry-backoff", false, false, "f.cpp", 2});
    auto r = aud.report(); REQUIRE(r.total == 2); REQUIRE(r.has_gaps());
}
TEST_CASE("P12 auditor clear", "[v23][p12]") {
    CloudSecurityCompletionAuditor aud;
    aud.add_item({CloudSecurityArea::kOfflineQueue, "offline-sync", true, true, "f.cpp", 1});
    aud.clear(); REQUIRE(aud.item_count() == 0);
}
TEST_CASE("P12 export json", "[v23][p12]") {
    CloudSecurityCompletionAuditor aud;
    aud.add_item({CloudSecurityArea::kTelemetryExport, "otlp", true, true, "f.cpp", 1});
    REQUIRE(aud.export_json().find("\"total\": 1") != std::string::npos);
}
TEST_CASE("P12 export markdown", "[v23][p12]") {
    CloudSecurityCompletionAuditor aud;
    REQUIRE(aud.export_markdown().find("Cloud Security") != std::string::npos);
}
TEST_CASE("P12 integration", "[v23][p12]") {
    CloudSecurityCompletionAuditor aud;
    aud.add_item({CloudSecurityArea::kEncryption, "aes", true, true, "f.cpp", 1});
    aud.add_item({CloudSecurityArea::kCredentialManagement, "keychain", false, true, "f.cpp", 2});
    aud.add_item({CloudSecurityArea::kSecurityHarness, "crypto-roundtrip", true, true, "f.cpp", 3});
    auto r = aud.report(); REQUIRE(r.complete == 2); REQUIRE(r.incomplete == 1);
}
