// test_command.cpp — 10 tests for Command and CommandHistory
#include "core/Command.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

class TestCommand : public Command
{
public:
    explicit TestCommand(int& counter, int delta = 1)
        : counter_(counter)
        , delta_(delta)
    {
    }
    void execute() override
    {
        counter_ += delta_;
    }
    void undo() override
    {
        counter_ -= delta_;
    }
    [[nodiscard]] auto description() const -> std::string override
    {
        return "TestCommand";
    }

private:
    int& counter_;
    int delta_;
};

TEST_CASE("CommandHistory starts empty", "[command]")
{
    CommandHistory history;
    CHECK_FALSE(history.can_undo());
    CHECK_FALSE(history.can_redo());
}

TEST_CASE("CommandHistory execute runs command", "[command]")
{
    int counter = 0;
    CommandHistory history;
    history.execute(std::make_unique<TestCommand>(counter, 5));
    CHECK(counter == 5);
    CHECK(history.can_undo());
}

TEST_CASE("CommandHistory undo reverses command", "[command]")
{
    int counter = 0;
    CommandHistory history;
    history.execute(std::make_unique<TestCommand>(counter, 10));
    CHECK(counter == 10);
    history.undo();
    CHECK(counter == 0);
    CHECK(history.can_redo());
}

TEST_CASE("CommandHistory redo re-applies command", "[command]")
{
    int counter = 0;
    CommandHistory history;
    history.execute(std::make_unique<TestCommand>(counter, 7));
    history.undo();
    CHECK(counter == 0);
    history.redo();
    CHECK(counter == 7);
}

TEST_CASE("CommandHistory new execute clears redo stack", "[command]")
{
    int counter = 0;
    CommandHistory history;
    history.execute(std::make_unique<TestCommand>(counter, 1));
    history.undo();
    CHECK(history.can_redo());
    history.execute(std::make_unique<TestCommand>(counter, 2));
    CHECK_FALSE(history.can_redo());
}

TEST_CASE("CommandHistory multiple undo/redo", "[command]")
{
    int counter = 0;
    CommandHistory history;
    history.execute(std::make_unique<TestCommand>(counter, 1));
    history.execute(std::make_unique<TestCommand>(counter, 2));
    history.execute(std::make_unique<TestCommand>(counter, 3));
    CHECK(counter == 6);
    history.undo(); // undo 3
    CHECK(counter == 3);
    history.undo(); // undo 2
    CHECK(counter == 1);
    history.redo(); // redo 2
    CHECK(counter == 3);
}

TEST_CASE("CommandHistory undo_description works", "[command]")
{
    int counter = 0;
    CommandHistory history;
    history.execute(std::make_unique<TestCommand>(counter));
    auto desc = history.undo_description();
    CHECK(desc == "TestCommand");
}

TEST_CASE("CommandHistory clear empties everything", "[command]")
{
    int counter = 0;
    CommandHistory history;
    history.execute(std::make_unique<TestCommand>(counter));
    history.clear();
    CHECK_FALSE(history.can_undo());
    CHECK_FALSE(history.can_redo());
}

TEST_CASE("CommandHistory undo on empty does nothing", "[command]")
{
    CommandHistory history;
    REQUIRE_NOTHROW(history.undo());
    REQUIRE_NOTHROW(history.redo());
}

TEST_CASE("Command is_mergeable_with defaults to false", "[command]")
{
    int counter = 0;
    TestCommand cmd1(counter);
    TestCommand cmd2(counter);
    CHECK_FALSE(cmd1.is_mergeable_with(cmd2));
}
