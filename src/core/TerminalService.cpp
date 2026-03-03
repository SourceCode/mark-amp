#include "TerminalService.h"

#include "Events.h"
#include "Logger.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace markamp::core
{

TerminalService::TerminalService(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

auto TerminalService::create_terminal(const std::string& name,
                                      const std::string& shell_path,
                                      const std::string& working_directory) -> std::optional<int>
{
    const int tid = next_id_++;
    const std::string resolved_shell = shell_path.empty() ? detect_default_shell() : shell_path;
    const std::string resolved_cwd =
        working_directory.empty() ? std::filesystem::current_path().string() : working_directory;

    auto session = std::make_unique<TerminalSession>();
    session->terminal_id = tid;
    session->name = name.empty() ? ("Terminal " + std::to_string(tid)) : name;
    session->shell_path = resolved_shell;
    session->working_directory = resolved_cwd;
    session->parser = std::make_unique<AnsiParser>();
    session->buffer = std::make_unique<TerminalBuffer>(80, 24, 10000);
    session->process = std::make_unique<PtyProcess>(resolved_shell, resolved_cwd, 80, 24);

    session->process->set_on_data([this, tid](const std::string& data)
                                  { on_terminal_data(tid, data); });

    session->process->set_on_exit([this, tid](int exit_code) { on_terminal_exit(tid, exit_code); });

    if (!session->process->start())
    {
        MARKAMP_LOG_ERROR("TerminalService: failed to start PTY for terminal {}", tid);
        return std::nullopt;
    }

    sessions_[tid] = std::move(session);
    active_terminal_id_ = tid;

    events::TerminalCreatedEvent created_evt;
    created_evt.terminal_id = tid;
    created_evt.name = sessions_[tid]->name;
    created_evt.shell_path = resolved_shell;
    event_bus_.publish(created_evt);

    MARKAMP_LOG_INFO("TerminalService: created terminal {} ({})", tid, resolved_shell);
    return tid;
}

auto TerminalService::send_text(int terminal_id, const std::string& text) -> bool
{
    auto session_it = sessions_.find(terminal_id);
    if (session_it == sessions_.end() || !session_it->second->process)
    {
        return false;
    }

    session_it->second->process->write(text);
    events::TerminalDataInputEvent input_evt;
    input_evt.terminal_id = terminal_id;
    input_evt.data = text;
    event_bus_.publish(input_evt);
    return true;
}

auto TerminalService::terminals() const -> std::vector<Terminal>
{
    std::vector<Terminal> result;
    result.reserve(sessions_.size());
    for (const auto& [tid, session] : sessions_)
    {
        Terminal term;
        term.terminal_id = tid;
        term.name = session->name;
        term.shell_path = session->shell_path;
        result.push_back(std::move(term));
    }
    return result;
}

auto TerminalService::close_terminal(int terminal_id) -> bool
{
    auto session_it = sessions_.find(terminal_id);
    if (session_it == sessions_.end())
    {
        return false;
    }

    if (session_it->second->process)
    {
        session_it->second->process->stop();
    }

    sessions_.erase(session_it);
    events::TerminalDestroyedEvent destroyed_evt;
    destroyed_evt.terminal_id = terminal_id;
    event_bus_.publish(destroyed_evt);

    // Update active terminal
    if (active_terminal_id_ == terminal_id)
    {
        active_terminal_id_ = sessions_.empty() ? 0 : sessions_.begin()->first;
    }

    MARKAMP_LOG_INFO("TerminalService: closed terminal {}", terminal_id);
    return true;
}

void TerminalService::close_all_terminals()
{
    std::vector<int> ids;
    ids.reserve(sessions_.size());
    for (const auto& [tid, _] : sessions_)
    {
        ids.push_back(tid);
    }
    for (const int tid : ids)
    {
        close_terminal(tid);
    }
}

auto TerminalService::get_buffer(int terminal_id) -> TerminalBuffer*
{
    auto session_it = sessions_.find(terminal_id);
    if (session_it == sessions_.end())
    {
        return nullptr;
    }
    return session_it->second->buffer.get();
}

void TerminalService::resize_terminal(int terminal_id, int cols, int rows)
{
    auto session_it = sessions_.find(terminal_id);
    if (session_it == sessions_.end())
    {
        return;
    }

    session_it->second->buffer->resize(cols, rows);

    if (session_it->second->process)
    {
        session_it->second->process->resize(cols, rows);
    }

    events::TerminalResizedEvent resized_evt;
    resized_evt.terminal_id = terminal_id;
    resized_evt.cols = cols;
    resized_evt.rows = rows;
    event_bus_.publish(resized_evt);
}

auto TerminalService::active_terminal_id() const -> int
{
    return active_terminal_id_;
}

void TerminalService::set_active_terminal(int terminal_id)
{
    if (sessions_.contains(terminal_id))
    {
        active_terminal_id_ = terminal_id;
    }
}

auto TerminalService::detect_default_shell() const -> std::string
{
    if (!preferred_shell_.empty())
    {
        return preferred_shell_;
    }

    // Read $SHELL environment variable
    const char* shell_env = std::getenv("SHELL");
    if (shell_env != nullptr && std::filesystem::exists(shell_env))
    {
        return shell_env;
    }

    // Fallback
    if (std::filesystem::exists("/bin/zsh"))
    {
        return "/bin/zsh";
    }
    return "/bin/sh";
}

auto TerminalService::detect_available_shells() const -> std::vector<ShellProfile>
{
    std::vector<ShellProfile> shells;

    // Read /etc/shells
    std::ifstream etc_shells("/etc/shells");
    std::string line;
    while (std::getline(etc_shells, line))
    {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        if (std::filesystem::exists(line))
        {
            ShellProfile profile;
            profile.path = line;

            // Extract name from path
            const auto slash_pos = line.rfind('/');
            profile.name = (slash_pos != std::string::npos) ? line.substr(slash_pos + 1) : line;
            profile.icon_hint = "shell_" + profile.name;

            const std::string default_shell = detect_default_shell();
            profile.is_default = (line == default_shell);

            shells.push_back(std::move(profile));
        }
    }

    return shells;
}

auto TerminalService::preferred_shell() const -> std::string
{
    return preferred_shell_;
}

void TerminalService::set_preferred_shell(const std::string& shell_path)
{
    preferred_shell_ = shell_path;
}

void TerminalService::kill_terminal(int terminal_id, bool force)
{
    auto session_it = sessions_.find(terminal_id);
    if (session_it == sessions_.end())
    {
        return;
    }

    if (force)
    {
        session_it->second->process->stop();
    }
    else
    {
        // Soft kill: send Ctrl+C
        session_it->second->process->write("\x03");
    }
}

void TerminalService::on_terminal_data(int terminal_id, const std::string& data)
{
    auto session_it = sessions_.find(terminal_id);
    if (session_it == sessions_.end())
    {
        return;
    }

    auto& session = session_it->second;

    // Parse ANSI sequences
    auto ops = session->parser->parse(data);

    // Check for title/CWD/bell events before applying to buffer
    for (const auto& op_variant : ops)
    {
        if (const auto* title = std::get_if<SetTitle>(&op_variant))
        {
            events::TerminalTitleChangedEvent title_evt;
            title_evt.terminal_id = terminal_id;
            title_evt.title = title->title;
            event_bus_.publish(title_evt);
        }
        else if (const auto* cwd_op = std::get_if<SetCwd>(&op_variant))
        {
            events::TerminalCwdChangedEvent cwd_evt;
            cwd_evt.terminal_id = terminal_id;
            cwd_evt.cwd = cwd_op->path;
            event_bus_.publish(cwd_evt);
        }
        else if (std::holds_alternative<Bell>(op_variant))
        {
            events::TerminalBellEvent bell_evt;
            bell_evt.terminal_id = terminal_id;
            event_bus_.publish(bell_evt);
        }
    }

    // Apply to buffer
    session->buffer->apply_batch(ops);

    // Publish raw data for UI
    events::TerminalDataOutputEvent output_evt;
    output_evt.terminal_id = terminal_id;
    output_evt.data = data;
    event_bus_.publish(output_evt);
}

void TerminalService::on_terminal_exit(int terminal_id, int exit_code)
{
    MARKAMP_LOG_INFO("TerminalService: terminal {} exited with code {}", terminal_id, exit_code);

    // Don't auto-remove — leave session alive for inspection
    // The user can manually close the tab
    auto session_it = sessions_.find(terminal_id);
    if (session_it != sessions_.end() && session_it->second->buffer)
    {
        // Append exit message to buffer
        std::string exit_msg =
            "\r\n\x1b[90m[Process exited with code " + std::to_string(exit_code) + "]\x1b[0m\r\n";
        auto ops = session_it->second->parser->parse(exit_msg);
        session_it->second->buffer->apply_batch(ops);
    }
}

} // namespace markamp::core
