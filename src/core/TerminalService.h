#pragma once

#include "AnsiParser.h"
#include "EventBus.h"
#include "PtyProcess.h"
#include "TerminalBuffer.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Represents an embedded terminal instance (metadata).
struct Terminal
{
    std::string name;       ///< Display name
    std::string shell_path; ///< Path to the shell executable
    int terminal_id{0};     ///< Unique terminal ID
};

/// Internal session state for a live terminal.
struct TerminalSession
{
    int terminal_id{0};
    std::string name;
    std::string shell_path;
    std::string working_directory;
    std::unique_ptr<PtyProcess> process;
    std::unique_ptr<AnsiParser> parser;
    std::unique_ptr<TerminalBuffer> buffer;
};

/// Shell profile descriptor.
struct ShellProfile
{
    std::string name;      // "zsh", "bash", "fish"
    std::string path;      // "/bin/zsh"
    std::string icon_hint; // "shell_zsh"
    bool is_default{false};
};

/// Phase 21: Real PTY-backed terminal service.
/// Manages multiple terminal sessions with PtyProcess, AnsiParser, and TerminalBuffer.
class TerminalService
{
public:
    explicit TerminalService(EventBus& event_bus);

    /// Create a new terminal session.
    auto create_terminal(const std::string& name,
                         const std::string& shell_path = {},
                         const std::string& working_directory = {}) -> std::optional<int>;

    /// Send text to a terminal's PTY process.
    auto send_text(int terminal_id, const std::string& text) -> bool;

    /// Get metadata for all active terminals.
    [[nodiscard]] auto terminals() const -> std::vector<Terminal>;

    /// Close a terminal session and kill the process.
    auto close_terminal(int terminal_id) -> bool;

    /// Close all terminal sessions.
    void close_all_terminals();

    /// Get the buffer for a terminal (for rendering).
    [[nodiscard]] auto get_buffer(int terminal_id) -> TerminalBuffer*;

    /// Resize a terminal's PTY and buffer.
    void resize_terminal(int terminal_id, int cols, int rows);

    /// Get the active terminal ID.
    [[nodiscard]] auto active_terminal_id() const -> int;

    /// Set the active terminal.
    void set_active_terminal(int terminal_id);

    /// Detect the default shell from $SHELL.
    [[nodiscard]] auto detect_default_shell() const -> std::string;

    /// Detect available shells from /etc/shells.
    [[nodiscard]] auto detect_available_shells() const -> std::vector<ShellProfile>;

    /// Get the preferred shell path.
    [[nodiscard]] auto preferred_shell() const -> std::string;

    /// Set the preferred shell path.
    void set_preferred_shell(const std::string& shell_path);

    /// Kill a terminal process (soft = Ctrl+C, hard = SIGHUP→SIGKILL).
    void kill_terminal(int terminal_id, bool force);

private:
    EventBus& event_bus_;
    std::unordered_map<int, std::unique_ptr<TerminalSession>> sessions_;
    int next_id_{1};
    int active_terminal_id_{0};
    std::string preferred_shell_;

    void on_terminal_data(int terminal_id, const std::string& data);
    void on_terminal_exit(int terminal_id, int exit_code);
};

} // namespace markamp::core
