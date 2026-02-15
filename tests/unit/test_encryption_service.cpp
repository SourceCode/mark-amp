/// @file test_encryption_service.cpp
/// @brief V4 Phase 28 – Encryption Service unit tests.

#include "core/Config.h"
#include "core/EncryptionService.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct EncryptionFixture
{
    EventBus event_bus;
    Config config;
    EncryptionService service{event_bus, config};
};

TEST_CASE("EncryptionService: encrypt/decrypt round-trip", "[encryption]")
{
    EncryptionFixture fixture;
    std::string plaintext = "Hello, this is a secret message!";
    std::string password = "strong_password_123";

    auto enc_result = fixture.service.encrypt(plaintext, password);
    REQUIRE(enc_result.has_value());

    auto dec_result = fixture.service.decrypt(*enc_result, password);
    REQUIRE(dec_result.has_value());
    REQUIRE(*dec_result == plaintext);
}

TEST_CASE("EncryptionService: wrong password fails", "[encryption]")
{
    EncryptionFixture fixture;
    std::string plaintext = "Secret data";
    std::string password = "correct_password";

    auto enc_result = fixture.service.encrypt(plaintext, password);
    REQUIRE(enc_result.has_value());

    auto dec_result = fixture.service.decrypt(*enc_result, "wrong_password");
    REQUIRE_FALSE(dec_result.has_value());
    REQUIRE(dec_result.error().find("Authentication failed") != std::string::npos);
}

TEST_CASE("EncryptionService: different salts produce different ciphertext", "[encryption]")
{
    EncryptionFixture fixture;
    std::string plaintext = "Same content";
    std::string password = "same_password";

    auto enc1 = fixture.service.encrypt(plaintext, password);
    auto enc2 = fixture.service.encrypt(plaintext, password);

    REQUIRE(enc1.has_value());
    REQUIRE(enc2.has_value());

    // Different random salts → different ciphertext.
    REQUIRE(enc1->salt != enc2->salt);
    REQUIRE(enc1->ciphertext != enc2->ciphertext);
}

TEST_CASE("EncryptionService: empty content", "[encryption]")
{
    EncryptionFixture fixture;
    std::string plaintext;
    std::string password = "password";

    auto enc_result = fixture.service.encrypt(plaintext, password);
    REQUIRE(enc_result.has_value());

    auto dec_result = fixture.service.decrypt(*enc_result, password);
    REQUIRE(dec_result.has_value());
    REQUIRE(dec_result->empty());
}

TEST_CASE("EncryptionService: large content", "[encryption]")
{
    EncryptionFixture fixture;
    std::string plaintext(10000, 'A'); // 10KB of 'A'
    plaintext += "unique_tail";
    std::string password = "large_content_password";

    auto enc_result = fixture.service.encrypt(plaintext, password);
    REQUIRE(enc_result.has_value());

    auto dec_result = fixture.service.decrypt(*enc_result, password);
    REQUIRE(dec_result.has_value());
    REQUIRE(*dec_result == plaintext);
}

TEST_CASE("EncryptionService: is_encrypted detection", "[encryption]")
{
    EncryptionFixture fixture;

    // Encrypted content has magic header.
    auto enc = fixture.service.encrypt("test", "pw");
    REQUIRE(enc.has_value());
    auto serialized = fixture.service.serialize(*enc);
    REQUIRE(fixture.service.is_encrypted_content(serialized));

    // Plain content does not.
    std::vector<uint8_t> plain = {'H', 'e', 'l', 'l', 'o'};
    REQUIRE_FALSE(fixture.service.is_encrypted_content(plain));
}

TEST_CASE("EncryptionService: serialize/deserialize round-trip", "[encryption]")
{
    EncryptionFixture fixture;
    auto enc = fixture.service.encrypt("serialize test", "pw123");
    REQUIRE(enc.has_value());

    auto serialized = fixture.service.serialize(*enc);
    auto deserialized = fixture.service.deserialize(serialized);
    REQUIRE(deserialized.has_value());

    // Decrypt the deserialized document.
    auto dec = fixture.service.decrypt(*deserialized, "pw123");
    REQUIRE(dec.has_value());
    REQUIRE(*dec == "serialize test");
}

TEST_CASE("EncryptionService: password caching", "[encryption]")
{
    EncryptionFixture fixture;
    REQUIRE_FALSE(fixture.service.get_cached_password("doc1").has_value());

    fixture.service.cache_password("doc1", "secret");
    auto cached = fixture.service.get_cached_password("doc1");
    REQUIRE(cached.has_value());
    REQUIRE(*cached == "secret");

    fixture.service.clear_password_cache();
    REQUIRE_FALSE(fixture.service.get_cached_password("doc1").has_value());
}

TEST_CASE("EncryptionService: change password", "[encryption]")
{
    EncryptionFixture fixture;
    std::string plaintext = "change password test";
    std::string old_pw = "old_password";
    std::string new_pw = "new_password";

    auto enc = fixture.service.encrypt(plaintext, old_pw);
    REQUIRE(enc.has_value());

    auto changed = fixture.service.change_password(*enc, old_pw, new_pw);
    REQUIRE(changed.has_value());

    // Old password should fail.
    auto dec_old = fixture.service.decrypt(*changed, old_pw);
    REQUIRE_FALSE(dec_old.has_value());

    // New password should work.
    auto dec_new = fixture.service.decrypt(*changed, new_pw);
    REQUIRE(dec_new.has_value());
    REQUIRE(*dec_new == plaintext);
}

TEST_CASE("EncryptionService: tampered data fails", "[encryption]")
{
    EncryptionFixture fixture;
    auto enc = fixture.service.encrypt("tamper test", "password");
    REQUIRE(enc.has_value());

    // Tamper with one byte of ciphertext.
    auto tampered = *enc;
    if (!tampered.ciphertext.empty())
    {
        tampered.ciphertext[0] ^= 0xFF;
    }

    auto dec = fixture.service.decrypt(tampered, "password");
    REQUIRE_FALSE(dec.has_value());
    REQUIRE(dec.error().find("Authentication failed") != std::string::npos);
}
