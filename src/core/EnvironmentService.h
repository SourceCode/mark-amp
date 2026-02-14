#pragma once

#include <string>

namespace markamp::core
{

/// Service providing environment information, clipboard access, and external URI opening.
///
/// Mirrors VS Code's `vscode.env` namespace. Extensions use this to:
/// - Query application identity and locale
/// - Access the system clipboard
/// - Open URIs in the user's default browser/application
///
/// Usage:
/// ```cpp
/// auto name = ctx.environment_service->app_name(); // "MarkAmp"
/// ctx.environment_service->open_external("https://docs.example.com");
/// ```
class EnvironmentService
{
public:
    EnvironmentService() = default;

    // ── Application Identity ──

    /// Application name.
    [[nodiscard]] auto app_name() const -> std::string;

    /// Path to the application root directory.
    [[nodiscard]] auto app_root() const -> std::string;

    /// Display language / locale (e.g. "en", "en-US").
    [[nodiscard]] auto language() const -> std::string;

    /// Unique machine identifier (hashed, privacy-safe).
    [[nodiscard]] auto machine_id() const -> std::string;

    /// URI scheme used for deep links (e.g. "markamp").
    [[nodiscard]] auto uri_scheme() const -> std::string;

    // ── Clipboard ──

    /// Read text from the system clipboard.
    [[nodiscard]] auto clipboard_read() const -> std::string;

    /// Write text to the system clipboard.
    void clipboard_write(const std::string& text);

    // ── External URIs ──

    /// Open a URI in the user's default application. Returns true on success.
    auto open_external(const std::string& uri) -> bool;

    // ── Overrides (for testing) ──

    void set_app_root(const std::string& root);
    void set_language(const std::string& lang);
    void set_machine_id(const std::string& machine_id);

private:
    std::string app_root_;
    std::string language_;
    mutable std::string machine_id_;
    std::string clipboard_;
};

} // namespace markamp::core
