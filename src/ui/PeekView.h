// ============================================================================
// File: src/ui/PeekView.h
// Phase 47: Peek View System — Peek view container model
// ============================================================================
#pragma once

#include "../core/PeekProvider.h"

#include <string>
#include <vector>

namespace markamp::ui
{

/// State of a peek view.
enum class PeekViewState : uint8_t
{
    Closed,
    Opening,
    Open,
    Closing
};

/// A frame in the peek navigation stack.
struct PeekFrame
{
    core::PeekCommand command{core::PeekCommand::Definition};
    std::string source_file;
    int source_line{0};
    int source_column{0};
    std::vector<core::PeekLocation> results;
    int selected_index{0};
};

/// Model for the peek view container.
/// Manages lifecycle, navigation stack, sizing, and content.
class PeekViewModel
{
public:
    PeekViewModel() = default;

    /// Open a peek with results.
    void open(core::PeekCommand command,
              const std::string& source_file,
              int source_line,
              int source_column,
              std::vector<core::PeekLocation> results);

    /// Close the peek view.
    void close();

    /// Push a new peek on the navigation stack (chaining).
    void push(core::PeekCommand command,
              const std::string& source_file,
              int source_line,
              int source_column,
              std::vector<core::PeekLocation> results);

    /// Pop back to the previous peek.
    void pop();

    /// Get current state.
    [[nodiscard]] auto state() const -> PeekViewState
    {
        return state_;
    }

    /// Get current peek frame.
    [[nodiscard]] auto current_frame() const -> const PeekFrame*;

    /// Get stack depth.
    [[nodiscard]] auto stack_depth() const -> int
    {
        return static_cast<int>(stack_.size());
    }

    /// Max stack depth (5 levels).
    static constexpr int kMaxStackDepth = 5;

    // --- Sizing ---

    /// Set height in pixels (clamped to min/max).
    void set_height(int height);

    /// Get current height.
    [[nodiscard]] auto height() const -> int
    {
        return height_;
    }

    static constexpr int kMinHeight = 80;
    static constexpr int kMaxHeight = 500;
    static constexpr int kDefaultHeight = 250;

    // --- Result selection ---

    /// Select a result by index.
    void select_result(int index);

    /// Navigate to next result.
    void next_result();

    /// Navigate to previous result.
    void prev_result();

    /// Get selected result index.
    [[nodiscard]] auto selected_index() const -> int;

    /// Get result count.
    [[nodiscard]] auto result_count() const -> int;

    /// Get selected location.
    [[nodiscard]] auto selected_location() const -> const core::PeekLocation*;

private:
    PeekViewState state_{PeekViewState::Closed};
    std::vector<PeekFrame> stack_;
    int height_{kDefaultHeight};
};

} // namespace markamp::ui
