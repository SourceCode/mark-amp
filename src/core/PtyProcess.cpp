#include "PtyProcess.h"

#include "Logger.h"

#include <array>
#include <csignal>
#include <cstdlib>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <util.h> // macOS forkpty

namespace markamp::core
{

PtyProcess::PtyProcess(const std::string& shell_path,
                       const std::string& working_directory,
                       int cols,
                       int rows)
    : shell_path_(shell_path)
    , working_directory_(working_directory)
    , cols_(cols)
    , rows_(rows)
{
}

PtyProcess::~PtyProcess()
{
    stop();
}

auto PtyProcess::start() -> bool
{
    struct winsize win_size = {};
    win_size.ws_col = static_cast<unsigned short>(cols_);
    win_size.ws_row = static_cast<unsigned short>(rows_);

    const int forked_pid = forkpty(&master_fd_, nullptr, nullptr, &win_size);

    if (forked_pid < 0)
    {
        MARKAMP_LOG_ERROR("PtyProcess: forkpty() failed");
        return false;
    }

    if (forked_pid == 0)
    {
        // Child process
        if (!working_directory_.empty())
        {
            if (chdir(working_directory_.c_str()) != 0)
            {
                // Best-effort; if it fails, use inherited CWD
            }
        }

        // Set TERM for proper terminal support
        setenv("TERM", "xterm-256color", 1);
        setenv("COLORTERM", "truecolor", 1);

        // Execute shell
        execl(shell_path_.c_str(), shell_path_.c_str(), "-l", nullptr);

        // If exec fails, exit
        _exit(127);
    }

    // Parent process
    child_pid_ = forked_pid;
    running_ = true;

    // Start read thread
    read_thread_ = std::thread(&PtyProcess::read_loop, this);

    MARKAMP_LOG_INFO("PtyProcess: spawned {} (pid={})", shell_path_, child_pid_);
    return true;
}

void PtyProcess::stop()
{
    if (!running_.exchange(false))
    {
        return;
    }

    if (child_pid_ > 0)
    {
        // Send SIGHUP first (graceful)
        kill(child_pid_, SIGHUP);

        // Wait briefly, then SIGTERM
        usleep(100000); // 100ms
        int status = 0;
        const int waited = waitpid(child_pid_, &status, WNOHANG);
        if (waited == 0)
        {
            kill(child_pid_, SIGTERM);
            waitpid(child_pid_, &status, 0);
        }
        child_pid_ = -1;
    }

    if (master_fd_ >= 0)
    {
        close(master_fd_);
        master_fd_ = -1;
    }

    if (read_thread_.joinable())
    {
        read_thread_.join();
    }

    MARKAMP_LOG_INFO("PtyProcess: stopped");
}

auto PtyProcess::is_running() const -> bool
{
    return running_.load();
}

void PtyProcess::write(const std::string& data)
{
    if (master_fd_ < 0 || !running_)
    {
        return;
    }

    const auto* buf = data.data();
    auto remaining = static_cast<ssize_t>(data.size());
    while (remaining > 0)
    {
        const ssize_t written = ::write(master_fd_, buf, static_cast<std::size_t>(remaining));
        if (written <= 0)
        {
            break;
        }
        buf += written;
        remaining -= written;
    }
}

void PtyProcess::resize(int cols, int rows)
{
    cols_ = cols;
    rows_ = rows;

    if (master_fd_ < 0)
    {
        return;
    }

    struct winsize win_size = {};
    win_size.ws_col = static_cast<unsigned short>(cols);
    win_size.ws_row = static_cast<unsigned short>(rows);
    ioctl(master_fd_, TIOCSWINSZ, &win_size);

    // Signal the child
    if (child_pid_ > 0)
    {
        kill(child_pid_, SIGWINCH);
    }
}

void PtyProcess::set_on_data(DataCallback callback)
{
    on_data_ = std::move(callback);
}

void PtyProcess::set_on_exit(ExitCallback callback)
{
    on_exit_ = std::move(callback);
}

auto PtyProcess::pid() const -> int
{
    return child_pid_;
}
auto PtyProcess::shell_path() const -> const std::string&
{
    return shell_path_;
}
auto PtyProcess::working_directory() const -> const std::string&
{
    return working_directory_;
}

void PtyProcess::read_loop()
{
    constexpr std::size_t kBufSize = 4096;
    std::array<char, kBufSize> buf{};

    while (running_)
    {
        const ssize_t bytes_read = read(master_fd_, buf.data(), kBufSize);
        if (bytes_read <= 0)
        {
            break;
        }

        if (on_data_)
        {
            on_data_(std::string(buf.data(), static_cast<std::size_t>(bytes_read)));
        }
    }

    // Process has exited
    running_ = false;

    int status = 0;
    if (child_pid_ > 0)
    {
        waitpid(child_pid_, &status, WNOHANG);
    }

    const int exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

    if (on_exit_)
    {
        on_exit_(exit_code);
    }
}

} // namespace markamp::core
