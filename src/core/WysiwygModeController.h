#pragma once

/// @file WysiwygModeController.h
/// @brief V9 Phase 45 Tasks 4–5 – Mode controller managing WYSIWYG mode transitions
///        and cursor-aware block activation/deactivation.
///
/// Header-only, pure logic. Manages per-file mode state and block activation.

#include "WysiwygTypes.h"

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace markamp::core
{

/// Callback type for mode change notifications.
using WysiwygModeChangeCallback = std::function<void(WysiwygMode old_mode, WysiwygMode new_mode)>;

/// Callback type for block activation change notifications.
using WysiwygBlockActivationCallback = std::function<void(int block_index, bool activated)>;

/// Controls WYSIWYG mode transitions and per-block activation state.
/// Manages mode per-file path, tracks which blocks are active/collapsed,
/// and emits callbacks on state changes.
class WysiwygModeController
{
public:
    WysiwygModeController() = default;

    // ── Mode management ──

    /// Get the current WYSIWYG mode.
    [[nodiscard]] auto current_mode() const noexcept -> WysiwygMode
    {
        return current_mode_;
    }

    /// Switch to a new mode. Fires the mode change callback if mode changes.
    void switch_mode(WysiwygMode new_mode)
    {
        if (new_mode == current_mode_)
        {
            return;
        }
        const WysiwygMode old_mode = current_mode_;
        current_mode_ = new_mode;

        // Deactivate all blocks when switching to source mode
        if (new_mode == WysiwygMode::kSource)
        {
            deactivate_all_blocks();
        }

        if (mode_change_callback_)
        {
            mode_change_callback_(old_mode, new_mode);
        }
    }

    /// Cycle through modes: Source → LivePreview → WYSIWYG → Source.
    void cycle_mode()
    {
        switch (current_mode_)
        {
            case WysiwygMode::kSource:
                switch_mode(WysiwygMode::kLivePreview);
                break;
            case WysiwygMode::kLivePreview:
                switch_mode(WysiwygMode::kWysiwyg);
                break;
            case WysiwygMode::kWysiwyg:
                switch_mode(WysiwygMode::kSource);
                break;
        }
    }

    /// Set mode change callback.
    void on_mode_changed(WysiwygModeChangeCallback callback)
    {
        mode_change_callback_ = std::move(callback);
    }

    /// Set block activation callback.
    void on_block_activation_changed(WysiwygBlockActivationCallback callback)
    {
        block_activation_callback_ = std::move(callback);
    }

    // ── Block editability checks ──

    /// Check if a block type supports inline editing in the current mode.
    [[nodiscard]] auto is_block_editable(WysiwygBlockType block_type) const noexcept -> bool
    {
        if (current_mode_ == WysiwygMode::kSource)
        {
            return true; // Everything is editable in source mode
        }

        // In LivePreview/WYSIWYG, these block types support inline editing
        switch (block_type)
        {
            case WysiwygBlockType::kParagraph:
            case WysiwygBlockType::kHeading:
            case WysiwygBlockType::kList:
            case WysiwygBlockType::kBlockquote:
            case WysiwygBlockType::kTable:
            case WysiwygBlockType::kCallout:
                return true;
            case WysiwygBlockType::kCodeBlock:
            case WysiwygBlockType::kMath:
            case WysiwygBlockType::kFrontmatter:
            case WysiwygBlockType::kImage:
            case WysiwygBlockType::kHtmlBlock:
            case WysiwygBlockType::kThematicBreak:
                return false; // These require special editors or are non-editable
        }
        return false;
    }

    /// Check if a block type should be rendered inline (vs. as raw markdown).
    [[nodiscard]] auto should_render_inline(WysiwygBlockType block_type) const noexcept -> bool
    {
        if (current_mode_ == WysiwygMode::kSource)
        {
            return false; // Nothing rendered inline in source mode
        }

        // In LivePreview, only fold syntax on non-active blocks
        // In WYSIWYG, render everything inline
        if (current_mode_ == WysiwygMode::kWysiwyg)
        {
            return true;
        }

        // LivePreview: render images, tables, math, callouts inline
        switch (block_type)
        {
            case WysiwygBlockType::kHeading:
            case WysiwygBlockType::kParagraph:
            case WysiwygBlockType::kList:
            case WysiwygBlockType::kBlockquote:
            case WysiwygBlockType::kImage:
            case WysiwygBlockType::kTable:
            case WysiwygBlockType::kMath:
            case WysiwygBlockType::kCallout:
            case WysiwygBlockType::kCodeBlock:
                return true;
            case WysiwygBlockType::kFrontmatter:
            case WysiwygBlockType::kHtmlBlock:
            case WysiwygBlockType::kThematicBreak:
                return false;
        }
        return false;
    }

    // ── Cursor-aware block activation (Task 5) ──

    /// Activate a block by index (cursor entered this block).
    /// The active block shows raw markdown for editing. Surrounding blocks remain rendered.
    void activate_block(int block_index)
    {
        if (current_mode_ == WysiwygMode::kSource)
        {
            return; // No block activation needed in source mode
        }

        // Deactivate the previously active block first
        if (active_block_index_.has_value() && active_block_index_.value() != block_index)
        {
            deactivate_block(active_block_index_.value());
        }

        active_block_index_ = block_index;

        if (block_activation_callback_)
        {
            block_activation_callback_(block_index, true);
        }
    }

    /// Deactivate a specific block by index (cursor left this block).
    void deactivate_block(int block_index)
    {
        if (active_block_index_.has_value() && active_block_index_.value() == block_index)
        {
            active_block_index_.reset();
        }

        if (block_activation_callback_)
        {
            block_activation_callback_(block_index, false);
        }
    }

    /// Deactivate all blocks.
    void deactivate_all_blocks()
    {
        if (active_block_index_.has_value())
        {
            const int prev = active_block_index_.value();
            active_block_index_.reset();
            if (block_activation_callback_)
            {
                block_activation_callback_(prev, false);
            }
        }
    }

    /// Get the index of the currently active block (-1 if none).
    [[nodiscard]] auto active_block_index() const noexcept -> int
    {
        return active_block_index_.value_or(-1);
    }

    /// Check if a specific block is the active block.
    [[nodiscard]] auto is_block_active(int block_index) const noexcept -> bool
    {
        return active_block_index_.has_value() && active_block_index_.value() == block_index;
    }

    // ── Per-file mode persistence ──

    /// Set mode for a specific file path (for per-file persistence).
    void set_file_mode(const std::string& file_path, WysiwygMode mode_val)
    {
        file_modes_[file_path] = mode_val;
    }

    /// Get mode for a specific file path, or default.
    [[nodiscard]] auto get_file_mode(const std::string& file_path) const -> WysiwygMode
    {
        auto iter = file_modes_.find(file_path);
        if (iter != file_modes_.end())
        {
            return iter->second;
        }
        return WysiwygMode::kLivePreview; // Default mode
    }

    /// Restore mode from per-file state when switching files.
    void restore_file_mode(const std::string& file_path)
    {
        switch_mode(get_file_mode(file_path));
    }

    // ── Collapsed blocks tracking ──

    /// Toggle a block's collapsed state.
    void toggle_collapsed(int block_index)
    {
        if (collapsed_blocks_.count(block_index) > 0)
        {
            collapsed_blocks_.erase(block_index);
        }
        else
        {
            collapsed_blocks_.insert(block_index);
        }
    }

    /// Check if a block is collapsed.
    [[nodiscard]] auto is_collapsed(int block_index) const -> bool
    {
        return collapsed_blocks_.count(block_index) > 0;
    }

    /// Get all collapsed block indices.
    [[nodiscard]] auto collapsed_blocks() const noexcept -> const std::unordered_set<int>&
    {
        return collapsed_blocks_;
    }

private:
    WysiwygMode current_mode_{WysiwygMode::kLivePreview};
    std::optional<int> active_block_index_;
    std::unordered_map<std::string, WysiwygMode> file_modes_;
    std::unordered_set<int> collapsed_blocks_;

    WysiwygModeChangeCallback mode_change_callback_;
    WysiwygBlockActivationCallback block_activation_callback_;
};

} // namespace markamp::core
