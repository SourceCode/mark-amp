#pragma once

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Represents an embedded terminal instance.
/// P4 stub — terminal integration is N/A for a Markdown editor.
struct Terminal
{
    std::string name;       ///< Display name
    std::string shell_path; ///< Path to the shell executable
    int terminal_id{0};     ///< Unique terminal ID
};

/// Stub service for terminal integration.
///
/// P4: Not applicable for a Markdown editor. This stub exists so that
/// extension API surface is complete and extensions that query terminal
/// support get well-defined "not available" responses rather than crashes.
class TerminalService
{
public:
    TerminalService() = default;

    /// Create a new terminal instance.
    /// Stub: always returns nullopt.
    auto create_terminal(const std::string& name, const std::string& shell_path = {})
        -> std::optional<int>;

    /// Send text to a terminal.
    /// Stub: always returns false.
    auto send_text(int terminal_id, const std::string& text) -> bool;

    /// Get all active terminals.
    [[nodiscard]] auto terminals() const -> const std::vector<Terminal>&;

    /// Close a terminal.
    /// Stub: always returns false.
    auto close_terminal(int terminal_id) -> bool;

private:
    std::vector<Terminal> terminals_;
};

} // namespace markamp::core
