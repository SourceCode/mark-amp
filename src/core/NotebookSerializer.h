/// @file NotebookSerializer.h
/// @brief V8 Phase 15 – Jupyter .ipynb format serialization and deserialization.
/// Parses nbformat v4 JSON into NotebookDocument and serializes back.

#pragma once

#include "core/EventBus.h"

#include <expected>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Notebook metadata structures
// ============================================================================

/// Kernel specification stored in notebook metadata.
struct NotebookKernelSpec
{
    std::string name;         ///< e.g., "python3"
    std::string display_name; ///< e.g., "Python 3.11"
    std::string language;     ///< e.g., "python"
};

/// Language info stored in notebook metadata.
struct NotebookLanguageInfo
{
    std::string name;            ///< e.g., "python"
    std::string version;         ///< e.g., "3.11.0"
    std::string file_extension;  ///< e.g., ".py"
    std::string mimetype;        ///< e.g., "text/x-python"
    std::string codemirror_mode; ///< e.g., "python"
    std::string pygments_lexer;  ///< e.g., "ipython3"
};

/// Cell output stored in serialized form.
struct SerializedCellOutput
{
    std::string output_type;            ///< "execute_result", "stream", "display_data", "error"
    std::string text;                   ///< Plain text content
    std::string html;                   ///< HTML content (if any)
    std::string png_base64;             ///< Base-64 PNG data (if any)
    std::string stream_name;            ///< "stdout" or "stderr" (for stream outputs)
    int execution_count{0};             ///< Execution count for execute_result
    std::string error_name;             ///< Error class name (for error outputs)
    std::string error_value;            ///< Error message
    std::vector<std::string> traceback; ///< Traceback lines
};

/// A single cell in the serialized notebook.
struct SerializedCell
{
    std::string cell_id;
    std::string cell_type; ///< "code", "markdown", "raw"
    std::string source;
    int execution_count{0};
    std::vector<SerializedCellOutput> outputs;
    std::unordered_map<std::string, std::string> metadata;
};

/// Top-level notebook document.
struct NotebookDocument
{
    int nbformat{4};
    int nbformat_minor{5};
    NotebookKernelSpec kernelspec;
    NotebookLanguageInfo language_info;
    std::unordered_map<std::string, std::string> custom_metadata;
    std::vector<SerializedCell> cells;
};

/// Validation issue found in a notebook.
struct ValidationIssue
{
    enum class Severity
    {
        kWarning,
        kError
    };
    Severity severity{Severity::kWarning};
    std::string message;
    int cell_index{-1}; ///< -1 means notebook-level issue
};

/// Result of notebook validation.
struct ValidationResult
{
    bool valid{true};
    std::vector<ValidationIssue> issues;
};

// ============================================================================
// NotebookSerializer
// ============================================================================

class NotebookSerializer
{
public:
    explicit NotebookSerializer(EventBus& event_bus);

    /// Parse a .ipynb JSON string into a NotebookDocument.
    [[nodiscard]] auto parse_ipynb(const std::string& json)
        -> std::expected<NotebookDocument, std::string>;

    /// Serialize a NotebookDocument to .ipynb JSON string.
    [[nodiscard]] auto serialize_ipynb(const NotebookDocument& doc) const -> std::string;

    /// Validate a notebook document for schema conformance.
    [[nodiscard]] auto validate_notebook(const NotebookDocument& doc) const -> ValidationResult;

    /// Upgrade a notebook from an older nbformat version.
    [[nodiscard]] auto upgrade_notebook(const NotebookDocument& doc, int target_version) const
        -> std::expected<NotebookDocument, std::string>;

    /// Strip all cell outputs (useful for git-clean notebooks).
    [[nodiscard]] auto strip_outputs(const NotebookDocument& doc) const -> NotebookDocument;

    /// Get the number of code cells in a document.
    [[nodiscard]] static auto code_cell_count(const NotebookDocument& doc) -> int;

    /// Get the number of markdown cells in a document.
    [[nodiscard]] static auto markdown_cell_count(const NotebookDocument& doc) -> int;

    /// Human-readable summary of a notebook document.
    [[nodiscard]] static auto summarize(const NotebookDocument& doc) -> std::string;

private:
    EventBus& event_bus_;

    /// Parse a single cell from a JSON-like string block.
    [[nodiscard]] auto parse_cell_block(const std::string& block, int index) const
        -> SerializedCell;

    /// Serialize a single cell to a JSON-like string.
    [[nodiscard]] auto serialize_cell(const SerializedCell& cell) const -> std::string;

    /// Generate a unique cell ID.
    [[nodiscard]] static auto generate_cell_id() -> std::string;
};

} // namespace markamp::core
