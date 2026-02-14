#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Identifies a text document by URI.
struct TextDocumentIdentifier
{
    std::string uri;
};

/// Represents a change range within a document.
struct ContentChangeRange
{
    int start_line{0};
    int start_character{0};
    int end_line{0};
    int end_character{0};
};

/// A single content change within a document.
struct ContentChange
{
    ContentChangeRange range; ///< The range that was replaced
    std::string text;         ///< The new text for the range
};

/// A text document item with full content.
/// Mirrors VS Code's `TextDocument`.
struct TextDocumentItem
{
    std::string uri;         ///< Document URI (file:///path/to/file)
    std::string language_id; ///< Language identifier (e.g. "markdown")
    std::int64_t version{0}; ///< Document version, incremented on each change
    std::string content;     ///< Full text content
};

/// Event fired when a document's content changes.
struct TextDocumentChangeEvent
{
    std::string uri;
    std::int64_t version{0};
    std::vector<ContentChange> changes;
};

/// A workspace folder root.
struct WorkspaceFolder
{
    std::string uri;  ///< Folder URI
    std::string name; ///< Display name
};

/// A single text edit operation for workspace edits.
struct WorkspaceTextEdit
{
    std::string uri;
    ContentChangeRange range;
    std::string new_text;
};

/// A workspace-wide edit containing multiple text edits.
struct WorkspaceEditBatch
{
    std::vector<WorkspaceTextEdit> edits;
};

/// Service providing document tracking, events, workspace folders, and file search.
///
/// Mirrors VS Code's `vscode.workspace` namespace. Extensions use this to:
/// - Track open documents and their content
/// - Listen for document lifecycle events (open, close, change, save)
/// - Access workspace folder roots
/// - Apply multi-file edits
/// - Search for files by glob pattern
///
/// Usage:
/// ```cpp
/// ctx.workspace_service->on_did_open([](const TextDocumentItem& doc) {
///     // A document was opened
/// });
/// auto docs = ctx.workspace_service->text_documents();
/// ```
class WorkspaceService
{
public:
    using DocumentCallback = std::function<void(const TextDocumentItem&)>;
    using ChangeCallback = std::function<void(const TextDocumentChangeEvent&)>;
    using UriCallback = std::function<void(const std::string&)>;

    WorkspaceService() = default;

    // ── Document Tracking ──

    /// Register a document as open.
    void open_document(TextDocumentItem document);

    /// Close a document by URI.
    void close_document(const std::string& uri);

    /// Apply a content change to a document.
    void change_document(const std::string& uri,
                         std::int64_t version,
                         std::vector<ContentChange> changes);

    /// Mark a document as saved.
    void save_document(const std::string& uri);

    /// Get a document by URI.
    [[nodiscard]] auto get_document(const std::string& uri) const -> const TextDocumentItem*;

    /// Get all open documents.
    [[nodiscard]] auto text_documents() const -> std::vector<TextDocumentItem>;

    // ── Document Events ──

    auto on_did_open(DocumentCallback callback) -> std::size_t;
    auto on_did_close(UriCallback callback) -> std::size_t;
    auto on_did_change(ChangeCallback callback) -> std::size_t;
    auto on_did_save(UriCallback callback) -> std::size_t;
    void remove_listener(std::size_t listener_id);

    // ── Workspace Folders ──

    [[nodiscard]] auto workspace_folders() const -> const std::vector<WorkspaceFolder>&;
    void add_workspace_folder(WorkspaceFolder folder);
    void remove_workspace_folder(const std::string& uri);

    // ── Workspace Edit ──

    auto apply_edit(const WorkspaceEditBatch& edit) -> bool;

    // ── File Search ──

    [[nodiscard]] auto find_files(const std::string& glob_pattern) const
        -> std::vector<std::string>;

private:
    std::unordered_map<std::string, TextDocumentItem> documents_;
    std::vector<WorkspaceFolder> workspace_folders_;

    // Listeners
    struct ListenerEntry
    {
        std::size_t listener_id{0};
        enum class Type : std::uint8_t
        {
            kOpen,
            kClose,
            kChange,
            kSave
        } type{Type::kOpen};
        DocumentCallback doc_callback;
        ChangeCallback change_callback;
        UriCallback uri_callback;
    };
    std::vector<ListenerEntry> listeners_;
    std::size_t next_listener_id_{0};
};

} // namespace markamp::core
