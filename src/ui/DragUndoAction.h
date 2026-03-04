#pragma once

#include "core/Command.h"

#include <string>

namespace markamp::ui
{

/// Phase 45: Undo command for drag operations (tab move, panel reorder).
class DragUndoAction : public core::Command
{
public:
    struct DragState
    {
        std::string source_id;      ///< Where the item came from.
        int source_index{0};        ///< Index at source.
        std::string destination_id; ///< Where the item ended up.
        int destination_index{0};   ///< Index at destination.
    };

    explicit DragUndoAction(DragState before, DragState after);

    void execute() override;
    void undo() override;

    [[nodiscard]] auto description() const -> std::string override
    {
        return "Drag Move";
    }

    [[nodiscard]] auto before() const -> const DragState&
    {
        return before_;
    }
    [[nodiscard]] auto after() const -> const DragState&
    {
        return after_;
    }

private:
    DragState before_;
    DragState after_;
};

} // namespace markamp::ui
