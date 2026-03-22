/// @file test_v24_p15_cloud_security.cpp
/// @brief V24 Phase 15 tests: Cloud sync types, encryption config, transport.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/CloudSyncTypes.h"

using namespace markamp::core;

// P15-T01: Cloud sync status
TEST_CASE("P15-T01 cloud sync status values", "[v24][p15]") {
    REQUIRE(CloudSyncStatus::Idle != CloudSyncStatus::Connecting);
    REQUIRE(CloudSyncStatus::Uploading != CloudSyncStatus::Downloading);
    REQUIRE(CloudSyncStatus::Completed != CloudSyncStatus::Failed);
}

TEST_CASE("P15-T01 cloud provider types", "[v24][p15]") {
    REQUIRE(CloudProvider::S3 != CloudProvider::WebDAV);
}

// P15-T02: Encryption config
TEST_CASE("P15-T02 encryption config defaults", "[v24][p15]") {
    EncryptionConfig enc;
    REQUIRE_FALSE(enc.enabled);
    REQUIRE(enc.argon2_memory_kb == 65536);
    REQUIRE(enc.argon2_iterations == 3);
    REQUIRE(enc.nonce_size == 12);
    REQUIRE(enc.tag_size == 16);
}

TEST_CASE("P15-T02 encryption config with passphrase", "[v24][p15]") {
    EncryptionConfig enc;
    enc.enabled = true;
    enc.passphrase = "my-secure-passphrase";
    REQUIRE(enc.enabled);
    REQUIRE(enc.passphrase == "my-secure-passphrase");
}

// P15-T03: Cloud sync config
TEST_CASE("P15-T03 S3 config structure", "[v24][p15]") {
    S3Config s3;
    s3.endpoint = "https://s3.amazonaws.com";
    s3.bucket = "markamp-sync";
    s3.region = "us-west-2";
    REQUIRE(s3.use_ssl);
    REQUIRE_FALSE(s3.path_style);
}

TEST_CASE("P15-T03 WebDAV config structure", "[v24][p15]") {
    WebDavConfig webdav;
    webdav.url = "https://cloud.example.com/remote.php/dav";
    webdav.username = "user";
    webdav.password = "pass";
    REQUIRE(webdav.use_ssl);
    REQUIRE(webdav.timeout_seconds == 30);
}

// P15-T04: Sync result
TEST_CASE("P15-T04 cloud sync result success", "[v24][p15]") {
    CloudSyncResult result;
    result.status = CloudSyncStatus::Completed;
    result.files_uploaded = 10;
    result.files_downloaded = 5;
    REQUIRE(result.succeeded());
    REQUIRE(result.files_uploaded == 10);
}

TEST_CASE("P15-T04 cloud sync result failure", "[v24][p15]") {
    CloudSyncResult result;
    result.status = CloudSyncStatus::Failed;
    result.errors.push_back("Connection timeout");
    REQUIRE_FALSE(result.succeeded());
}

// P15-T05: Sync ignore patterns and device info
TEST_CASE("P15-T05 sync ignore default patterns", "[v24][p15]") {
    auto patterns = SyncIgnorePatterns::default_patterns();
    REQUIRE(patterns.size() > 5);
}

TEST_CASE("P15-T05 device info structure", "[v24][p15]") {
    DeviceInfo device;
    device.device_id = "dev-1";
    device.device_name = "MacBook Pro";
    device.platform = "macOS";
    device.is_current = true;
    REQUIRE(device.is_current);
    REQUIRE(device.platform == "macOS");
}
