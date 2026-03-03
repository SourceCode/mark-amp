#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <thread>

namespace markamp::core
{

/// Phase 21 Task 4: PTY process wrapper.
/// Spawns a shell via forkpty() on macOS, manages I/O via master fd,
/// supports resize (TIOCSWINSZ/SIGWINCH), and lifecycle management.
class PtyProcess
{
public:
    using DataCallback = std::function<void(const std::string& data)>;
    using ExitCallback = std::function<void(int exit_code)>;

    PtyProcess(const std::string& shell_path,
               const std::string& working_directory,
               int cols,
               int rows);
    ~PtyProcess();

    PtyProcess(const PtyProcess&) = delete;
    auto operator=(const PtyProcess&) -> PtyProcess& = delete;
    PtyProcess(PtyProcess&&) = delete;
    auto operator=(PtyProcess&&) -> PtyProcess& = delete;

    /// Start the PTY process. Returns true on success.
    [[nodiscard]] auto start() -> bool;

    /// Stop the process (SIGHUP → SIGTERM).
    void stop();

    /// Check if the process is still running.
    [[nodiscard]] auto is_running() const -> bool;

    /// Write data to the PTY master fd.
    void write(const std::string& data);

    /// Resize the PTY window.
    void resize(int cols, int rows);

    /// Set callback for data read from the PTY.
    void set_on_data(DataCallback callback);

    /// Set callback for process exit.
    void set_on_exit(ExitCallback callback);

    /// Get the child process PID.
    [[nodiscard]] auto pid() const -> int;

    /// Get the shell path.
    [[nodiscard]] auto shell_path() const -> const std::string&;

    /// Get the working directory.
    [[nodiscard]] auto working_directory() const -> const std::string&;

private:
    std::string shell_path_;
    std::string working_directory_;
    int cols_;
    int rows_;
    int master_fd_{-1};
    int child_pid_{-1};
    std::atomic<bool> running_{false};
    std::thread read_thread_;
    DataCallback on_data_;
    ExitCallback on_exit_;

    void read_loop();
};

} // namespace markamp::core
