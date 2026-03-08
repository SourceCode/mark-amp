/// test_extension_storage.cpp — Unit tests for ExtensionStorage types
#include "core/ExtensionStorage.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("ExtensionSource: enum values", "[extension_storage]")
{
    REQUIRE(ExtensionSource::kGallery != ExtensionSource::kVsix);
    REQUIRE(ExtensionSource::kVsix != ExtensionSource::kBuiltin);
}

TEST_CASE("ExtensionMetadata: default values", "[extension_storage]")
{
    ExtensionMetadata meta;
    REQUIRE(meta.extension_id.empty());
    REQUIRE(meta.version.empty());
    REQUIRE(meta.source == ExtensionSource::kBuiltin);
    REQUIRE(meta.enabled);
    REQUIRE(meta.location.empty());
}

TEST_CASE("ExtensionMetadata: field assignment", "[extension_storage]")
{
    ExtensionMetadata meta;
    meta.extension_id = "publisher.theme-dark";
    meta.version = "1.2.3";
    meta.source = ExtensionSource::kGallery;
    meta.enabled = false;
    meta.location = "/home/user/.markamp/extensions/theme-dark";
    REQUIRE(meta.extension_id == "publisher.theme-dark");
    REQUIRE_FALSE(meta.enabled);
}

TEST_CASE("ExtensionStorageService: construction with path", "[extension_storage]")
{
    ExtensionStorageService svc(std::filesystem::path("/tmp/test_ext_storage.json"));
    (void)svc;
}
