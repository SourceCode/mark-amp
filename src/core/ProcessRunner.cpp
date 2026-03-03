#include "ProcessRunner.h"

#include "Logger.h"

#include <sstream>

namespace markamp::core
{

ProcessRunner::ProcessRunner() = default;
ProcessRunner::~ProcessRunner() = default;

void ProcessRunner::Launch(const std::string& program,
                           const std::vector<std::string>& args,
                           const std::string& working_dir)
{
    std::ostringstream cmd_line;
    cmd_line << program;
    for (const auto& arg : args)
    {
        cmd_line << " " << arg;
    }

    MARKAMP_LOG_INFO("ProcessRunner: Launching '{}' in {}", cmd_line.str(), working_dir);
    is_running_ = true;
    exit_code_ = -1;

    // Future: fork/exec or CreateProcess, pipe stdout/stderr to on_output_
    // Stub: simulate immediate completion
    is_running_ = false;
    exit_code_ = 0;

    if (on_output_)
    {
        on_output_("[ProcessRunner] Program exited with code 0", false);
    }
    if (on_exit_)
    {
        on_exit_(exit_code_);
    }
}

void ProcessRunner::Terminate()
{
    MARKAMP_LOG_INFO("ProcessRunner: Terminate requested");
    if (is_running_)
    {
        is_running_ = false;
        exit_code_ = -9; // SIGKILL equivalent
        if (on_exit_)
        {
            on_exit_(exit_code_);
        }
    }
}

auto ProcessRunner::IsRunning() const -> bool
{
    return is_running_;
}

auto ProcessRunner::ExitCode() const -> int
{
    return exit_code_;
}

void ProcessRunner::SetOnOutput(OutputCallback callback)
{
    on_output_ = std::move(callback);
}

void ProcessRunner::SetOnExit(ExitCallback callback)
{
    on_exit_ = std::move(callback);
}

} // namespace markamp::core
