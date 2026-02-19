#include "NodeDiagnostics.h"

#include <algorithm>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// Adding diagnostics
// ---------------------------------------------------------------------------

auto NodeDiagnostics::add_diagnostic(const Diagnostic& diagnostic) -> std::size_t
{
    auto entry = diagnostic;
    entry.id = next_id_++;
    diagnostics_.push_back(entry);
    return entry.id;
}

auto NodeDiagnostics::add_error(NodeId node_id, const std::string& message) -> std::size_t
{
    Diagnostic diag;
    diag.severity = DiagnosticSeverity::kError;
    diag.source_node = node_id;
    diag.message = message;
    return add_diagnostic(diag);
}

auto NodeDiagnostics::add_warning(NodeId node_id, const std::string& message) -> std::size_t
{
    Diagnostic diag;
    diag.severity = DiagnosticSeverity::kWarning;
    diag.source_node = node_id;
    diag.message = message;
    return add_diagnostic(diag);
}

auto NodeDiagnostics::add_info(NodeId node_id, const std::string& message) -> std::size_t
{
    Diagnostic diag;
    diag.severity = DiagnosticSeverity::kInfo;
    diag.source_node = node_id;
    diag.message = message;
    return add_diagnostic(diag);
}

// ---------------------------------------------------------------------------
// Querying diagnostics
// ---------------------------------------------------------------------------

auto NodeDiagnostics::diagnostics_for(NodeId node_id) const -> std::vector<Diagnostic>
{
    std::vector<Diagnostic> result;
    for (const auto& diag : diagnostics_)
    {
        if (diag.source_node == node_id)
        {
            result.push_back(diag);
        }
    }
    return result;
}

auto NodeDiagnostics::all_diagnostics() const -> const std::vector<Diagnostic>&
{
    return diagnostics_;
}

auto NodeDiagnostics::find_diagnostic(std::size_t diagnostic_id) const -> const Diagnostic*
{
    for (const auto& diag : diagnostics_)
    {
        if (diag.id == diagnostic_id)
        {
            return &diag;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Counts
// ---------------------------------------------------------------------------

auto NodeDiagnostics::error_count() const -> std::size_t
{
    return static_cast<std::size_t>(std::count_if(diagnostics_.begin(),
                                                  diagnostics_.end(),
                                                  [](const Diagnostic& diag)
                                                  { return diag.is_error(); }));
}

auto NodeDiagnostics::warning_count() const -> std::size_t
{
    return static_cast<std::size_t>(std::count_if(diagnostics_.begin(),
                                                  diagnostics_.end(),
                                                  [](const Diagnostic& diag)
                                                  { return diag.is_warning(); }));
}

auto NodeDiagnostics::total_count() const -> std::size_t
{
    return diagnostics_.size();
}

auto NodeDiagnostics::has_errors(NodeId node_id) const -> bool
{
    return std::any_of(diagnostics_.begin(),
                       diagnostics_.end(),
                       [&](const Diagnostic& diag)
                       { return diag.source_node == node_id && diag.is_error(); });
}

auto NodeDiagnostics::has_warnings(NodeId node_id) const -> bool
{
    return std::any_of(diagnostics_.begin(),
                       diagnostics_.end(),
                       [&](const Diagnostic& diag)
                       { return diag.source_node == node_id && diag.is_warning(); });
}

// ---------------------------------------------------------------------------
// Clearing
// ---------------------------------------------------------------------------

void NodeDiagnostics::clear(NodeId node_id)
{
    diagnostics_.erase(std::remove_if(diagnostics_.begin(),
                                      diagnostics_.end(),
                                      [&](const Diagnostic& diag)
                                      { return diag.source_node == node_id; }),
                       diagnostics_.end());
}

void NodeDiagnostics::clear_all()
{
    diagnostics_.clear();
}

void NodeDiagnostics::remove(std::size_t diagnostic_id)
{
    diagnostics_.erase(std::remove_if(diagnostics_.begin(),
                                      diagnostics_.end(),
                                      [&](const Diagnostic& diag)
                                      { return diag.id == diagnostic_id; }),
                       diagnostics_.end());
}

// ---------------------------------------------------------------------------
// Recovery
// ---------------------------------------------------------------------------

auto NodeDiagnostics::has_recovery(const Diagnostic& diagnostic) -> bool
{
    return diagnostic.has_recovery;
}

auto NodeDiagnostics::recovery_description(const Diagnostic& diagnostic) -> std::string
{
    return diagnostic.recovery_description;
}

} // namespace markamp::node_editor
