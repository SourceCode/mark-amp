// ============================================================================
// File: src/canvas/CanvasClipboardService.h
// Phase 12: Canvas Advanced Objects — clipboard cut/copy/paste/duplicate
// ============================================================================
#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"
#include "canvas/SelectionManager.h"
#include "canvas/UndoRedoStack.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Serialized clipboard payload holding one or more canvas objects.
struct ClipboardData
{
    std::string serialized_json;         ///< JSON array of serialized objects
    std::vector<CanvasObjectType> types; ///< Types of objects in clipboard
    size_t object_count{0};              ///< Number of objects
    Point2D original_center{0.0, 0.0};   ///< Center of the copied bounding box
    bool has_connectors{false};          ///< Whether clipboard contains connectors
};

/// Result of a paste operation.
struct PasteResult
{
    bool success{false};
    std::vector<ObjectId> pasted_ids; ///< IDs of newly created objects
    std::string error_message;
};

/// Canvas clipboard service for cut/copy/paste/duplicate operations.
///
/// Provides:
///   - Cut/copy selected objects to internal clipboard
///   - Paste with configurable offset or at a specific position
///   - Duplicate selection in-place
///   - Connector-aware copy (preserves connections between copied objects)
///   - Full UndoRedoStack integration
class CanvasClipboardService
{
public:
    CanvasClipboardService(Board& board, SelectionManager& selection, UndoRedoStack& undo_stack);

    // ── Core Operations ───────────────────────────────────────────

    /// Copy selected objects to the internal clipboard.
    auto copy() -> bool;

    /// Cut selected objects (copy + delete with undo).
    auto cut() -> bool;

    /// Paste clipboard contents at the default offset from original position.
    [[nodiscard]] auto paste() -> PasteResult;

    /// Paste clipboard contents centered at a specific board position.
    [[nodiscard]] auto paste_at(const Point2D& position) -> PasteResult;

    /// Duplicate the current selection in-place with an offset.
    [[nodiscard]] auto duplicate_selection() -> PasteResult;

    // ── Clipboard State ───────────────────────────────────────────

    /// Check if the clipboard has data.
    [[nodiscard]] auto has_data() const -> bool;

    /// Get the number of objects in the clipboard.
    [[nodiscard]] auto clipboard_count() const -> size_t;

    /// Clear the clipboard.
    auto clear() -> void;

    /// Get clipboard metadata (types, count, etc.) without deserializing.
    [[nodiscard]] auto clipboard_info() const -> const ClipboardData&;

    // ── Configuration ─────────────────────────────────────────────

    /// Set the offset applied to pasted objects (default: 20, 20).
    auto set_paste_offset(const Point2D& offset) -> void;

    /// Get the current paste offset.
    [[nodiscard]] auto paste_offset() const -> const Point2D&;

    /// Default offset for paste operations.
    static constexpr double kDefaultPasteOffset = 20.0;

private:
    Board& board_;
    SelectionManager& selection_;
    UndoRedoStack& undo_stack_;
    ClipboardData clipboard_;
    Point2D paste_offset_{kDefaultPasteOffset, kDefaultPasteOffset};
    int paste_count_{0}; ///< Tracks consecutive pastes for cascading offset

    /// Serialize a set of objects to JSON for clipboard storage.
    [[nodiscard]] auto serialize_objects(const std::vector<ObjectId>& ids) -> std::string;

    /// Deserialize clipboard JSON and create new object instances.
    [[nodiscard]] auto deserialize_objects(const std::string& json)
        -> std::vector<std::unique_ptr<CanvasObject>>;

    /// Compute the center of a bounding box enclosing all given objects.
    [[nodiscard]] auto compute_center(const std::vector<ObjectId>& ids) const -> Point2D;
};

} // namespace markamp::canvas
