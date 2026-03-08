// test_ai_types.cpp — 10 tests for AI type structs and enums
#include "core/AITypes.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AIMessage default role is User", "[ai][types]")
{
    AIMessage msg;
    CHECK(msg.role == AIRole::User);
    CHECK(msg.content.empty());
    CHECK(msg.timestamp == 0);
}

TEST_CASE("AIModelConfig defaults", "[ai][types]")
{
    AIModelConfig config;
    CHECK(config.provider == AIProvider::OpenAI);
    CHECK(config.model_name == "gpt-4");
    CHECK(config.temperature > 0.0);
    CHECK(config.max_tokens == 2048);
    CHECK(config.top_p == 1.0);
}

TEST_CASE("AIRequest defaults", "[ai][types]")
{
    AIRequest request;
    CHECK(request.action == AIAction::Chat);
    CHECK(request.messages.empty());
    CHECK(request.context.empty());
    CHECK(request.selected_text.empty());
}

TEST_CASE("AIResponse defaults", "[ai][types]")
{
    AIResponse response;
    CHECK_FALSE(response.success);
    CHECK(response.content.empty());
    CHECK(response.prompt_tokens == 0);
    CHECK(response.completion_tokens == 0);
    CHECK(response.total_tokens == 0);
}

TEST_CASE("AISession add_message appends messages", "[ai][types]")
{
    AISession session;
    session.session_id = "test-session";
    session.add_message(AIRole::User, "Hello");
    session.add_message(AIRole::Assistant, "Hi there");
    REQUIRE(session.messages.size() == 2);
    CHECK(session.messages[0].role == AIRole::User);
    CHECK(session.messages[0].content == "Hello");
    CHECK(session.messages[1].role == AIRole::Assistant);
    CHECK(session.messages[1].content == "Hi there");
}

TEST_CASE("AIUsageStats defaults to zero", "[ai][types]")
{
    AIUsageStats stats;
    CHECK(stats.prompt_tokens == 0);
    CHECK(stats.completion_tokens == 0);
    CHECK(stats.total_tokens == 0);
    CHECK(stats.request_count == 0);
    CHECK(stats.estimated_cost == 0.0);
}

TEST_CASE("AICustomPrompt fields", "[ai][types]")
{
    AICustomPrompt prompt;
    prompt.name = "Summarize";
    prompt.description = "Summarize selected text";
    prompt.system_prompt = "You are a summarizer.";
    prompt.variables = {"{{selection}}", "{{file}}"};
    CHECK(prompt.name == "Summarize");
    CHECK(prompt.variables.size() == 2);
}

TEST_CASE("AIConversationEntry defaults", "[ai][types]")
{
    AIConversationEntry entry;
    CHECK(entry.session_id.empty());
    CHECK(entry.title.empty());
    CHECK(entry.created_at == 0);
    CHECK(entry.last_active == 0);
    CHECK(entry.message_count == 0);
}

TEST_CASE("InlineSuggestion defaults", "[ai][types]")
{
    InlineSuggestion suggestion;
    CHECK(suggestion.text.empty());
    CHECK(suggestion.confidence == 0.0);
    CHECK(suggestion.request_id.empty());
}

TEST_CASE("GenerationStatus defaults to idle", "[ai][types]")
{
    GenerationStatus status;
    CHECK(status.state == GenerationState::kIdle);
    CHECK(status.progress == 0.0);
    CHECK(status.content_so_far.empty());
    CHECK(status.error_message.empty());
}
