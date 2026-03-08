// test_ai_conversation_history.cpp — 10 tests for AIConversationHistory
#include "core/AIConversationHistory.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AIConversationHistory starts empty", "[ai][conversation]")
{
    AIConversationHistory history;
    CHECK(history.conversation_count() == 0);
}

TEST_CASE("AIConversationHistory save and load", "[ai][conversation]")
{
    AIConversationHistory history;
    AISession session;
    session.session_id = "sess-001";
    session.created_at = 1000;
    session.last_active = 2000;
    session.add_message(AIRole::User, "Hello");
    history.save_conversation(session);
    CHECK(history.conversation_count() == 1);
    auto loaded = history.load_conversation("sess-001");
    REQUIRE(loaded.has_value());
    CHECK(loaded->session_id == "sess-001");
    CHECK(loaded->messages.size() == 1);
}

TEST_CASE("AIConversationHistory load missing returns error", "[ai][conversation]")
{
    AIConversationHistory history;
    auto result = history.load_conversation("nonexistent");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("AIConversationHistory delete conversation", "[ai][conversation]")
{
    AIConversationHistory history;
    AISession session;
    session.session_id = "sess-del";
    history.save_conversation(session);
    CHECK(history.conversation_count() == 1);
    CHECK(history.delete_conversation("sess-del"));
    CHECK(history.conversation_count() == 0);
}

TEST_CASE("AIConversationHistory delete nonexistent returns false", "[ai][conversation]")
{
    AIConversationHistory history;
    CHECK_FALSE(history.delete_conversation("ghost"));
}

TEST_CASE("AIConversationHistory list conversations", "[ai][conversation]")
{
    AIConversationHistory history;
    AISession s1;
    s1.session_id = "a";
    s1.created_at = 100;
    AISession s2;
    s2.session_id = "b";
    s2.created_at = 200;
    history.save_conversation(s1);
    history.save_conversation(s2);
    auto list = history.list_conversations();
    CHECK(list.size() == 2);
}

TEST_CASE("AIConversationHistory search conversations", "[ai][conversation]")
{
    AIConversationHistory history;
    AISession session;
    session.session_id = "search-test";
    session.add_message(AIRole::User, "quantum computing basics");
    history.save_conversation(session);
    auto results = history.search_conversations("quantum");
    CHECK_FALSE(results.empty());
}

TEST_CASE("AIConversationHistory retention days default", "[ai][conversation]")
{
    AIConversationHistory history;
    CHECK(history.retention_days() == 90);
}

TEST_CASE("AIConversationHistory set retention days", "[ai][conversation]")
{
    AIConversationHistory history;
    history.set_retention_days(30);
    CHECK(history.retention_days() == 30);
    history.set_retention_days(0);
    CHECK(history.retention_days() == 0);
}

TEST_CASE("AIConversationHistory clear all", "[ai][conversation]")
{
    AIConversationHistory history;
    AISession s1;
    s1.session_id = "x1";
    AISession s2;
    s2.session_id = "x2";
    history.save_conversation(s1);
    history.save_conversation(s2);
    CHECK(history.conversation_count() == 2);
    history.clear_all();
    CHECK(history.conversation_count() == 0);
}
