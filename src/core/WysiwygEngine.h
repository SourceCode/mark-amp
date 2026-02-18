#pragma once

/// @file WysiwygEngine.h
/// @brief V9 Phase 45 Task 3 – WYSIWYG engine coordinating document processing,
///        block activation, and edit commit.
///
/// Header-only, pure logic. Wraps WysiwygDocumentModel with cursor-awareness.

#include "WysiwygTypes.h"

#include <optional>
#include <sstream>
#include <string>

namespace markamp::core
{

/// Coordinates the WYSIWYG editing experience.
/// Given markdown content and cursor position, produces a document model
/// with active block tracking and supports block-level edits.
class WysiwygEngine
{
public:
    WysiwygEngine() = default;

    /// Set the render configuration.
    void set_config(const WysiwygRenderConfig& config) noexcept
    {
        config_ = config;
    }

    /// Get the current render configuration.
    [[nodiscard]] auto config() const noexcept -> const WysiwygRenderConfig&
    {
        return config_;
    }

    /// Process a full document and produce a document model.
    /// The cursor_line determines which block becomes "active" (shown as raw source).
    auto process_document(const std::string& markdown, int cursor_line) -> WysiwygDocumentModel
    {
        WysiwygDocumentModel model;
        model.parse(markdown);

        // Mark the active block based on cursor position
        for (auto& block : model.blocks_mut())
        {
            block.is_active = (cursor_line >= block.start_line && cursor_line <= block.end_line);
        }

        last_model_ = model;
        last_cursor_line_ = cursor_line;
        return model;
    }

    /// Get the currently active block (the one containing the cursor).
    /// Returns nullptr if no block is active.
    [[nodiscard]] auto get_active_block() const -> const WysiwygBlock*
    {
        return last_model_.get_block_at_line(last_cursor_line_);
    }

    /// Get the index of the active block, or -1 if none.
    [[nodiscard]] auto active_block_index() const -> int
    {
        const auto& blocks = last_model_.blocks();
        for (std::size_t idx = 0; idx < blocks.size(); ++idx)
        {
            if (blocks[idx].is_active)
            {
                return static_cast<int>(idx);
            }
        }
        return -1;
    }

    /// Commit an edit to a specific block.
    /// Returns the full updated markdown string.
    [[nodiscard]] auto commit_block_edit(int block_index, const std::string& new_content)
        -> std::string
    {
        last_model_.replace_block(block_index, new_content);
        return last_model_.to_markdown();
    }

    /// Toggle the WYSIWYG mode.
    void set_mode(WysiwygMode mode) noexcept
    {
        mode_ = mode;
    }

    /// Get the current mode.
    [[nodiscard]] auto mode() const noexcept -> WysiwygMode
    {
        return mode_;
    }

    /// Check if a block type should be rendered inline in the current config.
    [[nodiscard]] auto should_render_inline(WysiwygBlockType block_type) const noexcept -> bool
    {
        if (mode_ == WysiwygMode::kSource)
        {
            return false; // Source mode: never render inline
        }

        switch (block_type)
        {
            case WysiwygBlockType::kImage:
                return config_.render_images_inline;
            case WysiwygBlockType::kTable:
                return config_.render_tables_inline;
            case WysiwygBlockType::kMath:
                return config_.render_math_inline;
            case WysiwygBlockType::kCallout:
                return config_.render_callouts;
            case WysiwygBlockType::kCodeBlock:
                return config_.render_code_blocks;
            case WysiwygBlockType::kList:
                return config_.render_checkboxes; // Lists render checkboxes inline
            default:
                return true; // Headings, paragraphs, etc. always render inline in WYSIWYG
        }
    }

    /// Check if a block is currently editable (active + mode supports editing).
    [[nodiscard]] auto is_block_editable(const WysiwygBlock& block) const noexcept -> bool
    {
        if (mode_ == WysiwygMode::kSource)
        {
            return true; // All blocks editable in source mode
        }
        return block.is_active; // Only active block is editable in live preview / WYSIWYG
    }

    /// Access the last processed model.
    [[nodiscard]] auto document_model() const noexcept -> const WysiwygDocumentModel&
    {
        return last_model_;
    }

    /// Mutable access to the last processed model.
    [[nodiscard]] auto document_model_mut() noexcept -> WysiwygDocumentModel&
    {
        return last_model_;
    }

private:
    WysiwygRenderConfig config_;
    WysiwygDocumentModel last_model_;
    WysiwygMode mode_{WysiwygMode::kLivePreview};
    int last_cursor_line_{0};
};

} // namespace markamp::core
