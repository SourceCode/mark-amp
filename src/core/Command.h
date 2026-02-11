#pragma once

#include <memory>
#include <string>
#include <vector>

namespace markamp::core
{

/// Abstract command for undo/redo.
class Command
{
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    [[nodiscard]] virtual auto description() const -> std::string = 0;
    [[nodiscard]] virtual auto is_mergeable_with(const Command& other) const -> bool;
    virtual void merge_with(Command& other);
};

/// Manages an undo/redo history of executed commands.
class CommandHistory
{
public:
    void execute(std::unique_ptr<Command> cmd);
    void undo();
    void redo();
    [[nodiscard]] auto can_undo() const -> bool;
    [[nodiscard]] auto can_redo() const -> bool;
    [[nodiscard]] auto undo_description() const -> std::string;
    [[nodiscard]] auto redo_description() const -> std::string;
    void clear();

private:
    std::vector<std::unique_ptr<Command>> history_;
    std::size_t current_index_{0};
    static constexpr std::size_t kMaxHistorySize = 1000;
};

} // namespace markamp::core
