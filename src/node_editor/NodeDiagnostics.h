#pragma once

#include "NodeEditorTypes.h"

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// Diagnostic types
// ---------------------------------------------------------------------------

enum class DiagnosticSeverity
{
    kError,
    kWarning,
    kInfo,
    kHint
};

struct Diagnostic
{
    std::size_t id{0}; ///< Unique diagnostic ID
    DiagnosticSeverity severity{DiagnosticSeverity::kError};
    std::string message;
    NodeId source_node;
    std::optional<SocketId> source_socket;
    std::string suggestion; ///< Recovery suggestion text
    bool has_recovery{false};
    std::string recovery_description;

    [[nodiscard]] auto is_error() const -> bool
    {
        return severity == DiagnosticSeverity::kError;
    }
    [[nodiscard]] auto is_warning() const -> bool
    {
        return severity == DiagnosticSeverity::kWarning;
    }
};

// ---------------------------------------------------------------------------
// NodeDiagnostics — error/warning collection with recovery flows
// ---------------------------------------------------------------------------

class NodeDiagnostics
{
public:
    NodeDiagnostics() = default;

    // --- Adding diagnostics -----------------------------------------------

    /// Add a diagnostic entry. Returns the assigned ID.
    auto add_diagnostic(const Diagnostic& diagnostic) -> std::size_t;

    /// Add a simple error on a node.
    auto add_error(NodeId node_id, const std::string& message) -> std::size_t;

    /// Add a simple warning on a node.
    auto add_warning(NodeId node_id, const std::string& message) -> std::size_t;

    /// Add an info diagnostic.
    auto add_info(NodeId node_id, const std::string& message) -> std::size_t;

    // --- Querying diagnostics ---------------------------------------------

    /// Get diagnostics for a specific node.
    [[nodiscard]] auto diagnostics_for(NodeId node_id) const -> std::vector<Diagnostic>;

    /// Get all diagnostics.
    [[nodiscard]] auto all_diagnostics() const -> const std::vector<Diagnostic>&;

    /// Get a specific diagnostic by ID.
    [[nodiscard]] auto find_diagnostic(std::size_t diagnostic_id) const -> const Diagnostic*;

    // --- Counts -----------------------------------------------------------

    [[nodiscard]] auto error_count() const -> std::size_t;
    [[nodiscard]] auto warning_count() const -> std::size_t;
    [[nodiscard]] auto total_count() const -> std::size_t;

    /// Check if a node has errors.
    [[nodiscard]] auto has_errors(NodeId node_id) const -> bool;

    /// Check if a node has warnings.
    [[nodiscard]] auto has_warnings(NodeId node_id) const -> bool;

    // --- Clearing ---------------------------------------------------------

    /// Clear diagnostics for a specific node.
    void clear(NodeId node_id);

    /// Clear all diagnostics.
    void clear_all();

    /// Remove a specific diagnostic by ID.
    void remove(std::size_t diagnostic_id);

    // --- Recovery ---------------------------------------------------------

    /// Check if a diagnostic has a recovery action.
    [[nodiscard]] static auto has_recovery(const Diagnostic& diagnostic) -> bool;

    /// Get recovery description for a diagnostic.
    [[nodiscard]] static auto recovery_description(const Diagnostic& diagnostic) -> std::string;

private:
    std::vector<Diagnostic> diagnostics_;
    std::size_t next_id_{1};
};

} // namespace markamp::node_editor
