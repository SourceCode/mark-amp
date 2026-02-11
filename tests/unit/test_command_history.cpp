#include "core/Command.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// Test command that tracks execution
class TestCommand : public Command
{
public:
    explicit TestCommand(std::string desc, int& counter)
        : desc_(std::move(desc))
        , counter_(counter)
    {
    }

    void execute() override
    {
        counter_++;
    }
    void undo() override
    {
        counter_--;
    }
    [[nodiscard]] auto description() const -> std::string override
    {
        return desc_;
    }

private:
    std::string desc_;
    int& counter_;
};

// Mergeable command for testing merge behavior
class MergeableCommand : public Command
{
public:
    explicit MergeableCommand(std::string& target, char ch)
        : target_(target)
        , ch_(ch)
    {
    }

    void execute() override
    {
        target_ += ch_;
    }
    void undo() override
    {
        if (!target_.empty())
        {
            target_.pop_back();
        }
    }
    [[nodiscard]] auto description() const -> std::string override
    {
        return "Type char";
    }
    [[nodiscard]] auto is_mergeable_with([[maybe_unused]] const Command& other) const
        -> bool override
    {
        return dynamic_cast<const MergeableCommand*>(&other) != nullptr;
    }
    void merge_with(Command& other) override
    {
        auto& mc = dynamic_cast<MergeableCommand&>(other);
        ch_ = mc.ch_;
    }

private:
    std::string& target_;
    char ch_;
};

TEST_CASE("CommandHistory: execute and undo", "[core][command]")
{
    CommandHistory history;
    int counter = 0;

    history.execute(std::make_unique<TestCommand>("Increment", counter));
    REQUIRE(counter == 1);
    REQUIRE(history.can_undo());

    history.undo();
    REQUIRE(counter == 0);
    REQUIRE(!history.can_undo());
}

TEST_CASE("CommandHistory: redo after undo", "[core][command]")
{
    CommandHistory history;
    int counter = 0;

    history.execute(std::make_unique<TestCommand>("Increment", counter));
    history.undo();
    REQUIRE(history.can_redo());

    history.redo();
    REQUIRE(counter == 1);
    REQUIRE(!history.can_redo());
}

TEST_CASE("CommandHistory: new command clears redo stack", "[core][command]")
{
    CommandHistory history;
    int counter = 0;

    history.execute(std::make_unique<TestCommand>("First", counter));
    history.execute(std::make_unique<TestCommand>("Second", counter));
    REQUIRE(counter == 2);

    history.undo();
    REQUIRE(counter == 1);
    REQUIRE(history.can_redo());

    history.execute(std::make_unique<TestCommand>("Third", counter));
    REQUIRE(!history.can_redo()); // Redo stack cleared
    REQUIRE(counter == 2);
}

TEST_CASE("CommandHistory: descriptions", "[core][command]")
{
    CommandHistory history;
    int counter = 0;

    REQUIRE(history.undo_description().empty());
    REQUIRE(history.redo_description().empty());

    history.execute(std::make_unique<TestCommand>("Increment", counter));
    REQUIRE(history.undo_description() == "Increment");

    history.undo();
    REQUIRE(history.redo_description() == "Increment");
}

TEST_CASE("CommandHistory: clear resets everything", "[core][command]")
{
    CommandHistory history;
    int counter = 0;

    history.execute(std::make_unique<TestCommand>("A", counter));
    history.execute(std::make_unique<TestCommand>("B", counter));
    REQUIRE(history.can_undo());

    history.clear();
    REQUIRE(!history.can_undo());
    REQUIRE(!history.can_redo());
}
