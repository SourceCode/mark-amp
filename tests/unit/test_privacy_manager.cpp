// test_privacy_manager.cpp — 10 tests for PrivacyManager
#include "core/PrivacyManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("PrivacyManager starts empty", "[privacy][manager]")
{
    PrivacyManager manager;
    CHECK(manager.classification_count() == 0);
}

TEST_CASE("PrivacyManager classify_document", "[privacy][manager]")
{
    PrivacyManager manager;
    CHECK(manager.classify_document("doc-1", PrivacyLevel::kConfidential));
    CHECK(manager.classification_count() == 1);
}

TEST_CASE("PrivacyManager get_classification", "[privacy][manager]")
{
    PrivacyManager manager;
    manager.classify_document("doc-1", PrivacyLevel::kSecret);
    auto cls = manager.get_classification("doc-1");
    REQUIRE(cls != nullptr);
    CHECK(cls->level == PrivacyLevel::kSecret);
}

TEST_CASE("PrivacyManager get_classification missing", "[privacy][manager]")
{
    PrivacyManager manager;
    CHECK(manager.get_classification("ghost") == nullptr);
}

TEST_CASE("PrivacyManager remove_classification", "[privacy][manager]")
{
    PrivacyManager manager;
    manager.classify_document("doc-1", PrivacyLevel::kPublic);
    CHECK(manager.remove_classification("doc-1"));
    CHECK(manager.classification_count() == 0);
}

TEST_CASE("PrivacyManager set and get policy", "[privacy][manager]")
{
    PrivacyManager manager;
    PrivacyPolicy policy;
    policy.default_level = PrivacyLevel::kConfidential;
    policy.redact_on_export = true;
    policy.retention_days = 180;
    manager.set_policy(policy);
    auto retrieved = manager.get_policy();
    CHECK(retrieved.default_level == PrivacyLevel::kConfidential);
    CHECK(retrieved.redact_on_export);
    CHECK(retrieved.retention_days == 180);
}

TEST_CASE("PrivacyManager documents_at_level", "[privacy][manager]")
{
    PrivacyManager manager;
    manager.classify_document("pub-1", PrivacyLevel::kPublic);
    manager.classify_document("conf-1", PrivacyLevel::kConfidential);
    manager.classify_document("pub-2", PrivacyLevel::kPublic);
    auto public_docs = manager.documents_at_level(PrivacyLevel::kPublic);
    CHECK(public_docs.size() == 2);
}

TEST_CASE("PrivacyManager clear_all", "[privacy][manager]")
{
    PrivacyManager manager;
    manager.classify_document("d1", PrivacyLevel::kPublic);
    manager.clear_all();
    CHECK(manager.classification_count() == 0);
}

TEST_CASE("PrivacyPolicy defaults", "[privacy][policy]")
{
    PrivacyPolicy policy;
    CHECK(policy.default_level == PrivacyLevel::kInternal);
    CHECK_FALSE(policy.redact_on_export);
    CHECK_FALSE(policy.strip_metadata);
    CHECK(policy.retention_days == 365);
}

TEST_CASE("privacy_level_name returns display names", "[privacy][level]")
{
    auto public_name = privacy_level_name(PrivacyLevel::kPublic);
    auto secret_name = privacy_level_name(PrivacyLevel::kSecret);
    CHECK_FALSE(public_name.empty());
    CHECK_FALSE(secret_name.empty());
    CHECK(public_name != secret_name);
}
