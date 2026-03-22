/// @file test_v25_p15_encryption.cpp
/// @brief V25 Phase 15: Real encryption adapter tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P15: Encrypt and decrypt roundtrip", "[v25][p15]")
{
    RealEncryptionAdapter crypto;
    auto enc = crypto.encrypt("secret data", "key123");
    REQUIRE(enc.success);
    REQUIRE(enc.is_real_crypto);

    auto dec = crypto.decrypt(enc.output, "key123");
    REQUIRE(dec.success);
    REQUIRE(dec.output == "secret data");
}

TEST_CASE("V25 P15: Empty key rejected", "[v25][p15]")
{
    RealEncryptionAdapter crypto;
    auto result = crypto.encrypt("data", "");
    REQUIRE_FALSE(result.success);
}

TEST_CASE("V25 P15: Tampered ciphertext rejected", "[v25][p15]")
{
    RealEncryptionAdapter crypto;
    auto result = crypto.decrypt("invalid_data", "key123");
    REQUIRE_FALSE(result.success);
}

TEST_CASE("V25 P15: Uses authenticated encryption", "[v25][p15]")
{
    RealEncryptionAdapter crypto;
    REQUIRE(crypto.is_real_crypto());
    REQUIRE(crypto.uses_authenticated_encryption());
}
