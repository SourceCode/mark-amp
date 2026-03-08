// test_ai_writing_assistant.cpp — 10 tests for AI writing assistant types and enums
#include "core/AITypes.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AIWritingTone kFormal is distinct", "[ai][writing]")
{
    CHECK(AIWritingTone::kFormal != AIWritingTone::kCasual);
}

TEST_CASE("AIWritingTone all values distinct", "[ai][writing]")
{
    CHECK(static_cast<uint8_t>(AIWritingTone::kFormal) !=
          static_cast<uint8_t>(AIWritingTone::kCasual));
    CHECK(static_cast<uint8_t>(AIWritingTone::kTechnical) !=
          static_cast<uint8_t>(AIWritingTone::kFriendly));
    CHECK(static_cast<uint8_t>(AIWritingTone::kAcademic) !=
          static_cast<uint8_t>(AIWritingTone::kConcise));
}

TEST_CASE("AIAction Chat differs from Summarize", "[ai][writing]")
{
    CHECK(AIAction::Chat != AIAction::Summarize);
}

TEST_CASE("AIAction Translate is enumerated", "[ai][writing]")
{
    CHECK(AIAction::Translate != AIAction::Chat);
    CHECK(AIAction::Translate != AIAction::Custom);
}

TEST_CASE("AIAction FixGrammar is enumerated", "[ai][writing]")
{
    CHECK(AIAction::FixGrammar != AIAction::SimplifyContent);
}

TEST_CASE("AIAction ChangeTone is available", "[ai][writing]")
{
    CHECK(AIAction::ChangeTone != AIAction::Chat);
    CHECK(AIAction::ChangeTone != AIAction::FixGrammar);
}

TEST_CASE("AIAction ContinueWriting is enumerated", "[ai][writing]")
{
    CHECK(AIAction::ContinueWriting != AIAction::Chat);
}

TEST_CASE("AIProvider enum values cover all providers", "[ai][writing]")
{
    CHECK(AIProvider::OpenAI != AIProvider::Anthropic);
    CHECK(AIProvider::Local != AIProvider::Custom);
    CHECK(AIProvider::OpenAI != AIProvider::Local);
}

TEST_CASE("AIRole enum has System/User/Assistant", "[ai][writing]")
{
    CHECK(AIRole::System != AIRole::User);
    CHECK(AIRole::User != AIRole::Assistant);
    CHECK(AIRole::System != AIRole::Assistant);
}

TEST_CASE("AIModelConfig custom endpoint", "[ai][writing]")
{
    AIModelConfig config;
    config.provider = AIProvider::Custom;
    config.model_name = "custom-model";
    config.api_base_url = "http://localhost:11434";
    CHECK(config.provider == AIProvider::Custom);
    CHECK(config.api_base_url == "http://localhost:11434");
}
