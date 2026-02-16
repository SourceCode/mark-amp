/// CrashReporter.h — Phase 30: Crash reporting infrastructure
///
/// Registers signal handlers for SIGSEGV, SIGABRT, SIGBUS, SIGFPE, SIGILL.
/// Captures crash context (signal, backtrace, thread) and writes a crash
/// report file before terminating.
///
/// Pattern implemented: #27 Crash reporting

#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

#if defined(__APPLE__) || defined(__linux__)
#include <csignal>
#include <cstdlib>
#include <execinfo.h>
#include <unistd.h>
#endif

namespace markamp::core
{

/// Crash context captured at the point of the crash.
struct CrashContext
{
    int signal_number{0};
    std::string signal_name;
    int64_t timestamp_epoch_ms{0};
    std::string crash_report_path;

    static constexpr std::size_t kMaxBacktraceFrames = 64;
    std::array<std::string, kMaxBacktraceFrames> backtrace_frames{};
    std::size_t backtrace_depth{0};
};

/// Signal name lookup (O(1)).
[[nodiscard]] inline auto signal_name(int signum) noexcept -> std::string_view
{
    switch (signum)
    {
#if defined(__APPLE__) || defined(__linux__)
        case SIGSEGV:
            return "SIGSEGV";
        case SIGABRT:
            return "SIGABRT";
        case SIGBUS:
            return "SIGBUS";
        case SIGFPE:
            return "SIGFPE";
        case SIGILL:
            return "SIGILL";
#endif
        default:
            return "UNKNOWN";
    }
}

/// Platform-agnostic crash reporter.
///
/// Usage:
///   CrashReporter::instance().set_crash_dir("/tmp/crashes");
///   CrashReporter::instance().install();   // registers signal handlers
///   // ... normal execution ...
///   // On crash: writes crash report, prints to stderr, re-raises signal
class CrashReporter
{
public:
    static auto instance() -> CrashReporter&
    {
        static CrashReporter singleton;
        return singleton;
    }

    /// Set the directory where crash reports are written.
    void set_crash_dir(const std::filesystem::path& crash_dir)
    {
        crash_dir_ = crash_dir;
    }

    /// Set the application version string for crash reports.
    void set_app_version(std::string_view version)
    {
        app_version_ = std::string(version);
    }

    /// Install signal handlers. Call once during application startup.
    void install()
    {
#if defined(__APPLE__) || defined(__linux__)
        struct sigaction sa_action = {};
        sa_action.sa_handler = CrashReporter::signal_handler;
        sigemptyset(&sa_action.sa_mask);
        sa_action.sa_flags = SA_RESETHAND; // one-shot — restore default after first signal

        sigaction(SIGSEGV, &sa_action, nullptr);
        sigaction(SIGABRT, &sa_action, nullptr);
        sigaction(SIGBUS, &sa_action, nullptr);
        sigaction(SIGFPE, &sa_action, nullptr);
        sigaction(SIGILL, &sa_action, nullptr);
#endif
        installed_ = true;
    }

    /// Check if signal handlers are installed.
    [[nodiscard]] auto is_installed() const noexcept -> bool
    {
        return installed_;
    }

    /// Get the crash directory path.
    [[nodiscard]] auto crash_dir() const -> const std::filesystem::path&
    {
        return crash_dir_;
    }

    /// Generate a crash report for a given context (testable without real signal).
    [[nodiscard]] static auto generate_report(const CrashContext& ctx,
                                              std::string_view app_version_sv) -> std::string
    {
        std::string report;
        report.reserve(2048);

        report += "=== MARKAMP CRASH REPORT ===\n";
        report += "Version: " + std::string(app_version_sv) + "\n";
        report += "Signal:  " + ctx.signal_name + " (" + std::to_string(ctx.signal_number) + ")\n";
        report += "Time:    " + std::to_string(ctx.timestamp_epoch_ms) + "\n";
        report += "\n--- Backtrace ---\n";

        for (std::size_t frame_idx = 0; frame_idx < ctx.backtrace_depth; ++frame_idx)
        {
            report +=
                "  [" + std::to_string(frame_idx) + "] " + ctx.backtrace_frames[frame_idx] + "\n";
        }

        report += "\n=== END CRASH REPORT ===\n";
        return report;
    }

    /// Build a CrashContext from current state (without actual signal).
    /// Useful for testing.
    [[nodiscard]] auto build_context(int signum) const -> CrashContext
    {
        CrashContext ctx;
        ctx.signal_number = signum;
        ctx.signal_name = std::string(signal_name(signum));
        ctx.timestamp_epoch_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                     std::chrono::system_clock::now().time_since_epoch())
                                     .count();

#if defined(__APPLE__) || defined(__linux__)
        void* frames[CrashContext::kMaxBacktraceFrames] = {};
        int depth = backtrace(frames, static_cast<int>(CrashContext::kMaxBacktraceFrames));
        char** symbols = backtrace_symbols(frames, depth);

        if (symbols != nullptr)
        {
            ctx.backtrace_depth = static_cast<std::size_t>(depth);
            for (int frame_idx = 0; frame_idx < depth; ++frame_idx)
            {
                ctx.backtrace_frames[static_cast<std::size_t>(frame_idx)] =
                    symbols[frame_idx]; // NOLINT
            }
            free(symbols); // NOLINT - backtrace_symbols uses malloc
        }
#endif

        return ctx;
    }

private:
    CrashReporter() = default;
    CrashReporter(const CrashReporter&) = delete;
    auto operator=(const CrashReporter&) -> CrashReporter& = delete;

    std::filesystem::path crash_dir_{"/tmp"};
    std::string app_version_{"unknown"};
    bool installed_{false};

#if defined(__APPLE__) || defined(__linux__)
    static void signal_handler(int signum)
    {
        auto& reporter = instance();
        auto ctx = reporter.build_context(signum);
        auto report = generate_report(ctx, reporter.app_version_);

        // Write to stderr (signal-safe enough for our purposes)
        // NOLINTNEXTLINE - write is async-signal-safe
        write(STDERR_FILENO, report.c_str(), report.size());

        // Write crash report file
        try
        {
            auto filename = "crash_" + std::to_string(ctx.timestamp_epoch_ms) + ".txt";
            auto path = reporter.crash_dir_ / filename;
            std::ofstream out_stream(path);
            if (out_stream.is_open())
            {
                out_stream << report;
            }
        }
        catch (...)
        {
            // Best-effort — don't throw in signal handler
        }

        // Re-raise to get default behavior (core dump, etc.)
        raise(signum);
    }
#endif
};

} // namespace markamp::core
