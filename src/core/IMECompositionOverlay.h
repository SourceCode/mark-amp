#pragma once

#include <cstddef>
#include <string>

namespace markamp::core
{

/// State of an IME composition in progress.
///
/// Pattern implemented: #38 IMEs and composition handled as overlays
struct IMECompositionState
{
    std::string composing_text;
    std::size_t cursor_offset{0}; // cursor position within composing_text
    bool is_active{false};
};

/// IME composition overlay — composition text is separate from the
/// main PieceTable buffer until committed.
///
/// During composition, text is drawn as a translucent underlined overlay
/// at the caret position. On commit, the final text is merged into the
/// PieceTable via a normal insert.
///
/// Pattern implemented: #38 IMEs and composition handled as overlays, not buffer edits
class IMECompositionOverlay
{
public:
    IMECompositionOverlay() = default;

    /// Begin a new composition session.
    void begin_composition() noexcept
    {
        state_.is_active = true;
        state_.composing_text.clear();
        state_.cursor_offset = 0;
    }

    /// Update the composing text and cursor position.
    void update_composition(const std::string& text, std::size_t cursor)
    {
        state_.composing_text = text;
        state_.cursor_offset = cursor;
    }

    /// Commit the composition — returns the final text to insert
    /// into the PieceTable. Resets the composition state.
    [[nodiscard]] auto commit_composition() -> std::string
    {
        auto text = std::move(state_.composing_text);
        state_ = IMECompositionState{};
        return text;
    }

    /// Cancel the composition — discards all composing text.
    void cancel_composition() noexcept
    {
        state_ = IMECompositionState{};
    }

    /// Whether a composition is currently active.
    [[nodiscard]] auto is_active() const noexcept -> bool
    {
        return state_.is_active;
    }

    /// Get the current composing text.
    [[nodiscard]] auto composing_text() const noexcept -> const std::string&
    {
        return state_.composing_text;
    }

    /// Get the cursor position within the composing text.
    [[nodiscard]] auto cursor_offset() const noexcept -> std::size_t
    {
        return state_.cursor_offset;
    }

    /// Length of the composing text.
    [[nodiscard]] auto composing_length() const noexcept -> std::size_t
    {
        return state_.composing_text.size();
    }

    /// Access the full composition state.
    [[nodiscard]] auto state() const noexcept -> const IMECompositionState&
    {
        return state_;
    }

private:
    IMECompositionState state_;
};

} // namespace markamp::core
