#include "BuildService.h"

#include <regex>
#include <sstream>
#include <unordered_map>

namespace markamp::core
{

auto BuildService::start_build(const std::string& target, const std::string& config) -> bool
{
    if (status_ == BuildStatus::kBuilding || status_ == BuildStatus::kConfiguring)
    {
        return false;
    }

    status_ = BuildStatus::kBuilding;
    progress_ = {};
    errors_.clear();
    last_result_ = {};
    build_start_time_ = std::chrono::steady_clock::now();

    if (!target.empty())
    {
        active_target_ = target;
    }
    if (!config.empty())
    {
        active_config_ = config;
    }

    // Actual process spawning would be done via TaskRunner/PtyProcess.
    // This service tracks state; the caller must feed output via process_output_line().
    return true;
}

void BuildService::stop_build()
{
    if (status_ != BuildStatus::kBuilding)
    {
        return;
    }
    status_ = BuildStatus::kIdle;
    last_result_.success = false;
}

auto BuildService::clean(const std::string& /*config*/) -> bool
{
    // Would execute: cmake --build build --target clean
    return true;
}

auto BuildService::configure(const std::string& /*config*/) -> bool
{
    status_ = BuildStatus::kConfiguring;
    // Would execute: cmake -B build -DCMAKE_BUILD_TYPE=config
    return true;
}

void BuildService::detect_build_system(const std::string& workspace_path)
{
    workspace_path_ = workspace_path;
    // Check for CMakeLists.txt, Makefile, etc.
    // For now, default to CMake.
    build_system_ = BuildSystem::kCMake;
}

auto BuildService::build_system() const -> BuildSystem
{
    return build_system_;
}

void BuildService::set_build_system(BuildSystem system)
{
    build_system_ = system;
}

void BuildService::discover_targets(const std::string& /*build_dir*/)
{
    // Would parse cmake --build build --target help
    // Populate with default targets for now.
    targets_ = {
        {.name = "all", .description = "Build all targets", .is_default = true},
        {.name = "clean", .description = "Clean build artifacts"},
        {.name = "install", .description = "Install built targets"},
    };
}

auto BuildService::targets() const -> const std::vector<BuildTarget>&
{
    return targets_;
}

void BuildService::set_active_target(const std::string& target_name)
{
    active_target_ = target_name;
}

auto BuildService::active_target() const -> const std::string&
{
    return active_target_;
}

auto BuildService::configurations() const -> const std::vector<BuildConfiguration>&
{
    return configurations_;
}

void BuildService::set_active_configuration(const std::string& config_name)
{
    active_config_ = config_name;
}

auto BuildService::active_configuration() const -> const std::string&
{
    return active_config_;
}

auto BuildService::status() const -> BuildStatus
{
    return status_;
}

auto BuildService::last_result() const -> const BuildResult&
{
    return last_result_;
}

auto BuildService::is_building() const -> bool
{
    return status_ == BuildStatus::kBuilding || status_ == BuildStatus::kConfiguring;
}

auto BuildService::progress() const -> const BuildProgress&
{
    return progress_;
}

void BuildService::process_output_line(const std::string& line)
{
    // Parse progress markers
    parse_progress(line);

    // Parse compiler errors
    auto parsed = error_parser_.parse_line(line);
    for (auto& err : parsed)
    {
        errors_.push_back(std::move(err));
    }

    // Update error/warning counts in last_result
    last_result_.error_count = 0;
    last_result_.warning_count = 0;
    for (const auto& error : errors_)
    {
        if (error.severity == DiagnosticSeverity::kError)
        {
            last_result_.error_count++;
        }
        else if (error.severity == DiagnosticSeverity::kWarning)
        {
            last_result_.warning_count++;
        }
    }

    // Append to output
    last_result_.output += line + "\n";

    // Write to output channel
    if (output_service_ != nullptr)
    {
        auto* channel = output_service_->ensure_channel("Build");
        if (channel != nullptr)
        {
            channel->append_line(line);
        }
    }
}

auto BuildService::error_parser() -> CompilerErrorParser&
{
    return error_parser_;
}

auto BuildService::parsed_errors() const -> const std::vector<ParsedCompilerError>&
{
    return errors_;
}

void BuildService::set_diagnostics_service(DiagnosticsService* service)
{
    diagnostics_service_ = service;
}

void BuildService::set_output_service(OutputChannelService* service)
{
    output_service_ = service;
}

void BuildService::parse_progress(const std::string& line)
{
    // Ninja format: [42/100] Building CXX object ...
    static const std::regex kNinjaProgress(R"(^\[(\d+)/(\d+)\]\s*(.*)$)");
    // Make format: make[1]: ... (less structured)

    std::smatch match;
    if (std::regex_match(line, match, kNinjaProgress))
    {
        progress_.completed = std::stoi(match[1].str());
        progress_.total = std::stoi(match[2].str());
        if (progress_.total > 0)
        {
            progress_.percentage =
                static_cast<float>(progress_.completed) / static_cast<float>(progress_.total);
        }

        // Extract current file being compiled
        const std::string build_text = match[3].str();
        auto file_pos = build_text.rfind('/');
        if (file_pos != std::string::npos)
        {
            progress_.current_file = build_text.substr(file_pos + 1);
        }
        else
        {
            progress_.current_file = build_text;
        }
    }
}

void BuildService::publish_errors_to_diagnostics()
{
    if (diagnostics_service_ == nullptr)
    {
        return;
    }

    // Group errors by file
    std::unordered_map<std::string, std::vector<ParsedCompilerError>> errors_by_file;
    for (const auto& error : errors_)
    {
        errors_by_file[error.file].push_back(error);
    }

    // Set diagnostics per file
    for (const auto& [file, file_errors] : errors_by_file)
    {
        auto diags = CompilerErrorParser::to_diagnostics(file_errors);
        diagnostics_service_->set(file, std::move(diags));
    }
}

auto BuildService::ensure_built(const std::string& target, const std::string& config) -> bool
{
    if (!build_before_run_)
    {
        return true; // Skip if disabled
    }

    // If already building, wait for result
    if (is_building())
    {
        return false;
    }

    // If last build succeeded and no changes detected, skip
    if (status_ == BuildStatus::kFinished && last_result_.success)
    {
        return true;
    }

    // Trigger a fresh build and return whether it started
    return start_build(target, config);
}

void BuildService::set_build_before_run(bool enabled)
{
    build_before_run_ = enabled;
}

auto BuildService::build_before_run() const -> bool
{
    return build_before_run_;
}

void BuildService::discover_cmake_targets(const std::string& /*build_dir*/)
{
    // Would execute: cmake --build <build_dir> --target help
    // and parse the output for available targets.
    targets_ = {
        {.name = "all", .description = "Build all targets", .is_default = true},
        {.name = "clean", .description = "Clean build artifacts"},
        {.name = "install", .description = "Install built targets"},
        {.name = "test", .description = "Run CTest tests"},
        {.name = "rebuild_cache", .description = "Rebuild CMake cache"},
    };
}

void BuildService::discover_make_targets(const std::string& /*makefile_dir*/)
{
    // Would parse Makefile for target definitions (lines matching ^target:)
    targets_ = {
        {.name = "all", .description = "Build all targets", .is_default = true},
        {.name = "clean", .description = "Clean build artifacts"},
        {.name = "install", .description = "Install to prefix"},
        {.name = "check", .description = "Run tests"},
    };
}

} // namespace markamp::core
