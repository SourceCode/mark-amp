#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasTypes.h"

#include <utility>
#include <vector>

namespace markamp::canvas
{

/// Static utility class for group-aware z-index layering operations.
class LayeringService
{
public:
    LayeringService() = delete;

    /// Move all selected objects (expanding groups) to the front.
    static auto bring_to_front(Board& board, const std::vector<ObjectId>& ids) -> void;

    /// Move all selected objects to the back.
    static auto send_to_back(Board& board, const std::vector<ObjectId>& ids) -> void;

    /// Move all selected objects one step forward.
    static auto bring_forward(Board& board, const std::vector<ObjectId>& ids) -> void;

    /// Move all selected objects one step backward.
    static auto send_backward(Board& board, const std::vector<ObjectId>& ids) -> void;

    /// Get the min and max z-index on the board.
    [[nodiscard]] static auto z_range(const Board& board) -> std::pair<int, int>;

    /// Reassign z-indices 0..N-1 to remove gaps.
    static auto auto_distribute_z(Board& board) -> void;
};

} // namespace markamp::canvas
