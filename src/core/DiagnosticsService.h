#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Severity levels for diagnostics (mirrors VS Code's DiagnosticSeverity).
enum class DiagnosticSeverity : std::uint8_t
{
    kError = 0,
    kWarning = 1,
    kInformation = 2,
    kHint = 3
};

/// A position within a document.
struct DiagnosticPosition
{
    int line{0};
    int character{0};
};

/// A range within a document.
struct DiagnosticRange
{
    DiagnosticPosition start;
    DiagnosticPosition end;
};

/// A single diagnostic entry (error, warning, etc.).
struct Diagnostic
{
    DiagnosticRange range;
    std::string message;
    DiagnosticSeverity severity{DiagnosticSeverity::kError};
    std::string source; // e.g. "eslint", "tsc"
    std::string code;   // e.g. "TS2304", "no-unused-vars"
};

/// Service that manages diagnostics per URI (equivalent to VS Code's DiagnosticCollection).
class DiagnosticsService
{
public:
    DiagnosticsService() = default;

    /// Set diagnostics for a URI (replaces existing).
    void set(const std::string& uri, std::vector<Diagnostic> diagnostics);

    /// Get diagnostics for a URI.
    [[nodiscard]] auto get(const std::string& uri) const -> const std::vector<Diagnostic>&;

    /// Get all URIs that have diagnostics.
    [[nodiscard]] auto uris() const -> std::vector<std::string>;

    /// Remove diagnostics for a URI.
    void remove(const std::string& uri);

    /// Clear all diagnostics.
    void clear();

    /// Count diagnostics by severity across all URIs.
    [[nodiscard]] auto count_by_severity(DiagnosticSeverity severity) const -> std::size_t;

    /// Total diagnostic count.
    [[nodiscard]] auto total_count() const -> std::size_t;

    /// Listener for diagnostic changes.
    using ChangeListener = std::function<void(const std::string& uri)>;
    auto on_change(ChangeListener listener) -> std::size_t;
    void remove_listener(std::size_t listener_id);

private:
    std::unordered_map<std::string, std::vector<Diagnostic>> diagnostics_;
    static const std::vector<Diagnostic> kEmptyDiagnostics;

    std::vector<std::pair<std::size_t, ChangeListener>> listeners_;
    std::size_t next_listener_id_{0};

    void fire_change(const std::string& uri);
};

} // namespace markamp::core
