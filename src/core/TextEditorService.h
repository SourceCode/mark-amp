#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// A position in a text document (line and character offset).
/// Mirrors VS Code's `Position`.
struct TextPosition
{
    int line{0};
    int character{0};
};

/// A range in a text document.
/// Mirrors VS Code's `Range`.
struct TextRange
{
    TextPosition start;
    TextPosition end;
};

/// A selection in a text document, with anchor and active (cursor) positions.
/// Mirrors VS Code's `Selection`.
struct TextSelection
{
    TextPosition anchor; ///< The position where the selection started
    TextPosition active; ///< The position of the cursor (may differ from anchor)
};

/// Builder for applying atomic text edits to a document.
/// Mirrors VS Code's `TextEditorEdit` callback parameter.
class EditBuilder
{
public:
    struct Edit
    {
        enum class Type : int
        {
            kInsert,
            kReplace,
            kDelete
        };
        Type type{Type::kInsert};
        TextPosition position;
        TextRange range;
        std::string text;
    };

    /// Insert text at a position.
    void insert(TextPosition position, const std::string& text);

    /// Replace text in a range.
    void replace(TextRange range, const std::string& text);

    /// Delete text in a range.
    void delete_range(TextRange range);

    /// Get all accumulated edits.
    [[nodiscard]] auto edits() const -> const std::vector<Edit>&;

    /// Clear all edits.
    void clear();

private:
    std::vector<Edit> edits_;
};

/// Represents a text editor instance with document, selections, and edit capabilities.
/// Mirrors VS Code's `TextEditor`.
struct TextEditor
{
    std::string uri;         ///< Document URI
    std::string language_id; ///< Language identifier

    std::vector<TextSelection> selections; ///< Current selections

    /// Apply the edit callback's changes atomically. Returns true on success.
    std::function<bool(std::function<void(EditBuilder&)>)> edit;

    /// Scroll to reveal the given range.
    std::function<void(TextRange)> reveal_range;

    /// Insert a snippet at the current cursor position (or specified position).
    std::function<void(const std::string&, std::optional<TextPosition>)> insert_snippet;
};

/// Options for showing a document in the editor.
struct ShowDocumentOptions
{
    bool preview{true};         ///< Open as preview tab
    bool preserve_focus{false}; ///< Don't steal focus from current editor
    TextSelection selection;    ///< Initial selection/cursor position
};

/// Service providing active editor tracking, selection management, and edit operations.
///
/// Mirrors VS Code's `vscode.window.activeTextEditor` and related APIs.
///
/// Usage:
/// ```cpp
/// auto* editor = ctx.text_editor_service->active_editor();
/// if (editor) {
///     editor->edit([](EditBuilder& eb) {
///         eb.insert({0, 0}, "# Title\n");
///     });
/// }
/// ```
class TextEditorService
{
public:
    using EditorCallback = std::function<void(const TextEditor*)>;
    using SelectionCallback = std::function<void(const TextEditor&)>;

    TextEditorService() = default;

    // ── Active Editor ──

    [[nodiscard]] auto active_editor() -> TextEditor*;
    [[nodiscard]] auto active_editor() const -> const TextEditor*;

    /// Set the active editor. Fires on_did_change_active_editor listeners.
    void set_active_editor(TextEditor editor);

    /// Clear the active editor (e.g., all tabs closed).
    void clear_active_editor();

    // ── Visible Editors ──

    [[nodiscard]] auto visible_editors() const -> const std::vector<TextEditor>&;

    /// Add an editor to the visible editors list.
    void add_visible_editor(TextEditor editor);

    /// Remove a visible editor by URI.
    void remove_visible_editor(const std::string& uri);

    // ── Show Document ──

    /// Open a document in the editor. Returns a pointer to the new TextEditor.
    auto show_document(const std::string& uri, const ShowDocumentOptions& options = {})
        -> TextEditor*;

    // ── Events ──

    auto on_did_change_active_editor(EditorCallback callback) -> std::size_t;
    auto on_did_change_selection(SelectionCallback callback) -> std::size_t;
    void remove_listener(std::size_t listener_id);

    /// Fire selection change event (called by editor panel when selection changes).
    void fire_selection_change();

private:
    std::optional<TextEditor> active_editor_;
    std::vector<TextEditor> visible_editors_;

    struct ListenerEntry
    {
        std::size_t listener_id{0};
        bool is_editor_change{true};
        EditorCallback editor_callback;
        SelectionCallback selection_callback;
    };
    std::vector<ListenerEntry> listeners_;
    std::size_t next_listener_id_{0};
};

} // namespace markamp::core
