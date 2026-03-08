/// test_command.cpp — Unit tests for Command and CommandHistory
#include "core/Command.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

/// Concrete test command for testing
class TestCommand : public Command
{
public:
    int& counter;
    int delta;

    TestCommand(int& counter, int delta)
        : counter(counter)
        , delta(delta)
    {
    }

    void execute() override
    {
        counter += delta;
    }
    void undo() override
    {
        counter -= delta;
    }
    [[nodiscard]] auto description() const -> std::string override
    {
        return "TestCommand";
    }
};

class MergeableCommand : public Command
{
public:
    int& counter;
    int value;

    MergeableCommand(int& counter, int value)
        : counter(counter)
        , value(value)
    {
    }

    void execute() override
    {
        counter += value;
    }
    void undo() override
    {
        counter -= value;
    }
    [[nodiscard]] auto description() const -> std::string override
    {
        return "MergeableCommand";
    }
    [[nodiscard]] auto is_mergeable_with(const Command& other) const -> bool override
    {
        return dynamic_cast<const MergeableCommand*>(&other) != nullptr;
    }
    void merge_with(Command& other) override
    {
        if (auto* m = dynamic_cast<MergeableCommand*>(&other))
        {
            value += m->value;
        }
    }
};

TEST_CASE("CommandHistory: initially cannot undo or redo", "[command]")
{
    CommandHistory history;
    REQUIRE_FALSE(history.can_undo());
    REQUIRE_FALSE(history.can_redo());
}

TEST_CASE("CommandHistory: execute adds to history and runs command", "[command]")
{
    CommandHistory history;
    int counter = 0;
    history.execute(std::make_unique<TestCommand>(counter, 5));
    REQUIRE(counter == 5);
    REQUIRE(history.can_undo());
}

TEST_CASE("CommandHistory: undo reverses command", "[command]")
{
    CommandHistory history;
    int counter = 0;
    history.execute(std::make_unique<TestCommand>(counter, 5));
    REQUIRE(counter == 5);
    history.undo();
    REQUIRE(counter == 0);
    REQUIRE_FALSE(history.can_undo());
    REQUIRE(history.can_redo());
}

TEST_CASE("CommandHistory: redo re-applies command", "[command]")
{
    CommandHistory history;
    int counter = 0;
    history.execute(std::make_unique<TestCommand>(counter, 5));
    history.undo();
    history.redo();
    REQUIRE(counter == 5);
    REQUIRE(history.can_undo());
    REQUIRE_FALSE(history.can_redo());
}

TEST_CASE("CommandHistory: multiple undo/redo", "[command]")
{
    CommandHistory history;
    int counter = 0;
    history.execute(std::make_unique<TestCommand>(counter, 1));
    history.execute(std::make_unique<TestCommand>(counter, 2));
    history.execute(std::make_unique<TestCommand>(counter, 3));
    REQUIRE(counter == 6);

    history.undo();
    REQUIRE(counter == 3);
    history.undo();
    REQUIRE(counter == 1);
    history.redo();
    REQUIRE(counter == 3);
}

TEST_CASE("CommandHistory: new execute after undo clears redo", "[command]")
{
    CommandHistory history;
    int counter = 0;
    history.execute(std::make_unique<TestCommand>(counter, 5));
    history.undo();
    history.execute(std::make_unique<TestCommand>(counter, 10));
    REQUIRE_FALSE(history.can_redo());
    REQUIRE(counter == 10);
}

TEST_CASE("CommandHistory: undo_description returns command description", "[command]")
{
    CommandHistory history;
    int counter = 0;
    history.execute(std::make_unique<TestCommand>(counter, 1));
    REQUIRE(history.undo_description() == "TestCommand");
}

TEST_CASE("CommandHistory: redo_description returns command description", "[command]")
{
    CommandHistory history;
    int counter = 0;
    history.execute(std::make_unique<TestCommand>(counter, 1));
    history.undo();
    REQUIRE(history.redo_description() == "TestCommand");
}

TEST_CASE("CommandHistory: clear empties history", "[command]")
{
    CommandHistory history;
    int counter = 0;
    history.execute(std::make_unique<TestCommand>(counter, 1));
    history.clear();
    REQUIRE_FALSE(history.can_undo());
    REQUIRE_FALSE(history.can_redo());
}

TEST_CASE("Command: is_mergeable_with returns false by default", "[command]")
{
    int c1 = 0, c2 = 0;
    TestCommand cmd1(c1, 1);
    TestCommand cmd2(c2, 2);
    REQUIRE_FALSE(cmd1.is_mergeable_with(cmd2));
}
